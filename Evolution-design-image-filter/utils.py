import cv2 as cv

def load_image(path):
    img = cv.imread(path, 0)
    # resized = cv.resize(img, (256,256), interpolation = cv.INTER_AREA)
    return img