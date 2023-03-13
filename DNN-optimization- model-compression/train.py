"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Main module for performing the evolution
DISCLAIMER: Most of the implementation ideas are based on the given publication (https://arxiv.org/abs/1912.11527)
            where many of the solutions like finding the parents for the offspring and finding the best solution
            is based right on this article. Also the core idea for module hierarchy and the gene encoding was taken
            from https://github.com/nerdimite/neuroevolution/blob/master/Neuroevolution.ipynb
"""

import numpy as np
import tensorflow as tf
import population
from utils import load_dataset
import statistics
from csv import DictWriter
import fitness
import selection
import numpy as np
import argparse

tf.get_logger().setLevel('ERROR')

layer_ranges = {"Layer1": [ (2,16), (1,7), (1,4)],
                "Layer2": [ (2,32), (2,8), (1,5)]}

# np.random.seed(24)
# random.seed(24)
# tf.random.set_seed(24)

"""
Train function is responsible for calling all functions necessary for the evolution like
generating initial pop, counting fitness, selecting parents, finding best individual for each generation 
and generating new population from the old one.
"""
def train(num_gen, pop_size, mut_prob, mut_param, save_path):
    x_train, y_train, x_test, y_test, num_classes, input_shape = load_dataset()
    pop = population.create_population(pop_size) # initial population consists of different Nets
    all_fitness = []
    best_solutions = []

    # Open the file
    with open(save_path + 'best_summaries.txt','w') as report_file:
   
        for generation in range(num_gen):
            # count fitness for each individual in population
            pop_fitness = fitness.pop_fitness(pop, x_train, y_train, x_test, y_test, generation)

            # append and reshape (just changing the internal data structure for better use)
            all_fitness = np.append(all_fitness, pop_fitness)
            all_fitness = np.squeeze(all_fitness)

            # select 3 parents (heavy,light,knee)
            parents = selection.selection(pop_fitness, pop)

            # get the best result of the ongoing generation
            # the best result is always refered as the knee solution from pareto line (lowest params and highest accuracy)
            best_ind_params, best_ind_acc, best_id = selection.best_solution(pop_fitness)
            best_solutions.append({"id": best_id, "params": best_ind_params, "acc": best_ind_acc, "gen":generation})

            # generate new population of size pop_size 
            pop = population.new_population(parents, pop_size, mut_prob, mut_param)

            print("GENERATION: ", generation+1)
            print(f"BEST INDIVIDUAL(knee solution): (id: {best_id}, params:{best_ind_params}, acc:{best_ind_acc})")
            # find the best individual of this generation and write the summary to best_summaries.txt
            for parent in parents:
                if parent.name == best_id:
                    best_summary = parent
                    break
            report_file.write("GENERATION: " + str(generation + 1) + "\n")
            best_summary.summary(report_file)


    print(f"Settings of this experiment: (mutation_prob={mut_prob}, mutation_param={mut_param}, pop_size={pop_size}, num_generations={num_gen})")
    print(f"Layer settings: {layer_ranges}")

    keys = all_fitness[0].keys()
    
    # saving data to csv files
    with open(save_path + 'all_ind.csv', 'w', newline='') as output_file:
        dict_writer = DictWriter(output_file, keys)
        dict_writer.writeheader()
        dict_writer.writerows(all_fitness)
        output_file.close()

    keys = best_solutions[0].keys()
    with open(save_path + 'best_paret.csv', 'w', newline='') as output_file:
        dict_writer = DictWriter(output_file, keys)
        dict_writer.writeheader()
        dict_writer.writerows(best_solutions)
        output_file.close()
    
    # creating statistic plots
    statistics.create_statistics("all_ind", save_path)
    statistics.create_statistics("best_paret", save_path)



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--mut_prob', default=0.5, type=float, help='Mutation probability')
    parser.add_argument('--mut_param', default=1.0, type=float, help='Mutation parameter')
    parser.add_argument('--gen', default=30, type=int, help='Number of generations')
    parser.add_argument('--pop_size', default=10, type=int, help='Population size')
    parser.add_argument('--save_path', default="experiments/", type=str, help='Path where to save output')             
    args = parser.parse_args()

    train(args.gen, args.pop_size, args.mut_prob, args.mut_param, args.save_path)
