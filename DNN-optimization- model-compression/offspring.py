"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for generating new offspring.
"""

import random
import mutation

"""
Randomly choose one of the parent (three parents) and perform mutation if the condition is met to generate new offspring
"""
def generate_offspring(parents, mut_prob, mut_param):
    parent = random.choice(parents) # randomly select parent

    offspring = mutation.mutation(parent, mut_prob, mut_param) # create mutation of parent

    return offspring # retun new offspring