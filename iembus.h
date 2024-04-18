#ifndef IEMBus_h
#define IEMBus_h

#include "iembus.h"
#include "driver/twai.h"
#include "Arduino.h"

enum CANID_t {
    HallEffect = 0b0000,
    DutyCycle = 0b0011,
    BatteryVoltage = 0b0100,
    BatteryCurrent = 0b0110,
    ButtonPresses = 0b1001,
    RampSetting = 0b1010,
};

enum CANCode_t {
    TelemetryCode = 0b0001,
    AllCode = 0b0000,
};

enum CANMask_t {
    TelemetryMask = 0b1110,
    AllMask = 0b1111,
};

class IEMBus 
{
    private:

        uint8_t RX_PIN = 43;
        uint8_t TX_PIN = 44;
        uint8_t POLLING_RATE_MS = 10;

        uint32_t ALERTS = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
        // uint32_t ALERTS = TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
        
        int bus_errors = 0;
        bool ready = false;
        
        uint32_t alerts_triggered;

        twai_status_info_t twaistatus;

        twai_general_config_t g_config;
        twai_timing_config_t t_config;
        twai_filter_config_t f_config;

    public:

        twai_message_t message;

        IEMBus();
        bool init(uint8_t rx_pin, uint8_t tx_pin, uint8_t polling_rate_ms, CANCode_t rx_code, CANMask_t rx_mask);
        bool start(uint32_t alerts_to_enable);
        bool stop();
        bool reconfig_alerts(uint32_t alerts_to_enable);
        void get_events();

        bool receive();
        void print_errors();
        void print_msg_bytes();

        void float2array(uint8_t data_array[4], float data_float);
        float array2float(uint8_t data_array[4]);

        twai_message_t ready_msg(CANID_t message_id, uint8_t data_array[4]);
        bool transmit(twai_message_t tx_message);

        uint32_t RX_ALERTS = ALERTS;
        uint32_t TX_ALERTS = TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;

};

#endif