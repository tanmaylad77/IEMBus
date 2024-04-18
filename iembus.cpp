#include "Arduino.h"
#include "iembus.h"

IEMBus::IEMBus() {
}

// Initialise CAN chip here
bool IEMBus::init(uint8_t rx_pin = 43, uint8_t tx_pin = 44, uint8_t polling_rate_ms = 10, CANCode_t rx_code=AllCode, CANMask_t rx_mask=AllMask, twai_mode_t mode=TWAI_MODE_LISTEN_ONLY) {
    RX_PIN = rx_pin;
    TX_PIN = tx_pin;
    POLLING_RATE_MS = polling_rate_ms;

    // EDIT THESE FOR NOW, DEFINE FUNCTIONS TO SET THESE - TO:DO
    g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_PIN, (gpio_num_t)RX_PIN, mode);
    t_config = TWAI_TIMING_CONFIG_250KBITS();
    if (rx_code == AllCode) {
        f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    }
    else {
        f_config.acceptance_code = rx_code;
        f_config.acceptance_mask = rx_mask;
        f_config.single_filter = true;
    }
    
    f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ready = true;
        Serial.println("OK: IEMBus Driver Installed Successfully");
    } else {
        ready = false;
        Serial.println("ERROR: IEMBus Driver Not Installed");
    }

    return true;
}

bool IEMBus::start(uint32_t alerts_to_enable) {

    if (twai_start() == ESP_OK) {
        ready = true;
        Serial.println("OK: IEMBus Started");
    } else {
        ready = false;
        Serial.println("ERROR: IEMBus Not Started");
    }

    // TO:DO USE GET/SET FUNCTIONS
    reconfig_alerts(alerts_to_enable);

    return ready;
}

bool IEMBus::stop() {
    return (twai_stop());
}

bool IEMBus::reconfig_alerts(uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL) {
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
        ready = true;
        Serial.println("OK: IEMBus Alerts Configured");
    } else {
        ready = false;
        Serial.println("ERROR: IEMBus Not Configured");
    }
    return ready;
}

void IEMBus::get_events() {
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS)); 
    twai_get_status_info(&twaistatus);
}

void IEMBus::print_errors() {
    if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
        Serial.println("Alert: TWAI controller has become error passive.");
    }
    if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
        Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
        Serial.printf("Bus error count: %d\n", twaistatus.bus_error_count);
    }
    if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
        Serial.println("Alert: The RX queue is full causing a received frame to be lost.");
        Serial.printf("RX buffered: %d\t", twaistatus.msgs_to_rx);
        Serial.printf("RX missed: %d\t", twaistatus.rx_missed_count);
        Serial.printf("RX overrun %d\n", twaistatus.rx_overrun_count);
    }
}

// Receive a message and store it internally
// MUST call in a loop i.e.
// while (bus.receive()) {
//  [do something, like print_msg_bytes()]
// }
bool IEMBus::receive() {
    bool msg_received = (twai_receive(&message, 0) == ESP_OK);
    return msg_received;
}

// Print the last message (internally stored) bytes to Serial monitor 
void IEMBus::print_msg_bytes() {
    if (message.extd) {
        Serial.print("[DEBUG] Extended Format - ");
    } else {
        Serial.print("[DEBUG] Standard Format - ");
    }
    Serial.print("Data: ");
    for (int i = 0; i < message.data_length_code; i++) {
        Serial.print(message.data[i]);
        Serial.print(" ");
    }
    Serial.println("");
}

void IEMBus::float2array(uint8_t data_array[8], float data_float) {
    uint32_t data_int;
    memcpy(&data_int, &data_float, sizeof data_int);
    for (int i = 0; i < 4; i++) {
        data_array[i] = (uint8_t) (data_int >> i*8) & 0xFF;
    }
}

float IEMBus::array2float(uint8_t data_array[8]) {
    uint32_t data_int = 0;
    for (int i = 0; i < 4; i++) {
        data_int = data_int | (data_array[i] << i*8);
    }
    float data_float;
    memcpy(&data_float, &data_int, sizeof data_float);
    return data_float;
}

// Message maker
twai_message_t IEMBus::ready_msg(CANID_t message_id, uint8_t data_array[8]) {
    twai_message_t tx_message;
    tx_message.identifier = (uint32_t) message_id;
    tx_message.data_length_code = 4;
    for (int i = 0; i < 4; i++) {
        tx_message.data[i] = data_array[i];
    }
    return tx_message;
}

// Transmit
bool IEMBus::transmit(twai_message_t tx_message) {
    if (twai_transmit(&tx_message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        Serial.println("[OK] Message queued for transmission");
    } else {
        Serial.println("[ERROR] Could not transmit");
    }
    return false;
}
