import numpy as np 
import matplotlib.pyplot as plt 

workers = [100,10000, 20000, 30000, 40000, 50000,60000]
times = np.linspace(1,20,7)


plt.ylabel("Confidential proof generation time")
plt.xlabel("Number of workers")
plt.plot(workers, times)

# for label, time in zip(labels, times):
# 	plt.plot(epsilons, time, label = label)
# plt.legend(labels)
plt.show()