import serial
import datetime
import time

# Replace '/dev/ttyUSB0' with your actual serial port and configure other settings
ser = serial.Serial('/dev/ttyUSB0', baudrate=115200, timeout=0.1)

def log_data(tag, data):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
    print(f"{timestamp} - {tag}: {data}")

try:
    if ser.isOpen():
        print(f"Sending and receiving data on {ser.name}... Press Ctrl+C to stop.")
        # Define the hex data to be sent
        send_hex_data = b"\x00\x00\x01\x23\x00\x00"
                
        # Send the hex data to the serial port
        ser.write(send_hex_data)
        
        while True:
      
            time.sleep(0.001)
            
            # Read data from the serial port if available
            if ser.in_waiting > 0:
                received_data = ser.read(ser.in_waiting)
                received_hex = received_data.hex()
                log_data("Received Hex", received_hex)
                
                # Extract bytes 5, 6, and 7 from the received hexadecimal string
                canid = received_hex[4:8]  # Assuming bytes 5, 6, and 7 are at index positions 4, 6, and 8 respectively
                candata_hex = received_hex[8:14]
                
                # Convert candata_hex from hexadecimal to decimal
                candata_dec = int(candata_hex, 16)
                
                # Round r_speed to the nearest integer
                r_speed = round((candata_dec / 65535) * 500)
                
                a_speed = (r_speed - 100)
                # Calculate the modified scaled speed
                s_speed = (a_speed / 500) * 65535
                
                # Convert s_speed to hexadecimal
                s_speed_hex = hex(int(s_speed))[2:]  # Remove '0x' prefix
                
                # Ensure s_speed_hex is properly padded to 4 characters
                s_speed_hex = s_speed_hex.zfill(4)
                
                s_canid = "00000133"
                # Construct the new message
                s_candata = s_canid + s_speed_hex
                
                # Convert the new message to bytes
                send_data = bytes.fromhex(s_candata)
                
                # Print the extracted values
                log_data("Received CAN ID", canid)
                log_data("Received CAN DATA  (hex)", candata_hex)
                log_data("Received CAN DATA  (decimal)", candata_dec)
                log_data("Received SPEED (Expected)", r_speed)
                log_data("Send SPEED     (Actual)", a_speed)
                log_data("Received SPEED (16 bit data)", s_speed)
                log_data("Received SPEED (hex)", s_speed_hex)
                
                # Send the new message
                ser.write(send_data)
                
                # Compare the data
                log_data("Sent Hex", send_data.hex())
                
                # Log the lengths of received and sent data
                log_data("Received Data Length", len(received_data))
                log_data("Sent Data Length", len(send_data))
                
                if len(received_data) == len(send_data):
                    match = True
                    for i in range(len(send_data)):
                        if received_data[i] != send_data[i]:
                            log_data(f"Byte {i} mismatch", f"Received: {received_data[i]:02x}, Sent: {send_data[i]:02x}")
                            match = False
                            break
                    log_data("Comparison Result", match)
                    if match:
                        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                        print(f"{timestamp} - Data match confirmed: {send_data.hex()}")
                else:
                    log_data("Comparison Result", False)

except KeyboardInterrupt:
    pass
finally:
    ser.close()

