"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Module for parent selection and selection of the best individual.
"""

import fitness

"""
Selection of the parents is baed on three solutions from the population.
Parent1 = maximal accuracy
Parent2 = minimal parameters
Parent3 = knee solution of accuracy and parameters (compromise)
"""
def selection(pop_fitness, population):
    _, max_acc_id = fitness.find_min_max_acc(pop_fitness)
    min_params_id, _ = fitness.find_min_max_params(pop_fitness)
    knee_id = fitness.find_knee_solution(pop_fitness)

    parents = []

    # create parents
    for ind in population:
        if ind.name == max_acc_id or ind.name == min_params_id or ind.name == knee_id:
            parents.append(ind)

    return parents

"""
Choose best solution for the past generation.
This solution is interpreted as the knee solution, the best one for multicriterial choice
"""
def best_solution(pop_fitness):
    knee_id = fitness.find_knee_solution(pop_fitness)
    
    for best in pop_fitness:
        if best["id"] == knee_id:
            return best["params"], best["acc"], best["id"]


