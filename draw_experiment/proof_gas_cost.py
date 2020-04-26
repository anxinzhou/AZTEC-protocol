# deploy = 5291461
# 2^16  l = 2^12
import matplotlib.pyplot as plt
import numpy as np
# request = 283606
# register_worker = 193906
# register_sp = 93039
# upload_solution = 192146
# Discover_truth1 = 108855.58
# Discover_truth2 = 354287.102
# claim = 27126
# extra note ratio: 2

#  saving 496 -> 376
labels = ['aggr.(No VER),ENR=1','aggr.(No VER),ENR=2','aggr.(No VER),ENR=3'

,"aggr.,ENR=1","aggr.,ENR=2","aggr.,ENR=3"]
marker = ["o","x","^","*","s","v"]

x = [100, 200, 300, 400, 500]

# 百万
cost = np.array(
	[
[1.9015990,3.7818590,5.6621318,7.5423918,9.4226518], # aggr.(ENR=1)(No VER)
[3.7818590,7.5423918,11.3029118,15.0634318,18.8239518], # 'aggr.(ENR=2)\n(No VER)'
[5.6621318,11.3029118,16.9436918,22.5844718,28.2252518],
[5.0117576,9.9627819,14.9138445,19.8650647,24.8161509],
[9.9627819,19.8650647,29.7641333,39.6696114,49.5720600],
[14.9138445,29.7641333,44.6206406,59.4740355,74.3274555],	# "aggr(ENR=3)"

	]
	)

for l,c,m in zip(labels, cost,marker):
	plt.plot(x,c,label = l,marker = m)
plt.legend(labels)
plt.ylabel('gas cost(M)')
plt.xlabel('number of workers')
plt.show()