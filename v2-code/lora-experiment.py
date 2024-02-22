import serial
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import time
import tokens

from collections import deque

test_data = [
"001002F3",
"002002F3",
"001002F4",
"002002F4",
"001002F5",
"002002F5",
"001002F6",
"001002F7",
"002002F7",
# "001002F8",
"001002F9",
# "001002FA",
"001002FB",
"001002FC",
"001002FD",
"001002FE"
]

#pyserial
# serial_devices = [None, None, None]

# serial_devices[0] = serial.Serial()
# serial_devices[0].baudrate = 115200
# serial_devices[0].port = '/dev/tty.usbserial-130'
# serial_devices[0].open()

# serial_devices[1] = serial.Serial()
# serial_devices[1].baudrate = 115200
# serial_devices[1].port = '/dev/tty.usbmodem1301'
# serial_devices[1].open()

# serial_devices[2] = serial.Serial()
# serial_devices[2].baudrate = 115200
# serial_devices[2].port = '/dev/tty.usbmodem1301'
# serial_devices[2].open()

# serial_devices[3] = serial.Serial()
# serial_devices[3].baudrate = 115200
# serial_devices[3].port = '/dev/tty.usbmodem1301'
# serial_devices[3].open()

#influxdb_client
org = "werl"
token = tokens.influx
url="http://34.125.33.101:8086"
bucket = "lora-testing"

client = influxdb_client.InfluxDBClient(
    url=url,
    token=token,
    org=org
)

write_api = client.write_api(write_options=SYNCHRONOUS)

EXP_ID = '0'
# measurement_name = 'test-node-current'

def publish_current(line_str):
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


def publish_rx_success(device_id):
	print(f"device {device_id} received a packet successfully")

	# p = influxdb_client.Point("packet-rx-success")\
	# .tag("experiment_id",EXP_ID)\
	# .tag("device_id", device_id)

	# write_api.write(bucket=bucket, org=org, record=p)

def publish_rx_missed(device_id, n_packets_missed):
	print(f"missed {n_packets_missed} packets from device {device_id}")

	# p = influxdb_client.Point("packet-rx-missed")\
	# .tag("experiment_id",EXP_ID)\
	# .tag("device_id", device_id)\
	# .field("n_packets_missed", n_packets_missed)

	# write_api.write(bucket=bucket, org=org, record=p)

last_rx_count = [None, 0, 0, 0]

f_out = open(f"log_exp_{EXP_ID}.csv", "w")
f_out.write("device_id, count\n")

# for i, data in enumerate(test_data):
# 	device_id = int(f"0x{data[0:3]}", 0)
# 	count = int(f"0x{data[3:]}", 0)

# 	f_out.write(f"{device_id},{count}\n")

# 	publish_rx_success(device_id)

# 	if i != 0 and last_rx_count[device_id] != count - 1:
# 		n_packets_missed = count - last_rx_count[device_id] - 1
# 		publish_rx_missed(device_id, n_packets_missed)

# 	last_rx_count[device_id] = count

	# print(f"id: {id}, count: {count}")

tx_packet_lists = [None, deque(), deque(), deque()]
rx_packet_lists = [None, deque(), deque(), deque()]


while True:

	for ser in serial_devices:

		line = ser.readline().decode("utf-8")

		if line != "LTC2941 Raw Data: id, current_cumulative_C, current_cumulative_mAh\r\n" and "0x00" == line[0:4]:
			publish_current(line)
		else:
			device_id = int(f"0x{data[0:3]}", 0)
			count = int(f"0x{data[3:]}", 0)

			f_out.write(f"{device_id},{count}\n")

			
