import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


#kruskal = pd.read_csv('../out/files/kruskal.csv')
#filter = pd.read_csv('../out/files/filter.csv')
boruvka = pd.read_csv('../out/files/boruvka.csv')
boruvkaMerge = pd.read_csv('../out/files/boruvkaMerge.csv')
merge = pd.read_csv('../out/files/merge.csv')
mixedMerge = pd.read_csv('../out/files/mixedMerge.csv')

procSize = 11


nums = np.arange(procSize)
procs = 2 ** nums


#k_times = list(kruskal['run time'])
#f_times = list(filter['run time'])
b_times = list(boruvka['run time'])
m_times = list(merge['run time'])
mm_times = list(mixedMerge['run time'])
bm_times = list(boruvkaMerge['run time'])


#k_procs = list(kruskal['Processors'])
#f_procs = list(filter['Processors'])
b_procs = list(boruvka['Processors'])
m_procs = list(merge['Processors'])
mm_procs = list(mixedMerge['Processors'])
bm_procs = list(boruvkaMerge['Processors'])


def get_average(algo_procs, algo_time):
    average = np.zeros(procSize)
    count = np.zeros(procSize)

    for i in range(len(algo_procs)):
        for j in range(len(procs)):
            if algo_procs[i] == procs[j]:
                average[j] += algo_time[i]
                count[j] += 1

    for i in range(len(average)):
        if count[i] != 0:
            average[i] = average[i] / count[i]
            average[i] = average[i] / 1000
    return average



#k_avg = get_average(k_procs, k_times)
#f_avg = get_average(f_procs, f_times)
b_avg = get_average(b_procs, b_times)
m_avg = get_average(m_procs, m_times)
mm_avg = get_average(mm_procs, mm_times)
bm_avg = get_average(bm_procs, bm_times)

fig = plt.figure()
x = fig.add_subplot()


#x.plot(nums, k_avg, label='kruskal', color="gray")
#x.plot(nums, f_avg, label='filter-kruskal', color="brown")
x.plot(nums, b_avg, label='denseBoruvka', color="blue")
x.plot(nums, m_avg, label='merge-mst', color="red")
x.plot(nums, mm_avg, label='boruvka-mixed-merge', color="green")
x.plot(nums, bm_avg, label='boruvka-then-merge', color="orange")

plt.title('weak scale MST algorithms')
plt.xlabel('Number of Processors')
plt.ylabel('execution time [milliseconds]')
plt.legend()

x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)

plt.savefig('../out/plots/weak-scale-parallel.svg')
plt.show()
