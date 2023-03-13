import cv2 as cv
import numpy as np
import cgp

# fit(If,Ir)= ∑∑ |If(i,j) − Ir(i,j)|
def fitness(ref_img, fil_img):
    diff_img = cv.absdiff(ref_img, fil_img)
    diff = np.sum(diff_img)
    return diff


def fitness_pop(ref_img, corr_img, noise_map, population):
    best_fitness = float('inf')

    for chromosome in population:
        filtered_image = cgp.filter_image(corr_img, noise_map, chromosome)
        filtered_image = np.uint8(cv.copyMakeBorder(filtered_image, 2,2,2,2,cv.BORDER_CONSTANT,value=[0]))
        fitness_val = fitness(ref_img, filtered_image)

        print("Fitness: ", fitness_val)

        if fitness_val <= best_fitness:
            best_fitness = fitness_val
            best_chromosome = chromosome
            best_img = filtered_image

    return best_chromosome, best_img, best_fitness
        
