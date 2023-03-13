import numpy as np


def calculate_target_size(img_size: int, kernel_size: int) -> int:
    num_pixels = 0
    
    # From 0 up to img size (if img size = 224, then up to 223)
    for i in range(img_size):
        # Add the kernel size (let's say 3) to the current i
        added = i + kernel_size
        # It must be lower than the image size
        if added <= img_size:
            # Increment if so
            num_pixels += 1
            
    return num_pixels


def noise_map(img):

    kernel_size = 5
    target_size = calculate_target_size(img.shape[0], kernel_size)
    
    mask = np.zeros(shape=(target_size, target_size))
    mask_img = np.zeros(shape=(target_size, target_size))

    # Iterate over the rows
    for i in range(target_size):
        # Iterate over the columns
        for j in range(target_size):
            # Get the current matrix
            window = np.array(img[i:i+kernel_size, j:j+kernel_size])
            window = window.flatten()

            window_px13 = window[12]

            # Find out a vector I0 by sorting the pixels in ascending order and then 
            # find the median value of the sorted vector I0.
            I_O = np.sort(window)
            median = np.median(I_O)

            # In the vector I0, in between 0 and median find the average value of pixels and set its corresponding 
            # pixel in the V0 as the boundary B1.
            # In similar manner, the boundary B2 is set in between median and 255
            I_O_first_idx = np.where((I_O >= 0) & (I_O <= median))
            I_O_second_idx = np.where((I_O >= median) & (I_O <= 255))

            I_O_first = I_O[I_O_first_idx]
            I_O_second = I_O[I_O_second_idx]


            # If the candidate pixel belongs to the middle cluster, it is identified as an “uncorrupted” 
            # pixel, else it is identified as a “corrupted” pixel.
            if (window_px13.item() >= np.average(I_O_first).item() and window_px13.item() <= np.average(I_O_second).item()): 
                mask[i, j] = 0 # uncorrupted
                mask_img[i,j] = 0
            else: 
                mask[i, j] = 1 # corrupted
                mask_img[i,j] = 255
            
    mask = np.array(mask)
    mask = np.reshape(mask, (256,256))

    mask_img = np.array(mask_img)
    mask_img = np.reshape(mask_img, (256,256))

    return mask, mask_img