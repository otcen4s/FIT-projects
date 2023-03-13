#!/usr/bin/python3

"""test.py: Core module for image inferences creating inference model
configuration. Mask extraction, contour extraction and bone measurements are
included for both, full width bone or three separate regions."""

"""
NOTE1:
    For image inference was used Detectron2 library for object detection https://github.com/facebookresearch/detectron2.
NOTE2:
    Variable_name_C indicates that the value is precompiled as C variable type, for speed.
optimization.
"""

__author__ = "Matej Otčenáš"

import torch
"""
Imports for Detectron2
"""
from detectron2.config import get_cfg
from detectron2.engine import DefaultPredictor
from detectron2.config import get_cfg
from detectron2.utils.visualizer import Visualizer
from detectron2.data import MetadataCatalog, DatasetCatalog
from detectron2 import model_zoo
from detectron2.utils.visualizer import ColorMode
from detectron2.data.datasets import register_coco_instances
from detectron2.evaluation import COCOEvaluator, inference_on_dataset
from detectron2.data import build_detection_test_loader


"""
Basic python imports
"""
import numpy as np
import cv2
import random
import os
from pathlib import Path
import sys
import math
import cython
import glob
from tqdm import tqdm
from PIL import Image
from pathlib import Path

"""
Modules import
"""
from Modules import canny, distance, mask, roi, contour_coordinates, three_regions, config


