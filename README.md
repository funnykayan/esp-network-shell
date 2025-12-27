# ESP32 Serial + Network Shell (with LCD Status Display)

This project turns an ESP32 into a tiny, stubborn, vaguely Unix-inspired shell that you can talk to over **Serial** or **WiFi**, because apparently blinking LEDs alone wasn’t enough anymore.

It also shows WiFi status and the assigned IP address on an **I2C LCD**, so you don’t have to spam the serial monitor like a raccoon pressing buttons.

---

## What This Does

- Runs a **basic command shell** on the ESP32
- Accepts input from:
  - USB Serial
  - A network connection (TCP shell)
- Supports simple commands (help, echo, time, reboot, GPIO control, etc.)
- Lets you select and control an LED pin from the shell
- Displays WiFi status and IP address on an **I2C LCD**
- Uses ANSI colors in terminals that aren’t from the Stone Age

This is not a real OS.  
It will not replace Linux.  
It barely replaces a notebook and a sticky note.

---

## Hardware Used

Minimum required hardware:

- **ESP32 Dev Module**
- **USB cable** (yes, really)
- **I2C LCD display**  
  Typically:
  - 16x2 
  - I2C backpack (PCF8574)
- **LED + resistor** (optional, but recommended for sanity)

### I2C Wiring
if u dont know this figure it out urself its not that hard, 1 minute google search im not a library

If your LCD doesn’t show anything:
- Check the I2C address
- Check contrast
- Question your life choices

---

## How It Works (High Level)

1. ESP32 boots
2. Connects to WiFi (eventually)
3. Shows WiFi status + IP on the LCD
4. Starts:
   - Serial shell
   - Network shell (TCP)
5. Waits for you to type commands like it owes you money

---

## Using the Shell

### Connect via Serial

- Baud rate: **115200**
- Any terminal that isn’t hostile:
  - `screen`
  - `minicom`
  - `picocom`
  - literally anything except the Arduino Serial Monitor

### Connect via Network

Once WiFi is connected, the IP is shown on the LCD.

to connect (linux & mac):
```bash
nc (ur ip) 2323
```

to connect (windows):
```bash
ncat (ur ip) 2323
```
to connect (templeos):
idk bro


## Using the built-in MSBASIC-Style script system

Run: ```script start``` to start recording commands to the script.

Then input all of your commands (just built in commands that you can run in the shell) with a max of 20 lines to save RAM

Run ```script end``` to end and save your script.

Run ```script view``` to see your script.

Run ```script clear``` to clear the script.

Run ```run``` to run your script.





## the rest u can figure it out by yourself
