import numpy as np
import detector
import math


def filter_image(img, noise_map, chromosome):
    output_image = []
   
    kernel_size = 5
    target_size = detector.calculate_target_size(img.shape[0], kernel_size)

    # Iterate over the rows
    for i in range(target_size):
        # Iterate over the columns
        for j in range(target_size):
            # Get the current matrix
            window = np.array(img[i:i+kernel_size, j:j+kernel_size])
            window = window.flatten()
            
            noise_map_window = np.array(noise_map[i:i+kernel_size, j:j+kernel_size])
            noise_map_window = noise_map_window.flatten()

            window_px13 = window[12]
            map_px13 = noise_map_window[12]

            if map_px13 == 0: # do not filter this pixel
                output_image.append(window_px13)
            else: # filter pixel
                activation_map = np.where(noise_map_window == 1)[0]
                window = window.astype("float")
                window[activation_map] = float('nan') # dead inputs replace with nan

                output_image.append(np.uint8(cgp(chromosome, window)))
               

    output_image = np.array(output_image)
    output_image = np.reshape(output_image, (256,256))
    return output_image


def cgp(chromosome, inputs):
    last_node_used = chromosome[::-1][0][0] # reverse the chromosome and get the index of the last node that is connected to the output

    counter = len(inputs)
    for node in chromosome:
        if counter < last_node_used:
            in1 = node[0] # index of the node
            in2 = node[1] # index of the node
            op = node[2] # index of the operation

            if math.isnan(inputs[in1]) and not math.isnan(inputs[in2]):
                in1 = float('nan')
                in2 = int(inputs[in2])
            elif math.isnan(inputs[in2]) and not math.isnan(inputs[in1]):
                in1 = int(inputs[in1])
                in2 = float('nan')
            elif math.isnan(inputs[in1]) and math.isnan(inputs[in2]):
                in1 = float('nan')
                in2 = float('nan')
            else:
                in1 = int(inputs[in1])
                in2 = int(inputs[in2])

            output = operation(op, in1, in2) # apply operation of node to input values
            inputs = np.append(inputs, output) # append the result of the operation inputs (we can do this thanks to ordered nodes)
            counter += 1
        else: break # 
    return inputs[-1] # get the last output value


def operation(select, input1, input2):
    if math.isnan(input1) and not math.isnan(input2):
        return {
            0 : 255, 
            1: input2,
            2: 255 - input2,
            3: input2,
            4: input2,
            5: input2 >> 1,
            6: input2 >> 2,
            7: 255 if (input2 + input2) > 255 else input2 + input2,
            8: 255 if ((input2 + input2) >> 1) > 255 else ((input2 + input2) >> 1),
            9: input2,
            10: 255 if np.absolute(input2 - input2) > 255 else np.absolute(input2 - input2),
            11: 255 if (input2 << 1) > 255 else input2 << 1,
            12: 255 if (input2 << 2) > 255 else input2 << 2
        }[select]
    
    elif math.isnan(input2) and not math.isnan(input1):
        return {
            0 : 255, 
            1: input1,
            2: 255 - input1,
            3: input1,
            4: input1,
            5: input1 >> 1,
            6: input1 >> 2,
            7: 255 if (input1 + input1) > 255 else input1 + input1,
            8: 255 if ((input1 + input1) >> 1) > 255 else ((input1 + input1) >> 1),
            9: input1,
            10: 255 if np.absolute(input1 - input1) > 255 else np.absolute(input1 - input1),
            11: 255 if (input1 << 1) > 255 else input1 << 1,
            12: 255 if (input1 << 2) > 255 else input1 << 2
        }[select]

    elif math.isnan(input1) and math.isnan(input2):
        return float('nan') 
    else:
        return {
            0 : 255, 
            1: input1,
            2: 255 - input1,
            3: max(input1, input2),
            4: min(input1, input2),
            5: input1 >> 1,
            6: input1 >> 2,
            7: 255 if (input1 + input2) > 255 else input1 + input2,
            8: 255 if ((input1 + input2) >> 1) > 255 else ((input1 + input2) >> 1),
            9: input2 if input1 > 127 else input1,
            10: 255 if np.absolute(input1 - input2) > 255 else np.absolute(input1 - input2),
            11: 255 if (input1 << 1) > 255 else input1 << 1,
            12: 255 if (input1 << 2) > 255 else input1 << 2
        }[select]