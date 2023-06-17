import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('example.csv')

a = list(df['a'])
b = list(df['b'])


plt.plot(a)
plt.plot(b)


plt.savefig('../plots/example.png')
plt.show()
