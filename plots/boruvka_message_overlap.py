import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({'font.size': 15})
boruvka = pd.read_csv('../out/files/boruvka.csv')
boruvkaThread = pd.read_csv('../out/files/boruvka-thread.csv')

procSize = 11

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
localMSTcount = boruvka['localMSTcount'][0]
threadCount = boruvkaThread['boruvkaThread'][0]

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

x.plot(nums, b_avg, label='Boruvka-Allreduce', color='#02008f', marker="o") #blue
x.plot(nums, bt_avg, label='Boruvka-Allreduce mit Nachrichtenüberlagerung', color="#de1f1f", marker="s") #red
x.set_ylabel('Laufzeit [Millisekunden]')

plotName = '../out/plots/weak-scale-boruvkaThread.svg'

title = "Graph: " + str(graph) + ", log(n): " + str(numVertices)

if p == 0:
    plotName = '../out/plots/strong-scale-boruvkaThread.svg'
    title += ", log(m): " + str(edgeCount)
else:
    title += ", Edges per PE: " + str(p)

title += ", Weights: [" + str(minWeight) + "," + str(
    maxWeight) + "]\n" + " base case is " + baseCase + ". localMST=" + str(
    localMSTcount) + ", boruvkaThreadCount=" + str(threadCount)

x.set_title("GNM Graph mit $n=2^{" + str(int(numVertices)) + "}$ und  $m/p=2^{" + str(int(p)) + "}$")
x.set_xlabel('Anzahl an MPI Prozessen')
x.legend()
x.xaxis.set_ticks(nums)
x.xaxis.set_ticklabels(procs)
x.set_ylim(bottom=0)

plt.grid()
plt.savefig(plotName,bbox_inches='tight')

