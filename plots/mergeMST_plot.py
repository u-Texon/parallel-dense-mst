import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

merge = pd.read_csv('../out/files/merge.csv')

procSize = 9

nums = np.arange(9)
procs = 2 ** nums

m_times = list(merge['run time'])
m_procs = list(merge['Processors'])
m_tree = list(merge['tree-factor'])


def get_runtimes(tree_factor):
    times = []
    for p in range(procSize):
        for i in range(len(m_times)):
            if m_tree[i] == tree_factor and m_procs[i] == procs[p]:
                times.append(m_times[i] / 1000)
    return times


m2 = get_runtimes(2)
m3 = get_runtimes(3)
m4 = get_runtimes(4)
m5 = get_runtimes(5)

fig = plt.figure()
x = fig.add_subplot()

x.plot(nums, m2, label='d = 2', color="blue")
x.plot(nums, m3, label='d = 3', color="green")
x.plot(nums, m4, label='d = 4', color="brown")
x.plot(nums, m5, label='d = 5', color="red")

plt.title('mergeMST with different tree-factor d (2¹⁷ Edges per PE)')
plt.xlabel('Number of Processors')
plt.ylabel('execution time [milliseconds]')
plt.legend()

x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)

plt.savefig('../out/plots/mergeMST.svg')
plt.show()
