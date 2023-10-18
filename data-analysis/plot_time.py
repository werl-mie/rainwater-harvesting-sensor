#!/usr/bin/env python3

import pandas as pd
import plotly.express as px

df = pd.read_csv(
    "out_clean.csv",
    parse_dates=True,
    usecols=[
        "ts",
        "temp",
        "vbat_raw",
        "tank_raw",
        "float0",
        "float1",
        "float2",
        "float3",
    ],
)

fig = px.line(
    df,
    x="ts",
    y=df.columns,
    hover_data={"ts": "|%B %d, %Y"},
    title="custom tick labels",
)

fig.update_xaxes(dtick="M1", tickformat="%b\n%Y")

fig.show()
