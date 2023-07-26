import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

kruskal = pd.read_csv('../out/files/kruskal.csv')
filterKruskal = pd.read_csv('../out/files/filter.csv')

k_times = list(kruskal['run time'])
f_times = list(filterKruskal['run time'])

k_procs = list(kruskal['Processors'])
f_procs = list(filterKruskal['Processors'])

f_edges = list(filterKruskal['log(m)'])
k_edges = list(kruskal['log(m)'])

size = 7  # von 17 bis 23


def get_average(algo_Edges, algo_time, numEdges):
    average = 0
    count = 0

    for i in range(len(algo_time)):
        if algo_Edges[i] == numEdges:
            average += algo_time[i]
            count += 1

    if count == 0:
        return 0
    return average / count


k_avg = []
f_avg = []
labels = []
for i in range(17, 24):
    k_avg.append(get_average(k_edges, k_times, i))
    f_avg.append(get_average(f_edges, f_times, i))
    labels.append("2^" + str(i))

fig, p = plt.subplots(1)
fig.tight_layout(pad=5.0)

print(k_avg)



p.plot(k_avg, label='kruskal', color="green")
p.plot(f_avg, label='filter-kruskal', color="blue")
p.set_xticks(range(size ))
p.set_xticklabels(labels)
p.set_ylim(bottom=0)

plt.title('Compatison of sequential algorithms')
plt.xlabel('Number of Edges')
plt.ylabel('execution time [milliseconds]')
plt.legend()

plt.savefig('../out/plots/Sequential.svg')
plt.show()
