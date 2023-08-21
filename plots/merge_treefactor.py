import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

merge = pd.read_csv('../out/files/merge.csv')

procSize = 11

nums = np.arange(procSize)
procs = 2 ** nums

m_times = list(merge['run time'])
m_procs = list(merge['Processors'])
m_tree = list(merge['tree-factor'])


def get_average_runtimes(tree_factor):
    average = np.zeros(procSize)
    count = np.zeros(procSize)

    for p in range(procSize):
        for i in range(len(m_times)):
            if m_tree[i] == tree_factor and m_procs[i] == procs[p]:
                average[p] += m_times[i]
                count[p] += 1

    for i in range(len(average)):
        if count[i] != 0:
            average[i] = average[i] / count[i]
            average[i] = average[i] / 1000
    return average


m2 = get_average_runtimes(2)
m3 = get_average_runtimes(3)
m4 = get_average_runtimes(8)
m5 = get_average_runtimes(16)
m6 = get_average_runtimes(30)

fig = plt.figure()
x = fig.add_subplot()

x.plot(nums, m2, label='d = 2', color="blue")
x.plot(nums, m3, label='d = 3', color="green")
x.plot(nums, m4, label='d = 8', color="brown")
x.plot(nums, m5, label='d = 16', color="red")
x.plot(nums, m6, label='d = 30', color="orange")

plt.title('mergeMST with different tree-factor d (2²² Edges per PE)')
plt.xlabel('Number of Processors')
plt.ylabel('execution time [milliseconds]')
plt.legend()

x.set_ylim(bottom=0)

x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)

plt.savefig('../out/plots/mergeMST.svg')
plt.show()