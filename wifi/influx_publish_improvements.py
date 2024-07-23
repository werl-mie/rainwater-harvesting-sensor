
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
from time import sleep
import tokens



measurement_name = 'test-count'
EXP_ID = 0
influx_timeout=1000


#influxdb_client
org = "werl"
token = tokens.influx
url="https://rudy-web.ca:8443"
bucket = "debug"

client = influxdb_client.InfluxDBClient(
    url=url,
    token=token,
    org=org,
    verify_ssl=False,
    timeout=influx_timeout
)

write_api = client.write_api(write_options=SYNCHRONOUS)



def publish(count):
	p = influxdb_client.Point(measurement_name)\
	.tag("experiment_id",EXP_ID)\
	.field("count", count)

	try: 
		write_api.write(bucket=bucket, org=org, record=p)
	except:
		print(f"ReadTimeoutError on {count}")
	else:
		print(f"Wrote {count} to influxdb")


for i in range(0,100):
	publish(i)
	# sleep(1000)
		
