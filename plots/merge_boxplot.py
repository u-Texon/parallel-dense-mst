import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/merge_boxplot.csv')

iterations = np.array(list(boruvka['Iteration']))
numEdges = np.array(list(boruvka['NumEdges']))
procs = np.array(list(boruvka['Processor Number']))

sizeIt = np.max(iterations) + 1

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)

edges = []
labels = []

for i in range(sizeIt):
    if i == 0:
        labels.append("Start")
    elif i == 1:
        labels.append("Local MST")
    else:
        labels.append("Merge-Step " + str(i - 1))

    edgesProc = []
    for j in range(len(iterations)):
        if iterations[j] == i:
            edgesProc.append(numEdges[j])
    edges.append(edgesProc)

p.boxplot(edges)
p.set_title('Edges per Merge-Step')
p.set_ylabel("Edges")
p.set_xticklabels(labels)

plt.savefig('../out/plots/merge_box.svg')
plt.show()
