
#include <cstdint>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef RPB_1600_H
#define RPB_1600_H

// Uncomment the below #define to enable debugging print statements.
// NOTE: You must call Serial.being(<baud rate>) in your setup() for this to work
#define RPB_1600_DEBUG

/**
 * @brief The maximum number of bytes we could possibly expect to receive from the charger
 */
#define MAX_RECEIVE_BYTES 12

#define N_EXPONENT_MASK 0xF800 // Bitmask for pulling out the first 5 bytes of the payload (the N exponent value)
#define MANTISSA_MASK 0x07FF   // Bitmask for pulling out the last 11 bytes of the payload (the Mantissa)
#define N_EXPONENT_LENGTH 5
#define MANTISSA_LENGTH 11
#define N_EXPONENT_SHIFT MANTISSA_LENGTH

struct readings
{
    uint16_t v_in;
    float v_out;
    uint16_t i_out;
    uint16_t fan_speed_1;
    uint16_t fan_speed_2;
};

struct mfr_data
{
    char id[12];
    char model[12];
    char revision[6];
    char location[3];
    char date[6];
    char serial[12];
};

struct curve_config
{
    uint8_t charge_curve_type;
    uint8_t temp_compensation;
    uint8_t num_charge_stages;
    bool cc_timeout_indication_enabled;
    bool cv_timeout_indication_enabled;
    bool float_stage_timeout_indication_enabled;
};

struct charge_status
{
    bool fully_charged;
    bool in_cc_mode;
    bool in_cv_mode;
    bool in_float_mode;
    // From datasheet:
    // When EEPROM Charge Parameter Error occurs, the charger stops
    // charging the battery and the LED indicator turns red. The
    // charger needs to re-power on to re-start charging the battery.
    bool EEPROM_error;
    // From datasheet: When Temperature Compensation Short occurs, the
    // charger output will shut down and the LED indicator will turn red.
    // The charger will automatically restart after the Temperature
    // Compensation Short condition is removed.
    bool temp_compensation_short_circuit;
    // From datasheet: When there is no battery detected, the charger
    // stops charging the battery and the LED indicator turns red. The
    // charger needs to re-power on to re-start charging the battery
    bool battery_detected;
    // From datasheet: When timeout arises in the Constant Current stage,
    // the charger stops charging the battery and the LED indicator turns
    // red. The charger needs to re-power on to re-start charging the
    // battery
    bool timeout_flag_cc_mode;
    // From datasheet: When timeout arises in the Constant Voltage stage,
    // the charger stops charging the battery and the LED indicator turns
    // red. The charger needs to re-power on to re-start charging the
    // battery
    bool timeout_flag_cv_mode;
    // From datasheet: When timeout arises in the Float stage, the
    // charger stops charging the battery and the LED indicator turns
    // green. This charging flow is finished; the charger needs to
    // re-power on to start charging a different battery
    bool timeout_flag_float_mode;
};

struct curve_parameters
{
    uint16_t cc;
    float cv;
    float floating_voltage;
    uint16_t taper_current;
    curve_config config;
    uint16_t cc_timeout;
    uint16_t cv_timeout;
    uint16_t float_timeout;
    charge_status status;
};

class RPB_1600
{
public:
    RPB_1600();
    bool Init(uint8_t chargerAddress);

    /**
     * @brief Query charger for voltage & current readings, populate a "readings" struct
     * @return true on successful read, false otherwise
     */
    bool getReadings(readings *data);

    /**
     * @brief Query charger for status bytes, populate a "charge_status" struct
     * @return true on successful read, false otherwise
     */
    bool getChargeStatus(charge_status *status);

    /**
     * @brief Query charger for curve parameter bytes, populate a "curve_parameters" struct
     * @return true on successful read, false otherwise
     */
    bool getCurveParams(curve_parameters *params);

    /**
     * @brief Write two arbitrary bytes with commandID
     * @return true on successful write, false otherwise
     */
    bool writeTwoBytes(uint8_t commandID, uint8_t *data);

    /**
     * @brief Write linear data with commandID
     * @details See the PMBus 1.1 Spec for more info on how the linear data format works
     * @param N the exponent
     * @param value the mantissa (Y)
     * @return True on success, false on failure
     */
    bool writeLinearDataCommand(uint8_t commandID, int8_t N, int16_t value);

    /**
     * @brief Sends commandID to the charger, and reads the receiveLength byte(s) long response into my_rx_buffer[] 
     * @return true if we received the number of bytes we were expecting, false otherwise.
     */
    bool readWithCommand(uint8_t commandID, uint8_t receiveLength);

private:
    /**
     * @brief The address of the charger we're communicating with
     * @details This is set using the A0, A1, and A2 pins on the RPB-1600. These three pins control 
     * the lowest 3 bits of the 7 bit address, and the MSB is always 1. For example, if all the 
     * pins are tied high, the address would be 0x47. 
     * @note Address 0 is a reserved address.
     */
    uint8_t my_charger_address;

    /**
     * @brief Buffer to hold bytes received over i2c
     */
    uint8_t my_rx_buffer[MAX_RECEIVE_BYTES];

    /**
     * @brief Parses the first two bytes of my_rx_buffer[] in the "Linear Data" format outlined int the PMBus Specification
     * @details see the PMBus V1.1 Section 7.1 "Linear Data Format" for more info
     */
    uint16_t parseLinearData(void);

    /**
     * @brief Parse a voltage reading in the linear format
     * @details See the PMBus 1.1 spec section 8.3.1 for more info
     */
    float parseLinearVoltage(int8_t N);

    /**
     * @brief Parses the first two bytes of my_rx_buffer[] into a curve_config struct and returns it via argument.
     * @details Meant to be called after calling readWithCommand(CMD_CODE_CURVE_CONFIG, CMD_LENGTH_CURVE_CONFIG)
     */
    void parseCurveConfig(curve_config *config);

    /**
     * @brief Parses the first two bytes of my_rx_buffer[] into a charge_status struct and returns it via argument.
     * @details Meant to be called after calling readWithCommand(CMD_CODE_CHG_STATUS, CMD_LENGTH_CHG_STATUS)
     */
    void parseChargeStatus(charge_status *status);

    /**
     * @brief Takes in a twos complement number that's length bits and converts it to a 16 bit twos complement number
     * @details Slightly modified version of this https://www.codeproject.com/Tips/1079637/Twos-Complement-for-Unusual-Integer-Sizes
     */
    int16_t UpscaleTwosComplement(int16_t value, size_t length);

    /**
     * @brief Zeros my_rx_buffer
     */
    void clearRXBuffer(void);
};

#endif // RPB_1600_H