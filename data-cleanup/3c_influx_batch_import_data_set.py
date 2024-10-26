"""
Import VIX - CBOE Volatility Index - from "vix-daily.csv" file into InfluxDB 2.0

https://datahub.io/core/finance-vix#data
"""

from collections import OrderedDict
from csv import DictReader

import reactivex as rx
from reactivex import operators as ops

from influxdb_client import Point, InfluxDBClient, WriteOptions

import tokens

org = "werl"
token = tokens.influx
url="https://rudy-web.ca:8443"
bucket = "rwh-pilot-data"

file_in = 'elena-may-2-gigastone-clean.csv'
# file_in = 'dataRawCottonCandyQuitelpec.csv'
# file_in = 'test.csv'


def parse_row(row: OrderedDict):
    """Parse row of CSV file into Point with structure:

        financial-analysis,type=vix-daily close=18.47,high=19.82,low=18.28,open=19.82 1198195200000000000

    CSV format:
        Date,VIX Open,VIX High,VIX Low,VIX Close\n
        2004-01-02,17.96,18.68,17.54,18.22\n
        2004-01-05,18.45,18.49,17.44,17.49\n
        2004-01-06,17.66,17.67,16.19,16.73\n
        2004-01-07,16.72,16.75,15.5,15.5\n
        2004-01-08,15.42,15.68,15.32,15.61\n
        2004-01-09,16.15,16.88,15.57,16.75\n
        ...

    :param row: the row of CSV file
    :return: Parsed csv row to [Point]
    """

    """
    For better performance is sometimes useful directly create a LineProtocol to avoid unnecessary escaping overhead:
    """
    # from datetime import timezone
    # import ciso8601
    # from influxdb_client.client.write.point import EPOCH
    #
    # time = (ciso8601.parse_datetime(row["Date"]).replace(tzinfo=timezone.utc) - EPOCH).total_seconds() * 1e9
    # return f"financial-analysis,type=vix-daily" \
    #        f" close={float(row['VIX Close'])},high={float(row['VIX High'])},low={float(row['VIX Low'])},open={float(row['VIX Open'])} " \
    #        f" {int(time)}"


    return Point("daq-reading-3") \
        .field("vbat_raw", float(row["vbat_raw"])) \
        .field("tank_raw", float(row["tank_raw"])) \
        .field("bucket_mm", float(row["bucket_mm"])) \
        .field("bucket_mm_h", float(row["bucket_mm_h"])) \
        .field("temp_am2320", float(row["temp_am2320"])) \
        .field("temp_rtc", float(row["temp_rtc"])) \
        .tag("device", "daq2") \
        .time(row['ts'])
        # .field("cistern_level", float(row["cistern_level"])) \
        # .field("tlq_level", int(row["tlq_level"])) \


"""
Converts vix-daily.csv into sequence of data point
"""
data = rx \
    .from_iterable(DictReader(open(file_in, 'r'))) \
    .pipe(ops.map(lambda row: parse_row(row)))


with InfluxDBClient(url=url, token=token, org=org, debug=True, verify_ssl=False, timeout=1000) as client:


    """
    Create client that writes data in batches with 50_000 items.
    """
    with client.write_api(write_options=WriteOptions(batch_size=50_000, flush_interval=10_000)) as write_api:

        """
        Write data into InfluxDB
        """
        write_api.write(bucket=bucket, record=data)



    """
    Querying max value of CBOE Volatility Index
    """
    # query = 'from(bucket:"")' \
    #         ' |> range(start: 0, stop: now())' \
    #         ' |> filter(fn: (r) => r._measurement == "financial-analysis")' \
    #         ' |> max()'
    # result = client.query_api().query(query=query)

    """
    Processing results
    """
    # print()
    # print("=== results ===")
    # print()
    # for table in result:
    #     for record in table.records:
    #         print('max {0:5} = {1}'.format(record.get_field(), record.get_value()))
