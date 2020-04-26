import numpy as np 
import matplotlib.pyplot as plt 
epsilons = np.linspace(0.3,3.3,16)

times = np.array([
np.linspace(1,20,16),
np.linspace(3,40,16),
	])
labels = ["Non oblivious", "oblivious"]

plt.ylabel("Cost time")
plt.xlabel("Ɛ")

for label, time in zip(labels, times):
	plt.plot(epsilons, time, label = label)
plt.legend(labels)
plt.show()