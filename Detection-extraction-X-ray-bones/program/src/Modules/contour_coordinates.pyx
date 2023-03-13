#!/usr/bin/python3

"""contour_coordinates.py: Exports the set of [x,y] coordinates of detected
contour of third metacarpal bone. The output is stored into '.tps' files for
each image separately.

NOTE:
    Variable_name_C indicates that the value is precompiled as C variable type, for speed
optimization.
"""

__author__ = "Matej Otčenáš"

import sys
import cython

@cython.boundscheck(False)
def export_coordinates(img_edged, img_name, OUTPUT, int BONE_ID_C):
    """
    Function exports contour coordinates into output file. If the image has
    detected more than one bone, two or more bone contours will be saved into
    one file.

    Parameters
    ----------
    img_edged: list
        Input image with detected contour (image consisted of 255 or 0 values)
    img_name: str
        Name of the image bone (e.g. 1004_1962LP_F_upr)
    OUTPUT: str
        Name of the output folder
    BONE_ID_C: int
        Bone identifier number
    """

    cdef int height_C = img_edged.shape[0]
    cdef int width_C = img_edged.shape[1]
    cdef int x_C, y_C, pixel_C

    name = img_name + '.tps'
    with open(OUTPUT / 'Contour_coordinates' / name, 'a') as file:
        file.write(f"IMAGE={img_name}\nBONE_NUMBER={BONE_ID_C}\n")
        for y_C in range(height_C): # iterate through entire image
            for x_C in range(width_C):
                pixel_C = img_edged.item(y_C, x_C)
                if pixel_C == 255: # edge found
                    file.write(f"[{x_C},{y_C}]")
        file.write("\n")
