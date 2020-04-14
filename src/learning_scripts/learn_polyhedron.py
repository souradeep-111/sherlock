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

# batch_size = 5
episodes = 5000
learning_rate = 0.0005

def compute_polyhedron(no_of_constraints, positive_examples, negative_examples):
    tf.reset_default_graph()
    assert(len(positive_examples) > 0)
    assert(len(negative_examples) > 0)
    space_dim = len(positive_examples[0])
    batch_size = int(len(positive_examples)/10)

    # placeholders for inputs
    X = tf.compat.v1.placeholder(dtype=tf.float32, name="X", shape=[None, space_dim])
    Y = tf.compat.v1.placeholder(dtype=tf.float32, name="Y", shape=[None, space_dim])

    # initialize weights
    c_init = constraints_init.initialize_weights(space_dim, no_of_constraints)

    C = tf.compat.v1.get_variable("C", dtype=tf.float32, initializer=tf.constant(c_init))
    C_n = tf.norm(C, 2, 1)
    #C_nbc = tf.broadcast_to(C_n, [batch_size, n_edges])

    # initialize biases
    # x_sample = positive_examples[int(len(positive_examples)*rd.random())]
    x_sample = data_gen.find_centre(positive_examples)

    d_init = constraints_init.initialize_biases(space_dim, no_of_constraints, c_init, x_sample)

    d = tf.compat.v1.get_variable("d", dtype=tf.float32, initializer=tf.constant(d_init))
    d_bc = tf.broadcast_to(d, [batch_size, no_of_constraints])



    # plot initial constraints
    #t = np.arange(-1, 5.5, 0.5)
    #for i in range(n_edges):
    #  u = -(c_init[i][0]/c_init[i][1])*t + d_init[i]/c_init[i][1]
    #  plt.plot(t,u)



    # graph construction

    # each good point must respect all constraints
    cx     = tf.matmul(X, C, transpose_b=True) # actually XtC because this requires fewer transpositions later on
    cxd_r  = tf.nn.relu(cx - d_bc)
    cxd_rs = tf.reduce_sum(cxd_r)

    # each bad point must violate at least one constraint
    cy      = tf.matmul(Y, C, transpose_b=True)
    cyd_r   = tf.nn.relu(d_bc - cy)
    cyd_rm  = tf.reduce_min(cyd_r, 1)
    cyd_rms = tf.reduce_sum(cyd_rm)

    # the sum of squared cosines of planes must be as low as possible
    ccs = 0
    for i in range(no_of_constraints):
      for k in range(i):
        ccs += tf.square(tf.tensordot(C[i], C[k], 1) / (C_n[i] * C_n[k]))

    # loss function
    loss = cxd_rs + cyd_rms + ccs

    train = tf.compat.v1.train.AdamOptimizer(learning_rate).minimize(loss)

    # session
    with tf.compat.v1.Session() as sess:
    #  writer = tf.compat.v1.summary.FileWriter("./tf_log", sess.graph)

      sess.run(tf.compat.v1.global_variables_initializer())

    #  print("\ninitial weights:\n", sess.run(C))
    #  print("initial biases: ", sess.run(d))

      for j in range(episodes):
          episode_loss = 0.0
          for i in range(int(len(positive_examples)/batch_size)):
              _, l = sess.run([train, loss], feed_dict={X: positive_examples[i*batch_size : (i+1)*batch_size][:], \
              Y: negative_examples[i*batch_size : (i+1)*batch_size][:]})
              episode_loss += l
          print("Loss at episode ", j ,  " is - ", episode_loss)
          c_f = sess.run(C)
          d_f = sess.run(d)
          if ((space_dim == 2) and (((j % 500) == 0) or (j == 0))):
              results_eval.plot_results(no_of_constraints, space_dim, c_f, d_f, positive_examples, negative_examples, j)


      # compute the cost of each final constraint w.r.t. good points
      results_eval.compute_costs(no_of_constraints, space_dim, c_f, d_f, positive_examples)

      # compute accuracy for good & bad points
      results_eval.compute_accuracy(space_dim, c_f, d_f, positive_examples, negative_examples)

      # display results (when in 2 dimensions)
      if space_dim == 2:
        results_eval.plot_results(no_of_constraints, space_dim, c_f, d_f, positive_examples, negative_examples)

    return c_f.tolist(), d_f.tolist()

def find_partition(positive_examples):
    tf.reset_default_graph()
    assert(len(positive_examples) > 0)
    space_dim = len(positive_examples[0])
    batch_size = int(len(positive_examples)/10)

    X = tf.compat.v1.placeholder(dtype=tf.float32, name="X", shape=[None, space_dim])

    initial_direction = []
    initial_bias = 0

    for j in range(space_dim):
        coeff = 2*rd.random() - 1
        initial_direction.append([coeff])


    axis = tf.compat.v1.get_variable("axis", dtype=tf.float32, initializer=tf.constant(initial_direction))
    axis_norm = tf.norm(axis)

    # Find the mid point and add that as a constraint :
    mid_point = data_gen.find_centre(positive_examples)

    print("Mid point learned - ", mid_point)

    axis_eval = 0.0
    norm = 0.0
    for j in range(space_dim):
      axis_eval += (initial_direction[j][0] * mid_point[j])
      norm += (initial_direction[j][0] ** 2.0)


    bias = tf.compat.v1.get_variable("bias", dtype=tf.float32, initializer=tf.constant(axis_eval))
    # bias = tf.constant(-axis_eval)
    bias_bc = tf.broadcast_to(bias, [batch_size, 1])

    tf_mid_point = tf.constant(mid_point)
    mid_point_constraint = tf.square(tf.reduce_sum(tf.multiply(tf_mid_point, axis)) - bias)

    distance_squared = tf.square(tf.matmul(X, axis)/axis_norm - bias_bc)

    loss = -tf.reduce_sum(tf.reduce_sum(distance_squared, 0)) + (1e3 * len(positive_examples)) * mid_point_constraint
    train = tf.compat.v1.train.AdamOptimizer(learning_rate).minimize(loss)

    axis_saved = 0
    bias_saved = 0
    # session
    with tf.compat.v1.Session() as sess:
    #  writer = tf.compat.v1.summary.FileWriter("./tf_log", sess.graph)

      sess.run(tf.compat.v1.global_variables_initializer())

    #  print("\ninitial weights:\n", sess.run(C))
    #  print("initial biases: ", sess.run(d))

      for j in range(int(episodes)):
          episode_loss = 0.0
          for i in range(int(len(positive_examples)/batch_size)):
              data = positive_examples[i*batch_size : (i+1)*batch_size][:]
              _, l = sess.run([train, loss], feed_dict={X: data})
              episode_loss += l
          print("Loss at episode ", j ,  " is - ", episode_loss)
          axis_saved = sess.run(axis)
          bias_saved = sess.run(bias)
          if ((space_dim == 2) and (((j % 500) == 0) or (j == 0))):
              return_axis = []
              for dim in range(len(axis_saved)):
                  return_axis.append(axis_saved[dim][0])
              results_eval.plot_partition(return_axis, bias_saved, positive_examples, j)


    return_axis = []
    for dim in range(len(axis_saved)):
        return_axis.append(axis_saved[dim][0])

    return return_axis, bias_saved
