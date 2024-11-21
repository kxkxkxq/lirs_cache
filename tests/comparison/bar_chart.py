import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt('results.txt')

belady_hits = data[:, 0]
lirs_hits   = data[:, 1]
nrequests   = data[:, 2]
cache_size  = data[:, 3]

i = np.arange(len(belady_hits))

bar_width = 0.25
plt.figure(figsize = (10, 6))

belady = plt.bar(i - (bar_width / 2), belady_hits, width = bar_width, label = 'Belady')
lirs = plt.bar(i + (bar_width / 2), lirs_hits, width = bar_width, label = 'LIRS')

plt.grid(True, which = 'both', linestyle = '--', linewidth = 0.50)

for bar in belady:
    y = bar.get_height()
    plt.text(bar.get_x() + (bar.get_width() / 2), y + 0.5, int(y), ha = 'center', va = 'bottom', fontsize = 9)

for bar in lirs:
    y = bar.get_height()
    plt.text(bar.get_x() + (bar.get_width() / 2), y + 0.5, int(y), ha = 'center', va = 'bottom', fontsize = 9)

n  = nrequests[0]
c = cache_size[0]

plt.xlabel('Test number')
plt.ylabel('Hits')
plt.title(f'Comparison of Belady and LIRS cache algorithms (for {int(n)} requests with cache size {int(c)})')

plt.savefig('bar_chart.png')

plt.xticks(i)
plt.legend(loc = 'lower right')
plt.show()