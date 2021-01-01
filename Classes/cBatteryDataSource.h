class cBatteryDataSource: public cDataSource {
public:
    const byte a_address = 0x48;
    const byte v_address = 0x49;

    unsigned char
        Charge = 0; //percentages 0-100
    unsigned long
        Instance = 1;
    double
        Voltage = N2kDoubleNA,
        Current = N2kDoubleNA,
        Temperature = N2kDoubleNA,
        PowerConsumed = 0;

    static bool VoltageValid(double _Voltage){return (1 < abs(_Voltage) && abs(_Voltage) < 48.0);} //we also want to keep negative
    static bool CurrentValid(double _Current){return (0 < _Current && _Current < 50.0);}
    static bool TemperatureValid(double _Temperature){return (-51.0 < _Temperature && _Temperature < 100.0);}

    bool VoltageValid(){return VoltageValid(Voltage);}
    bool CurrentValid(){return CurrentValid(Current);}
    bool TemperatureValid(){return TemperatureValid(Temperature);}
    bool ChargeValid(){return (0 <= Charge && Charge <= 100 && VoltageValid());}
    bool Charging(){return (Voltage >= ChargeTable.chargingVoltage);}
    bool Low(){return (Charge <= GlobalConfig.battery_low);}
    bool PowerConsumedValid(){return (0.0001 < PowerConsumed && PowerConsumed < 120000);} //if we have more than 10 000A/h used

    String VoltageStr(){ return VoltageValid() ? String(Voltage, 2) : EmptyValue;}
    String CurrentStr(){ return CurrentValid() ? String(Current, 2) : EmptyValue;}
    String TemperatureStr(){ return TemperatureValid() ? String(Temperature, 1) : EmptyValue;}
    String ChargeStr(){ return ChargeValid() ? String(Charge) : EmptyValue;}
    String PowerConsumedStr(){ return PowerConsumedValid() ? String(PowerConsumed,1) : EmptyValue;}

    void InitDataSource(cConfig &Config, unsigned char BatteryInstance){
        cDataSource::InitDataSource(Config);
        Instance = BatteryInstance;
        if (BatteryInstance == 1 || BatteryInstance == 2){
            ChargeTable = GlobalConfig.batteries[BatteryInstance - 1];
        }
    }

protected:
    unsigned long ReadTimer;  // Time for readings - because readings are VERY time consuming
    Ammeter ammeter;
    Voltmeter voltmeter;

    double
        _Voltage = N2kDoubleNA,
        _Current = N2kDoubleNA,
        _PowerConsumed = 0;

    const int ReadInterval = 1000;

    cConfig::tBatteryChargeTable ChargeTable;
    double
        TestVoltage = -1;

    unsigned char CalculateCharge(){
        if(!VoltageValid()) return 0;

        if (Voltage <= ChargeTable.volts[0]) return 0;
        if (Charging() || Voltage >= ChargeTable.volts[ChargeTable.TableSize - 1]) return 100;

        //lower and higher limits
        double volts[2] = {0,0}; //LO;HI
        unsigned char percents[2]= {0,0}; //LO;HI
        for (int i = 0; i <  ChargeTable.TableSize; i++){
            volts[1] = ChargeTable.volts[i]; percents[1] = ChargeTable.percents[i]; //get high value
            if (ChargeTable.volts[i] > Voltage){break;}
            volts[0] = ChargeTable.volts[i]; percents[0] = ChargeTable.percents[i]; //get lower value
        }
        if (volts[1] == volts[0]){ return 0; } //if motherfucker puts equals - give him 0%
        return round((Voltage-volts[0]) / (volts[1]-volts[0]) * (percents[1]-percents[0]) + percents[0]);
    }

    cSmoother VSmoother = cSmoother(10, 100); //this means 10 sec avg because reading is 1000ms
    cSmoother ASmoother = cSmoother(10, 100); //this means 10 sec avg because reading is 1000ms

//================================= FROM BASIC CLASS
    void SetDataSourceConfig(){
        DataSourceConfig = GlobalConfig.battery_datasource;
    }

    void SendToN2K(){
        tN2kMsg N2kMsg;
        unsigned char SID = _SID();
        SetN2kDCBatStatus(N2kMsg, Instance, Voltage, Current, Temperature, SID);
        SendN2KMessage(N2kMsg);
    }

    bool RefreshDataFromDevice(){
        if (millis() > ReadTimer + ReadInterval) {
            ReadTimer = millis();
            // getCurrent return in mA,
            // abs value to protect from reverse polarity
            // Converting to A and apply shunt ratio
            if (i2cDeviceAvailable(a_address)){
                _Current = abs(ammeter.getCurrent()) * GlobalConfig.ameter_shunt_ratio / 1000;
                ASmoother.Smooth(&_Current);
            }
            else{
                _Current = N2kDoubleNA;
            }
            //getVoltage return in mv
            if (i2cDeviceAvailable(v_address)){
                _Voltage = voltmeter.getVoltage() / 1000;
                VSmoother.Smooth(&_Voltage);
            }
            else{
                _Voltage = N2kDoubleNA;
            }

            if (VoltageValid(_Voltage)) _Voltage += ChargeTable.offset;
            //must be here because of ReadInterval
            if (VoltageValid(_Voltage) && CurrentValid(_Current)) _PowerConsumed += _Current * _Voltage / (3600 * 1000 / ReadInterval);
        }
        //because of high measurement interval we keep the previous data;
        Current = _Current;
        Voltage = _Voltage;
        PowerConsumed = _PowerConsumed;
        //at least something is valid
        return (CurrentValid() || VoltageValid());
    }

    void RefreshDataFromTest(){
        if (TestVoltage < 11.8){ TestVoltage = 13;}
        TestVoltage -= 0.001;
        Voltage = TestVoltage;
        Current = 2 + MyRandDouble(-1.0, 1.0);
        Temperature = 35 + MyRandDouble(-5.0, 5.0);
        if (CurrentValid() && VoltageValid()){
            PowerConsumed += Current * Voltage / (3600 * 1000 / GlobalConfig.page_refresh_speed) * (100/60); // because test data mutates once per page refresh in 60% cases
        }
    }

    void ProcessData(){
        Charge = CalculateCharge();
    }

    void ResetData(){
        Voltage = N2kDoubleNA;
        Current = N2kDoubleNA;
        Temperature = N2kDoubleNA;
        Charge = 0;
    }

    void InitHardware(){
        ammeter.setMode(SINGLESHOT);
        ammeter.setRate(RATE_128); //speed of measuring RATE_128 = 1000/128 = 8ms
        ammeter.setGain(PAG_256);
        voltmeter.setMode(SINGLESHOT);
        voltmeter.setRate(RATE_128);  //speed of measuring RATE_128 = 1000/128 = 8ms
        voltmeter.setGain(PAG_512); //16V
    }
};