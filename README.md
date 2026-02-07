# Arduino Latency Tester

> Test glass to glass latency in a video system.

This tool has been developed to easily and cheaply measrure glass to glass latency in FPV systems.

To learn more about how to use this tool, check out my blog post about [measuring FPV latency](https://brushlesswhoop.com/measuring-fpv-latency).

## Building & Flashing

This project uses [PlatformIO](https://platformio.org/). Install the PlatformIO IDE extension for VS Code or the PlatformIO Core CLI.

### Build
```bash
pio run
```

### Flash
```bash
pio run -t upload
```

### Serial Monitor
```bash
pio device monitor
```

The serial monitor runs at 115200 baud. Send `1` to calibrate and `2` to start measurements.

## Pinout

### LED (pin 13)

| Arduino | LED    |
|---------|--------|
| D13     | Anode (+) via resistor |
| GND     | Cathode (-) |

A current-limiting resistor (220-330 Ohm) should be placed in series between pin 13 and the LED anode.

### Photoresistor / LDR (pin A0)

| Arduino | Component      |
|---------|----------------|
| 5V      | LDR (one leg)  |
| A0      | LDR/Resistor junction |
| GND     | Pull-down resistor (10k Ohm) |

Wire the LDR and a 10k Ohm pull-down resistor as a voltage divider: one leg of the LDR to 5V, the other leg to both A0 and one leg of the 10k resistor, with the other leg of the 10k resistor to GND.

```
5V ─── [LDR] ──┬── A0
                │
           [10k Ohm]
                │
               GND
```
