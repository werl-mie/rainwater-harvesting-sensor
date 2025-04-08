import pandas as pd
import numpy as np
from scipy import signal


# ----------------------------------------
#  Rainfall peak detection
# ----------------------------------------

WIN_WIDTH_HRS = 1
WIN_WIDTH = WIN_WIDTH_HRS*4*60

PEAK_DISTANCE_HRS = 24
PEAK_DISTANCE = PEAK_DISTANCE_HRS*4*60

PEAK_WIDTH_HRS = 0.5
PEAK_WIDTH = PEAK_WIDTH_HRS*4*60

sd = '2023-08-01' # Start date
ed = '2024-04-20'

sig = df_data_raw.bucket_mm[sd:ed].to_numpy() # Rainfall data

win = signal.windows.triang(WIN_WIDTH) 
filtered = signal.convolve(sig, win, mode='same') / sum(win)
peaks, peak_properties = signal.find_peaks(filtered, height=0, prominence=0, width=PEAK_WIDTH, distance=PEAK_DISTANCE)

# ----------------------------------------
#   Classify rainfall events
# ----------------------------------------

type_rfe = {'peak': int, 'lb': int, 'rb': int, 'area': float}

rainfall_events = pd.DataFrame(data=None, columns=['peak','lb','rb','area'])

for i, peak in enumerate(peaks):
    lb = peak_properties['left_bases'][i]
    rb = peak_properties['right_bases'][i]
    area = filtered[lb:rb].sum()*36.2

    if (area > 200):
        row = pd.DataFrame([[peak,lb,rb,area]], columns=rainfall_events.columns)
        rainfall_events = pd.concat([rainfall_events,row],ignore_index=True)

rainfall_events = rainfall_events.astype(type_rfe)

# ----------------------------------------
#   First flush diverter edge detection
# ----------------------------------------

def edge_find(data, thresh, kind=None):
    sign = data >= thresh
    if kind == 'rise':
        pos = np.where(np.convolve(sign, [1, -1]) == 1)
    elif kind == 'fall':
        pos = np.where(np.convolve(sign, [1, -1]) == -1)

    return pos[0]

filling_events = edge_find(df_data_raw[sd:ed]['tlq_level'].values, 1.5, 'rise')
emptying_events = edge_find(df_data_raw[sd:ed]['tlq_level'].values,1.5, 'fall')

# ----------------------------------------
#   Classify maintenance events
# ----------------------------------------

SEQ_LEN = 4

length = len(df_data_raw[sd:ed]) 
zeros = np.zeros((length,1))

df_events = pd.DataFrame(zeros, columns=['event'])
df_events['event'].iloc[rainfall_ends] = 'rain_end'
df_events['event'].iloc[rainfall_starts] = 'rain_start'
df_events['event'].iloc[filling_events] = 'filling'
df_events['event'].iloc[emptying_events] = 'emptying'
df_events = df_events[df_events['event'] != 0]

seq_good = np.array(['rain_start','filling','rain_end','emptying'])
seq_bad = np.array(['rain_start','filling','rain_end','rain_start'])

for i in range(0,len(df_events)):
    if (i < len(df_events) - SEQ_LEN) and df_events.iloc[i]['event'] == 'rain_start':

        if (df_events[i:i+4]['event'].values == seq_bad).all():
            print(f"BAD sequence at {df_data_raw[sd:ed].iloc[int(df_events.iloc[i].name)].name} to {df_p1_elena[sd:ed].iloc[int(df_events.iloc[i+4].name)].name}")
            
        if (df_events[i:i+4]['event'].values == seq_good).all():
            print(f"GOOD sequence from {df_data_raw[sd:ed].iloc[int(df_events.iloc[i].name)].name} to {df_p1_elena[sd:ed].iloc[int(df_events.iloc[i+4].name)].name}")
    
