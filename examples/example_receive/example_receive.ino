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

  bus.init(43, 44, 10);
  
  bus.start(bus.RX_ALERTS);
  
  // Can be used to reconfigure alerts after bus.start is called
  //bus.reconfig_alerts(bus.RX_ALERTS);

}

void loop() {

  twai_message_t message;

  bus.get_events();
  bus.print_errors();

  while (bus.receive()) {
    bus.print_msg_bytes();
  }

}