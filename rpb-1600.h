

struct readings
{
    uint16_t v_in;
    uint16_t v_out;
    uint16_t i_out;
    uint16_t fan_speed_1;
    uint16_t fan_speed_2;
};

struct mfr_data
{
    char id [12];
    char model [12];
    char revision [6];
    char location [3];
    char date [6];
    char serial [12];
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
    // From datasheet: When Temperature Compensation Short occurs, the charger output will shut down and the                    LED indicator will turn red. The charger will automatically restart after the Temperature                   Compensation Short condition is removed.
    bool temp_compensation_short_circuit;
    // From datasheet: When there is no battery detected, the charger stops charging the battery and the LED               indicator turns red. The charger needs to re-power on to re-start charging the battery
    bool battery_detected;
    // From datasheet: When timeout arises in the Constant Current stage, the charger stops charging the battery                   and the LED indicator turns red. The charger needs to re-power on to re-start charging the                   battery
    bool timeout_flag_cc_mode;
    // From datasheet: When timeout arises in the Constant Voltage stage, the charger stops charging the battery                   and the LED indicator turns red. The charger needs to re-power on to re-start charging the                   battery
    bool timeout_flag_cv_mode;
    // From datasheet: When timeout arises in the Float stage, the charger stops charging the battery and the LED                   indicator turns green. This charging flow is finished; the charger needs to re-power on to                   start charging a different battery
    bool timeout_flag_float_mode;
};

struct curve_data
{
    uint16_t cc;
    uint16_t cv;
    uint16_t floating_voltage;
    uint16_t taper_current;
    curve_config config;
    uint16_t cc_timeout;
    uint16_t cv_timeout;
    charge_status status;
};



class RPB_1600
{
    public:
        


    private:

}