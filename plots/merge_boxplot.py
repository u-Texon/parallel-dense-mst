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
fig.set_figheight(8)
fig.set_figwidth(10)


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
            edgesProc.append(numEdges[j] / 1000)
    edges.append(edgesProc)

p.boxplot(edges)
p.set_title('Edges per Merge-Step')
p.set_ylabel("Edges * 1000")
p.set_xticklabels(labels)
p.set_xticks(p.get_xticks(), p.get_xticklabels(), rotation=45, ha='right')

plt.savefig('../out/plots/merge_box.svg')
plt.show()
