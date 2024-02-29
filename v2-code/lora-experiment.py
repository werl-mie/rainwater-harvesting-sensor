import serial
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import time

from collections import deque
from datetime import datetime

# Custom modules
import tokens
from DLL import DLL

#pyserial
serial_devices = [None, None]#, None, None]

serial_devices[0] = serial.Serial()
serial_devices[0].baudrate = 115200
serial_devices[0].port = '/dev/tty.usbmodem11401'
serial_devices[0].open()

serial_devices[1] = serial.Serial()
serial_devices[1].baudrate = 115200
serial_devices[1].port = '/dev/tty.usbmodem11301'
serial_devices[1].open()

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

def publish_rx_missed(count, device_id):
	print(f"missed {count} from device {device_id}")
	fout2.write(f"missed {count} from device {device_id}")

	# p = influxdb_client.Point("packet-rx-missed")\
	# .tag("experiment_id",EXP_ID)\
	# .tag("device_id", device_id)\
	# .field("n_packets_missed", n_packets_missed)

	# write_api.write(bucket=bucket, org=org, record=p)

def check_packets(device_id):

	curr_tx_packet = tx_packet_lists[device_id].head
	curr_rx_packet = rx_packet_lists[device_id].head

	while curr_tx_packet != None and curr_rx_packet != None:

		# print(f"id {device_id}: comparing tx val {curr_tx_packet.val} from {tx_packet_lists[device_id]} with rx val {curr_rx_packet.val} from {rx_packet_lists[device_id]}")

		if curr_rx_packet.val == curr_tx_packet.val:
			# curr_rx_packet was received (delete curr_rx packet)
			received_packets[device_id].append(curr_tx_packet.val)

			#if curr_tx packet is not the first, prior packets are 'missed'
			if curr_tx_packet.prev != None:
				missed_tx_packet = curr_tx_packet.prev

				while missed_tx_packet != None:
					missed_packets[device_id].append(missed_tx_packet.val)
					missed_tx_packet = missed_tx_packet.prev

			rx_packet_lists[device_id].remove_up_to(curr_rx_packet)
			tx_packet_lists[device_id].remove_up_to(curr_tx_packet)
			curr_tx_packet = tx_packet_lists[device_id].head
		else:
			curr_tx_packet = curr_tx_packet.next

		curr_rx_packet = rx_packet_lists[device_id].head

		# print(f"new tx list: {tx_packet_lists[device_id]}, new rx list:{rx_packet_lists[device_id]}\n")

	# print(f"checked all packets from device {device_id}")

	for missed_packet in missed_packets[device_id]:
		publich_rx_missed(missed_packet, device_id)

last_rx_count = [None, 0, 0, 0]

fout = open(f"log_exp_{EXP_ID}.csv", "w")
fout2 = open(f"missed_packets_exp_{EXP_ID}.csv", "w")
fout.write("ts, direction, device_id, count\n")

tx_packet_lists = [None, DLL(), DLL(), DLL()]
rx_packet_lists = [None, DLL(), DLL(), DLL()]

missed_packets = [None, deque(), deque(), deque()]	
received_packets = [None, deque(), deque(), deque()]


while True:

	for ser in serial_devices:

		line = ser.readline().decode("utf-8")
		line_lst = line.split(",")
		header = line_lst[0]

		if header == "cc":
			publish_current(line)
		else:
			print(f"received {line.strip()}")

			device_id = int(f"0x{line_lst[1][0:3]}", 0)
			count = int(f"0x{line_lst[1][3:]}", 0)

			fout.write(f"{datetime.now()},{header},{device_id},{count}\n")

			if header == "tx":
				tx_packet_lists[device_id].append(count)
				
			elif header == "rx":
				rx_packet_lists[device_id].append(count)

				check_packets(device_id)

			# print(f"tx_list[{device_id}]: ",end='')
			# print(tx_packet_lists[device_id])

			# print(f"rx_list[{device_id}]: ",end='')
			# print(rx_packet_lists[device_id])

			# print()

			# f_out.write(f"{datetime.datetime.now()},{device_id},{count}\n")

	

# Code for testing check_packets(device_id)

# print(tx_packet_lists[1])

# for tx,rx in zip(test_tx_data, test_rx_data):

# 	if tx != "":
# 		device_id_tx = int(f"0x{tx[0:3]}", 0)
# 		count_tx = int(f"0x{tx[3:]}", 0)
# 		tx_packet_lists[device_id_tx].append(count_tx)	

# 	if rx != "":
# 		device_id_rx = int(f"0x{rx[0:3]}", 0)
# 		count_rx = int(f"0x{rx[3:]}", 0)	
# 		rx_packet_lists[device_id_rx].append(count_rx)

# 	check_packets(device_id_rx)

# for i in range(1,4):
# 	print(f"Missed packets {i}: ", end='')
# 	print(missed_packets[i])
# 	print(f"Received packets {i}: ", end='')
# 	print(received_packets[i])
# 	print()
# 	print(f"tx_list {i}: ", end='')
# 	print(tx_packet_lists[i])
# 	print(f"rx_list {i}: ", end='')
# 	print(rx_packet_lists[i])

# quit()		
