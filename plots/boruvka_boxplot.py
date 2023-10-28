import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({'font.size': 15})
boruvka = pd.read_csv('../out/files/boruvka_boxplot-2048.csv')

iterations = np.array(list(boruvka['Iteration']))
numVertices = np.array(list(boruvka['numVertices']))
numEdges = np.array(list(boruvka['NumEdges']))
procs = np.array(list(boruvka['Processor Number']))

sizeIt = np.max(iterations) + 1




fig, p = plt.subplots(2)
fig.tight_layout(pad=1.0)
fig.set_figheight(8)
fig.set_figwidth(10)




vertices = []
edges = []
labels = []

for i in range(sizeIt):
    if i == 0:
        labels.append("Start")
    else:
        labels.append("BoruvkaStep " + str(i))

    edgesProc = []
    for j in range(len(iterations)):
        if iterations[j] == i:
            if procs[j] == 0:
                vertices.append(numVertices[j])
            edgesProc.append(numEdges[j])
    edges.append(edgesProc)

p[0].boxplot(edges)
p[0].set_title('Kanten pro BoruvkaStep')
p[0].set_ylabel("Kanten")
p[0].set_xticklabels(labels)
p[0].set_ylim(bottom=40000, top=17000000)
p[0].set_yscale('log', base=2)

p[1].bar(range(sizeIt), vertices)
p[1].set_title('Knoten pro BoruvkaStep')
p[1].set_ylabel("Knoten")
p[1].set_xticks([i for i in range(sizeIt)])
p[1].set_xticklabels(labels)
p[1].bar_label(p[1].containers[0])
plt.yscale('log', base=2)
p[1].set_ylim(top=2500000)

#p[1].set_xticks(p[1].get_xticks(), p[1].get_xticklabels(), rotation=45, ha='right')
#p[0].set_xticks(p[0].get_xticks(), p[0].get_xticklabels(), rotation=45, ha='right')



plt.savefig('../out/plots/boruvka_box.svg',bbox_inches='tight')
