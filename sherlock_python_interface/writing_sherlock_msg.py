import sherlock_message_pb2

# Making a Sherlock Message
sherlock_message = sherlock_message_pb2.sherlock_message()

network_description = sherlock_message.network_description

network_description.onnx_filename = "../network_files/sine_net/300n/sine_net_2_150.onnx"
network_description.input_tensor_name = "input_tensor"
network_description.output_tensor_name = "output_tensor"


# network_description.input_node_indices.append(1)
# network_description.input_node_indices.append(2)
# network_description.input_node_indices.append(3)
#
# network_description.output_node_indices.append(101)
# network_description.output_node_indices.append(102)
# network_description.output_node_indices.append(103)

# The input intevals
input_interval = sherlock_message.input_interval
mapping = input_interval.limits.add()
mapping.node_index = 0
mapping.upper_limit = 0.5
mapping.lower_limit = -0.5

mapping = input_interval.limits.add()
mapping.node_index = 1
mapping.upper_limit = 0.4
mapping.lower_limit = -0.4

mapping = input_interval.limits.add()
mapping.node_index = 2
mapping.upper_limit = 0.3
mapping.lower_limit = -0.3


# The optimization problem
optimization_problem = sherlock_message.optimization_problem
optimization_problem.direction = True

# Building the linear map
for index in range(3):
    linear_combo = optimization_problem.linear_terms.add()
    linear_combo.coefficient = 4.56 + 3.0 * float(index)
    linear_combo.node_index = index + 1

optimization_problem.status_flag = sherlock_message_pb2.objective.NOT_STARTED
optimization_problem.constant = 39.456


# Printing it to a file
f = open("sherlock_problem.sherlock_message", "wb")
f.write(sherlock_message.SerializeToString())
f.close()



# Reading a Sherlock Message
f = open("sherlock_problem.sherlock_message", "rb")
sherlock_problem = sherlock_message_pb2.sherlock_message()
sherlock_problem.ParseFromString(f.read())
f.close()

print("Status - ", sherlock_problem.optimization_problem.status_flag)

print("Sherlock message formed : ", sherlock_problem)
