import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({'font.size': 15})
merge = pd.read_csv('../out/files/merge.csv')

procSize = 12

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
m3 = get_average_runtimes(4)
m4 = get_average_runtimes(8)
m5 = get_average_runtimes(16)
m6 = get_average_runtimes(3)

fig = plt.figure()
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)


x = fig.add_subplot()

x.plot(nums, m2, label='D = 2', color='#02008f', marker="o") #blue
x.plot(nums, m6, label='D = 3', color="black", marker="x")
x.plot(nums, m3, label='D = 4', color="#02a131", marker="*") #green
x.plot(nums, m4, label='D = 8', color='#f7930f', marker="D") #yellow
x.plot(nums, m5, label='D = 16', color="#de1f1f", marker="s") #red

plt.title('RHG Graph mit $n=2^{18}$ und $m/p=2^{20}$')
plt.xlabel('Anzahl an PEs')
plt.ylabel('Laufzeit [Millisekunden]')
plt.legend()

x.set_ylim(bottom=0, top=850)

x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)
plt.grid()

plt.savefig('../out/plots/mergeMST.svg',bbox_inches='tight')
