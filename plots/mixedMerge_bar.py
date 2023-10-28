import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({'font.size': 15})
boruvka = pd.read_csv('../out/files/only-mixedMerge-boruvka-proc2048-iter2.csv')

mm_initVariables = np.array(list(boruvka['init variables'])) /1000
mm_initialMST = list(boruvka['initial local MST'])[0] /1000

b_localMST = np.array(list(boruvka[' calculate local MST'])) /1000
b_iterations = np.array(list(boruvka['iteration'])) /1000
b_allreduce = np.array(list(boruvka['allreduce'])) /1000
b_parallelEdges = np.array(list(boruvka['remove parallel edges'])) /1000
b_shrink = np.array(list(boruvka['shrink'])) /1000
b_calcIncident = np.array(list(boruvka['calc-incident'])) /1000
b_parentArray = np.array(list(boruvka['parentArray'])) /1000
b_relabel = np.array(list(boruvka['relabel'])) /1000

merge = pd.read_csv('../out/files/only-mixedMerge-merge-proc2048-iter2.csv')

m_localMST = np.array((merge['calculate local MST'])) /1000
m_iterations = np.array(list(merge['iteration'])) /1000
m_sendRecv = np.array(list(merge['send/receive MST'])) /1000

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

p.bar(0, mm_initialMST, color='#f7930f') #yellow
#p.bar(0, mm_initVariables, bottom=mm_initialMST, color="black")


b_bottom = b_localMST + b_calcIncident + b_allreduce + b_parentArray + b_relabel + b_parallelEdges

p.bar(b_range, b_shrink, bottom=b_bottom, color="#02a131") #green

b_bottom -= b_calcIncident
p.bar(b_range, b_calcIncident, bottom=b_bottom, color="purple")
b_bottom -= b_allreduce
p.bar(b_range, b_allreduce, bottom=b_bottom, color="#de1f1f") #red
b_bottom -= b_parentArray
p.bar(b_range, b_parentArray, bottom=b_bottom, color='#02008f') #blue
b_bottom -= b_relabel
p.bar(b_range, b_relabel, bottom=b_bottom, color="#02cfc8") #hellblau
b_bottom -= b_parallelEdges
p.bar(b_range, b_parallelEdges, bottom=b_bottom, color="gray")
b_bottom -= b_localMST
p.bar(b_range, b_localMST, bottom=b_bottom, color='#f7930f') #yellow
#b_bottom -= b_rest
# p.bar(b_range, b_rest, color="green")




p.bar(m_range, m_localMST, bottom=m_sendRecv, color="#f7930f")
p.bar(m_range, m_sendRecv, color="#de1f1f")
# p.bar(m_range, m_rest,  color="green")



labels = ["Lokaler MST"]
stepCount = 1

for i in range(1, 2 * size + 1):
    if i % 2 == 1:
        labels.append("Boruvka-Step " + str(stepCount))
    else:
        labels.append("Merge-Step " + str(stepCount))
        stepCount += 1


p.set_xticks(range(2 * size + 1))
p.set_xticklabels(labels, fontsize=10)
p.set_ylim(bottom=0)
p.set_xticks(p.get_xticks(), p.get_xticklabels(), rotation=45, ha='right')

p.legend(["Lokalen MST Berechnen", "Arrays verkleinern", "Inzidente Kanten berechnen",
          "Allreduce bzw. MST Senden/Empfangen", "Parent Array berechnen", "Graph kontrahieren"])


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
    p) + ",\nWeights: [" + str(minWeight) + "," + str(maxWeight) + "]" + " base case is " + baseCase

plt.title("Boruvka-Mixed-Merge")


plt.savefig('../out/plots/mixedMerge_bar.svg', bbox_inches='tight')
