# deploy = 5291461
# 2^16  l = 2^12
import matplotlib.pyplot as plt
# request = 283606
# register_worker = 193906
# register_sp = 93039
# upload_solution = 192146
# Discover_truth1 = 108855.58
# Discover_truth2 = 354287.102
# claim = 27126
# extra note ratio: 2

#  saving 496 -> 376
x = ['sol.', 'reg.\n(worker)', 'reg.\n(sp)', 'sub.','aggr.\n(No VER)','aggr.','claim']

h = [284.038, 234.862, 93.267, 212.544, 376.479, 496.323, 27.126]

plt.ylabel('gas cost(K)')
plt.bar(x, h);
plt.show()