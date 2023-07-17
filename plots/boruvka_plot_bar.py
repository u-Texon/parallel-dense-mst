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
    rest.append(iterations[i] - allreduce[i] - parallelEdges[i] - localMST[i] - shrink[i] - calcIncident[i] - parentArray[i] - relabel[i])


plt.bar(range(size), shrink)
plt.bar(range(size), localMST, bottom=shrink, color="blue")
plt.bar(range(size), calcIncident, bottom=shrink + localMST)
plt.bar(range(size), allreduce, bottom=localMST + shrink+ calcIncident, color="red")
plt.bar(range(size), parentArray, bottom=shrink + localMST + calcIncident + allreduce)
plt.bar(range(size), relabel, bottom=parentArray + shrink + localMST + calcIncident + allreduce)

plt.bar(range(size), parallelEdges, bottom=parentArray + shrink + localMST + calcIncident + allreduce + relabel, color="orange")
plt.bar(range(size), rest, bottom=parallelEdges + parentArray + shrink + localMST + calcIncident + allreduce + relabel ,color="green")

plt.legend(["shrink old arrays", "calculate local MST", "calculate incident edges", "allreduce", "calculate parent array", "relabel" ,"remove parallel edges", "rest"])

plt.xlabel("Boruvka Round")
plt.ylabel("Total Run Time [microseconds]")

plt.savefig('../out/plots/boruvka_bar.svg')
plt.show()
