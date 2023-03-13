#!/usr/bin/python3

"""config.py: Configuration module for training and inference. Values are specific
for use of Detectron2 library.
https://detectron2.readthedocs.io/en/latest/tutorials/getting_started.html
https://github.com/facebookresearch/detectron2

NOTE:
    Models were primary trained on Google Colab(https://colab.research.google.com/)
    due to high quality of GPU utilization.
"""

__author__ = "Matej Otčenáš"

MODEL_USE1 = 1
MODEL_USE2 = 1


def TrainMeta():
    """
    Function for training custom model based on given image annotations using pretrained
    deep neural network (DNN) called Mask RCNN for instance segmentation. Models
    utilize COCO(common objects in context) large-scale detection dataset.
    Training is provided on full x-ray image of human hand, where the third metacarpal is
    the only important.



    Returns
    -------
    str, str, str, str, int, int, int, float, int, int
        set of multiple parameters for model configuration
    """

    if MODEL_USE1 == 1:
        MODEL_META = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'  # loading model zoo configuration, using  ResNet and FPN(Feature Pyramid Networks) backbone
        WEIGHT_PATH = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'  # loading pre-trained weights based on model zoo (https://github.com/facebookresearch/detectron2/blob/master/MODEL_ZOO.md) for instance segmentation
        MAX_ITER_META = 10000  # number of iterations
        NUM_WORKERS = 2  # number of parallel data loading workers
        IMS_PER_BATCH = 2  # number of images per batch across all machines (depends on number of GPUs), each GPU will see 2 images per batch
        BASE_LR = 0.00025  # hyperparameter that controls how much to change the model in response to the estimated error each time the model weights are updated (it has big impact for resulting model)
        BATCH_SIZE_PER_IMAGE = 128  # number of samples(images) that will be propagated through the network
        NUM_CLASSES = 1  # number of thing classes for R-CNN
        COCO_NAME = 'Annotations_JSON/Metacarpals_Train.json'  # image annotations exported to '.json' file in COCO format (https://roboflow.com/formats/coco-json)
        COCO_ANNOTS = 'META_Train'  # name of the annotated images folder

    return COCO_NAME, COCO_ANNOTS, MODEL_META, WEIGHT_PATH, MAX_ITER_META, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES


def TestMeta():
    """
    Function for inference on given dataset of hands based on the trained model. Coming from
    'TrainMeta()' function.

    Returns
    -------
    str, str, str, str, int, int, float
        set of multiple parameters for model prediction
    """
    if MODEL_USE1 == 1:
        MODEL_META = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'  # loading model zoo configuration, using  ResNet and FPN(Feature Pyramid Networks) backbone
        WEIGHT_PATH = '../Models/metacarpal_model.pth'  # loading custom trained model weights from model trained on metacarpal bones
        SCORE_THRESH_TEST = 0.99  # sets score threshold for detected object which will pass through
        NUM_CLASSES = 1  # # number of thing classes for R-CNN
        DETECTIONS_PER_IMAGE = 2  # controls the maximum number of objects to be detected
        COCO_NAME = 'Annotations_JSON/Metacarpals_Train.json'  # image annotations exported to '.json' file in COCO format
        COCO_ANNOTS = 'META_Train'  # name of the annotated images folder

    return COCO_NAME, COCO_ANNOTS, MODEL_META, WEIGHT_PATH, DETECTIONS_PER_IMAGE, NUM_CLASSES, SCORE_THRESH_TEST


def TrainROI():
    """
    Function for training custom model based on given image annotations using pretrained
    deep neural network (DNN) called Mask RCNN for instance segmentation. Models
    utilize COCO(common objects in context) large-scale detection dataset.
    Training is provided on region of interest (ROI) of detected bones where
    the bone width is shortest.

    Returns
    -------
    str, str, str, str, int, int, int, float, int, int
        set of multiple paremeters for model configuration
    """
    if MODEL_USE1 == 1:
        MODEL_META = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'
        WEIGHT_PATH = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'
        MAX_ITER_META = 10000
        NUM_WORKERS = 4
        IMS_PER_BATCH = 2
        BASE_LR = 0.00025
        BATCH_SIZE_PER_IMAGE = 128
        NUM_CLASSES = 3
        COCO_NAME = 'Annotations_JSON/ROI_Train.json'
        COCO_ANNOTS = 'ROI_Train'

    return COCO_NAME, COCO_ANNOTS, MODEL_META, WEIGHT_PATH, MAX_ITER_META, NUM_WORKERS, IMS_PER_BATCH, BASE_LR, BATCH_SIZE_PER_IMAGE, NUM_CLASSES


def TestROI():
    """
    Function for inference on given dataset of ROI of third metacarpal bone
    based on the trained model. Coming from
    'TrainROI()' function.

    Returns
    -------
    str, str, str, str, int, int, float
        set of multiple parameters for model prediction
    """

    if MODEL_USE2 == 1:
        MODEL_ROI = 'COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml'
        WEIGHT_PATH = '../Models/ROI_model.pth'
        SCORE_THRESH_TEST = 0.75
        NUM_CLASSES = 3
        DETECTIONS_PER_IMAGE = 3
        COCO_NAME = 'Annotations_JSON/ROI_Train.json'
        COCO_ANNOTS = 'ROI_Train'

    return COCO_NAME, COCO_ANNOTS, MODEL_ROI, WEIGHT_PATH, DETECTIONS_PER_IMAGE, NUM_CLASSES, SCORE_THRESH_TEST
