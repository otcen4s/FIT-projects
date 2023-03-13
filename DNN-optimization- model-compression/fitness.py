"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for fitness function counting
"""

import numpy as np


"""
Count fitness function for each individual in population 
"""
def pop_fitness(population, x_train, y_train, x_test, y_test, generation):
    pop_fitness = []
    
    for individual in population:
        pop_fitness.append(fitness(individual, x_train, y_train, x_test, y_test, generation))
        
    return pop_fitness


"""
Count fitness of one individual (one neural net). 
AC(Accuracy), SP(Size of Parameters).
"""
def fitness(individual, x_train, y_train, x_test, y_test, generation):
    batch_size = 128
    epochs = 1
    # compile and train the individual(net)
    individual.compile(loss="categorical_crossentropy", optimizer="adam", metrics=["accuracy"])
    individual.fit(x_train, y_train, batch_size=batch_size, epochs=epochs, validation_split=0.1)
    
    # evaluate the model on test data
    score = individual.evaluate(x_test, y_test)
    SP = total_params(individual)
    AC = score[1]
    return {"id":individual.name, "params": int(SP), "acc": float(AC), "gen":generation}


"""
Count the total number of parameters in the net
"""
def total_params(individual):
    trainableParams = np.sum([np.prod(v.get_shape()) for v in individual.trainable_weights], dtype=np.uint32)
    nonTrainableParams = np.sum([np.prod(v.get_shape()) for v in individual.non_trainable_weights], dtype=np.uint32)
    return trainableParams + nonTrainableParams


"""
Find the id of net with the minimal and maximal number of parameters
"""
def find_min_max_params(pop_fitness):
    min_value = float('inf')
    max_value = float('-inf')
    
    # loop over each individual net
    for ind in pop_fitness:
        if int(ind["params"]) < min_value:
            min_value = int(ind["params"])
            min_val_id = ind["id"]
        if int(ind["params"]) > max_value:
            max_value = int(ind["params"])
            max_val_id = ind["id"]
        

    return min_val_id, max_val_id

"""
Find the id of net with the minimal and maximal accuracy
"""
def find_min_max_acc(pop_fitness):
    min_value = float('inf')
    max_value = float('-inf')

    for ind in pop_fitness:
        if float(ind["acc"]) > max_value:
            max_value = float(ind["acc"])
            max_val_id = ind["id"]
        if float(ind["acc"]) < min_value:
            min_value = float(ind["acc"])
            min_val_id = ind["id"]

    return min_val_id, max_val_id


"""
Find the knee solution from the given set.
This solution is selected by manhattan distance between the points and should be the most optimal solution
regarding number of params and accuracy.
https://arxiv.org/abs/1912.11527
"""
def find_knee_solution(pop_fitness):
    min_distance = float('inf')
    min_acc_id, max_acc_id = find_min_max_acc(pop_fitness)
    min_params_id, max_params_id = find_min_max_params(pop_fitness)

    for ind in pop_fitness:
        if ind["id"] == min_acc_id:
            min_acc = float(ind["acc"])
        if ind["id"] == max_acc_id:
            max_acc = float(ind["acc"])
        if ind["id"] == min_params_id:
            min_params = int(ind["params"])
        if ind["id"] == max_params_id:
            max_params = int(ind["params"])


    for ind in pop_fitness:
        # modified manhattan distance
        # very important part of the algorithm, where we select the knee solution using the distance caluclation 
        distance = ((int(ind["params"]) - min_params) / (max_params - min_params)) + (max_acc - (float(ind["acc"])) / (max_acc - min_acc))
        
        # find the minimum distance
        if distance < min_distance:
            min_distance = distance
            knee_id = ind["id"]

    return knee_id
