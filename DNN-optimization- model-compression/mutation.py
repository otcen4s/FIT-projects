"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for performing the mutation on the parent.
"""

import random
import layer
import net

"""
Mutation function is trying to mutate parent solution with certain probability.
If the condition for mutation is not reached, no mutation is performed and the offspring is the old parent.
"""
def mutation(parent, mut_prob, mut_param):
  
    layers = parent._layers
    new_params = []
    new_layers = []

    # iterate each layer in the parent
    for _layer in layers:
        layer_ranges = _layer.layer_ranges
        layer_params = _layer.layer_params
        # iterate over each parameter of the layer and the given ranges defined in Layer class
        for layer_param, layer_range in zip(layer_params, layer_ranges):

            # mutate if the probability is less than mut_prob
            if random.uniform(0, 1) < mut_prob: # mutate
                choose_range = random.uniform(0, 1) # choose which bound will be selected (lower or upper)
                if choose_range <= 0.5:
                    # perform mutation for and generate new parameter (lower range)
                    new_param = round(layer_param + ((2*choose_range)**(1/(1+mut_param)) - 1 )*(layer_param - layer_range[0]))
                else:
                    # perform mutation for and generate new parameter (upper range)
                    new_param = round(layer_param + ( 1 - (2*(1 - choose_range))**(1/(1+mut_param)) )*(layer_range[1] - layer_param))
            else: # no mutation
                new_param = layer_param
            new_params.append(new_param)

        new_layers.append(layer.Layer(new_params, layer_ranges))
        new_params = [] 

    # generate new net (offspring)
    offspring = net.Net(new_layers)
    return offspring