class Tester:
    """
    Description
    -------
    Tester class with set of methods for input image inference.

    Methods
    -------
    run(roi=False, img=None, img_name=None, top_y=None, bottom_y=None)
        Starts the inference process on third metacarpal bone image or one of the three regions
    loadImages(directory, predictor, metadata)
        Loads input image from given database (input directory)
    predictMeta(img, metadata, predictor, directory, img_name, BONE_ID)
        Starts image prediction on metacarpal bone
    predictROI(img, predictor, img_name, top_y, bottom_y)
        Starts image prediction on ROI for three separate regions
    saveOutput(img, path)
        Saves image output to given directory
    """

    metadata = None
    full_width = 0
    left_compact = 0
    medullary_cavity = 0
    right_compact = 0

    def __init__(self, COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, DETECTIONS_PER_IMAGE, NUM_CLASSES, SCORE_THRESH_TEST, INPUT, OUTPUT, START_IMAGE):
        """
        Parameters
        ----------
        COCO_NAME: str
            Name of the annotated images folder
        COCO_ANNOTS: str
            Image annotations exported to '.json' file in COCO format
        MODEL_TYPE: str
            Loading model zoo configuration, using  ResNet and FPN(Feature Pyramid Networks) backbone
        WEIGHT_PATH: str
            Loading custom trained model weights from model trained on metacarpal bones
        DETECTIONS_PER_IMAGE: int
            Controls the maximum number of objects to be detected
        NUM_CLASSES: int
            Number of thing classes for R-CNN
        SCORE_THRESH_TEST: float
            Sets score threshold for detected object which will pass through
        INPUT: str
            Input test images directory
        OUTPUT: str
            Output directory for inferences
        START_IMAGE: str
            Start image position name
        """

        self.COCO_NAME = COCO_NAME
        self.COCO_ANNOTS = COCO_ANNOTS
        self.MODEL_TYPE = MODEL_TYPE
        self.WEIGHT_PATH = WEIGHT_PATH
        self.DETECTIONS_PER_IMAGE = DETECTIONS_PER_IMAGE
        self.NUM_CLASSES = NUM_CLASSES
        self.SCORE_THRESH_TEST = SCORE_THRESH_TEST
        self.INPUT = INPUT
        self.OUTPUT = OUTPUT
        self.START_IMAGE = START_IMAGE

    def run(self, roi=False, img=None, img_name=None, int top_y_C=0, int bottom_y_C=0):
        """
        Starts the inference process on third metacarpal bone image or one of the three regions.

        Parameters
        ----------
        roi: boolean
            Indicates if there should be inference on ROI or not (default is False)
        img: array
            Input image for ROI inference
        img_name: str
            Image name for ROI inference
        top_y: float
            Boundary point for ROI
        bottom_y: float
            Boundary point for ROI
        """

        if roi:
            self.COCO_NAME, self.COCO_ANNOTS, self.MODEL_TYPE, self.WEIGHT_PATH, self.DETECTIONS_PER_IMAGE, self.NUM_CLASSES, self.SCORE_THRESH_TEST = config.TestROI() # get configuration for ROI prediction

        try:
            register_coco_instances(self.COCO_NAME, {}, self.COCO_ANNOTS, self.COCO_NAME) # register training dataset in coco format
        except: # already registered but don't fail and continue
            pass

        # configuration for test
        cfg = get_cfg()
        cfg.MODEL.DEVICE = "cpu"
        cfg.merge_from_file(model_zoo.get_config_file(Path(self.MODEL_TYPE)))
        cfg.DATASETS.TRAIN = (Path(self.COCO_NAME),)
        cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = self.SCORE_THRESH_TEST
        cfg.MODEL.WEIGHTS = self.WEIGHT_PATH
        cfg.MODEL.ROI_HEADS.NUM_CLASSES = self.NUM_CLASSES
        cfg.TEST.DETECTIONS_PER_IMAGE = self.DETECTIONS_PER_IMAGE


        predictor = DefaultPredictor(cfg)
        metadata = MetadataCatalog.get(self.COCO_NAME)
        self.metadata = metadata

        if not roi:
            self.loadImages(predictor)
        else:
            self.predictROI(img, predictor, img_name, top_y_C, bottom_y_C)

    @cython.boundscheck(False)
    def loadImages(self, predictor):
        """
        Loads input image from given database (input directory).

        Parameters
        ----------
        predictor: detectron2.engine.defaults.DefaultPredictor
            Detectron2 predictor for making predictions on given image input
        """
        images = list()
        for name in self.INPUT.rglob('*.tif'): # obtainig list of picture names in directory
            images.append(name)
        is_listed = False

        # create CSV head if it is not already created
        path = Path(self.OUTPUT / 'Distances/measure.csv')
        try:
            with open(path) as f:
                pass
        except IOError:
            with open(path, "a") as file:
                file.write(f"image_name,full_bone_width,left_compact,medullary_cavity,right_compact,bone_number\n")


        for i, img_name in enumerate(tqdm(images)):
            img_name = Path(img_name)

            if self.START_IMAGE is None:
                is_listed = True
            else:
                if img_name.parts[-1].split('.')[0] == self.START_IMAGE:
                    is_listed = True

            if is_listed:
                BONE_ID = 0

                #****************************************************************#
                print("\n--------------------------------------------------")
                print(f"Image name: {img_name.parts[-1].split('.')[0]}")
                #****************************************************************#

                img = cv2.imread(str(img_name)) # reading image
                img = self.predictMeta(img, predictor, img_name.parts[-1].split('.')[0], BONE_ID) # run prediction

                #****************************************************************#
                print("--------------------------------------------------")
                #****************************************************************#

                path = Path(self.OUTPUT / 'Detectron_contours' / img_name.parts[-1])
                self.saveOutput(img, path) # saving image to image output directory

    @cython.boundscheck(False)
    def predictMeta(self, img, predictor, img_name, int BONE_ID_C):
        """
        Starts image prediction on metacarpal bone.

        Parameters
        ----------
        img: array
            Input image for making prediction
        predictor: detectron2.engine.defaults.DefaultPredictor
            Detectron2 predictor for making predictions on given image input
        img_name: str
            Input image name
        BONE_ID_C: int
            Bone identifier number

        Returns
        -------
        array
            image prediction mask
        """

        outputs = predictor(img)

        cdef int detected_bones_C = len(outputs["instances"].pred_boxes) # number of detected bones in one image
        cdef int height_C = img.shape[1]
        cdef int width_C = img.shape[2]
        cdef int i_C, top_y_C, bottom_y_C
        cdef int x1_C, y1_C, x2_C, y2_C
        cdef double circumference_C

        lined_img = np.zeros((height_C, width_C, 3), np.uint8)

        meta_scores = outputs["instances"].get("scores").tolist() # convert tensor to list

        for i_C in range(detected_bones_C):
            masks = outputs['instances'].pred_masks[i_C] # process each mask separately

            #****************************************************************#
            print("**************************************************")
            #****************************************************************#
            img_mask = mask.Mask(masks) # creating mask

            name = img_name + '.png'
            path = Path(self.OUTPUT / 'Masks' /name)
            self.saveOutput(img_mask, path)

            img_edged = canny.Canny(img_mask) # creating contour from mask

            path = Path(self.OUTPUT / 'Canny_contours' /name)
            self.saveOutput(img_edged, path)
            #****************************************************************#
            print(f"Exporting contour coordinates of bone No.{i_C}")
            #****************************************************************#

            contour_coordinates.export_coordinates(img_edged, img_name, self.OUTPUT, BONE_ID_C)

            #****************************************************************#
            print(f"Saved to directory \'{self.OUTPUT}/Contour_coordinates\'")
            #****************************************************************#

            boxes = outputs["instances"].pred_boxes.tensor.numpy() # convert to numpy array from tensor

            #****************************************************************#
            print(f"Counting distance of bone No.{i_C}")
            #****************************************************************#

            distancer = distance.Distancer(img_edged, boxes[i_C], img_name, BONE_ID_C, self.OUTPUT)
            circumference_C, min_len_coordinates = distancer.measureMeta()
            self.full_width = circumference_C # save for further export to csv

            path = Path(self.OUTPUT / 'Distances/full_width.tps')
            with open(path, "a") as file:
                file.write(f"IMAGE={img_name}\nBONE_NUMBER={BONE_ID_C}\nWIDTH={circumference_C}\n")

            #****************************************************************#
            print(f"Saved to \'{self.OUTPUT}/Distances/full_width.tps\'")
            #****************************************************************#

            img_roi, top_y_C, bottom_y_C = three_regions.regions(img, min_len_coordinates)

            path = Path(self.OUTPUT / 'Distances/regions_width.tps')
            with open(path, "a") as file:
                file.write(f"IMAGE={img_name}\nBONE_NUMBER={BONE_ID_C}\n")

            self.run(roi=True, img=img_roi, img_name=img_name, top_y_C=top_y_C, bottom_y_C=bottom_y_C) # creating prediction on ROI


            # coordinates of two points for width visualization
            x1_C = min_len_coordinates[0][0]
            y1_C = min_len_coordinates[0][1]
            x2_C = min_len_coordinates[1][0]
            y2_C = min_len_coordinates[1][1]

            lined_img = cv2.line(img, (x1_C, y1_C), (x2_C, y2_C), (0, 255, 255), 2) # draw line to visualize detected width of bone

            BONE_ID_C += 1

            # CSV measurements export rounded to sixth decimal point
            path = Path(self.OUTPUT / 'Distances/measures.csv')
            with open(path, "a") as file:
                file.write(f"{img_name},{round(self.full_width,6)},{round(self.left_compact,6)},{round(self.medullary_cavity,6)},{round(self.right_compact,6)},{BONE_ID_C}\n")

            # we need to clear all variables for the new bone measurements, otherwise if there is some issue there would be no sign of it
            self.full_width = 0
            self.left_compact = 0
            self.medullary_cavity = 0
            self.right_compact = 0

        img_name = img_name + '.png'
        self.saveOutput(lined_img, self.OUTPUT / 'Bone_width' / img_name)

        path = Path(self.OUTPUT / 'Distances/full_width.tps')
        with open(path, "a") as file:
            file.write(f"\n")

        # image output visualization enhancement
        outputs["instances"].remove("pred_boxes") # remove bounding boxes
        outputs["instances"].remove("scores") # remove scores
        outputs["instances"].remove("pred_classes") # remove classes names

        v = Visualizer(img[:, :, ::-1], metadata=self.metadata, scale=1, instance_mode=ColorMode.IMAGE_BW) # Detectron2 function for instance segmentation visualization
        v = v.draw_instance_predictions(outputs["instances"].to("cpu"))

        return v.get_image()[:, :, ::-1]

    @cython.boundscheck(False)
    def predictROI(self, img, predictor, img_name, int top_y_C, int bottom_y_C):
        """
        Starts image prediction on ROI for three separate regions.

        Parameters
        ----------
        img: array
            Input image
        predictor: detectron2.engine.defaults.DefaultPredictor
            Detectron2 predictor creates instance segmentation
        img_name: str
            Input image name
        top_y_C: float
            Boundary point for ROI
        bottom_y_C: float
            Boundary point for ROI
        """

        bones_order = dict()
        bone_measures = dict()

        outputs = predictor(img)

        cdef int detected_bones_C = len(outputs["instances"].pred_boxes) # number of detected bones in one image
        cdef int i_C, topleft_x_C
        cdef int x1_C, y1_C, x2_C, y2_C
        cdef double circumference_C

        roi_scores = outputs["instances"].get("scores").tolist()

        for i_C in range(detected_bones_C):
            masks = outputs['instances'].pred_masks[i_C]
            img_mask = mask.Mask(masks) # creating mask

            img_edged = canny.Canny(img_mask) # creating edges

            boxes = outputs["instances"].pred_boxes.tensor.numpy() # convert to numpy array from tensor

            #****************************************************************#
            print(f"Counting distance of region No.{i_C}")
            #****************************************************************#

            distancer = distance.Distancer(img_edged, boxes[i_C])
            circumference_C, min_len_coordinates = distancer.measureROI(top_y_C, bottom_y_C)

            # coordinates of two points for width visualization
            x1_C = min_len_coordinates[0][0]
            y1_C = min_len_coordinates[0][1]
            x2_C = min_len_coordinates[1][0]
            y2_C = min_len_coordinates[1][1]

            # changing line colors for each detected width (max 3 unique widths)
            if i_C == 0:
                cv2.line(img, (x1_C, y1_C), (x2_C, y2_C), (0, 0, 255), 2)
            elif i_C == 1:
                cv2.line(img, (x1_C, y1_C), (x2_C, y2_C), (0, 255, 0), 2)
            elif i_C == 2:
                cv2.line(img, (x1_C, y1_C), (x2_C, y2_C), (255, 0, 0), 2)

            bone_measures["bone" + str(i_C)] = circumference_C

            _, _, _, topleft_x_C = roi.ROI(boxes[i_C])

            bones_order["bone" + str(i_C)] = topleft_x_C


        sort_bones = sorted(bones_order.items(), key=lambda x: x[1]) # sorts bones order from left to right

        cdef int bone_position_C = 0

        path = Path(self.OUTPUT / 'Distances/regions_width.tps')
        # saving three separate regions bones width into one file
        for bones in sort_bones:
            bone_position_C += 1

            for bone_id in bone_measures:
                if bone_id == bones[0]: # for example bone1 == bone2
                    if bone_position_C == 1:
                        with open(path, "a") as file:
                            file.write(f"L\nwidth={bone_measures[bone_id]}\n")
                            self.left_compact = bone_measures[bone_id]
                    elif bone_position_C == 2:
                        with open(path, "a") as file:
                            file.write(f"M\nwidth={bone_measures[bone_id]}\n")
                            self.medullary_cavity = bone_measures[bone_id]
                    elif bone_position_C == 3:
                        with open(path, "a") as file:
                            file.write(f"R\nwidth={bone_measures[bone_id]}\n\n")
                            self.right_compact = bone_measures[bone_id]
                    break

        #****************************************************************#
        print(f"Saved to \'{self.OUTPUT}/Distances/regions_width.tps\'")
        #****************************************************************#


    def saveOutput(self, img, path):
        """
        Method saves the image input to given directory.

        Parameters
        ----------
        img: array
            Input image
        path: str
            Full image path including image name
        """
        cv2.imwrite(str(path), img)
