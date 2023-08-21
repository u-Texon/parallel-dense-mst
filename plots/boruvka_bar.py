import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/only-boruvka.csv')

localMST = np.array(list(boruvka['calculate local MST']))
iterations = np.array(list(boruvka['iteration']))
allreduce = np.array(list(boruvka['allreduce']))
parallelEdges = np.array(list(boruvka['remove parallel edges']))
shrink = np.array(list(boruvka['shrink']))
calcIncident = np.array(list(boruvka['calc-incident']))
parentArray = np.array(list(boruvka['parentArray']))
relabel = np.array(list(boruvka['relabel']))

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

p.bar(range(size), shrink, color="cyan")
p.bar(range(size), localMST, bottom=shrink, color="orange")
p.bar(range(size), calcIncident, bottom=shrink + localMST, color="purple")
p.bar(range(size), allreduce, bottom=localMST + shrink + calcIncident, color="red")
p.bar(range(size), parentArray, bottom=shrink + localMST + calcIncident + allreduce, color="blue")
p.bar(range(size), relabel, bottom=parentArray + shrink + localMST + calcIncident + allreduce, color="brown")
p.bar(range(size), parallelEdges, bottom=parentArray + shrink + localMST + calcIncident + allreduce + relabel,
      color="gray")
p.bar(range(size), rest, bottom=parallelEdges + parentArray + shrink + localMST + calcIncident + allreduce + relabel,
      color="green")

p.legend(
    ["shrink old arrays", "calculate local MST", "calculate incident edges", "allreduce", "calculate parent array",
     "relabel", "remove parallel edges", "rest"])


p.set_xticks(range(size))
p.set_xticklabels(range(1,size +1))
p.set_xlabel("Boruvka Round")
p.set_ylabel("Total Run Time [microseconds]")
p.set_ylim(bottom=0)

plt.savefig('../out/plots/boruvka_bar.svg')
plt.show()
