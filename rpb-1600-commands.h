/**
 * This file contains #defines for all of the commands supported by the RPB-1600
 * It also includes data length and N values for decoding/encoding values
 * See the PMBus 1.1 specifications "Linear data" section for more info
 */

// Command Codes
#define CMD_CODE_OPERATION 0x01
#define CMD_CODE_ON_OFF_CONFIG 0x02
#define CMD_CODE_CAPABILITY 0x19
#define CMD_CODE_VOUT_MODE 0x20
#define CMD_CODE_VOUT_COMMAND 0x21
#define CMD_CODE_VOUT_TRIM 0x22
#define CMD_CODE_IOUT_OC_FAULT_LIMIT 0x46
#define CMD_CODE_IOUT_OC_FAULT_RESPONSE 0x47
#define CMD_CODE_STATUS_WORD 0x79
#define CMD_CODE_STATUS_VOUT 0x7A
#define CMD_CODE_STATUS_IOUT 0x7B
#define CMD_CODE_STATUS_INPUT 0x7C
#define CMD_CODE_STATUS_TEMPERATURE 0x7E
#define CMD_CODE_STATUS_CML 0x7E
#define CMD_CODE_STATUS_MFR_SPECIFIC 0x80
#define CMD_CODE_STATUS_FANS_1_2 0x81
#define CMD_CODE_READ_VIN 0x88
#define CMD_CODE_READ_VOUT 0x8B
#define CMD_CODE_READ_IOUT 0x8C
#define CMD_CODE_READ_FAN_SPEED_1 0x90
#define CMD_CODE_READ_FAN_SPEED_2 0x91
#define CMD_CODE_PMBUS_REVISION 0x98
#define CMD_CODE_MFR_ID 0x99
#define CMD_CODE_MFR_MODEL 0x9A
#define CMD_CODE_MFR_REVISION 0x9B
#define CMD_CODE_MFR_LOCATION 0x9C
#define CMD_CODE_MFR_DATE 0x9D
#define CMD_CODE_MFR_SERIAL 0x9E

// Charing Curve config command codes
#define CMD_CODE_CURVE_CC 0xB0
#define CMD_CODE_CURVE_CV 0xB1
#define CMD_CODE_CURVE_FV 0xB2
#define CMD_CODE_CURVE_TC 0xB3
#define CMD_CODE_CURVE_CONFIG 0xB4
#define CMD_CODE_CURVE_CC_TIMEOUT 0xB5
#define CMD_CODE_CURVE_CV_TIMEOUT 0xB6
#define CMD_CODE_CURVE_FLOAT_TIMEOUT 0xB7
#define CMD_CODE_CHG_STATUS 0xB8

// Command lengths in bytes
#define CMD_LENGTH_OPERATION 1
#define CMD_LENGTH_ON_OFF_CONFIG 1
#define CMD_LENGTH_CAPABILITY 1
#define CMD_LENGTH_VOUT_MODE 1
#define CMD_LENGTH_VOUT_COMMAND 2
#define CMD_LENGTH_VOUT_TRIM 2
#define CMD_LENGTH_IOUT_OC_FAULT_LIMIT 2
#define CMD_LENGTH_IOUT_OC_FAULT_RESPONSE 1
#define CMD_LENGTH_STATUS_WORD 2
#define CMD_LENGTH_STATUS_VOUT 1
#define CMD_LENGTH_STATUS_IOUT 1
#define CMD_LENGTH_STATUS_INPUT 1
#define CMD_LENGTH_STATUS_TEMPERATURE 1
#define CMD_LENGTH_STATUS_CML 1
#define CMD_LENGTH_STATUS_MFR_SPECIFIC 1
#define CMD_LENGTH_STATUS_FANS_1_2 1
#define CMD_LENGTH_READ_VIN 2
#define CMD_LENGTH_READ_VOUT 2
#define CMD_LENGTH_READ_IOUT 2
#define CMD_LENGTH_READ_FAN_SPEED_1 2
#define CMD_LENGTH_READ_FAN_SPEED_2 2
#define CMD_LENGTH_PMBUS_REVISION 1
#define CMD_LENGTH_MFR_ID 12
#define CMD_LENGTH_MFR_MODEL 12
#define CMD_LENGTH_MFR_REVISION 6
#define CMD_LENGTH_MFR_LOCATION 3
#define CMD_LENGTH_MFR_DATE 6
#define CMD_LENGTH_MFR_SERIAL 12

// Charing Curve config command lengths in bytes
#define CMD_LENGTH_CURVE_CC 2
#define CMD_LENGTH_CURVE_CV 2
#define CMD_LENGTH_CURVE_FV 2
#define CMD_LENGTH_CURVE_TC 2
#define CMD_LENGTH_CURVE_CONFIG 2
#define CMD_LENGTH_CURVE_CC_TIMEOUT 2
#define CMD_LENGTH_CURVE_CV_TIMEOUT 2
#define CMD_LENGTH_CURVE_FLOAT_TIMEOUT 2
#define CMD_LENGTH_CHG_STATUS 2

// Linear data N values
#define CMD_N_VALUE_VOUT_MODE -9
#define CMD_N_VALUE_VOUT_COMMAND -9
#define CMD_N_VALUE_VOUT_TRIM -9
#define CMD_N_VALUE_IOUT_OC_FAULT_LIMIT -2
#define CMD_N_VALUE_READ_VIN -1
#define CMD_N_VALUE_READ_VOUT -9
#define CMD_N_VALUE_READ_IOUT -2
#define CMD_N_VALUE_READ_FAN_SPEED_1 5
#define CMD_N_VALUE_READ_FAN_SPEED_2 5
#define CMD_N_VALUE_CURVE_CC -2
#define CMD_N_VALUE_CURVE_CV -9
#define CMD_N_VALUE_CURVE_FV -9
#define CMD_N_VALUE_CURVE_TC -2
#define CMD_N_VALUE_CURVE_CC_TIMEOUT 0
#define CMD_N_VALUE_CURVE_CV_TIMEOUT 0
#define CMD_N_VALUE_CURVE_FLOAT_TIMEOUT 0