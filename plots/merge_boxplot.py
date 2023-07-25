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

for i in range(sizeIt):
    edgesProc = []
    for j in range(len(iterations)):
        if iterations[j] == i:
            edgesProc.append(numEdges[i])
    edges.append(edgesProc)

p.boxplot(edges)
p.set_title('Edges per Mergestep')
p.set_ylabel("Edges")
p.set_xlabel("Iteration, where Iteration 0 is the initial local MST")
p.set_xticklabels(range(sizeIt))

plt.savefig('../out/plots/merge_box.svg')
plt.show()
