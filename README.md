# ESP32_Wired_Communication

# ESP32 Interface Verification: UART, I²C and SPI

Verification of the three wired interfaces on an ESP32 (WROOM-32) board using the Arduino IDE 2.3.8, with the Serial Monitor at 115200 baud (COM4).

## 1. Pin table

| Signal | GPIO used |
|---|---|
| UART TX | 17 |
| UART RX | 16 |
| I²C SDA | 21 |
| I²C SCL | 22 |
| SPI SCK | 18 |
| SPI MOSI | 23 |
| SPI MISO | 19 |
| SPI CS | 27 |

None of these are strapping pins (0, 2, 5, 12, 15), flash pins (6–11) or input-only pins (34–39), and every signal has its own pin.

## 2. Wiring diagram

```mermaid
flowchart LR
    subgraph ESP["ESP32 WROOM-32"]
        direction TB
        G17["GPIO17 (UART TX)"]
        G16["GPIO16 (UART RX)"]
        G23["GPIO23 (SPI MOSI)"]
        G19["GPIO19 (SPI MISO)"]
        G21["GPIO21 (I²C SDA)"]
        G22["GPIO22 (I²C SCL)"]
        V33["3V3"]
        GNDP["GND"]
        G18["GPIO18 (SPI SCK) not wired"]
        G27["GPIO27 (SPI CS) not wired"]
    end

    subgraph MOD["I²C module (OLED or BME/BMP280)"]
        direction TB
        SDA["SDA"]
        SCL["SCL"]
        VCC["VCC"]
        GNDM["GND"]
    end

    G17 ---|"UART loopback jumper"| G16
    G23 ---|"SPI loopback jumper"| G19
    G21 ---|"I²C data"| SDA
    G22 ---|"I²C clock"| SCL
    V33 ---|"I²C power"| VCC
    GNDP ---|"I²C ground"| GNDM

    classDef unwired stroke-dasharray: 5 5,color:#888
    class G18,G27 unwired

    linkStyle 0 stroke:#1D9E75,stroke-width:3px
    linkStyle 1 stroke:#7F77DD,stroke-width:3px
    linkStyle 2 stroke:#378ADD,stroke-width:3px
    linkStyle 3 stroke:#EF9F27,stroke-width:3px
    linkStyle 4 stroke:#E24B4A,stroke-width:3px
    linkStyle 5 stroke:#5F5E5A,stroke-width:3px
```

| Connection | From | To |
|---|---|---|
| UART loopback jumper | GPIO17 (TX) | GPIO16 (RX) |
| SPI loopback jumper | GPIO23 (MOSI) | GPIO19 (MISO) |
| I²C data | GPIO21 (SDA) | Module SDA |
| I²C clock | GPIO22 (SCL) | Module SCL |
| I²C power | 3V3 | Module VCC |
| I²C ground | GND | Module GND |

GPIO18 (SCK) and GPIO27 (CS) are assigned in the SPI sketch but are not wired, because the SPI test is a loopback.

## 3. Serial Monitor output

### UART with jumper
<img width="479" height="400" alt="image" src="https://github.com/user-attachments/assets/dd2d9862-ad03-44a4-b002-ece8bdf7bae8" />

The ESP32 prints `PING` once per second.
<img width="975" height="769" alt="image" src="https://github.com/user-attachments/assets/f77c927f-c273-4f49-be6e-5b76867c5a58" />

### UART without jumper
No `PING` appears, because nothing is connected between TX and RX.

### I²C with module
<img width="846" height="650" alt="image" src="https://github.com/user-attachments/assets/65854533-5ffe-444a-beda-9f9a83b310aa" />

The scanner found one device at address `0x3C`.

<img width="975" height="536" alt="image" src="https://github.com/user-attachments/assets/0d873335-441c-44ff-a93a-58e8b81f5353" />


### I²C without module (4 wires removed)
The scanner printed `No I2C devices found`.

<img width="975" height="578" alt="image" src="https://github.com/user-attachments/assets/e12a3d55-70db-4326-9d9f-f4c4c185e98f" />


### SPI with jumper
<img width="823" height="554" alt="image" src="https://github.com/user-attachments/assets/41f04b25-3f06-43e2-a0f9-61cdd79af7cf" />


Every line shows `Sent 0xA5, received 0xA5`.

<img width="975" height="571" alt="image" src="https://github.com/user-attachments/assets/dc7cebb7-5754-46e5-b340-86085b8156e0" />


### SPI without jumper
Every line shows `Sent 0xA5, received 0x00`.

<img width="975" height="572" alt="image" src="https://github.com/user-attachments/assets/ec2159f5-d249-460d-9d65-164832473b56" />



### Sketch at 9600, monitor at 115200
Garbled characters (unreadable squares) appear instead of `PING`.

<img width="975" height="548" alt="image" src="https://github.com/user-attachments/assets/caecc905-6fbe-4e03-bfaa-cb2c2689402e" />

### Sketch at 9600, monitor at 9600
Clean `PING` lines appear once per second.

<img width="975" height="549" alt="image" src="https://github.com/user-attachments/assets/7028eb63-d22a-4be7-b903-28b1fb25c1bb" />

## 4. Expected vs observed

| Test | Expected | Observed |
|---|---|---|
| UART with jumper | `PING` printed once per second | `PING` printed once per second |
| UART without jumper | Nothing received; the monitor stays blank | No `PING` received |
| I²C with module | `Found device at 0x3C` (OLED) | `Found device at 0x3C` |
| I²C without module | `No I2C devices found` | `No I2C devices found` |
| SPI with jumper | `Sent 0xA5, received 0xA5` | `Sent 0xA5, received 0xA5` |
| SPI without jumper | A value other than `0xA5` (often `0x00` or `0xFF`) | `Sent 0xA5, received 0x00` |
| Sketch at 9600, monitor at 115200 | Garbled characters | Garbled characters (unreadable squares) |
| Sketch at 9600, monitor at 9600 | Clean `PING` | Clean `PING` once per second |

## 5. Questions

**1. Why does UART need both sides to use the same baud rate, while SPI uses a clock line?**
```UART has no clock wire, so the receiver times each bit with its own internal timer, and that only lines up with the sender if both use the same baud rate. SPI sends a clock signal along with the data, so the receiver reads each bit on the clock edges and the speed doesn't need to be agreed in advance.```

**2. Why can two devices with the same address not share one I²C bus?**
```Every I²C transaction starts with the controller sending an address, and both devices with that address would answer at the same time on the shared SDA line. Their signals would collide, so the controller could not tell which device replied or talk to just one of them.```

**3. Why is CS set HIGH between SPI transfers?**
```CS is active-low, so setting it HIGH tells the device the transfer is finished and it should stop listening and reset its bit counter. It also deselects the device so it ignores the shared clock and data lines while the controller talks to other devices.```

## Notes

- The `Serial.begin(9600)` change was made only for the baud-rate experiment. The committed `uart_loopback.ino` uses `Serial.begin(115200)`.
- The burst of text at boot (`ets Jul 29 2019...`, `rst:0x1...`) is the ESP32 bootloader message, which is printed at 115200 baud.
