#!/usr/bin/env python3

import pandas as pd

df_sensor = pd.read_csv(
    "./2_intermediate/out2_clean.csv", index_col="ts", parse_dates=True
)
df_weather = pd.read_csv(
    # "open-meteo-19.30N99.20W2632m.csv", index_col="ts", parse_dates=True, header=5
    "./1_raw/open-meteo-19.23N99.08W2597m_18aug-15dec.csv",
    index_col="ts",
    parse_dates=True,
    header=3,
)

df = pd.merge(df_sensor, df_weather, how="left", on="ts").sort_index()

df.rain_mm = df.rain_mm.fillna(0)
df.id = df.id.fillna("open-meteo")
df = df.interpolate()

df.to_csv("sensor_rainfall_merged2.csv")
