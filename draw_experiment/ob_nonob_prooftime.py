import numpy as np 
import matplotlib.pyplot as plt 

threshold = [9, 10, 11, 12]

money = np.arange(13,26,2)

# plt.xlabel("Total reward")
# plt.ylabel("Extra notes ratio")
# # plt.xlim(min(money),max(money))
# plt.xticks(money,money)
# labels = []
# for l in threshold:
# 	label = "threshold = "+ str(l)
# 	overhead = money - l + 1
# 	plt.plot(money, overhead, label= label)
# 	labels.append(label)
# plt.legend(labels)
# plt.show()


money = np.arange(13,26,2)
times = np.array([
np.array([10]*7),
np.linspace(20,40,7),
	])
labels = ["non oblivious", "oblivious"]

plt.ylim(0,max(times[1])+5)

plt.ylabel("Cost time")
plt.xlabel("Total reward for workers")
for label, time in zip(labels, times):
	plt.plot(money, time, label = label)
plt.legend(labels)
plt.show()
