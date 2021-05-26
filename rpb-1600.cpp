#include "rpb-1600.h"
#include "rpb-1600-commands.h"
#include "Wire.h"

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

    data->v_out = parseLinearData();

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
}

bool RPB_1600::getCurveConfig(curve_config *conf)
{
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