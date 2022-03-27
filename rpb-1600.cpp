#include "rpb-1600.h"
#include "rpb-1600-commands.h"
#include "Wire.h"

//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------

RPB_1600::RPB_1600()
{
}

bool RPB_1600::Init(uint8_t chargerAddress)
{
    my_charger_address = chargerAddress;

    Wire.setClock(100000);
    Wire.begin();
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Init complete!\n");
#endif
    return true;
}

bool RPB_1600::getReadings(readings *data)
{
    if (!readWithCommand(CMD_CODE_READ_VIN, CMD_LENGTH_READ_VIN))
    {
        return false;
    }

    data->v_in = parseLinearData();

    if (!readWithCommand(CMD_CODE_READ_VOUT, CMD_LENGTH_READ_VOUT))
    {
        return false;
    }

    data->v_out = parseLinearVoltage(CMD_N_VALUE_READ_VOUT);

    if (!readWithCommand(CMD_CODE_READ_IOUT, CMD_LENGTH_READ_IOUT))
    {
        return false;
    }

    data->i_out = parseLinearData();

    if (!readWithCommand(CMD_CODE_READ_FAN_SPEED_1, CMD_LENGTH_READ_FAN_SPEED_1))
    {
        return false;
    }

    data->fan_speed_1 = parseLinearData();

    if (!readWithCommand(CMD_CODE_READ_FAN_SPEED_2, CMD_LENGTH_READ_FAN_SPEED_2))
    {
        return false;
    }

    data->fan_speed_2 = parseLinearData();

    return true;
}

bool RPB_1600::getChargeStatus(charge_status *status)
{
    if (!readWithCommand(CMD_CODE_CHG_STATUS, CMD_LENGTH_CHG_STATUS))
    {
        return false;
    }

    parseChargeStatus(status);

    return true;
}

bool RPB_1600::getCurveParams(curve_parameters *params)
{
    /* Query the charger for all charge curve related commands, and populate the following items:
    params->cc;
    params->cv;
    params->floating_voltage;
    params->taper_current;
    params->config;
    params->cc_timeout;
    params->cv_timeout;
    params->status; */

    if (!readWithCommand(CMD_CODE_CURVE_CC, CMD_LENGTH_CURVE_CC))
    {
        return false;
    }

    params->cc = parseLinearData();

    if (!readWithCommand(CMD_CODE_CURVE_CV, CMD_LENGTH_CURVE_CV))
    {
        return false;
    }

    params->cv = parseLinearVoltage(CMD_N_VALUE_CURVE_CV);

    if (!readWithCommand(CMD_CODE_CURVE_FV, CMD_LENGTH_CURVE_FV))
    {
        return false;
    }

    params->floating_voltage = parseLinearVoltage(CMD_N_VALUE_CURVE_FV);

    if (!readWithCommand(CMD_CODE_CURVE_TC, CMD_LENGTH_CURVE_TC))
    {
        return false;
    }

    params->taper_current = parseLinearData();

    if (!readWithCommand(CMD_CODE_CURVE_CONFIG, CMD_LENGTH_CURVE_CONFIG))
    {
        return false;
    }

    parseCurveConfig(&params->config);

    if (!readWithCommand(CMD_CODE_CURVE_CC_TIMEOUT, CMD_LENGTH_CURVE_CC_TIMEOUT))
    {
        return false;
    }

    params->cc_timeout = parseLinearData();

    if (!readWithCommand(CMD_CODE_CURVE_CV_TIMEOUT, CMD_LENGTH_CURVE_CV_TIMEOUT))
    {
        return false;
    }

    params->cv_timeout = parseLinearData();

    if (!readWithCommand(CMD_CODE_CURVE_FLOAT_TIMEOUT, CMD_LENGTH_CURVE_FLOAT_TIMEOUT))
    {
        return false;
    }

    params->float_timeout = parseLinearData();

    getChargeStatus(&params->status);

    return true;
}

