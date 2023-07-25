import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/boruvkaMerge_boxplot.csv')

iterations = np.array(list(boruvka['Iteration']))
numVertices = np.array(list(boruvka['numVertices']))
numEdges = np.array(list(boruvka['NumEdges']))
procs = np.array(list(boruvka['Processor Number']))

sizeIt = np.max(iterations) + 1

fig, p = plt.subplots(2)
fig.tight_layout(pad=5.0)

vertices = []
edges = []

for i in range(sizeIt):
    edgesProc = []

    for j in range(len(iterations)):
        if iterations[j] == i:
            if procs[j] == 0:
                vertices.append(numVertices[j])
            edgesProc.append(numVertices[i])
    edges.append(edgesProc)

p[0].boxplot(edges)
p[0].set_title('Edges per BoruvkaThenMerge-step')
p[0].set_ylabel("Edges")
p[0].set_xlabel("Iteration")
p[0].set_xticklabels(range(sizeIt))

p[1].bar(range(sizeIt), vertices)
p[1].set_title('Vertices per BoruvkaThenMerge-step')
p[1].set_xlabel("Iteration")
p[1].set_ylabel("Vertices")
p[1].xaxis.set_ticks(range(sizeIt))


plt.savefig('../out/plots/boruvkaMerge_box.svg')
plt.show()
