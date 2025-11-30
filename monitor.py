import time
import serial

# Connect to Arduino (Check your specific port, e.g., /dev/ttyUSB0)
arduino = serial.Serial('/dev/ttyUSB0', 9600)
time.sleep(2) # Wait for connection

while True:
    # Read CPU temp file
    with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
        temp_raw = int(f.read()) // 1000 # Convert 45000 to 45

    # Send to Arduino
    arduino.write(f"{temp_raw}\n".encode())
    time.sleep(1)