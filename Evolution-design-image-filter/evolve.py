"""
Author: Matej Otcenas(xotcen01)
Date: 1.5.2022
Description: Main module for performing the evolution
"""

import numpy as np
import argparse
import cv2 as cv

# Internal modules
import fitness
import utils
import detector
import population
from csv import DictWriter


def compare_chromosomes(list1, list2):
    if list2 is not None:
        diff = [x for x in list1 if x not in list2]
        if len(diff) == 0:
            is_equal = True
        else:
            is_equal = False
    else:
        is_equal = False
    return is_equal

def compare_fitness(fit1, fit2):
    if fit1 == fit2:
        return True
    else:
        return False

def train(num_gen, pop_size, mut_prob, save_path, rows=8, cols=9, mut_inc = 0.05 ,img_path="images/", img1_name="chessboard256", img2_name="chessboard256_20%"):
    operations = [0,1,2,3,4,5,6,7,8,9,10,11,12]
    ref_img = utils.load_image(img_path + img1_name + ".png")
    test_img = utils.load_image(img_path + img2_name + ".png")

    # make borders because of 5x5 window
    ref_img = np.uint8(cv.copyMakeBorder(ref_img,2,2,2,2,cv.BORDER_CONSTANT,value=[0]))
    padded_test_img = np.uint8(cv.copyMakeBorder(test_img,2,2,2,2,cv.BORDER_CONSTANT,value=[0]))

    detection_map, mask_img = detector.noise_map(padded_test_img)
    detection_map = np.uint8(cv.copyMakeBorder(detection_map,2,2,2,2,cv.BORDER_CONSTANT,value=[0]))

    cv.imwrite(save_path + "reconstruction/" + img2_name + "_detection_map.png", mask_img)

    pop = population.create_population(rows, cols, operations, pop_size)

    # save best chromosome
    best_individual = (None, None, float('inf'))
    # couunter to overcome divergence
    best_individual_counter = 0
    # save best fitness functions for statistics
    all_best = []

    for generation in range(num_gen):
        # find best individual from current population in the ongoing generation
        best_chromosome, best_filtered_image, best_fitness = fitness.fitness_pop(ref_img, padded_test_img, detection_map, pop)
        
        if compare_chromosomes(best_chromosome, best_individual[0]) or compare_fitness(best_fitness, best_individual[2]): # compare best with previous iteration
            best_individual_counter += 1
        else:
            if best_individual[2] > best_fitness:
                best_individual = (best_chromosome, best_filtered_image, best_fitness) # save chromosome, image and fitness
                best_individual_counter = 0
            
        all_best.append({"id": generation, "fitness":best_individual[2], "mut": mut_prob})
        
        if best_individual_counter == 5 and mut_prob < 1.0: # mutate best to get something new
            if mut_prob >= 0.95:
                mut_prob = 1.0
            else:
                mut_prob += mut_inc
            best_individual_counter = 0
            print("Raising mutation rate to: ", mut_prob)

        print(f"GENERATION: ", generation + 1)
        print(f"BEST FITNESS: ",best_individual[2])

        # repopulate using the best individual from this generation
        pop = population.repopulate(best_chromosome, pop_size, rows, operations, mut_prob) # best_chromosome

    keys = all_best[0].keys()
    
    # saving data to csv files
    with open(save_path + "CSV/" + img2_name + "_convergence.csv", 'w', newline='') as output_file:
        dict_writer = DictWriter(output_file, keys)
        dict_writer.writeheader()
        dict_writer.writerows(all_best)
        output_file.close()

    cv.imwrite(save_path + "reconstruction/" + img2_name + ".png", best_individual[1])
  

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--mut_prob', default=0.1, type=float, help='Mutation probability')
    parser.add_argument('--gen', default=100, type=int, help='Number of generations')
    parser.add_argument('--pop_size', default=5, type=int, help='Population size')
    parser.add_argument('--save_path', default="experiments/", type=str, help='Path where to save output')
    parser.add_argument('--rows', default=8, type=int, help='CGP rows')
    parser.add_argument('--cols', default=9, type=int, help='CGP cols')
    parser.add_argument('--mut_inc', default=0.05, type=float, help='Mutation increment')
    parser.add_argument('--img_path', default="images/", type=str, help='Input images path')
    parser.add_argument('--img1_name', default="chessboard256", type=str, help='Input image1 name')
    parser.add_argument('--img2_name', default="chessboard256_20%", type=str, help='Input image2 name')

    args = parser.parse_args()

    train(args.gen, args.pop_size, args.mut_prob, args.save_path, 
          args.rows, args.cols, args.mut_inc, args.img_path, args.img1_name, args.img2_name)