bool RPB_1600::readWithCommand(uint8_t commandID, uint8_t receiveLength)
{
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Attempting to read command 0x%x with length %d\n", commandID, receiveLength);
#endif
    // Send the command ID to the charger, and don't terminate the transmission
    Wire.beginTransmission(my_charger_address);
    Wire.write(commandID); // Write to I2C Tx buffer
    Wire.endTransmission(false);

    // Request bytes from the device
    Wire.beginTransmission(my_charger_address);
    Wire.requestFrom(my_charger_address, receiveLength);

    clearRXBuffer();

    // Count the number of bytes we receive
    uint8_t num_bytes = 0;

    // Pull bytes from the internal i2c RX buffer
    while (Wire.available())
    {
        // Write the received byte to the buffer
        my_rx_buffer[num_bytes] = Wire.read();

#ifdef RPB_1600_DEBUG
        Serial.printf("<RPB-1600 DEBUG> Received byte %d with value 0x%x\n", num_bytes, my_rx_buffer[num_bytes]);
#endif

        // Make sure we aren't getting more bytes then we expect
        if (++num_bytes > MAX_RECEIVE_BYTES)
        {
            return false;
        }
    }

#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Total bytes received: %d\n", num_bytes);
    Serial.printf("<RPB-1600 DEBUG> RX Buffer: [");
    for (int i = 0; i < num_bytes; i++)
    {
        Serial.printf("0x%x,", my_rx_buffer[i]);
    }
    Serial.printf("]\n");
#endif

    return num_bytes == receiveLength;
}

bool RPB_1600::writeTwoBytes(uint8_t commandID, uint8_t *data)
{
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Attempting to write 0x%x (low) and 0x%x (high) with command 0x%x\n", data[0], data[1], commandID);
#endif

    Wire.beginTransmission(my_charger_address);
    Wire.write(commandID);
    uint8_t bytes_written = Wire.write(data, 2);
    Wire.endTransmission();

    return bytes_written == 2;
}

bool RPB_1600::writeLinearDataCommand(uint8_t commandID, int8_t N, int16_t value)
{
    uint16_t Y = value;

    // The Y value is calculated by doing the following: Y = value * 2 ^ N
    // however, N can be positive or negative, so we shift the mantissa accordingly
    // to perform the multiplication by a power of 2.
    if (N > 0)
    {
        Y <<= N;
    }
    else if (N < 0)
    {
        Y >>= (-N);
    }

    return writeLinearDataHelper(commandID, N, Y);
}

//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------

bool RPB_1600::writeLinearDataHelper(uint8_t commandID, int8_t N, int16_t Y)
{
    // Make sure the N value isn't bigger then 5 bits
    if (abs(N) > 15)
    {
#ifdef RPB_1600_DEBUG
        Serial.printf("<RPB-1600 DEBUG> N value too large! Can't convert to linear format. N = %d\n", N);
#endif
        return false;
    }

    // Make sure the Y (Mantissa) can fit in 11 bits
    if (abs(Y) > 2047)
    {
#ifdef RPB_1600_DEBUG
        Serial.printf("<RPB-1600 DEBUG> Mantissa (Y) value too large! Can't convert to linear format. Y = %d\n", Y);
#endif
        return false;
    }

    // The goal here is to truncate Y to 11 bits, and put those in the lowest 11 bits of the outgoing data
    // Then we need to truncate N to 5 bits, and put those bits in the highest 5 bits of the outgoing data
    // See Section 7.1 of the PMBus 1.1 specification for more info on the "Linear Data Format"
    // Note that data[0] is the low byte (sent first) and data[1] is the high byte (sent second)
    uint8_t data[2] = {0x00, 0x00};

    // Mask the lowest 8 bits of the mantissa, and put those bits in the low byte of the outgoing data
    data[0] = Y & 0x00FF;
    // Mask the lowest 3 bits of the high byte of the mantissa, and put those bits in the high byte of the outgoing data
    data[1] = (Y & 0x0700) >> 8;
    // Mask the lowest 5 bits of N, and shift them to be the highest 5 bytes of the high byte
    data[1] = (N & 0x1F) << 3;
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Attempting to write linear data with N = %d and Value (Y) = %d\n", N, Y);
#endif
    writeTwoBytes(commandID, data);

    return true;
}

