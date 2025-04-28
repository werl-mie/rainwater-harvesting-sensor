# %% [markdown]
# # Data Ingestion

# %%
import pandas as pd

df_p1_elena = pd.read_csv('./data-cleanup/elena-may-1-kingston_clean.csv',parse_dates=True,index_col='ts')
df_p2_sylvia = pd.read_csv('./data/p2-sylvia-clean.csv',parse_dates=True,index_col='time')


# %% [markdown]
# # Data Exploration

# %% [markdown]
# Going to try out IPython for ["rich" outputs](https://ipython.readthedocs.io/en/stable/interactive/plotting.html)

# %%
%matplotlib inline
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

# %% [markdown]
# ## Prototype 1 - Elena (08/23 - 04/24)

# %%
ax1 = plt.subplot(311)
plt.plot(df_p1_elena.index, df_p1_elena.bucket_mm_h)
plt.tick_params('x',labelbottom=False)
plt.title('Rainfall (mm)',size=8)

ax2 = plt.subplot(312,sharex=ax1)
plt.plot(df_p1_elena.index, df_p1_elena.tlq_level, df_p1_elena.cistern_level)
plt.tick_params('x',labelbottom=False)
ax2.legend(['tlq','cist'])
plt.title('Tank Levels',size=8)

ax3 = plt.subplot(313,sharex=ax1)
plt.plot(df_p1_elena.index, df_p1_elena.tank_raw)
plt.tick_params('x',labelsize=6)
plt.title('Cistern Levels', size=8)


# %% [markdown]
# ## Prototype 2 - Sylvia ()

# %%
start_date = '2024-05-30'
end_date = '2024-10-30'

df_p2_sylvia = df_p2_sylvia.resample('6h').mean()

ax1 = plt.subplot(211)
plt.plot(df_p2_sylvia[start_date:end_date].index, df_p2_sylvia[start_date:end_date].pot, '-', linewidth=2, markersize=2)
plt.tick_params('x',labelbottom=False)
plt.title('Cistern Level',size=8)

ax2 = plt.subplot(212,sharex=ax1)
plt.plot(df_p2_sylvia[start_date:end_date].index, df_p2_sylvia[start_date:end_date].t_lvl_lo, '-', df_p2_sylvia[start_date:end_date].t_lvl_hi, '-', markersize=1)
ax2.legend(['t_lo','t_hi'])
plt.tick_params('x',labelsize=6)
plt.title('Tlaloque Levels',size=8)




