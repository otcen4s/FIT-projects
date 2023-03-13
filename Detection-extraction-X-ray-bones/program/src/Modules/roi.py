#!/usr/bin/python3

"""roi.py: Module obtains bounding box points coordinates."""

__author__ = "Matej Otčenáš"

def ROI(boxes):
    """
    Function extracts top left and bottom right point from given bounding box.

    Parameters
    ----------
    boxes: array
        Bounding box of processed bone.

    Returns
    -------
    int, int, int, int
        set of coordinates as separate numbers
    """
    topleft_x = int(boxes[0])
    topleft_y = int(boxes[1])
    bottomright_x = int(boxes[2])
    bottomright_y = int(boxes[3])


    return topleft_y, bottomright_y, bottomright_x, topleft_x
