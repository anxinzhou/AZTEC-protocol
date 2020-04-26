import numpy as np 
import matplotlib.pyplot as plt 

workers = [10, 100, 1000, 5000, 10000, 20000, 50000,100000]
times = np.linspace(1,20,8)


plt.ylabel("Cost time")
plt.xlabel("Number of workers")
plt.plot(workers, times)

# for label, time in zip(labels, times):
# 	plt.plot(epsilons, time, label = label)
# plt.legend(labels)
plt.show()