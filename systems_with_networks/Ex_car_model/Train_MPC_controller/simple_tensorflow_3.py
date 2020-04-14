from __future__ import print_function
import tensorflow as tf
import numpy as np
import random
# import matplotlib.pyplot as plt

# Config the matlotlib backend as plotting inline in IPython
# %matplotlib inline

no_of_inputs = 4
no_of_outputs = 2
my_random_seed= 101
np.random.seed(my_random_seed) # Make predictable
episodes = 100000
batch_size = 500
# hidden_units = 500,100
hidden_units_1 = 100
hidden_units_2 = 50
hidden_units_3 = 20
learning_rate = 0.0001

# l rate
# units
# batch size
# episode'

def weight_variable(shape):
    initial = tf.truncated_normal(shape, stddev=0.1, seed=my_random_seed)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.1, shape=shape)
    return tf.Variable(initial)

def relu(x):
    if x > 0:
        return x
    else:
        return 0

# Produce the data
data_stash = np.loadtxt("MPC_data")

total_length = len(data_stash)
no_of_data_points = int(total_length/(no_of_inputs + no_of_outputs))

x_data = np.zeros((no_of_data_points,no_of_inputs)).astype(np.float32)
y_data = np.zeros((no_of_data_points,no_of_outputs)).astype(np.float32)

pointer = 0;

print('No of data points =',no_of_data_points)

for i in range(no_of_data_points):
    # pointer = random.randint(0,no_of_data_points-1)
    # pointer = pointer * (no_of_inputs + no_of_outputs)
    for j in range(no_of_inputs):
        x_data[i][j] = data_stash[pointer]
        pointer += 1
    y_data[i][0] =  data_stash[pointer] + 100
    pointer += 1
    y_data[i][1] =  data_stash[pointer] + 100
    pointer += 1
    # print ('x value = [ ', x_data[i,0],' , ', x_data[i,1],' , ', x_data[i,2],' , ', x_data[i,3],' ]  \n ')
    # print('y Data =',y_data[i])

# reshape data ...
x_data = x_data.reshape(no_of_data_points, no_of_inputs)
y_data = y_data.reshape(no_of_data_points, no_of_outputs)

# create placeholders to pass the data to the model
x = tf.placeholder('float', shape=[None, no_of_inputs])
y_ = tf.placeholder('float', shape=[None, no_of_outputs])



W1 = weight_variable([no_of_inputs, hidden_units_1])
b1 = bias_variable([hidden_units_1])
r1 = tf.nn.relu(tf.matmul(x, W1) + b1)

# Input of r1 into r2
W2 = weight_variable([hidden_units_1, hidden_units_2])
b2 = bias_variable([hidden_units_2])
r2 = tf.nn.relu(tf.matmul(r1,W2)+b2)
#
# # Input of r2 into r3

W3 = weight_variable([hidden_units_2, hidden_units_3])
b3 = bias_variable([hidden_units_3])
r3 = tf.nn.relu(tf.matmul(r2,W3)+b3)
# #
# Input of r3 into r4
# #
# W4 = weight_variable([hidden_units, hidden_units])
# b4 = bias_variable([hidden_units])
# r4 = tf.nn.relu(tf.matmul(r3,W4)+b4)
# #
# # # # Input of r4 into r5
# # #
# W5 = weight_variable([hidden_units, hidden_units])
# b5 = bias_variable([hidden_units])
# r5 = tf.nn.relu(tf.matmul(r4,W5)+b5)
# #
# # Input of r5 into r6
# #
# W6 = weight_variable([hidden_units, hidden_units])
# b6 = bias_variable([hidden_units])
# r6 = tf.nn.relu(tf.matmul(r5,W6)+b6)
# #
# # # Input of r6 into r7
# #
# W7 = weight_variable([hidden_units, hidden_units])
# b7 = bias_variable([hidden_units])
# r7 = tf.nn.relu(tf.matmul(r6,W7)+b7)
#
# W8 = weight_variable([hidden_units, hidden_units])
# b8 = bias_variable([hidden_units])
# r8 = tf.nn.relu(tf.matmul(r7,W8)+b8)
#
# W9 = weight_variable([hidden_units, hidden_units])
# b9 = bias_variable([hidden_units])
# r9 = tf.nn.relu(tf.matmul(r8,W9)+b9)
#
# W10 = weight_variable([hidden_units, hidden_units])
# b10 = bias_variable([hidden_units])
# r10 = tf.nn.relu(tf.matmul(r9,W10)+b10)
#
# W11 = weight_variable([hidden_units, hidden_units])
# b11 = bias_variable([hidden_units])
# r11 = tf.nn.relu(tf.matmul(r10,W11)+b11)
#
# W12 = weight_variable([hidden_units, hidden_units])
# b12 = bias_variable([hidden_units])
# r12 = tf.nn.relu(tf.matmul(r11,W12)+b12)
#
# W13 = weight_variable([hidden_units, hidden_units])
# b13 = bias_variable([hidden_units])
# r13 = tf.nn.relu(tf.matmul(r12,W13)+b13)

