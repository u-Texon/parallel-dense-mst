import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 15})
boruvka = pd.read_csv('../out/files/boruvka.csv')
boruvkaMerge = pd.read_csv('../out/files/boruvkaMerge.csv')
merge = pd.read_csv('../out/files/merge.csv')
mixedMerge = pd.read_csv('../out/files/mixedMerge.csv')

procSize = 12

nums = np.arange(procSize)
procs = 2 ** nums

b_times = list(boruvka['run time'])
m_times = list(merge['run time'])
mm_times = list(mixedMerge['run time'])
bm_times = list(boruvkaMerge['run time'])

b_procs = list(boruvka['Processors'])
m_procs = list(merge['Processors'])
mm_procs = list(mixedMerge['Processors'])
bm_procs = list(boruvkaMerge['Processors'])

numVertices = boruvka['log(n)'][0]
minWeight = boruvka['minimum weight'][0]
maxWeight = boruvka['maximum weight'][0]
graph = boruvka['graph-type'][0]
baseCase = boruvka['kruskal as base case'][0]
shuffled = boruvka['edges are shuffled'][0]
p = boruvka['edges per processor'][0]
edgeCount = boruvka['log(m)'][0]

if shuffled == 1:
    graph = graph + " shuffled"
if baseCase == 0:
    baseCase = "filter-kruskal"
else:
    baseCase = "kruskal"


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


b_avg = get_average(b_procs, b_times)
m_avg = get_average(m_procs, m_times)
mm_avg = get_average(mm_procs, mm_times)
bm_avg = get_average(bm_procs, bm_times)

fig, x = plt.subplots(1)
fig.set_figheight(8)
fig.set_figwidth(10)

x.plot(nums, b_avg, label='Boruvka-Allreduce', color='#02008f', marker="o") #blue
x.plot(nums, m_avg, label='Merge-Local-MST', color="#de1f1f", marker="s") #red
x.plot(nums, mm_avg, label='Boruvka-Mixed-Merge', color="#02a131", marker="*") #green
x.plot(nums, bm_avg, label='Boruvka-Then-Merge', color='#f7930f', marker="D") #yellow


plotName = '../out/plots/weak-scale-parallel.svg'
title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Edges per PE: " + str(
    p) + ",\nWeights: [" + str(minWeight) + "," + str(maxWeight) + "]" + " base case is " + baseCase

if p == 0:
    plotName = '../out/plots/strong-scale-parallel.svg'
    title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", log(m): " + str(
        edgeCount) + ", Weights: [" + str(minWeight) + "," + str(maxWeight) + "]" + " base case is " + baseCase

x.set_title("Pair Graph mit $n=2^{" + str(numVertices) + "}$ und $m/p=2^{" + str(p) + "}$ ")
x.set_xlabel('Anzahl an PEs')
x.legend()
x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)
x.set_ylim(bottom=0, top=650)

x.set_ylabel('Laufzeit [Millisekunden]')
#frame1 = plt.gca()
#frame1.axes.yaxis.set_ticklabels([])

plt.grid()

plt.savefig(plotName, bbox_inches='tight')
