import sherlock_message_pb2

 # Reading a Sherlock Message
f = open("sherlock_answer.sherlock_message", "rb")
sherlock_problem = sherlock_message_pb2.sherlock_message()
sherlock_problem.ParseFromString(f.read())
f.close()

print("Sherlock message formed : ", sherlock_problem)
