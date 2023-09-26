import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

merge = pd.read_csv('../out/files/only-merge-proc2048-iter2.csv')

initialMST = np.array(list(merge['calculate initial MST'])[0])
localMST = np.array(list(merge['calculate local MST']))

iterations = np.array(list(merge['iteration']))
sendRecv = np.array(list(merge['send/receive MST']))

rest = []

labels = ["initial local MST"]

size = len(iterations)
for i in range(size):
    labels.append("Merge-Step " + str(i + 1))
    rest.append(iterations[i] - sendRecv[i] - localMST[i])

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)


p.bar(0, initialMST, color="orange")
p.bar(range(1, size + 1), localMST, color="blue")
p.bar(range(1, size + 1), sendRecv, bottom=localMST, color="red")
p.bar(range(1, size + 1), rest, bottom=sendRecv + localMST, color="green")

p.legend(["calculate initial local MST", "calculate local MST", "send/receive MST", "rest"])

p.set_ylabel("Total Run Time [microseconds]")
p.set_xticks(range(size + 1))
p.set_xticklabels(labels)
p.set_xticks(p.get_xticks(), p.get_xticklabels(), rotation=45, ha='right')
p.set_ylim(bottom=0)


numVertices = merge['log(n)'][0]
minWeight = merge['minimum weight'][0]
maxWeight = merge['maximum weight'][0]
graph = merge['graph-type'][0]
baseCase = merge['kruskal as base case'][0]
shuffled = merge['edges are shuffled'][0]
p = merge['edges per processor'][0]
edgeCount = merge['log(m)'][0]

if shuffled == 1:
    graph = graph + " shuffled"
if baseCase == 0:
    baseCase = "filter-kruskal"
else:
    baseCase = "kruskal"

title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Edges per PE: " + str(
        p) + ", Weights: [" + str(minWeight) + "," + str(maxWeight) + "]\n" + " base case is " + baseCase

plt.title(title)


plt.savefig('../out/plots/merge_bar.svg')
