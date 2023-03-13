#!/usr/bin/python3

"""main.py: Main module for running the contour detection and bone measurement algorithm."""

__author__ = "Matej Otčenáš"

import argparse
import os
from pathlib import Path
from Modules import test, train, config

def main():
    """
    Main function starts an algorithm
    """
    # basic argument parsing
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input", required=False,
    	help="path to the input dataset directory(e.g. /my/test/dataset/images)")
    ap.add_argument("-n", "--name", required=False,
    	help="image name from where you want to start the program")
    ap.add_argument("-o", "--output", required=False,
    	help="path to output directory, if not specified, the output will be in this program directory")
    args = vars(ap.parse_args())

    input_dir = args["input"]
    start_image = args["name"]
    output_dir = args["output"]

    if input_dir is None:
	    input_dir = Path('../../images')
    else:
        input_dir = Path(input_dir)

    if output_dir is None:
        output_dir = Path('../results/')
    else:
        output_dir = Path(output_dir)
        output_dir = Path(output_dir / "results/")

    if not os.path.exists(output_dir / 'Contour_coordinates'):
        os.makedirs(output_dir / 'Contour_coordinates')
    if not os.path.exists(output_dir / 'Canny_contours'):
        os.makedirs(output_dir / 'Canny_contours')
    if not os.path.exists(output_dir / 'Distances'):
        os.makedirs(output_dir / 'Distances')
    if not os.path.exists(output_dir / 'Masks'):
        os.makedirs(output_dir / 'Masks')
    if not os.path.exists(output_dir / 'Detectron_contours'):
        os.makedirs(output_dir / 'Detectron_contours')
    if not os.path.exists(output_dir / 'Bone_width'):
        os.makedirs(output_dir / 'Bone_width')


    ## these lines are for training the model but GPU accelerated device is recommended

    # COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, MAX_ITER_META, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES = config.TrainMeta() # configuration for metecarpal trainig process
    # trainer = train.Trainer(COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, MAX_ITER_META, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES) # initialization
    # trainer.run() # starting training process
    #
    # COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, MAX_ITER_ROI, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES = config.TrainROI() # configuration for ROI training process
    # trainer = train.Trainer(COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, MAX_ITER_ROI, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES) # initialization
    # trainer.run() # starting training process

    COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, DETECTIONS_PER_IMAGE, NUM_CLASSES, SCORE_THRESH_TEST = config.TestMeta() # configuration for metecarpal inference
    tester = test.Tester(COCO_NAME, COCO_ANNOTS, MODEL_TYPE, WEIGHT_PATH, DETECTIONS_PER_IMAGE, NUM_CLASSES, SCORE_THRESH_TEST, input_dir, output_dir, start_image)
    tester.run()


if __name__ == "__main__":
    main()
