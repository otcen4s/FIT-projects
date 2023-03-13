import random

def mutate(chromosome, rows, operations, mut_prob):
    row_counter = 0
    new_chromosome = []

    for i in range(0, len(chromosome) - 1): # start indexing from 9 to 48
        if random.uniform(0, 1) < mut_prob: # apply mutation for operation
            operation = random.choice(operations)
        else: # no mutation
            operation = chromosome[i][2]

        if row_counter < rows: # connect first column to inputs 
            if random.uniform(0, 1) < mut_prob: # apply mutation for input1
                in1 = random.randint(0, 24)
            else: # no mutation
                in1 = chromosome[i][0]
            if random.uniform(0, 1) < mut_prob: # apply mutation for input2
                in2 = random.randint(0, 24)
               
            else: # no mutation
                in2 = chromosome[i][1]
            row_counter += 1

        else:
            if random.uniform(0, 1) < mut_prob: # apply mutation for input1
                in1 = random.randint(0, i - 1)
              
            else: # no mutation
                in1 = chromosome[i][0]
            if random.uniform(0, 1) < mut_prob: # apply mutation for input2
                in2 = random.randint(0, i - 1)
            else: # no mutation
                in2 = chromosome[i][1]
            
        new_chromosome.append([in1, in2, operation]) # chromosome is created of nodes (in1,in2, operation)
        max_idx_used = i
        
    # connect the output to the randomly selected node from the range(9, max_node_index)
    max_idx_used = random.randint(25, max_idx_used - 1)
    new_chromosome.append([max_idx_used])

    return new_chromosome