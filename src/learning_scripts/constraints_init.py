from __future__ import absolute_import
from __future__ import division

import numpy as np
import random as rd

trials_limit = 100

def initialize_weights(dim, n_edges):
  c_init = []

  # first direction is chosen randomly
  c1 = []

  for j in range(dim):
      c1j = 2*rd.random() - 1
      c1.append(c1j)

  c_init.append(c1)

  # for each following direction, we try 100 vectors and keep the one whose angles with
  # previous directions have the lowest (squared) cosine
  for i in range(1, n_edges):
      ci = []
      min_cos_sum = np.inf

      for trial in range(trials_limit):
          ci_t = []
          ci_t_sqnorm = 0

          # Building a candidate direction
          for j in range(dim):
              cij_t = 2.0*rd.random() - 1.0
              ci_t.append(cij_t)
              ci_t_sqnorm += cij_t**2

          cos_sum = 0

          # Computing dot product with everything else previously in the list
          for prev_vector_index in range(i):
              dot_prod = 0
              ck_sqnorm = 0
              for j in range(dim):
                  dot_prod += c_init[prev_vector_index][j] * ci_t[j]
                  ck_sqnorm += c_init[prev_vector_index][j]**2

              cos_sum += (dot_prod**2 / (ck_sqnorm * ci_t_sqnorm))

          # Update the minimum one
          if (cos_sum < min_cos_sum):
              ci = ci_t
              min_cos_sum = cos_sum

      c_init.append(ci)

  return c_init



def initialize_biases(dim, n_edges, c_init, sample):
  d_init = []

  # biases are chosen such that each constraint is respected for a sample good point
  for i in range(n_edges):
    di = 0.1*rd.random()

    for j in range(dim):
      di += c_init[i][j] * sample[j]

    d_init.append(di)

  return d_init
