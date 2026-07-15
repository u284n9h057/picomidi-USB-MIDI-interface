/*
 * picoMIDI dual-output USB MIDI interface (option 4)
 *
 * USB cable 1 -> DIN MIDI OUT 1 (GPIO 0 / UART0 TX)
 * USB cable 2 -> DIN MIDI OUT 2 (GPIO 4 / UART1 TX)
 * DIN MIDI IN  -> USB cable 1 input (GPIO 1 / UART0 RX)
 */

#include <Adafruit_TinyUSB.h>

constexpr uint32_t MIDI_BAUD = 31250;
constexpr uint8_t USB_MIDI_CABLES = 2;

// One USB MIDI interface containing two independently routable virtual cables.
Adafruit_USBD_MIDI usbMidi;

constexpr uint32_t ACTIVITY_LED_MS = 30;
uint32_t activityLedUntil = 0;

// Number of MIDI bytes carried by each USB-MIDI 1.0 Code Index Number.
constexpr uint8_t CIN_LENGTH[16] = {
  0, 0, 2, 3, 3, 1, 2, 3,
  3, 3, 3, 3, 2, 2, 3, 1
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(MIDI_BAUD);

  Serial2.setTX(4);
  Serial2.begin(MIDI_BAUD);

  // Cable count must be configured before the USB MIDI interface starts.
  usbMidi.setCables(USB_MIDI_CABLES);
  usbMidi.begin();
}

void sendPacketToDin(const uint8_t packet[4]) {
  const uint8_t cable = packet[0] >> 4;
  const uint8_t cin = packet[0] & 0x0F;
  const uint8_t length = CIN_LENGTH[cin];

  if (length == 0) {
    return;
  }

  HardwareSerial *destination = nullptr;
  if (cable == 0) {
    destination = &Serial1;  // USB cable/port 1 -> DIN OUT 1
  } else if (cable == 1) {
    destination = &Serial2;  // USB cable/port 2 -> DIN OUT 2
  } else {
    return;
  }

  destination->write(packet + 1, length);
}

void loop() {
  // The single physical DIN input is presented on USB cable/port 1.
  while (Serial1.available() > 0) {
    usbMidi.write(static_cast<uint8_t>(Serial1.read()));
  }

  // Read complete USB-MIDI event packets so their cable number is retained.
  uint8_t packet[4];
  while (usbMidi.readPacket(packet)) {
    sendPacketToDin(packet);
    digitalWrite(LED_BUILTIN, HIGH);
    activityLedUntil = millis() + ACTIVITY_LED_MS;
  }

  if (digitalRead(LED_BUILTIN) == HIGH &&
      static_cast<int32_t>(millis() - activityLedUntil) >= 0) {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
