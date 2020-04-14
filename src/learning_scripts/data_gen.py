from __future__ import absolute_import
from __future__ import division

import numpy as np
import random as rd



def create_rectangle(dim, d_min, d_max):
  c_rec = []
  d_rec = []

  for i in range(dim):
    ci1 = np.zeros(dim)
    ci1[i] = 1
    c_rec.append(ci1)

    di1 = d_min + (d_max - d_min) * rd.random()
    d_rec.append(di1)

    ci2 = np.zeros(dim)
    ci2[i] = -1
    c_rec.append(ci2)

    di2 = d_min + (d_max - d_min) * rd.random()
    d_rec.append(di2)

  c_rec = np.array(c_rec)
  d_rec = np.array(d_rec)
  return c_rec, d_rec



def generate_samples_from_rectangle(n_points, dim, c_rec, d_rec):
  x_data = []
  y_data = []

  for i in range(n_points):
    p = []
    p_flag = True

    for j in range(dim):
      pj = 2**(1.1/dim) * (-d_rec[2*j+1] + (d_rec[2*j] + d_rec[2*j+1]) * rd.random())
      p.append(pj)

      if pj > d_rec[2*j] or pj < -d_rec[2*j+1]:
          p_flag = False

    if p_flag:
      x_data.append(p)
    else:
      y_data.append(p)

  x_data = np.array(x_data)
  y_data = np.array(y_data)

  return x_data, y_data


def generate_2d_non_convex_data(n_points):

    '''
    The following to pieces have been hardcoded here :
    try it in desmos to visualize the graph :
        (x+y-5>0) { 2 < x < 5 } { x + y -7 < 0 }
        (x-y+3>0) {-3 < x < 2} { x - y + 1 < 0 }
    '''
    x_range = [-5.0, 5.0]
    y_range = [-2.0, 6.0]

    positive_examples = []
    negative_examples = []

    trials = 2 * n_points
    for index in range(n_points):

        point_x = x_range[0] + rd.random() * (x_range[1] - x_range[0])
        point_y = y_range[0] + rd.random() * (y_range[1] - y_range[0])

        condition_1a = False
        condition_1b = False
        condition_1c = False

        if((point_x + point_y - 5.0) > 0.0 ):
            condition_1a = True
        if((point_x > 2.0) and (point_x < 5.0)):
            condition_1b = True
        if((point_x + point_y - 7.0) < 0.0):
            condition_1c = True

        condition_1 = condition_1a and condition_1b and condition_1c

        condition_2a = False
        condition_2b = False
        condition_2c = False

        if((point_x - point_y + 3.0) > 0.0 ):
            condition_2a = True
        if((point_x > -3.0) and (point_x < 2.0)):
            condition_2b = True
        if((point_x - point_y + 1.0) < 0.0):
            condition_2c = True

        condition_2 = condition_2a and condition_2b and condition_2c

        if(condition_1 or condition_2):
            positive_examples.append([point_x, point_y])
        else:
            negative_examples.append([point_x, point_y])

    return positive_examples, negative_examples

def find_centre(points):
    number_of_points = len(points)
    assert(number_of_points > 0)
    space_dim = len(points[0])
    mid_point = []

    for current_dim in range(space_dim):
        dim_mid = 0
        sum = 0.0
        for point_index in range(number_of_points):
            sum += points[point_index][current_dim]

        dim_mid = sum / float(number_of_points)
        mid_point.append(dim_mid)

    return mid_point
