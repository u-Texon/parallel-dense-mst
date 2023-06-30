import pandas as pd
import matplotlib.pyplot as plt

boruvka = pd.read_csv('../out/files/only-boruvka.csv')

init = list(boruvka['init variables'])[0]
localMSTtime = list(boruvka['calculate local MST'])[0]
iterations = list(boruvka['iteration'])
allreduce = list(boruvka['allreduce'])
parallelEdges = list(boruvka['remove parallel edges'])

labels = ['initialize variables', 'calculate local MST']
values = [init, localMSTtime]

size = len(iterations)

for i in range(size):
    labels.append('iteration ' + str(i))
    values.append(iterations[i])
    labels.append('allreduce ' + str(i))
    values.append(allreduce[i])
    labels.append('remove parallel edges ' + str(i))
    values.append(parallelEdges[i])

fig, ax = plt.subplots()
ax.pie(values, autopct='%1.1f%%')

plt.legend(labels, bbox_to_anchor=(1.05, 1.0), loc='upper left')

plt.savefig('../out/plots/boruvka.svg')
plt.show()
