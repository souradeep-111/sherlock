from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import matplotlib.pyplot as plt



def relu(x):
  if x > 0:
    return x
  else:
    return 0



# compute the cost of each final constraint w.r.t. good points
def compute_costs(n_edges, dim, c_f, d_f, x_data):
  cost = []

  for i in range(n_edges):
    cost_i = 0
    for j in range(len(x_data)):
      cost_ij = 0

      for k in range(dim):
          cost_ij += c_f[i,k] * x_data[j][k]

      cost_i += relu(cost_ij - d_f[i])
    cost.append(cost_i)

  print("cost of each constraint: ", cost)



# compute accuracy for good & bad points
def compute_accuracy(dim, c_f, d_f, x_data, y_data):
  n_edges = len(c_f)
  total_x = len(x_data)
  wrong_x = 0

  for i in range(total_x):
    for k in range(n_edges):
      cfx = 0

      for j in range(dim):
        cfx += c_f[k][j] * x_data[i][j]

      if cfx > d_f[k]:
        wrong_x += 1
        break

  x_accuracy = 100*(total_x - wrong_x)/total_x
  print("accuracy for good points: ", x_accuracy, "%")


  total_y = len(y_data)
  correct_y = 0

  for i in range(total_y):
    for k in range(n_edges):
      cfy = 0

      for j in range(dim):
        cfy += c_f[k][j] * y_data[i][j]

      if cfy > d_f[k]:
        correct_y += 1
        break

  y_accuracy = 100*correct_y/total_y
  print("accuracy for bad points: ", y_accuracy, "%")

  return (x_accuracy * 0.01),(y_accuracy*0.01)


# display results (when in 2 dimensions)
def plot_results(n_edges, dim, c_f, d_f, x_data, y_data, index = -1):
  assert dim == 2
  fig = plt.figure()

  plt.plot(np.transpose(x_data)[0], np.transpose(x_data)[1], 'go')
  plt.plot(np.transpose(y_data)[0], np.transpose(y_data)[1], 'ro')

  t = np.arange(-11, 12, 1)
  for i in range(n_edges):
    u = -(c_f[i][0]/c_f[i][1])*t + d_f[i]/c_f[i][1]
    plt.plot(t,u)

  plt.axis([-11, 11, -11, 11])

  # plt.show()
  plt.savefig('./Figures/image_' + str(index) + '.png' )
  plt.close(fig)


def plot_partition(c_f, d_f, x_data, index = -1):
  dim = len(x_data[0])
  fig = plt.figure()

  plt.plot(np.transpose(x_data)[0], np.transpose(x_data)[1], 'go')

  t = np.arange(-11, 12, 1)
  u = -(c_f[0]/c_f[1])*t + d_f/c_f[1]
  plt.plot(t,u)

  plt.axis([-11, 11, -11, 11])

  # plt.show()
  plt.savefig('./Figures/partition_' + str(index) + '.png' )
  plt.close(fig)


def plot_polyhedrons(polyhedrons, positive_data, negative_data, index = -1):
    assert len(polyhedrons) > 0


    count = 0
    for key in polyhedrons.keys():
        count += 1
        polyhedron = polyhedrons[key]['poly']
        partition = polyhedrons[key]['partition']

        direction = polyhedron[0]
        direction.append(partition[0])
        bias = polyhedron[1]
        bias.append(partition[1])

        dim = len(direction[0])
        assert len(direction) == len(bias)
        plot_results(len(direction), dim, direction, bias, positive_data, negative_data, index*10+count)
