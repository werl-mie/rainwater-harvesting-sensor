import pandas as pd

log_file = 'logs/log_node-current_4.csv'

df = pd.read_csv(log_file,parse_dates=True)

tf = pd.Timestamp(df.iloc[len(df) -1].ts)
ti = pd.Timestamp(df.iloc[0].ts)
td = pd.Timedelta(tf-ti)
mah = df.iloc[len(df)-1].current_cumulative_mah
td_h = td.seconds/3600
ma = mah/td_h


print(f"Results from: {log_file}")
print(f"td: {td}\nmah: {mah}\nma: {ma}")


