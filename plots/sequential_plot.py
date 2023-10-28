import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({'font.size': 15})
kruskal = pd.read_csv('../out/files/kruskal.csv')
filterKruskal = pd.read_csv('../out/files/filter.csv')

k_times = list(kruskal['run time'])
f_times = list(filterKruskal['run time'])

k_procs = list(kruskal['Processors'])
f_procs = list(filterKruskal['Processors'])

f_edges = list(filterKruskal['log(m)'])
k_edges = list(kruskal['log(m)'])

minEdges = min(kruskal['log(m)'])
maxEdges = max(kruskal['log(m)'])
size = maxEdges - minEdges + 1

numVertices = kruskal['log(n)'][0]
minWeight = kruskal['minimum weight'][0]
maxWeight = kruskal['maximum weight'][0]
graph = kruskal['graph-type'][0]


def get_average(algo_Edges, algo_time, numEdges):
    average = 0
    count = 0

    for i in range(len(algo_time)):
        if algo_Edges[i] == numEdges:
            average += algo_time[i]
            count += 1

    if count == 0:
        return 0
    return average / count


k_avg = []
f_avg = []
labels = []
for i in range(minEdges, maxEdges + 1):
    k_avg.append(get_average(k_edges, k_times, i) / 1000)
    f_avg.append(get_average(f_edges, f_times, i) / 1000)
    labels.append("2^" + str(i))

fig, p = plt.subplots(1)
fig.tight_layout(pad=1.0)
fig.set_figheight(8)
fig.set_figwidth(10)

#p[0].plot(k_avg, label='kruskal', color="green")
#p[0].plot(f_avg, label='filter-kruskal', color="blue")
#p[0].set_xticks(range(size))
#p[0].set_xticklabels(labels)
#p[0].set_ylim(bottom=0)

title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Weights: [" + str(minWeight) + "," + str(
    maxWeight) + "]"
#p[0].set_title(title)
#p[0].set_xlabel('Number of Edges')
#p[0].set_ylabel('execution time \n [milliseconds]')
#p[0].legend()

k_avg = []
f_avg = []
labels = []
for i in range(minEdges, maxEdges + 1):
    k_avg.append(get_average(k_edges, k_times, i) / pow(2, i))
    f_avg.append(get_average(f_edges, f_times, i) / pow(2, i))
    labels.append(str((pow(2, i) / pow(2, numVertices))))




p.plot(k_avg, label='Kruskal', color="#de1f1f", marker="s") #red
p.plot(f_avg, label='Filter-Kruskal', color='#02008f', marker="o") #blue
p.set_xticks(range(size))
p.set_xticklabels(labels)
p.set_ylim(bottom=0)
plt.grid()

title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Weights: [" + str(minWeight) + "," + str(
    maxWeight) + "]"
p.set_title("GNM Graph mit $2^{"  + str(numVertices) + "}$ Knoten")
p.set_ylabel('Zeit pro Kante \n [Millisekunden]')
p.set_xlabel('Knotengrad ')
p.legend()

plt.savefig('../out/plots/Sequential.svg',bbox_inches='tight')
