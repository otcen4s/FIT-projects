#!/usr/bin/python3

"""train.py: Module for training the model based on custom
dataset (annotated metacarpal bones or annotated ROI)."""

"""NOTE: for model creation was used Detectron2 library for object detection
https://github.com/facebookresearch/detectron2"""

__author__ = "Matej Otčenáš"

"""
Imports for Detectron2
"""
import torch, torchvision
import detectron2
from detectron2.data.datasets import register_coco_instances
from detectron2.engine import DefaultPredictor
from detectron2.config import get_cfg
from detectron2.utils.visualizer import Visualizer
from detectron2.data import MetadataCatalog
from detectron2 import model_zoo
from detectron2.data import DatasetCatalog, MetadataCatalog
from detectron2.engine import DefaultTrainer
from detectron2.config import get_cfg
from detectron2.utils.logger import setup_logger

"""
Common module imports
"""
import numpy as np
import cv2
import matplotlib.pyplot as plt
import random
import os

from Modules import config


class Trainer:
    """
    Description
    -------
    Trainer class including method for training the model.

    Methods
    -------
    run()
        Starts the training process
    """

    def __init__(self, COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, MAX_ITER_META, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES):
        """
        Parameters
        ----------
        COCO_NAME: str
            Image annotations exported to '.json' file in COCO format (https://roboflow.com/formats/coco-json)
        COCO_ANNOTS: str
            Name of the annotated images folder
        MODEL_TYPE: str
            Loading model zoo configuration, using  ResNet and FPN(Feature Pyramid Networks) backbone
        WEIGHT_PATH: str
            Loading pre-trained weights based on model zoo
            (https://github.com/facebookresearch/detectron2/blob/master/MODEL_ZOO.md) for instance segmentation
        MAX_ITER_META: int
            Number of iterations
        NUM_WORKERS: int
            Number of parallel data loading workers
        IMS_PER_BATCH: int
            Number of images per batch across all machines (depends on number of GPUs), each GPU will see 2 images per batch
        BASE_LR: float
            Hyperparameter that controls how much to change the model in response to the
            estimated error each time the model weights are updated (it has big impact for resulting model)
        BATCH_SIZE_PER_IMAGE: int
            Number of samples(images) that will be propagated through the network
        NUM_CLASSES: int
            Number of thing classes for R-CNN
        """

        self.COCO_NAME = COCO_NAME
        self.COCO_ANNOTS = COCO_ANNOTS
        self.MODEL_TYPE = MODEL_TYPE
        self.WEIGHT_PATH = WEIGHT_PATH
        self.MAX_ITER_META = MAX_ITER_META
        self.NUM_WORKERS = NUM_WORKERS
        self.IMS_PER_BATCH = IMS_PER_BATCH
        self.BASE_LR = BASE_LR
        self.BATCH_SIZE_PER_IMAGE = BATCH_SIZE_PER_IMAGE
        self.NUM_CLASSES = NUM_CLASSES


    def run(self):
        """
        Method for starting the trainig process including configuration for Detectron2
        """
        register_coco_instances(self.COCO_NAME, {}, self.COCO_ANNOTS, self.COCO_NAME) # register training dataset in coco format

        dataset_dicts = DatasetCatalog.get(self.COCO_ANNOTS)
        metadata = MetadataCatalog.get(self.COCO_NAME)

        # ground truth visualization
        for d in random.sample(dataset_dicts, 3):
            img = cv2.imread(d["file_name"])
            v = Visualizer(img[:, :, ::-1], metadata=metadata, scale=0.5)
            v = v.draw_dataset_dict(d)
            plt.figure(figsize = (14, 10))
            plt.imshow(cv2.cvtColor(v.get_image()[:, :, ::-1], cv2.COLOR_BGR2RGB))
            plt.show()

        # creating configuration for training model
        cfg = get_cfg()
        cfg.MODEL.DEVICE='cpu' # if no GPU utilization available on local machine, use CPU instead
        cfg.merge_from_file(model_zoo.get_config_file(self.MODEL_META))
        cfg.DATASETS.TRAIN = (self.COCO_NAME,)
        cfg.DATASETS.TEST = ()
        cfg.DATALOADER.NUM_WORKERS = self.NUM_WORKERS
        cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url(self.WEIGHT_PATH)
        cfg.SOLVER.IMS_PER_BATCH = self.IMS_PER_BATCH
        cfg.SOLVER.BASE_LR = self.BASE_LR
        cfg.SOLVER.MAX_ITER = self.MAX_ITER_META
        cfg.SOLVER.STEPS = []
        cfg.MODEL.ROI_HEADS.BATCH_SIZE_PER_IMAGE =  self.BATCH_SIZE_PER_IMAGE
        cfg.MODEL.ROI_HEADS.NUM_CLASSES = self.NUM_CLASSES


        os.makedirs(cfg.OUTPUT_DIR, exist_ok=True) # creates predefined output directory
        trainer = DefaultTrainer(cfg)
        trainer.resume_or_load(resume=False)
        trainer.train() # starts training process
