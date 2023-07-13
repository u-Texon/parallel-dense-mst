import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

merge = pd.read_csv('../out/files/only-merge.csv')

initialMST = np.array(list(merge['calculate initial MST'])[0])
localMST = np.array(list(merge['calculate local MST']))

iterations = np.array(list(merge['iteration']))
sendRecv = np.array(list(merge['send/receive MST']))

rest = []

size = len(iterations)
for i in range(size):
    rest.append(iterations[i] - sendRecv[i] - localMST[i])

plt.bar(0, initialMST, color="brown")
plt.bar(range(1, size + 1), localMST, color="blue")
plt.bar(range(1, size + 1), sendRecv, bottom=localMST, color="red")
plt.bar(range(1, size + 1), rest, bottom=sendRecv + localMST, color="green")

plt.legend(["calculate local MST", "send/receive MST", "rest"])

plt.xlabel("Merge Step")
plt.ylabel("Total Run Time [microseconds]")

plt.savefig('../out/plots/merge_bar.svg')
plt.show()
