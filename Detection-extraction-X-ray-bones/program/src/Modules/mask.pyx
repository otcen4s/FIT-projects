#!/usr/bin/python3

"""mask.py: Module for third metacarpal bone mask creation.

NOTE:
    Variable_name_C indicates that the value is precompiled as C variable type, for speed
optimization.
"""

__author__ = "Matej Otčenáš"

import numpy as np
import cython

@cython.boundscheck(False)
def Mask(masks):
    """
    Function converts boolean values to binary image and obtains the bone mask.
    Output image has values of 255 or 0.

    Parameters
    ----------
    masks: tensor
        Input binary values of True and False

    Returns
    -------
    array
        binary mask image
    """
    mask = masks.numpy() # converting to numpy type
    cdef int height_C = mask.shape[0] # extracting shape of image
    cdef int width_C = mask.shape[1]

    binary_mask = np.zeros((height_C, width_C)) # creating binary mask
    binary_mask += mask

    cdef int x_C, y_C, pixel_C

    # binarization of image to values 255 or 0
    for y_C in range(height_C):
        for x_C in range(width_C):
            pixel_C = binary_mask.item(y_C, x_C) # obtaining value of pixel which can be 0 or 1 (False/True)
            if pixel_C:
                binary_mask[y_C,x_C] = 255
            else:
                binary_mask[y_C,x_C] = 0

    return binary_mask
