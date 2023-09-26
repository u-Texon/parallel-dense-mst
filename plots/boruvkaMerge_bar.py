import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/only-boruvkaMerge-boruvka-proc2048-iter2.csv')

initVariables = np.array(list(boruvka['init variables']))
initialMST = list(boruvka['initial local MST'])[0]

localMST = np.array(list(boruvka[' calculate local MST']))
iterations = np.array(list(boruvka['iteration']))
allreduce = np.array(list(boruvka['allreduce']))
parallelEdges = np.array(list(boruvka['remove parallel edges']))
shrink = np.array(list(boruvka['shrink']))
calcIncident = np.array(list(boruvka['calc-incident']))
parentArray = np.array(list(boruvka['parentArray']))
relabel = np.array(list(boruvka['relabel']))

rest = []

b_size = len(iterations)
for i in range(b_size):
    rest.append(
        iterations[i] - allreduce[i] - parallelEdges[i] - localMST[i] - shrink[i] - calcIncident[i] - parentArray[i] -
        relabel[i])

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)

p.bar(0, initialMST, color="orange")
p.bar(0, initVariables, bottom=initialMST)
p.bar(range(1, b_size + 1), shrink, color="cyan")
p.bar(range(1, b_size + 1), localMST, bottom=shrink, color="orange")
p.bar(range(1, b_size + 1), calcIncident, bottom=shrink + localMST, color="purple")
p.bar(range(1, b_size + 1), allreduce, bottom=localMST + shrink + calcIncident, color="red")
p.bar(range(1, b_size + 1), parentArray, bottom=shrink + localMST + calcIncident + allreduce, color="blue")
p.bar(range(1, b_size + 1), relabel, bottom=parentArray + shrink + localMST + calcIncident + allreduce, color="brown")
p.bar(range(1, b_size + 1), parallelEdges, bottom=parentArray + shrink + localMST + calcIncident + allreduce + relabel,
      color="gray")
p.bar(range(1, b_size + 1), rest,
      bottom=parallelEdges + parentArray + shrink + localMST + calcIncident + allreduce + relabel,
      color="green")

merge = pd.read_csv('../out/files/only-boruvkaMerge-merge-proc2048-iter2.csv')

initialMST = np.array(list(merge['calculate initial MST'])[0])
localMST = np.array(list(merge['calculate local MST']))
iterations = np.array(list(merge['iteration']))
sendRecv = np.array(list(merge['send/receive MST']))

m_size = len(iterations)

rest = []

size = len(iterations)
for i in range(size):
    rest.append(iterations[i] - sendRecv[i] - localMST[i])

p.bar(b_size + 1, initialMST, color="orange")
p.bar(range(b_size + 2, b_size + m_size + 2), localMST, color="blue")
p.bar(range(b_size + 2, b_size + m_size + 2), sendRecv, bottom=localMST, color="red")
p.bar(range(b_size + 2, b_size + m_size + 2), rest, bottom=sendRecv + localMST, color="green")

labels = []
count = 1
for i in range(b_size + m_size + 2):
    if i == 0:
        labels.append("Initial MST (Boruvka)")
    elif i < b_size + 1:
        labels.append("Boruvka-Step " + str(i))
    elif i == b_size + 1:
        labels.append("Initial MST (Merge)")
    else:
        labels.append("Merge-Step " + str(count))
        count += 1

p.set_xticks(range(b_size + m_size + 2))
p.set_xticklabels(labels)
p.set_ylim(bottom=0)
p.set_xticks(p.get_xticks(), p.get_xticklabels(), rotation=45, ha='right')

p.legend(["initial MST", "initialize variables", "shrink arrays", "localMST", "calculate incident edges",
          "allreduce or Send/Recv", "calculate parent array", "relabel", "remove parallel edges", "rest"])

numVertices = boruvka['log(n)'][0]
minWeight = boruvka['minimum weight'][0]
maxWeight = boruvka['maximum weight'][0]
graph = boruvka['graph-type'][0]
baseCase = boruvka['kruskal as base case'][0]
shuffled = boruvka['edges are shuffled'][0]
p = boruvka['edges per processor'][0]
edgeCount = boruvka['log(m)'][0]

if shuffled == 1:
    graph = graph + " shuffled"
if baseCase == 0:
    baseCase = "filter-kruskal"
else:
    baseCase = "kruskal"

title = "Graph: " + str(graph) + ", log(n): " + str(numVertices) + ", Edges per PE: " + str(
    p) + ", Weights: [" + str(minWeight) + "," + str(maxWeight) + "]\n" + " base case is " + baseCase

plt.title(title)

plt.savefig('../out/plots/boruvkaMerge_bar.svg')