# Input of r7 into last ReLU (which is just y)
W14 = weight_variable([hidden_units_3, no_of_outputs])
b14 = bias_variable([no_of_outputs])
y = tf.nn.relu(tf.matmul(r3,W14)+b14)



mean_square_error = tf.reduce_mean(tf.reduce_mean(tf.square(y-y_),1), 0)
training = tf.train.AdamOptimizer(learning_rate).minimize(mean_square_error)

sess = tf.InteractiveSession()
sess.run(tf.initialize_all_variables())

min_error = np.inf
error_array_min=[]
error_array_all=[]
max_error = -np.inf
min_error = np.inf

for epoch in range(episodes):
    # iterrate trough every row (with batch size)
    total_episode_error = 0.0
    for i in range(int(x_data.shape[0]/batch_size)):
        _, error = sess.run([training, mean_square_error],  \
        feed_dict={x: x_data[i * batch_size : (i+1) * batch_size,:], \
        y_:y_data[i * batch_size : (i+1) * batch_size, :]})
        total_episode_error += error

    if(total_episode_error < min_error):
        W_1 = sess.run(W1)
        b_1 = sess.run(b1)
        W_2 = sess.run(W2)
        b_2 = sess.run(b2)
        W_3 = sess.run(W3)
        b_3 = sess.run(b3)
        W_14 = sess.run(W14)
        b_14 = sess.run(b14)
        min_error = total_episode_error
    print("Training error in episode : ", epoch, " is ", total_episode_error)

# error = sess.run([training, mean_square_error],  feed_dict={x: x_data[i:i+batch_size], y_:y_data[i:i+batch_size]})
# if error != None:
#    print(error)


# Extracting the weights below :

filename = "neural_network_controller"
file = open(filename, "w")
# Write the number of inputs
s = str(no_of_inputs) + '\n'
file.writelines(s)
# Write the number of ouputs
s = '2' + '\n'
file.writelines(s)
# Write the number of layers (hidden)
s = '3' + '\n'
file.writelines(s)
# Write the number of neurons in each layer (hidden)
s = str(hidden_units_1) + '\n'
file.writelines(s)
s = str(hidden_units_2) + '\n'
file.writelines(s)
s = str(hidden_units_3) + '\n'
file.writelines(s)
# # For each layer l write the value of weights crossed with the layer (l-1)
# # Do this for the layers from layer 1 to the output layer, thus for 'l' layers
# # we can have l+1 such sets
#
# # Writing weights in the connection of input layer with the 1st hidden layer
#
weights = W_1
biases = b_1

for i in range(hidden_units_1):
    for j in range(no_of_inputs):
        x = weights[j,i]
        s = str(x) + '\n'
        file.writelines(s)
    x = biases[i]
    s = str(x)+ '\n'
    file.writelines(s)

# # Writing weights in the connection of 1st layer hidden with the 2nd hidden layer
#
weights = W_2
biases = b_2
#
for i in range(hidden_units_2):
    for j in range(hidden_units_1):
        x = weights[j,i]
        s = str(x) + '\n'
        file.writelines(s)
    x = biases[i]
    s = str(x) + '\n'
    file.writelines(s)

# # # Writing weights in the connection of 2nd hidden layer with the 3rd hidden layer
#
weights = W_3
biases = b_3
#
# W_3 = weights
# b_3 = biases
#
for i in range(hidden_units_3):
    for j in range(hidden_units_2):
        x = weights[j,i]
        s = str(x) + '\n'
        file.writelines(s)
    x = biases[i]
    s = str(x) + '\n'
    file.writelines(s)
