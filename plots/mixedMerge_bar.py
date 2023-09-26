import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/only-mixedMerge-boruvka-proc2048-iter2.csv')

mm_initVariables = np.array(list(boruvka['init variables']))
mm_initialMST = list(boruvka['initial local MST'])[0]

b_localMST = np.array(list(boruvka[' calculate local MST']))
b_iterations = np.array(list(boruvka['iteration']))
b_allreduce = np.array(list(boruvka['allreduce']))
b_parallelEdges = np.array(list(boruvka['remove parallel edges']))
b_shrink = np.array(list(boruvka['shrink']))
b_calcIncident = np.array(list(boruvka['calc-incident']))
b_parentArray = np.array(list(boruvka['parentArray']))
b_relabel = np.array(list(boruvka['relabel']))

merge = pd.read_csv('../out/files/only-mixedMerge-merge-proc2048-iter2.csv')

m_localMST = np.array((merge['calculate local MST']))
m_iterations = np.array(list(merge['iteration']))
m_sendRecv = np.array(list(merge['send/receive MST']))

m_rest = []
size = len(b_iterations)
for i in range(size):
    m_rest.append(m_iterations[i] - m_sendRecv[i] - m_localMST[i])

b_rest = []
for i in range(size):
    b_rest.append(
        b_iterations[i] - b_allreduce[i] - b_parallelEdges[i] - b_localMST[i] - b_shrink[i] - b_calcIncident[i] -
        b_parentArray[i] -
        b_relabel[i])

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)

m_range = []
b_range = []
for i in range(1, 2*size + 1):
    if i % 2 == 1:
        b_range.append(i)
    else:
        m_range.append(i)

p.bar(0, mm_initialMST, color="orange")
p.bar(0, mm_initVariables, bottom=mm_initialMST, color="black")


b_bottom = b_localMST + b_calcIncident + b_allreduce + b_parentArray + b_relabel + b_parallelEdges + b_rest

p.bar(b_range, b_shrink, bottom=b_bottom, color="cyan")
b_bottom -= b_localMST
p.bar(b_range, b_localMST, bottom=b_bottom, color="orange")
b_bottom -= b_calcIncident
p.bar(b_range, b_calcIncident, bottom=b_bottom, color="purple")
b_bottom -= b_allreduce
p.bar(b_range, b_allreduce, bottom=b_bottom, color="red")
b_bottom -= b_parentArray
p.bar(b_range, b_parentArray, bottom=b_bottom, color="blue")
b_bottom -= b_relabel
p.bar(b_range, b_relabel, bottom=b_bottom, color="brown")
b_bottom -= b_parallelEdges
p.bar(b_range, b_parallelEdges, bottom=b_bottom, color="gray")
b_bottom -= b_rest
p.bar(b_range, b_rest, color="green")




p.bar(m_range, m_localMST, bottom=m_rest + m_sendRecv, color="orange")
p.bar(m_range, m_sendRecv, bottom=m_rest, color="red")
p.bar(m_range, m_rest,  color="green")



labels = ["Initial MST"]
stepCount = 1

for i in range(1, 2 * size + 1):
    if i % 2 == 1:
        labels.append("Boruvka-Step " + str(stepCount))
    else:
        labels.append("Merge-Step " + str(stepCount))
        stepCount += 1

p.set_xticks(range(2 * size + 1))
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


plt.savefig('../out/plots/mixedMerge_bar.svg')
