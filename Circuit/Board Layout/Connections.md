# Pixel Schematic Pinout

This does not yet include the connections for an external battery, for the 5V and 3.3V voltage regulators, or JST switch.

## Primary Teensy 3.1

In summary, the primary Teensy 3.1 connects to IMU, RGB LEDs, piezo speaker, Synapse module, and the secondary Teensy 3.1 (see below for its pinout).

**Synapse RF266PC1**

Serial1 (constant MESH_SERIAL in Communication.h)

| Teensy 3.1 | Synapse RF266PC1 |
| ---------- | ---------------- |
| 0/RX1      | TX               |
| 1/TX1      | RX               |

**Secondary Teensy 3.1**

The primary Teensy 3.1 communicates with the secondary Teensy 3.1 via `Serial3`. The firmware references this with the constant `DEVICE_SERIAL` in `Serial.h`.

| Teensy 3.1 | Secondary Teensy 3.1 |
| ---------- | ---------------- |
| 7/RX3      | 8/TX3            |
| 8/TX3      | 7/RX3            |

**SparkFun RGB LED Breakout (WS2811/WS2812)**

constant LED_OUTPUT_PIN, in Color.h

| Teensy 3.1 | WS2811 Breakout  |
| ---------- | ---------------- |
| 6          | DI               |


**Piezo Speaker (PC Mount 12mm 2.048kHz)**

The firmware references this with the constant `SPEAKER_OUT_PIN` in `Sound.h`.

| Teensy 3.1 | RGB LED Breakout |
| ---------- | ---------------- |
| 22/PWM     | (+)              |

**Pololu AltIMU-10 v4**

Communicates with Teensy via I²C.

| Teensy 3.1 | AltIMU-10 v4     |
| ---------- | ---------------- |
| 18/SDA0    | SDA              |
| 19/SCL0    | SCL              |

## Secondary Teensy 3.1

The secondary Teensy 3.1 connects to the Adafruit CC3000 and the primary Teensy 3.1.

**Primary Teensy 3.1**

The secondary Teensy 3.1 communicates with the primary Teensy 3.1 via `Serial3`. The firmware references this with the constant `DEVICE_SERIAL` in `Serial.h`.

| Secondary Teensy 3.1 | Primary Teensy 3.1 |
| ---------- | ---------------- |
| 7/RX3      | 8/TX3            |
| 8/TX3      | 7/RX3            |

**Adafruit CC3000 WiFi Breakout with Onboard Antenna**

| Secondary Teensy 3.1 | CC3000 Breakout |
| ---------- | ---------------- |
| 1          | IRQ              |
| 3          | VBEN             |
| 10/CS      | CS               |
| 11/DOUT    | MISO             |
| 12/DIN     | MOSI             |
| 13/SCK     | CLK              |

The firmware references the connection to VBEN with the constant `VBAT` in `WebServer.h`.