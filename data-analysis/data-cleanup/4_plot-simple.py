import pandas as pd
import matplotlib.pyplot as plt
import numpy as num


df = pd.read_csv('out2_clean.csv')

fig, ax = plt.subplots()

ax.plot(df["bucket_mm_h"])

plt.show()