import numpy as np 
import matplotlib.pyplot as plt 

tasks = [10, 100, 1000, 5000, 10000, 20000, 50000]
times = tasks.copy()


plt.ylabel("Cost time")
plt.xlabel("Number of tasks")
plt.plot(tasks, times)

# for label, time in zip(labels, times):
# 	plt.plot(epsilons, time, label = label)
# plt.legend(labels)
plt.show()