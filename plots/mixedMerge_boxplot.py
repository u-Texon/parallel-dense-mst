import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/mixedMerge_boxplot.csv')

iterations = np.array(list(boruvka['Iteration']))
numVertices = np.array(list(boruvka['numVertices']))
numEdges = np.array(list(boruvka['NumEdges']))
procs = np.array(list(boruvka['Processor Number']))

sizeIt = np.max(iterations) + 1
minVertices = np.min(numVertices)

fig, p = plt.subplots(2)
fig.tight_layout(pad=5.0)

vertices = []
edges = []
labels = ["Start", "local MST"]

bCounter = 1
mCounter = 1

minCounter = 0

for i in range(sizeIt):
    edgesProc = []

    for j in range(len(iterations)):
        if iterations[j] == i:
            if procs[j] == 0:
                vertices.append(numVertices[j])
                if i >= 2:
                    if i % 2 == 0:
                        labels.append("Boruvka-Step " + str(bCounter))
                        bCounter += 1
                    else:
                        labels.append("Merge-Step " + str(mCounter))
                        mCounter += 1
            edgesProc.append(numEdges[j])
    edges.append(edgesProc)

p[0].boxplot(edges)
p[0].set_title('Edges per BoruvkaMixedMerge-step')
p[0].set_ylabel("Edges")
p[0].set_xticklabels(labels)

p[1].bar(range(sizeIt), vertices)
p[1].set_title('Vertices per BoruvkaMixedMerge-step')
p[1].set_ylabel("Vertices")
p[1].set_xticks([i for i in range(sizeIt)])
p[1].set_xticklabels(labels)

plt.savefig('../out/plots/mixeMerge_box.svg')
plt.show()
