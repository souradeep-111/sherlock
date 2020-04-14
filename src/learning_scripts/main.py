from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import random as rd
import time
import sys

import data_gen
import constraints_init
import results_eval
import learn_polyhedron


n_points = 1000
dim = 2
no_of_constraints = 4
max_partitions = 3
target_accuracy_good = 0.9 # Estimates what percentage of good points are inside
target_accuracy_bad = 0.9 # Estimates what percentage  of bad points are outside


# generate goal constraints (a rectangle for now)
# c_rec, d_rec = data_gen.create_rectangle(dim, 1.0, 5.0)

# generate samples and classify them into good (inside the goal constraints) or bad
# positive_data, negative_data = data_gen.generate_samples_from_rectangle(n_points, dim, c_rec, d_rec)

positive_data, negative_data = data_gen.generate_2d_non_convex_data(n_points)

# partition_direction, partition_bias = learn_polyhedron.find_partition(positive_data)
#
# print("Partition direction - ", partition_direction)
# print("Partition bias - ", partition_bias)

# results_eval.plot_partition(partition_direction, partition_bias, positive_data, 0)
# constraints, biases = learn_polyhedron.compute_polyhedron(no_of_constraints, positive_data, negative_data)


def run_bound_learning(positive_data, negative_data, target_good, target_bad, max_partitions):
    assert len(positive_data) > 0
    assert len(negative_data) > 0
    assert len(positive_data[0]) > 0
    assert len(negative_data[0]) > 0

    dim = len(positive_data[0])
    directions = []
    bias = []
    directions, bias = learn_polyhedron.compute_polyhedron(no_of_constraints, positive_data, negative_data)
    current_accuracy_good, current_accuracy_bad = results_eval.compute_accuracy(dim, directions, bias, positive_data, negative_data)

    if((current_accuracy_good > target_accuracy_good) \
    and (current_accuracy_bad > target_accuracy_bad)):
        return [[constraints, biases]]

    partitions_count = 0
    index = 0
    all_polyhedrons = {}
    # The above dictionary has the following :
    # Key --> {polyhedron, +ve_points, -ve_points, accuracy_good, accuracy_bad}
    polyhedron = [directions, bias]
    all_polyhedrons[index] = {'poly':polyhedron, 'data_plus':positive_data, 'data_neg':negative_data, \
    'acc_good':current_accuracy_good, 'acc_bad':current_accuracy_bad}

    current_accuracy_good = 0.0
    current_accuracy_bad = 0.0
    worst_key = -1

    while((current_accuracy_good < target_accuracy_good) \
    or (current_accuracy_bad < target_accuracy_bad)):
        '''
        Algorithm here :
        Find the key with the worst accuracy for bad points
        Partition data in 2 parts : Part 1 , Part 2
        Learn Polyhedron Part 1, get accuracy_1, Part 2, get accuracy_2
        Add the partition to the polyhedrals learnt in the right way
        Delete the chosen key's content from the dictionary
        '''
        if(partitions_count > max_partitions):
            break

        if(worst_key < 0):
            worst_key = find_worst_accuracy(all_polyhedrons)

        p_direction, p_bias = learn_polyhedron.find_partition(all_polyhedrons[worst_key]['data_plus'])

        positive_true, negative_true, positive_false, negative_false = \
        partition_data(p_direction, p_bias, all_polyhedrons[worst_key]['data_plus'],\
        all_polyhedrons[worst_key]['data_neg'])

        # assert len(positive_true)
        # assert len(negative_true)
        # assert len(positive_false)
        # assert len(negative_false)

        index += 1

        if((len(positive_true) > 0) ):
            new_direction, new_bias = learn_polyhedron.compute_polyhedron(no_of_constraints, positive_true,\
            negative_data)
            current_accuracy_good, current_accuracy_bad = results_eval.compute_accuracy\
            (dim, new_direction, new_bias, positive_true, negative_true)
            polyhedron = [new_direction, new_bias]
            all_polyhedrons[index] = {'poly':polyhedron, 'data_plus':positive_true, 'data_neg':negative_true, \
            'acc_good':current_accuracy_good, 'acc_bad':current_accuracy_bad, 'partition':[p_direction, p_bias]}

        index += 1

        if((len(positive_false) > 0)):
            new_direction, new_bias = learn_polyhedron.compute_polyhedron(no_of_constraints, positive_false,\
            negative_data)
            p_direction = list(map(lambda x:-1.0*x, p_direction))
            p_bias = -p_bias
            current_accuracy_good, current_accuracy_bad = results_eval.compute_accuracy\
            (dim, new_direction, new_bias, positive_false, negative_false)
            polyhedron = [new_direction, new_bias]
            all_polyhedrons[index] = {'poly':polyhedron, 'data_plus':positive_false, 'data_neg':negative_false, \
            'acc_good':current_accuracy_good, 'acc_bad':current_accuracy_bad, 'partition':[p_direction, p_bias]}

        del all_polyhedrons[worst_key]
        partitions_count += 1
        worst_key = find_worst_accuracy(all_polyhedrons)
        current_accuracy_good = all_polyhedrons[worst_key]['acc_good']
        current_accuracy_bad = all_polyhedrons[worst_key]['acc_bad']


    return all_polyhedrons

def find_worst_accuracy(all_polyhedron_collection):

    min = 1.0
    for key in all_polyhedron_collection.keys():
        if(all_polyhedron_collection[key]['acc_bad'] < min):
            min = all_polyhedron_collection[key]['acc_bad']
            worst_key = key

    return worst_key

def partition_data(directions, bias, positive_data, negative_data):

    positive_true = []
    negative_true = []
    positive_false = []
    negative_false = []

    for data in positive_data:
        if(eval(data, directions, bias)):
            positive_true.append(data)
        else:
            positive_false.append(data)

    for data in negative_data:
        if(eval(data, directions, bias)):
            negative_true.append(data)
        else:
            negative_false.append(data)

    return positive_true, negative_true, positive_false, negative_false

def eval(point, direction, bias):
    assert len(point) == len(direction)
    sum = -bias
    for dim in range(len(point)) :
        sum += (point[dim] * direction[dim])

    if(sum > 0.0):
        return True
    else:
        return False

all_polyhedron = run_bound_learning(positive_data, negative_data, 0.9, 0.9, 2)

results_eval.plot_polyhedrons(all_polyhedron, positive_data, negative_data, 1)


print("All polyhedrons -- ")
for key in all_polyhedron.keys():
    polyhedron = all_polyhedron[key]['poly']
    print("At key - ", key, " polyhedron - ", polyhedron)
