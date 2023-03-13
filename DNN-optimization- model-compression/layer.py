"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for Layer class definition and generation of new layers.
"""

import numpy as np

# Class of one convolutional layer
# Each such layer stores the new randomly generated params (nb_fil, fil_size, stride)
# and the corresponding ranges
class Layer():
    def __init__(self, layer_params, layer_ranges):
        self.layer_ranges = layer_ranges
        self.layer_params = layer_params
        self.nb_filters = layer_params[0]
        self.filter_size = layer_params[1]
        self.stride = layer_params[2]


"""
Initialize 'n' new layers with random parameters
"""
def generate_new_layers():                                
                         # nb_filters  fil_size  stride   
    layer_ranges = {"Layer1": [ (2,16), (1,7), (1,4)],
                    "Layer2": [ (2,32), (2,8), (1,5)]}

    layers = []

    # iterate over each "Layer"
    for layer_range in layer_ranges.values():
        params = []
        # create new parameter from given ranges (e.g. (4,16) -> 7)
        for lower, upper in layer_range:
            params.append(np.random.randint(lower,upper)) # [14, 3, 2]

        new_layer = Layer(params, layer_range) 
        layers.append(new_layer) # store created layer
        
    return layers # [Layer1, Layer2]


 

         
        