uint16_t RPB_1600::parseLinearData(void)
{
    uint16_t rawData = my_rx_buffer[0] | (my_rx_buffer[1] << 8);

    // Mask & shift out the "N" and "mantissa" values and convert them from 2s complement
    int16_t rawN = (rawData & N_EXPONENT_MASK) >> N_EXPONENT_SHIFT;
    int16_t N = UpscaleTwosComplement(rawN, N_EXPONENT_LENGTH);
    int16_t rawMantissa = (rawData & MANTISSA_MASK);
    int16_t mantissa = UpscaleTwosComplement(rawMantissa, MANTISSA_LENGTH);

    uint16_t result = static_cast<uint16_t>(mantissa);
    // The result is calculated by doing the following: result = mantissa * 2 ^ N
    // however, N can be positive or negative, so we shift the mantissa accordingly
    // to perform the multiplication by a power of 2.
    if (N > 0)
    {
        result <<= N;
    }
    else if (N < 0)
    {
        result >>= (-N);
    }
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> Raw Data: 0x%x\n", rawData);
    Serial.printf("<RPB-1600 DEBUG> Raw N: 0x%x\n", rawN);
    Serial.printf("<RPB-1600 DEBUG> N: %d\n", N);
    Serial.printf("<RPB-1600 DEBUG> Raw Mantissa : 0x%x\n", rawMantissa);
    Serial.printf("<RPB-1600 DEBUG> Mantissa: %d\n", mantissa);
    Serial.printf("<RPB-1600 DEBUG> Result: %d\n", result);
#endif

    return result;
}

float RPB_1600::parseLinearVoltage(int8_t N)
{
    uint16_t rawData = my_rx_buffer[0] | (my_rx_buffer[1] << 8);

    // Calculate divisor using N
    uint16_t divisor = 0x01 << abs(N);

    Serial.printf("<RPB-1600 DEBUG> Parsing linear voltage w/N: %d\n", N);

    float result = (float)rawData;

    // Divide based on the polarity of N
    if (N > 0)
    {
        result /= -divisor;
    }
    else if (N < 0)
    {
        result /= divisor;
    }

    Serial.printf("<RPB-1600 DEBUG> Linear voltage result: %4.2fV\n", result);

    return result;
}

void RPB_1600::parseCurveConfig(curve_config *config)
{
    // Bits 0 & 1 of low byte
    config->charge_curve_type = (my_rx_buffer[0] & 0x02);
    // Bits 2 & 3 of low byte
    config->temp_compensation = (my_rx_buffer[0] & 0x0C) >> 2;
    // Bit 7 of low byte (0 = 3 stage, 1 = 2 stage)
    config->num_charge_stages = (my_rx_buffer[0] && 0x40) ? 2 : 3;
    // Bit 0 of high byte
    config->cc_timeout_indication_enabled = (my_rx_buffer[1] && 0x01);
    // Bit 1 of high byte
    config->cv_timeout_indication_enabled = (my_rx_buffer[1] && 0x02);
    // Bit 2 of high byte
    config->float_stage_timeout_indication_enabled = (my_rx_buffer[1] && 0x04);
}

void RPB_1600::parseChargeStatus(charge_status *status)
{
    // Low byte:
    status->fully_charged = (my_rx_buffer[0] && 0x01); // Bit 0
    status->in_cc_mode = (my_rx_buffer[0] && 0x02);    // Bit 1
    status->in_cv_mode = (my_rx_buffer[0] && 0x04);    // Bit 2
    status->in_float_mode = (my_rx_buffer[0] && 0x08); // Bit 3
    // High byte:
    status->EEPROM_error = (my_rx_buffer[1] && 0x01);                    // Bit 0
    status->temp_compensation_short_circuit = (my_rx_buffer[1] && 0x04); // Bit 1
    status->battery_detected = (my_rx_buffer[1] && 0x08);                // Bit 3
    status->timeout_flag_cc_mode = (my_rx_buffer[1] && 0x20);            // Bit 5
    status->timeout_flag_cv_mode = (my_rx_buffer[1] && 0x40);            // Bit 6
    status->timeout_flag_float_mode = (my_rx_buffer[1] && 0x80);         // Bit 7
}

// Slightly modified version of this https://www.codeproject.com/Tips/1079637/Twos-Complement-for-Unusual-Integer-Sizes
int16_t RPB_1600::UpscaleTwosComplement(int16_t value, size_t length)
{
    uint16_t mask = (~0) << length;
    // Too small for complement?
    if (length < 2)
    {
        return (~mask & value);
    }
    // Two's complement?
    uint16_t msb = 1 << (length - 1);
    if (value & msb)
    {
        return (mask | value);
    }
    else
    {
        return (~mask & value);
    }
}

void RPB_1600::clearRXBuffer(void)
{
    memset(my_rx_buffer, 0, MAX_RECEIVE_BYTES);
#ifdef RPB_1600_DEBUG
    Serial.printf("<RPB-1600 DEBUG> RX Buffer Cleared!\n");
#endif
}