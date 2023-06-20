import pandas as pd
import matplotlib.pyplot as plt

boruvka = pd.read_csv('only-boruvka.csv')

init = list(boruvka['init variables'])[0]
runTime = list(boruvka['run time'])[0]
localMSTtime = list(boruvka['calculate local MST'])[0]
iterations = list(boruvka['iteration'])
allreduce = list(boruvka['allreduce'])
parallelEdges = list(boruvka['remove parallel edges'])

data = [['total run time', runTime], ['initialize variables', init], ['calculate local MST', localMSTtime]]

size = len(iterations)

for i in range(size):
    data.append(['iteration ' + str(i), iterations[i]])
    data.append(['allreduce ' + str(i), allreduce[i]])
    data.append(['remove parallel edges ' + str(i), parallelEdges[i]])

df = pd.DataFrame(data, columns=["Name", "run time"])
df.plot(x="Name", y="run time", kind="bar")

plt.savefig('../plots/boruvka.svg')
plt.show()
