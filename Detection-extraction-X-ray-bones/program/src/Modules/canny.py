#!/usr/bin/python3

"""canny.py: Module uses basic approach for contour extraction using OpenCV
functions such as threshold or Canny edge detection algorithm."""

__author__ = "Matej Otčenáš"


import cv2
import numpy as np


def Canny(img_mask):
    """
    Method computes edges of given image mask using right threshold and Canny edge
    detection algorithm.

    Parameters
    ----------
    img_mask : list
        Input image mask

    Returns
    -------
    array
        matrix of an image
    """
    img_mask_rgb = np.uint8(img_mask) # converting to unsigned int of 8-bit size (0-255 values)
    edged = cv2.Canny(img_mask_rgb, 100, 200)

    return edged
