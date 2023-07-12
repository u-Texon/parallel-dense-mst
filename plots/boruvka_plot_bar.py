import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

boruvka = pd.read_csv('../out/files/only-boruvka.csv')

localMST = np.array(list(boruvka['calculate local MST']))
iterations = np.array(list(boruvka['iteration']))
allreduce = np.array(list(boruvka['allreduce']))
parallelEdges = np.array(list(boruvka['remove parallel edges']))

rest = []

size = len(iterations)
for i in range(size):
    rest.append(iterations[i] - allreduce[i] - parallelEdges[i] - localMST[i])

plt.bar(range(size), localMST, color="blue")
plt.bar(range(size), allreduce, bottom=localMST, color="red")
plt.bar(range(size), parallelEdges, bottom=allreduce + localMST, color="orange")
plt.bar(range(size), rest, bottom=parallelEdges + allreduce + localMST, color="green")

plt.legend(["calculate local MST", "allreduce", "remove parallel edges", "rest"])

plt.xlabel("Boruvka Round")
plt.ylabel("Total Run Time [microseconds]")

plt.savefig('../out/plots/boruvka_bar.svg')
plt.show()
