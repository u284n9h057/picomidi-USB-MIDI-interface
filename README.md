# picoMIDI USB MIDI Interface

Arduino firmware that turns a Midimuso picoMIDI board and Raspberry Pi Pico
into a class-compliant USB MIDI interface with one physical MIDI input and two
independently addressable MIDI outputs.

The interface is powered entirely through the Pico's USB connection. No 12 V
power supply is required.

## Features

- Class-compliant USB MIDI
- One physical 5-pin DIN MIDI input
- Two independently addressable 5-pin DIN MIDI outputs
- Supports notes, controllers, program changes, pitch bend, MIDI clock,
  transport messages, SysEx and other MIDI 1.0 messages
- No custom Raspberry Pi driver required
- Built-in LED indicates outgoing USB MIDI activity
- USB powered

## MIDI routing

| USB MIDI port on the host | picoMIDI connection |
| --- | --- |
| Port 1 input | DIN MIDI IN |
| Port 1 output | DIN MIDI OUT 1 |
| Port 2 output | DIN MIDI OUT 2 |
| Port 2 input | Unused virtual input |

The USB library creates virtual MIDI cables as bidirectional pairs. Therefore,
the host shows two MIDI inputs even though the picoMIDI board has only one
physical input. The second USB input is harmless and can be ignored.

## Requirements

### Hardware

- Raspberry Pi Pico (RP2040)
- Midimuso picoMIDI expansion board
- Micro-USB data cable
- MIDI cables and MIDI equipment as required
- Raspberry Pi, PC or another USB MIDI host

### Software

- Arduino IDE
- Earle F. Philhower's Raspberry Pi Pico/RP2040 Arduino core
- Adafruit TinyUSB Library with `setCables()` and `readPacket()` support

## Jumper configuration

Use these picoMIDI jumper positions:

| Function | Pico GPIO | UART |
| --- | ---: | --- |
| MIDI IN RX | GPIO 1 | UART0 RX |
| MIDI OUT 1 TX | GPIO 0 | UART0 TX |
| MIDI OUT 2 TX | GPIO 4 | UART1 TX |

These correspond to the board's normal default positions.

## Power configuration

This project uses USB power only:

- Do not connect a 12 V barrel supply.
- Do not connect Eurorack power.
- Leave the two-pin USB power override jumper **off**.
- Connect the Pico's USB port to the Raspberry Pi or other USB host.

## Arduino IDE installation

1. Open **File > Preferences** in Arduino IDE.
2. Add the following Boards Manager URL:

       https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

3. Open **Tools > Board > Boards Manager**.
4. Install **Raspberry Pi Pico/RP2040** by Earle F. Philhower.
5. Open **Tools > Manage Libraries** and install **Adafruit TinyUSB Library**.
6. Open `picoMIDI_USB_Dual_Output.ino`.
7. Select **Tools > Board > Raspberry Pi RP2040 Boards > Raspberry Pi Pico**.
8. Select **Tools > USB Stack > Adafruit TinyUSB**.
9. Upload the sketch.

For the first upload, hold the Pico's **BOOTSEL** button while connecting its
USB cable. Release the button after the Pico appears as a USB storage device,
then upload from Arduino IDE.

## Raspberry Pi OS setup

The Pico should be detected automatically as a USB MIDI interface. List the
ALSA MIDI ports with:

    aconnect -l

For the raw MIDI hardware list, use:

    amidi -l

If those commands are unavailable, install the ALSA utilities:

    sudo apt update
    sudo apt install alsa-utils

The two USB cables may appear as two ports beneath a single MIDI client.

## Testing

### Test MIDI input

1. Connect a keyboard or controller's MIDI OUT to the picoMIDI MIDI IN.
2. Find the Pico's first input port with `aconnect -l`.
3. Monitor it using:

       aseqdump -p <client:port>

For example:

    aseqdump -p 24:0

Playing the controller should produce MIDI events in the terminal.

### Test MIDI output

1. Connect picoMIDI OUT 1 or OUT 2 to a synthesizer's MIDI IN.
2. Route a sequencer or MIDI application to the corresponding Pico USB output.
3. Send some notes.

The Pico's built-in LED flashes whenever outgoing USB MIDI reaches the
firmware:

- If the LED flashes but the synth remains silent, check the TX jumper, MIDI
  cable, selected synth MIDI channel and synth input settings.
- If the LED does not flash, check that the application is connected to the
  correct Pico USB output port.

## How it works

The firmware exposes two virtual USB MIDI cables through Adafruit TinyUSB. It
retains the cable number from each four-byte USB MIDI event packet and routes
the message to the matching hardware UART:

```text
DIN MIDI IN  -- UART0 RX --> USB MIDI port 1 input

USB MIDI port 1 output --> UART0 TX --> DIN MIDI OUT 1
USB MIDI port 2 output --> UART1 TX --> DIN MIDI OUT 2
```

## Troubleshooting

### The Pico does not appear as a MIDI device

- Confirm **Adafruit TinyUSB** was selected as the USB stack before uploading.
- Use a USB data cable rather than a charge-only cable.
- Disconnect and reconnect the Pico after uploading.

### MIDI input works but MIDI output does not

- Confirm OUT 1 uses GPIO 0 and OUT 2 uses GPIO 4.
- Watch the Pico LED while sending MIDI.
- Confirm the MIDI cable runs from picoMIDI **OUT** to the synth's **IN**.
- Check that the host application is using an output port, not one of the Pico
  input ports.

### Two MIDI inputs are displayed

This is expected. Only the first USB input is connected to the physical DIN
input. Ignore the second USB input.

## Firmware

The complete firmware is contained in
[`picoMIDI_USB_Dual_Output.ino`](picoMIDI_USB_Dual_Output.ino).

## Disclaimer

Use this firmware and hardware at your own risk. Check the picoMIDI board
manual and jumper positions before applying power or connecting external MIDI
equipment. Midimuso and Raspberry Pi are trademarks of their respective
owners; this project is not affiliated with or endorsed by them.
