import random
import mutation

def create_population(rows, cols, operations, pop_size):
    population = []

    for _ in range(pop_size):
        row_counter = 0
        chromosome = []
        for i in range(25, rows*cols + 24): # start indexing from 9 to 48
            operation = random.choice(operations)
            if row_counter < rows: # connect first column to inputs 
                in1 = random.randint(0, 24)
                in2 = random.randint(0, 24)
                row_counter += 1
            else:
                in1 = random.randint(0, i - 1)
                in2 = random.randint(0, i - 1)

            chromosome.append([in1, in2, operation]) # chromosome is created of nodes (in1,in2, operation)
            max_idx_used = i
            
        # connect the output to the randomly selected node from the range(25, max_node_index)
        max_idx_used = random.randint(25, max_idx_used - 1)
        chromosome.append([max_idx_used])

        population.append(chromosome) # add chromosome to population
            
    return population

def repopulate(parent, pop_size, rows, operations, mut_prob):
    new_pop = []

    new_pop.append(parent)

    for _ in range(pop_size - 1):
        offspring = mutation.mutate(parent, rows, operations, mut_prob)
        new_pop.append(offspring)
    
    return new_pop