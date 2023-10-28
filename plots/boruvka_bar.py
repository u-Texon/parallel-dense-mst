import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 15})

boruvka = pd.read_csv('../out/files/only-boruvka-proc2048-iter3.csv')

localMST = np.array(list(boruvka['calculate local MST'])) / 1000
iterations = np.array(list(boruvka['iteration'])) / 1000
allreduce = np.array(list(boruvka['allreduce'])) / 1000
parallelEdges = np.array(list(boruvka['remove parallel edges'])) / 1000
shrink = np.array(list(boruvka['shrink'])) / 1000
calcIncident = np.array(list(boruvka['calc-incident'])) / 1000
parentArray = np.array(list(boruvka['parentArray'])) / 1000
relabel = np.array(list(boruvka['relabel'])) / 1000

rest = []

size = len(iterations)
for i in range(size):
    rest.append(
        iterations[i] - allreduce[i] - parallelEdges[i] - localMST[i] - shrink[i] - calcIncident[i] - parentArray[i] -
        relabel[i])

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)
fig.set_figheight(8)
fig.set_figwidth(10)

p.bar(range(size), shrink, color="#02a131") #green
p.bar(range(size), localMST, bottom=shrink, color='#f7930f') #yellow
p.bar(range(size), calcIncident, bottom=shrink + localMST, color="purple")
p.bar(range(size), allreduce, bottom=localMST + shrink + calcIncident, color="#de1f1f") #red
p.bar(range(size), parentArray, bottom=shrink + localMST + calcIncident + allreduce, color='#02008f') #blue
p.bar(range(size), relabel, bottom=parentArray + shrink + localMST + calcIncident + allreduce, color="#02cfc8") #hellblau
p.bar(range(size), parallelEdges, bottom=parentArray + shrink + localMST + calcIncident + allreduce + relabel, color="gray")

p.legend(
    ["Arrays verkleinern","Lokalen MST Berechnen", "Inzidente Kanten berechnen", "Allreduce", "Parent Array berechnen",
     "Graph kontrahieren", "Parallele Kanten entfernen"])

labels = []
for i in range(1, size + 1):
    labels.append("BoruvkaStep " + str(i))

p.set_xticks(range(size))
p.set_ylabel("Laufzeit [Millisekunden]")

p.set_xticklabels(labels)
p.set_ylim(bottom=0)
p.set_xticks(p.get_xticks(), p.get_xticklabels(), rotation=45, ha='right')

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

plt.title("Boruvka-Allreduce")
plt.savefig('../out/plots/boruvka_bar.svg', bbox_inches='tight')
