import serial
import keyboard
import time

# Replace 'COM3' with your Arduino port
arduino = serial.Serial('COM7', 9600, timeout=0.1)
time.sleep(2)  # Wait for Arduino to reset

print("Press 'S' to start, SPACE to jump, R to restart, ESC to quit.")

try:
    while True:
        if keyboard.is_pressed("s"):
            arduino.write(b'S')
            time.sleep(0.2)  # debounce
        if keyboard.is_pressed("space"):
            arduino.write(b'J')
            time.sleep(0.1)  # debounce
        if keyboard.is_pressed("r"):
            arduino.write(b'R')
            time.sleep(0.2)
        if keyboard.is_pressed("esc"):
            print("Exiting...")
            break
except KeyboardInterrupt:
    pass

arduino.close()
