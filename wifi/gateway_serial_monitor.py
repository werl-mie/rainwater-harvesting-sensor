import os
import sys
import serial
import serial.tools.list_ports
import threading
import queue
import logging
from datetime import datetime

SERIAL_PORT = '/dev/ttyUSB0'
SERIAL_BAUD_RATE = 9600
ROOT_LOGGING_DIR = '/home/pi/logs'

SERIAL_PROGRAMMER_STRING = 'FT232R'
        
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

  try:
    ser=serial.Serial(serial_port,SERIAL_BAUD_RATE,timeout=1)
  except:
      logging.error("Unable to open the port " + str(serial_port))
      exit(1)

  # Open the USB serial connection
  ser=serial.Serial(serial_port,9600,timeout=1)
  
  with open(data_log_file_path, 'w') as f:
    header = "time,node,parent,POT,level1,level2\n"
    f.write(header)
    f.flush()
    
    while True:
      
      try:
        serial_input=ser.readline().decode('utf-8')

        if serial_input:
          
          log_entry=str(serial_input)
          logging.debug(log_entry)
          
          if "[DATA]" in serial_input:
            elements = serial_input.split()
            
            node_addr = elements[1]
            data_len = int(elements[2],16)
            data = elements[3] # The data is printed as a hex string where each byte is represented in 2-digit
            
            if len(data) != data_len * 2:
              logging.warning("Data length and the actual data size mismatch. Packet discarded.")
          
            elif len(data) == 12:
              parent_addr = data[:4]
              pot_reading = int(data[4:8], 16)
              level_1_reading = int(data[8:10],16)
              level_2_reading = int(data[10:12],16)
                        
              logging.info("Packet from node {0} (parent {1}): POT={2}, level1={3}, level2={4}".format(
                        node_addr, parent_addr, pot_reading, level_1_reading, level_2_reading))
                          
              timestamp = datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
              
              line = "{0},{1},{2},{3},{4},{5}\n".format(timestamp, node_addr, parent_addr, pot_reading, 
                      level_1_reading,level_2_reading)
              
              f.write(line)
              f.flush()

      except serial.SerialException as se:
        logging.error("Serial exception: ", + str(se))
      except UnicodeDecodeError as ude:
        logging.warning("Unicode decode error: " + str(ude))
      except TypeError as e:
        logging.warning("USB-UART Disconnected. Script existing")
        exit(0)