#
# # # # Writing weights in the connection of 2nd hidden layer with the 3rd hidden layer
# # #
# weights = W_4
# biases = b_4
# #
# # W_3 = weights
# # b_3 = biases
# #
# for i in range(hidden_units):
#     for j in range(hidden_units):
#         x = weights[j,i]
#         s = str(x) + '\n'
#         file.writelines(s)
#     x = biases[i]
#     s = str(x) + '\n'
#     file.writelines(s)
# # # Writing weights in the connection of 2nd hidden layer with the 3rd hidden layer
# #
# weights = W_5
# biases = b_5
# #
# # W_3 = weights
# # b_3 = biases
# #
# for i in range(hidden_units):
#     for j in range(hidden_units):
#         x = weights[j,i]
#         s = str(x) + '\n'
#         file.writelines(s)
#     x = biases[i]
#     s = str(x) + '\n'
#     file.writelines(s)
# # # Writing weights in the connection of 2nd hidden layer with the 3rd hidden layer
# #
# weights = W_6
# biases = b_6
# #
# # W_3 = weights
# # b_3 = biases
# #
# for i in range(hidden_units):
#     for j in range(hidden_units):
#         x = weights[j,i]
#         s = str(x) + '\n'
#         file.writelines(s)
#     x = biases[i]
#     s = str(x) + '\n'
#     file.writelines(s)
# # # Writing weights in the connection of 2nd hidden layer with the 3rd hidden layer
# #
# weights = W_7
# biases = b_7
# #
# # W_3 = weights
# # b_3 = biases
# #
# for i in range(hidden_units):
#     for j in range(hidden_units):
#         x = weights[j,i]
#         s = str(x) + '\n'
#         file.writelines(s)
#     x = biases[i]
#     s = str(x) + '\n'
#     file.writelines(s)

# # Writing weights in the connection of 3rd hidden layer with the output layer
#
weights = W_14
biases = b_14

# W_4 = weights
# b_4 = biases
#
for i in range(no_of_outputs):
    for j in range(hidden_units_3):
        x = weights[j,i]
        s = str(x) + '\n'
        file.writelines(s)
    x = biases[i]
    s = str(x)+ '\n'
    file.writelines(s)


file.close()
#
max_error = -np.inf;
#
for i in range(no_of_data_points):
    # print('Starts here\n')
    x = x_data[i,:]
    # print(x)
    x = (np.matmul(x , W_1) + b_1)
    # print(x)
    for j in range(hidden_units_1):
        x[j] = relu(x[j])
    # # print(x)
    x = (np.matmul(x , W_2) + b_2)
    # print(x)
    for j in range(hidden_units_2):
        x[j] = relu(x[j])
    # # # print(x)
    x = (np.matmul(x , W_3) + b_3)
    # # print(x)
    for j in range(hidden_units_3):
        x[j] = relu(x[j])
    # # # print(x)
    # x = (np.matmul(x , W_4) + b_4)
    # # # print(x)
    # for j in range(hidden_units):
    #     x[j] = relu(x[j])
    # # print(x)
    # x = (np.matmul(x , W_5) + b_5)
    # # # print(x)
    # for j in range(hidden_units):
    #     x[j] = relu(x[j])
    # # print(x)
    # x = (np.matmul(x , W_6) + b_6)
    #
    # for j in range(hidden_units):
    #     x[j] = relu(x[j])
    # x = (np.matmul(x , W_7) + b_7)
    # # print(x)
    # for j in range(hidden_units):
    #     x[j] = relu(x[j])
    x = (np.matmul(x , W_14) + b_14)

    # print(x)
    for j in range(no_of_outputs):
        x[j] = relu(x[j])

    y = x
    print('Predicted value = ',y)
    print('Actual value = ', y_data[i][:])

    if(( np.absolute(y[0] - y_data[i][0]) + np.absolute(y[1] - y_data[i][1]) ) > max_error):
        max_error = np.absolute(y[0] - y_data[i][0]) + np.absolute(y[1] - y_data[i][1])
    # print('\n')

print('max_error =',max_error)

sess.close()


# print("should be: min_error: 7.44752e-05")
