import numpy as np 
import matplotlib.pyplot as plt 

threshold = [9, 10, 11, 12]

money = np.arange(13,26,2)

plt.xlabel("Total reward")
plt.ylabel("Extra notes ratio (2^)")
# plt.xlim(min(money),max(money))
plt.xticks(money,money)
labels = []
for l in threshold:
	label = "threshold = "+ str(l)
	overhead = money - l + 1
	plt.plot(money, overhead, label= label)
	labels.append(label)
plt.legend(labels)
plt.show()
