#include "driver/twai.h"
#include "iembus.h"

// Pins used to connect to CAN bus transceiver:
#define RX_PIN 43
#define TX_PIN 44
#define POLLING_RATE_MS 10

IEMBus bus;

void setup() {
  // Start Serial:
  Serial.begin(115200);

  bus.init(RX_PIN, TX_PIN, POLLING_RATE_MS, AllCode, AllMask, TWAI_MODE_LISTEN_ONLY);
  bus.start(bus.RX_ALERTS);
  
  // Can be used to reconfigure alerts after bus.start is called
  //bus.reconfig_alerts(bus.RX_ALERTS);

}

void loop() {

  bus.get_events();
  bus.print_errors();

  if (bus.alerts_triggered & TWAI_ALERT_RX_DATA) {
    while (bus.receive()) {
      bus.print_msg_bytes();
    }
  }
}