#!/usr/bin/python3

"""three_regions.py: Module for extraction ROI image including top y and bottom y
coordinates to get region with the narrowest part.

NOTE:
    Variable_name_C indicates that the value is precompiled as C variable type, for speed
optimization.
"""

__author__ = "Matej Otčenáš"

import matplotlib.pyplot as plt
import cv2
import math
import cython
import numpy as np

@cython.boundscheck(False)
def regions(img, min_len_coordinates):
    """
    Function obtains ROI for three separate regions including predefined offsets
    to improve prediciton and ommit not important parts of image.

    Parameters
    ----------
    img: array
        Input image
    min_len_coordinates: list
        List of two points indicating the narrowest part of the bone

    Returns
    -------
    array, float, float
        roi image array and float coordinates
    """
    cdef int offset1_C = 1
    cdef int offset2_C = 50

    roi = getROI(offset1_C, img, min_len_coordinates)

    indices = np.where(roi != [0]) # list of y coordinates from top to bottom of the ROI image
    cdef int top_y_C = offset2_C
    cdef int bottom_y_C = max(indices[0]) + offset2_C

    roi = getROI(offset2_C, img, min_len_coordinates)

    return roi, top_y_C, bottom_y_C


def getROI(int offset_C, img, min_len_coordinates):
    """
    Function estimates the ROI image.

    Parameters
    ----------
    offset: int
        Offset for adujstment the image coordinates (size of the output image)
    img: array
        Input image
    min_len_coordinates: list
        List of two points indicating the narrowest part of the bone

    Returns
    -------
    array
        roi image coordinates
    """
    cdef int x1_C, y1_C, x2_C, y2_C
    if min_len_coordinates[0][1] < min_len_coordinates[1][1]:
        x1_C = min_len_coordinates[0][0] - offset_C
        x2_C = min_len_coordinates[1][0] + offset_C
        y1_C = min_len_coordinates[0][1] + abs(min_len_coordinates[0][1] - min_len_coordinates[1][1]) + offset_C
        y2_C = min_len_coordinates[1][1] - abs(min_len_coordinates[0][1] - min_len_coordinates[1][1]) - offset_C
        roi = img[y2_C:y1_C, x1_C:x2_C]
    else:
        x1_C = min_len_coordinates[0][0] - offset_C
        x2_C = min_len_coordinates[1][0] + offset_C
        y1_C = min_len_coordinates[0][1] - abs(min_len_coordinates[0][1] - min_len_coordinates[1][1]) - offset_C
        y2_C = min_len_coordinates[1][1] + abs(min_len_coordinates[0][1] - min_len_coordinates[1][1]) + offset_C
        roi = img[y1_C:y2_C, x1_C:x2_C]

    return roi
