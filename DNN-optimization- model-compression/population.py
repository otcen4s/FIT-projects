"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for creating initial and new population (repopulation step).
"""

import layer
import net
import offspring
import copy
import keras.backend as K

# create population of given size from internal representation of chromosome
"""
Create initial population of the given range of population.
"""
def create_population(population_size):
    return [net.Net(layer.generate_new_layers()) for _ in range(population_size)]

"""
Generate new population based on the old one from the finished generation.
"""
def new_population(parents, pop_size, mut_prob, mut_param):
    new_population = []

    # implicitly insert three parents to the new population (knee, heavy, light)
    # each of these parents have to reset trained weights from the previous training 
    for parent in parents:
        parent = reset_parent_weights(parent)
        new_population.append(copy.deepcopy(parent))
    
    # generate new offsprings of the range (populatioon size) - (numbers of parents)
    for _ in range(pop_size - len(parents)):
        new_offspring = offspring.generate_offspring(parents,  mut_prob, mut_param)
        new_population.append(new_offspring)

    return new_population

"""
Reset the trained weights for the initial ones.
"""
def reset_parent_weights(parent):
    layers = parent._layers
    new_old_params = []
    new_layers = []

    # iterate each layer
    for _layer in layers:
        layer_ranges = _layer.layer_ranges
        layer_params = _layer.layer_params
        # iterate each parameter of the layer
        for layer_param in layer_params:
            new_old_params.append(layer_param)

        new_layers.append(layer.Layer(new_old_params, layer_ranges))
        new_old_params = []

    # reset the weights
    parent = net.Net(new_layers)

    return parent
 