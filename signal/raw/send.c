#include "radio.h"

// Function to send a raw signal at 433.92 Mhz in AM650
void send_raw_signal(uint16_t* data, uint8_t len) {
    // Initialize the radio with the specified frequency, modulation, and data rate
    radio_init(RADIO_FREQ_433_92_MHZ, RADIO_MOD_AM_650, RADIO_DATA_RATE_4800_BPS);

    // Set the transmission power to the maximum allowed
    radio_set_tx_pwr(RADIO_TX_PWR_MAX);

    // Set the PA ramp time to 50 microseconds
    radio_set_pa_ramp(RADIO_PA_RAMP_50_US);

    // Send the raw data provided in the data array
    radio_send_raw(data, len);

    // Put the radio to sleep
    radio_sleep();
}