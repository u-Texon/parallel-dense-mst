import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/boruvka.csv')
boruvkaThread = pd.read_csv('../out/files/boruvka-thread.csv')

procSize = 12

nums = np.arange(procSize)
procs = 2 ** nums

b_times = list(boruvka['run time'])
bt_times = list(boruvkaThread['run time'])

b_procs = list(boruvka['Processors'])
bt_procs = list(boruvkaThread['Processors'])

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
bt_avg = get_average(bt_procs, bt_times)

fig, x = plt.subplots(1)
fig.set_figheight(8)
fig.set_figwidth(10)

x.plot(nums, b_avg, label='boruvka-allreduce', color="blue")
x.plot(nums, bt_avg, label='boruvka with message overlap', color="red")
x.set_ylabel('execution time [milliseconds]')

plotName = '../out/plots/weak-scale-boruvkaThread.svg'
title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Edges per PE: " + str(
    p) + ", Weights: [" + str(minWeight) + "," + str(maxWeight) + "]" + " base case is " + baseCase

if p == 0:
    plotName = '../out/plots/strong-scale-boruvkaThread.svg'
    title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", log(m): " + str(
        edgeCount) + ", Weights: [" + str(minWeight) + "," + str(maxWeight) + "]" + " base case is " + baseCase

x.set_title(title)
x.set_xlabel('Number of Processors')
x.legend()
x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)
x.set_ylim(bottom=0)

plt.savefig(plotName)
plt.show()
