#!/usr/bin/python3

"""distance.pyx: Module where circumference of third metacarpal bone is counted
using analytical approach. Two types of bones width can be computed including
full width or three separate regions of bone (left side of bone width, middle
tunnel width and right side of bone width).

NOTE:
    Variable_name_C indicates that the value is precompiled as C variable type, for speed
optimization.
"""

__author__ = "Matej Otčenáš"

import sys
import numpy as np
import math
import cython
import cv2
from libc.math cimport sqrt # C language sqrt
from Modules import roi


class Distancer:
    """
    Distancer class contains methods for analytical approach for third metacarpal
    bone width measurement.

    Methods
    -------
    measureMeta()
        Counts the circumference of third metacarpal bone.
    measureROI(top_y, bottom_y)
        Counts the circumference of three separate regions of third metacarpal bone.
    divideBone(topleft_x, topleft_y, bottomright_x, bottomright_y, offset)
        Computes center line using cross product.
    findMinLength(left_side, right_side, top_y=None, bottom_y=None, roi=False)
        Finds minimal distance between two points.
    measurePythagoras(x1, y1, x2, y2)
        Computes distance between two points.
    pixelsToMm(pixels_len)
        Converts pixels distance to real distance in milimeters using DPI(dots per inch).
    topMiddleX(topleft_y, bottomright_x, topleft_x, offset)
        Finds top middle x coordinate.
    bottomMiddleX(bottomright_y, bottomright_x, topleft_x, offset)
        Finds bottom middle x coordinate.
    """

    def __init__(self, img, box, filename=None, bone_id=None, OUTPUT=None):
        """
        Parameters
        ----------
        img: array
            Input image (Canny edged image)
        box: array
            Bounding box of third metacarapl bone
        filename: str
            Name of the image
        bone_id: int
            Bone identifier number
        OUTPUT: str
            Output directory
        """

        self.img = img
        self.box = box
        self.filename = filename
        self.bone_id = bone_id
        self.OUTPUT = OUTPUT

    def measureMeta(self):
        """
        Counts the circumference of third metacarpal bone using various methods
        and saves the distance to '.tps' file.

        Returns
        -------
        float, list
            minimal distance as float and list of coordinates for minimal distance
        """
        topleft_y, bottomright_y, bottomright_x, topleft_x = roi.ROI(self.box)
        top_mid_x = self.topMiddleX(topleft_y, bottomright_x, topleft_x, 40) # magic number 40 stands for number of iterations for the best top mid x coordinate approximation (could be higher or lower)
        bottom_mid_x = self.bottomMiddleX(bottomright_y, bottomright_x, topleft_x, 40)
        left_side, right_side = self.divideBone(top_mid_x, topleft_y, bottom_mid_x, bottomright_y, 150) # magic number 150 stands for the set of pixels ommited from distance approximation (useless pixels)
        min_len_coordinates, min_len = self.findMinLength(left_side, right_side)
        circumference = self.pixelsToMm(min_len)

        return circumference, min_len_coordinates

    def measureROI(self, top_y, bottom_y):
        """
        Counts the circumference of three separate regions of third metacarpal bone.

        Parameters
        ----------
        top_y: float
            Boundary point for ROI
        bottom_y: float
            Boundary point for ROI

        Returns
        -------
        float, list
            minimal distance as float and list of coordinates for minimal distance
        """
        topleft_y, bottomright_y, bottomright_x, topleft_x = roi.ROI(self.box)
        top_mid_x = self.topMiddleX(topleft_y, bottomright_x, topleft_x, 6) # magic number 6 stands for number of iterations for the best top mid x coordinate approximation (could be higher or lower)
        bottom_mid_x = self.bottomMiddleX(bottomright_y, bottomright_x, topleft_x, 6)
        left_side, right_side = self.divideBone(top_mid_x, topleft_y, bottom_mid_x, bottomright_y, 5) # magic number 5 stands for the set of pixels ommited from distance approximation (useless pixels)
        min_len_coordinates, min_len = self.findMinLength(left_side, right_side, top_y, bottom_y, roi=True)
        circumference = self.pixelsToMm(min_len)

        return circumference, min_len_coordinates

    @cython.boundscheck(False)
    def divideBone(self, int topleft_x_C, int topleft_y_C, int bottomright_x_C, int bottomright_y_C, int offset_C):
        """
        Computes center line using cross product to enable divison the bone into
        two similar halfs. This is not an easy task due to irregularity
        of the bone shape which is irregular polygon.

        Parameters
        ----------
        topleft_x_C: float
            Top left x coordinate for bounding box approximation
        topleft_y_C: float
            Top left y coordinate for bounding box approximation
        bottomright_x_C: float
            Bottom right x coordinate for bounding box approximation
        bottomright_y_C: float
            Bottom right y coordinate for bounding box approximation
        offset_C: int
            Number of pixels which can be ommited

        Returns
        -------
        list, list
            two lists of pixels for left and right side of the bone
        """
        left_side = list()
        right_side = list()

        CP_isabove = lambda p3, p1, p2: np.cross(p3-p1, p2-p1) < 0 # CP = cross product

        p1 = np.array([topleft_x_C, topleft_y_C])
        p2 = np.array([bottomright_x_C, bottomright_y_C])

        cdef int x_C, y_C
        cdef int height_C = self.img.shape[0]
        cdef int width_C = self.img.shape[1]

        for y_C in range(height_C):
            for x_C in range(width_C):
                if y_C >= topleft_y_C + offset_C and y_C <= bottomright_y_C - offset_C:
                    if self.img[y_C,x_C] == 255:
                        p3 = np.array([x_C,y_C])
                        if CP_isabove(p3, p1, p2):
                            left_side.append([y_C,x_C])
                        else:
                            right_side.append([y_C,x_C])

        return left_side, right_side

    @cython.boundscheck(False)
    def findMinLength(self, left_side, right_side, int top_y_C=0, int bottom_y_C=0, roi=False):
        """
        Finds the minimal length by measuring each pixel of left side of the
        divided bone with the each pixel of right side using Pythagoras sentence.

        Parameters
        ----------
        left_side: list
            List of left side coordinates based on center line division
        right_side: list
            List of right side coordinates based on center line division
        top_y_C: float
            Boundary point for ROI
        bottom_y_C: float
            Boundary point for ROI
        roi: list
            Indicates if the roi should be computed (three separate regions)

        Returns
        -------
        list, float
            set of pixels as list and minimal distance as float
        """

        min_len_coordinates = list()

        cdef long long int min_len_C = sys.maxsize
        cdef int pythagoras_distance_C

        for left_coordinate in left_side:
            for right_coordinate in right_side:
                if roi:
                    if left_coordinate[0] <= bottom_y_C and left_coordinate[0] >= top_y_C and right_coordinate[0] <= bottom_y_C and right_coordinate[0] >= top_y_C:
                        pythagoras_distance_C = self.measurePythagoras(left_coordinate[1], left_coordinate[0], right_coordinate[1], right_coordinate[0])

                        if min_len_C > pythagoras_distance_C:
                            min_len_coordinates = [[left_coordinate[1], left_coordinate[0]], [right_coordinate[1], right_coordinate[0]]]
                        min_len_C = min(min_len_C, pythagoras_distance_C)
                else:
                    pythagoras_distance_C = self.measurePythagoras(left_coordinate[1], left_coordinate[0], right_coordinate[1], right_coordinate[0])

                    if min_len_C > pythagoras_distance_C:
                        min_len_coordinates = [[left_coordinate[1], left_coordinate[0]], [right_coordinate[1], right_coordinate[0]]]
                    min_len_C = min(min_len_C, pythagoras_distance_C)

        return min_len_coordinates, min_len_C

    @cython.boundscheck(False)
    def measurePythagoras(self, int x1, int y1, int x2, int y2):
        """
        Counts distance between two points using Pythagoras sentence.

        Parameters
        ----------
        x1: float
            X coordinate of first point
        y1: float
            Y coordinate of first point
        x2: float
            X coordinate of second point
        y2: float
            Y coordinate of second point

        Returns
        -------
        float
            distance in pixels
        """
        return sqrt((y1 - y2)**2 + (x1 - x2)**2) # C optimization

    def pixelsToMm(self, pixels_len):
        """
        Converts pixels distance to real distance in milimeters using
        DPI(dots per inch).
        Formula: distance[mm] = pixels_len * 25,4[mm] / DPI.

        Parameters
        ----------
        pixels_len: float
            Length of measured bone in pixels

        Returns
        -------
        float
            distance in millimeters
        """
        DPI = 450  # original DPI is 150, but the images were zoomed by 300%
        inch_to_mm = 25.4 # 1 inch is 25,4 mm
        return pixels_len * inch_to_mm / DPI

    @cython.boundscheck(False)
    def topMiddleX(self, int topleft_y_C, int bottomright_x_C, int topleft_x_C, int offset_C):
        """
        Finds top middle x coordinate using mean average approach.

        Parameters
        ----------
        topleft_y_C: float
            Top left y coordinate for bounding box approximation
        bottomright_x_C: float
            Bottom right x coordinate for bounding box approximation
        topleft_x_C: float
            Top left x coordinate for bounding box approximation
        offset_C: int
            Number of iterations for the best top mid x coordinate approximation

        Returns
        -------
        int
            average top middle x coordinate
        """
        cdef int x_C, y_C, i_C
        cdef int height_C = self.img.shape[0]
        cdef int width_C = self.img.shape[1]
        x_coords = list()

        for i_C in range(offset_C):
            for y_C in range(height_C):
                for x_C in range(width_C):
                    if x_C >= topleft_x_C and x_C <= bottomright_x_C:
                        if y_C == topleft_y_C - i_C:
                            if self.img[y_C,x_C] == 255: # edge found
                                x_coords.append(x_C)
                        if y_C == topleft_y_C + i_C + 1:
                            if self.img[y_C,x_C] == 255: # edge found
                                x_coords.append(x_C)

        avg = np.mean(x_coords)

        return round(avg) # top middle x coordinate

    @cython.boundscheck(False)
    def bottomMiddleX(self, int bottomright_y_C, int bottomright_x_C, int topleft_x_C, int offset_C):
        """
        Finds bottom middle x coordinate using mean average approach.

        Parameters
        ----------
        bottomright_y_C: float
            Bottom right y coordinate for bounding box approximation
        bottomright_x_C: float
            Bottom right x coordinate for bounding box approximation
        topleft_x_C: float
            Top left x coordinate for bounding box approximation
        offset_C: float
            Number of iterations for the best bottom mid x coordinate approximation

        Returns
        -------
        int
            average bottom middle x coordinate
        """
        cdef int x_C, y_C, i_C
        cdef int height_C = self.img.shape[0]
        cdef int width_C = self.img.shape[1]

        x_coords = list()

        for i_C in range(offset_C):
            for y_C in range(height_C):
                for x_C in range(width_C):
                    if x_C >= topleft_x_C and x_C <= bottomright_x_C:
                        if y_C == bottomright_y_C - i_C:
                            if self.img[y_C,x_C] == 255: # edge found
                                x_coords.append(x_C)
                        if y_C == bottomright_y_C + i_C + 1:
                            if self.img[y_C,x_C] == 255: # edge found
                                x_coords.append(x_C)

        avg = np.mean(x_coords)

        return round(avg) # bottom midddle x coordinate
