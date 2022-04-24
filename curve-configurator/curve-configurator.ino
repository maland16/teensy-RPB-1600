#include <rpb-1600-commands.h>
#include <rpb-1600.h>

RPB_1600 charger;

void setup()
{
  Serial.begin(115200);
  charger.Init(0x47);
  delay(500);
}

void loop()
{
  Serial.printf("##################################################################################\n");
  Serial.printf("Welcome to the curve configurator! Type in the number corresponding to an option below and hit enter.\n");
  Serial.printf("1) Read current curve configuration\n");
  Serial.printf("2) Set configuration\n");
  Serial.printf("3) Stream Voltage/Current readings (disable debug #define)\n");
  Serial.printf("##################################################################################\n");

  char input = getInput();

  if (input == '1') // Read curve configuration
  {
    Serial.printf("Attempting to read curve configuration...\n");

    curve_parameters curveInfo{};

    if (!charger.getCurveParams(&curveInfo))
    {
      Serial.printf("Failed to read curve config!!! Restarting...\n");
    }
    else
    {
      // Looks like we got the curve info successfully, print it out
      Serial.printf("------- BEGIN CURVE CONFIGURATION -------\n");
      Serial.printf("Constant Current: %dA\n", curveInfo.cc);
      Serial.printf("Constant Voltage: %4.2fV\n", curveInfo.cv);
      Serial.printf("Floating Voltage: %4.2fV\n", curveInfo.floating_voltage);
      Serial.printf("Taper Current: %dA\n", curveInfo.taper_current);
      Serial.printf("Charge curve setting: ");

      if (curveInfo.config.charge_curve_type == 0)
      {
        Serial.printf("Custom charge curve (default)\n");
      }
      else if (curveInfo.config.charge_curve_type == 1)
      {
        Serial.printf("Gel battery\n");
      }
      else if (curveInfo.config.charge_curve_type == 2)
      {
        Serial.printf("Flooded Battery\n");
      }
      else if (curveInfo.config.charge_curve_type == 3)
      {
        Serial.printf("AGM Battery\n");
      }

      Serial.printf("Temperature compensation: ");

      if (curveInfo.config.temp_compensation == 0)
      {
        Serial.printf("Disabled \n");
      }
      else if (curveInfo.config.temp_compensation == 1)
      {
        Serial.printf("-3 mV/C/Cell (default)\n");
      }
      else if (curveInfo.config.temp_compensation == 2)
      {
        Serial.printf("-4 mV/C/Cell \n");
      }
      else if (curveInfo.config.temp_compensation == 3)
      {
        Serial.printf("-5 mV/C/Cell \n");
      }

      Serial.printf("Number of charge stages: %d\n", curveInfo.config.num_charge_stages);
      Serial.printf("Constant Current Stage timeout indication enable: %d\n", curveInfo.config.cc_timeout_indication_enabled);
      Serial.printf("Constant Voltage Stage timeout indication enable: %d\n", curveInfo.config.cv_timeout_indication_enabled);
      Serial.printf("Float Stage timeout indication enable: %d\n", curveInfo.config.float_stage_timeout_indication_enabled);

      Serial.printf("Constant current timeout: %d\n", curveInfo.cc_timeout);
      Serial.printf("Constant voltage timeout: %d\n", curveInfo.cv_timeout);
      Serial.printf("Float timeout: %d\n", curveInfo.float_timeout);

      Serial.printf("Charge Status:\n");
      Serial.printf("Fully charged: %d\n", curveInfo.status.fully_charged);
      Serial.printf("In Constant Current Mode: %d\n", curveInfo.status.in_cc_mode);
      Serial.printf("In Constnat Voltage Mode: %d\n", curveInfo.status.in_cv_mode);
      Serial.printf("EEPROM Error: %d\n", curveInfo.status.EEPROM_error);
      Serial.printf("Temperature compensation short: %d\n", curveInfo.status.temp_compensation_short_circuit);
      Serial.printf("Battery detected: %d\n", curveInfo.status.battery_detected);
      Serial.printf("Timeout flag CC mode: %d\n", curveInfo.status.timeout_flag_cc_mode);
      Serial.printf("Timeout flag CV mode: %d\n", curveInfo.status.timeout_flag_cv_mode);
      Serial.printf("Timeout flag float mode: %d\n", curveInfo.status.timeout_flag_float_mode);

      Serial.printf("------- END CURVE CONFIGURATION -------\n");
    }
  }
  else if (input == '2') // Set curve configuration
  {
    Serial.printf("Curve configuration setting options:\n");
    Serial.printf("1) Write to arbitrary command/data (write two bytes)\n");
    Serial.printf("2) Write to command with linear data\n");
    Serial.printf("3) Read data via specified command (must have debug #defined uncommented)\n");

    input = getInput();

    if (input == '1')
    {
      Serial.printf("--- WRITE ARBITRARY COMMAND & DATA ---\n");
      Serial.printf("Example HEX data entry: \"40\" = 0x40\n");
      Serial.printf("Type the command in HEX and press ENTER\n");

      uint8_t cmd = getHexByteInput();

      uint8_t data[2];

      Serial.printf("Enter the LOW byte to write and press ENTER\n");

      data[0] = getHexByteInput();

      Serial.printf("Enter the HIGH byte to write and press ENTER\n");

      data[1] = getHexByteInput();

      // TODO: The padding on the hex here isn't correct in certain cases.
      // Ex: if data[0] is 0x72, and data[1] is 0x00, it will print 0x720
      Serial.printf("Attempting to write 0x%x%x with command 0x%x...\n", data[1], data[0], cmd);

      if (charger.writeTwoBytes(cmd, data))
      {
        Serial.printf("Writing succeeded!\n");
      }
      else
      {
        Serial.printf("Writing failed :(\n");
      }
    }
    else if (input == '2') // Write command w/linear data
    {
      Serial.printf("--- WRITE COMMAND W/LINEAR DATA ---\n");

      Serial.printf("Type the command in HEX and press ENTER\n");

      uint8_t cmd = getHexByteInput();

      Serial.printf("Type the N value and press ENTER (negative numbers represented with minus sign)\n");

      while (Serial.available() == 0)
      {
      }; // Wait for input

      int8_t N = (int8_t)Serial.parseInt();
      while (Serial.available())
      {
        Serial.read();
      } // Flush the serial RX buffer

      Serial.printf("Type the value and press ENTER (negative numbers represented with minus sign)\n");

      while (Serial.available() == 0)
      {
      }; // Wait for input

      int16_t value = (int16_t)Serial.parseInt();
      while (Serial.available())
      {
        Serial.read();
      } // Flush the serial RX buffer

      Serial.printf("Command = 0x%x, N = %d, value = %d\n", cmd, N, value);

      if (charger.writeLinearDataCommand(cmd, N, value))
      {
        Serial.printf("Writing succeeded!\n");
      }
      else
      {
        Serial.printf("Writing failed :(\n");
      }
    }
    else if (input == '3') // Read w/ command
    {
      Serial.printf("--- READ WITH COMMAND ---\n");
      Serial.printf("Make sure you have #define RPB_1600_DEBUG uncommented\n");
      Serial.printf("Enter the command in HEX and press ENTER\n");

      uint8_t cmd = getHexByteInput();

      Serial.printf("Enter the expected number of bytes in the reply\n");

      while (Serial.available() == 0)
      {
      }; // Wait for input

      int8_t expected_length = (int8_t)Serial.parseInt();
      while (Serial.available())
      {
        Serial.read();
      } // Flush the serial RX buffer

      Serial.printf("Sending command 0x%x with length %d\n", cmd, expected_length);
      if (charger.readWithCommand(cmd, expected_length))
      {
        Serial.printf("Read succeeded!\n");
      }
      else
      {
        Serial.printf("Read failed :(\n");
      }
    }
    else // Invalid entry within set curve config
    {
      Serial.printf("Invalid option selected, restarting...\n");
    }
  }                      // End set curve config
  else if (input == '3') // Stream data
  {

    readings chargerReadings{};

    while (Serial.available() == 0)
    {
      if (charger.getReadings(&chargerReadings))
      {
        Serial.printf("VIN: %d ", chargerReadings.v_in);
        Serial.printf("VOUT: %4.2f ", chargerReadings.v_out);
        Serial.printf("IOUT: %d \n", chargerReadings.i_out);
        // Serial.printf("Fan speed 1: %d ",chargerReadings.fan_speed_1);
        // Serial.printf("Fan speed 2: %d\n",chargerReadings.fan_speed_2);
      }
      else
      {
        Serial.printf("FAILED TO READ FROM CHARGER. Restarting...\n");
        break; // Leave this loop
      }
      delay(300);
    } // End while (Serial.available() == 0)

    while (Serial.available())
    {
      Serial.read();
    } // Flush the serial RX buffer
  }
  else // Invalid input main menu
  {
    Serial.printf("Invalid option selected, restarting...\n");
  }

  while (Serial.available())
  {
    Serial.read();
  } // Flush the serial RX buffer
} // End loop()

char getInput(void)
{
  while (Serial.available() == 0)
  {
  }
  char input = Serial.read();
  Serial.read(); // To discard the new line that gets sent
  return input;
}

uint8_t getHexByteInput(void)
{
  uint8_t myData[2];
  while (Serial.available() == 0)
  {
  }; // Wait for input
  Serial.readBytesUntil('\n', myData, 10);
  uint8_t myHex[2] = {};

  // Parse out the input characters as HEX
  for (int i = 0; i < 2; i++)
  {
    byte y = myData[i];
    if (y == 0x30)
    {
      myHex[i] = 0;
    }
    else if (y <= 0x39)
    {
      myHex[i] = y - 0x30; // preserve: 1 - 9
    }
    else
    {
      myHex[i] = y - 0x37; // preserve A - F
    }
  }

  return ((uint8_t)(myHex[0] << 4) | (uint8_t)myHex[1]);
}
