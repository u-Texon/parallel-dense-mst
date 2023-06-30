import pandas as pd
import matplotlib.pyplot as plt

boruvka = pd.read_csv('../out/files/boruvka.csv')
kruskal = pd.read_csv('../out/files/kruskal.csv')
filter = pd.read_csv('../out/files/filter.csv')
boruvkaMerge = pd.read_csv('../out/files/boruvkaMerge.csv')
merge = pd.read_csv('../out/files/merge.csv')
mixedMerge = pd.read_csv('../out/files/mixedMerge.csv')

procs = [1, 2, 4, 8, 16, 32, 64]

b_times = list(boruvka['run time'])
k_times = list(kruskal['run time'])
f_times = list(filter['run time'])
m_times = list(merge['run time'])
mm_times = list(mixedMerge['run time'])
bm_times = list(boruvkaMerge['run time'])

b_procs = list(boruvka['Processors'])
k_procs = list(kruskal['Processors'])
f_procs = list(filter['Processors'])
m_procs = list(merge['Processors'])
mm_procs = list(mixedMerge['Processors'])
bm_procs = list(boruvkaMerge['Processors'])


def get_average(algo_procs, algo_time):
    average = [0, 0, 0, 0, 0, 0, 0]
    count = [0, 0, 0, 0, 0, 0, 0]

    for i in range(len(algo_procs)):
        for j in range(len(procs)):
            if algo_procs[i] == procs[j]:
                average[j] += algo_time[i]
                count[j] += 1

    for i in range(len(average)):
        if count[i] != 0:
            average[i] = average[i] / count[i]

    return average


b_avg = get_average(b_procs, b_times)
k_avg = get_average(k_procs, k_times)
f_avg = get_average(f_procs, f_times)
m_avg = get_average(m_procs, m_times)
mm_avg = get_average(mm_procs, mm_times)
bm_avg = get_average(bm_procs, bm_times)

plt.plot(procs, b_avg, label='denseBoruvka')
plt.plot(procs, k_avg, label='kruskal')
plt.plot(procs, f_avg, label='filter-kruskal')
plt.plot(procs, m_avg, label='merge-mst')
plt.plot(procs, mm_avg, label='boruvka-mixed-merge')
plt.plot(procs, bm_avg, label='boruvka-then-merge')

plt.title('Run times of MST algorithms')
plt.xlabel('Number of Processors')
plt.ylabel('execution time (in microseconds??)')
plt.legend()

plt.savefig('../out/plots/parallelRunTimes.svg')
plt.show()
