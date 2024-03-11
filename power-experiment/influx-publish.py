import serial
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import time
import tokens



#pyserial
serial_devices = [None]#, None, None]

serial_devices[0] = serial.Serial()
serial_devices[0].baudrate = 115200
serial_devices[0].port = '/dev/tty.usbserial-140'
serial_devices[0].open()

# serial_devices[1] = serial.Serial()
# serial_devices[1].baudrate = 115200
# serial_devices[1].port = '/dev/tty.usbmodem1301'
# serial_devices[1].open()

# serial_devices[2] = serial.Serial()
# serial_devices[2].baudrate = 115200
# serial_devices[2].port = '/dev/tty.usbmodem1301'
# serial_devices[2].open()

#influxdb_client
org = "werl"
token = tokens.influx
url="http://34.125.33.101:8086"
bucket = "power-testing"

client = influxdb_client.InfluxDBClient(
    url=url,
    token=token,
    org=org
)

write_api = client.write_api(write_options=SYNCHRONOUS)

EXP_ID = '2'
measurement_name = 'node-current'

def publish(line_str):
	data = line_str.split(',')
	device_id = data[0]
	current_cumulative_C = float(data[1])
	current_cumulative_mAh = float(data[2])

	p = influxdb_client.Point(measurement_name)\
	.tag("experiment_id",EXP_ID)\
	.tag("device_id", device_id)\
	.field("current_cumulative_C", current_cumulative_C)\
	.field("current_cumulative_mAh", current_cumulative_mAh)

	write_api.write(bucket=bucket, org=org, record=p)

	print(f"id: {device_id}, C: {current_cumulative_C}, mAh: {current_cumulative_mAh}")


while True:

	for ser in serial_devices:
		line = ser.readline().decode("utf-8")

		if line != "LTC2941 Raw Data: id, current_cumulative_C, current_cumulative_mAh\r\n":
			publish(line)
		
