import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/boruvka_boxplot.csv')

iterations = np.array(list(boruvka['Iteration']))
numVertices = np.array(list(boruvka['numVertices']))
numEdges = np.array(list(boruvka['NumEdges']))
procs = np.array(list(boruvka['Processor Number']))

sizeIt = np.max(iterations) + 1

fig, p = plt.subplots(2)
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)

vertices = []
edges = []
labels = []

for i in range(sizeIt):
    if i == 0:
        labels.append("Start")
    else:
        labels.append("Boruvka-Step " + str(i))

    edgesProc = []
    for j in range(len(iterations)):
        if iterations[j] == i:
            if procs[j] == 0:
                vertices.append(numVertices[j])
            edgesProc.append(numEdges[j])
    edges.append(edgesProc)

p[0].boxplot(edges)
p[0].set_title('Edges per Boruvkastep')
p[0].set_ylabel("Edges")
p[0].set_xticklabels(labels)

p[1].bar(range(sizeIt), vertices)
p[1].set_title('Vertices per Boruvkastep')
p[1].set_ylabel("Vertices")
p[1].set_xticks([i for i in range(sizeIt)])
p[1].set_xticklabels(labels)


plt.savefig('../out/plots/boruvka_box.svg')
plt.show()
