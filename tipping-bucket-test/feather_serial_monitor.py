# Adapted from gateway_serial_monitor.py (by Dison Wu)
# Date: May 19th, 2024
# Author: Eren Rudy

import os
import sys
import serial
import serial.tools.list_ports
import threading
import queue
import logging
from datetime import datetime

# INFLUX-PUBLISH
import influxdb_client
from influxdb_client.client.write_api import SYNCHRONOUS
import time
import tokens

org = "werl"
token = tokens.influx
url="https://rudy-web.ca:8443"
bucket = "july2024_rg_test"
influx_timeout = 1000

### 

# client = influxdb_client.InfluxDBClient(
#     url=url,
#     token=token,
#     org=org,
#     verify_ssl=False,
#     timeout=10000
# )

# write_api = client.write_api(write_options=SYNCHRONOUS)

SITE_ID = 0

# INFLUX-PUBLISH



# SERIAL_PORT = '/dev/ttyUSB0'
SERIAL_PORT = '/dev/tty.usbmodem11301'
SERIAL_BAUD_RATE = 115200
# ROOT_LOGGING_DIR = '/home/pi/logs'
ROOT_LOGGING_DIR = "./logs"

# SERIAL_PROGRAMMER_STRING = 'FT232R'
SERIAL_PROGRAMMER_STRING = 'Feather'
        
if __name__ == "__main__":
  
  log_date = datetime.now()
  date_str = log_date.strftime('%Y%m%d-%H-%M-%S')
  
  logFormatter = logging.Formatter("%(asctime)s [%(levelname)-5.5s]  %(message)s")
  rootLogger = logging.getLogger()
  
  log_dir = "{0}/{1}".format(ROOT_LOGGING_DIR, date_str)
  
  if not os.path.exists(log_dir):
    os.makedirs(log_dir)
  
  serial_log_file_path = "{0}/{1}.txt".format(log_dir, "CottonCandyGW")
  fileHandler = logging.FileHandler(serial_log_file_path)
  fileHandler.setFormatter(logFormatter)
  
  data_log_file_path = "{0}/{1}.csv".format(log_dir, "data")
                      
  rootLogger.addHandler(fileHandler)

  consoleHandler = logging.StreamHandler()
  consoleHandler.setFormatter(logFormatter)
  rootLogger.addHandler(consoleHandler)  
  
  rootLogger.setLevel(logging.DEBUG)
  
  logging.info("Start Serial Monitor. Logging to " + serial_log_file_path)
  logging.info("Data is logged to " + data_log_file_path)
  
  serial_port = "/dev/ttyUSB0"

  port_list = serial.tools.list_ports.comports()

  for port in port_list:
      logging.info(port.device + ":" + port.description )

      if SERIAL_PROGRAMMER_STRING in  port.description:
          serial_port = port.device
          logging.info(SERIAL_PROGRAMMER_STRING + " port found")
          break

  # serial_port = "/dev/tty.usbmodem1301"

  try:
    ser=serial.Serial(serial_port,SERIAL_BAUD_RATE,timeout=1)
  except:
      logging.error("Unable to open the port " + str(serial_port))
      exit(1)

  # Open the USB serial connection
  ser=serial.Serial(serial_port,115200,timeout=1)
  
  with open(data_log_file_path, 'w') as f:
    header = "time,rain_gauge_event\n"
    f.write(header)
    f.flush()
    
    while True:

      ts = datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
      
      try:
        serial_input=ser.readline().decode('utf-8').strip()

        logging.info(serial_input)

        if serial_input:
          
          log_entry=str(serial_input)
          elements = serial_input.split()
          
          if "[EVENT]" in serial_input:

            bucket_counter = int(elements[1])

            p = influxdb_client.Point("bucket_event")\
            .field("bucket_counter", bucket_counter)

            logging.info("Creating influxdb client")
            client = influxdb_client.InfluxDBClient(
                url=url,
                token=token,
                org=org,
                verify_ssl=False,
                timeout=influx_timeout
            )

            logging.info("Creating influxdb write_api client")
            write_api = client.write_api(write_options=SYNCHRONOUS)

            logging.info("Writing to influxdb")
            try:
              write_api.write(bucket=bucket, org=org, record=p)
            except:
              logging.error("ReadTimeoutError")
            else:
              logging.info("Wrote to influxdb")

            write_api.close()
            client.close()

            f.write(f"{ts},{bucket_counter}\n")
            f.flush() 
          elif "[STATUS]" in serial_input:
            pass
          elif "[ERROR]" in serial_input:
            f.write(f"{ts},{serial_input}\n")
            f.flush() 


          


      except serial.SerialException as se:
        logging.error("Serial exception: " + str(se))
      except UnicodeDecodeError as ude:
        logging.warning("Unicode decode error: " + str(ude))
      except TypeError as e:
        logging.warning("USB-UART Disconnected. Script existing")
        exit(0)


