flowchart TD
    A(raw_data.txt) --> B[pre-process.py]
    B --> C(out2.csv)
    C --> D[data_cleanup.py]
    D --> E(out2_clean.csv)
    F(open-meteo.csv) --> G[merge_rainfall.py]
    E --> G
    G --> H(sensor_rainfall_merged2.csv)
  
