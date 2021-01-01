class cEnvDataSource: public cDataSource {
public:
    const byte p_address = 0x76; //BMP280
    const byte ht_address = 0x44; //SHT30

    double
        Temp = N2kDoubleNA,
        Humidity = N2kDoubleNA,
        Pressure = N2kDoubleNA;

    bool TempValid(){return abs(Temp) < 100;}
    bool HumidityValid(){return (1 < Humidity && Humidity <= 100);}
    bool PressureValid(){return (900 <= Pressure && Pressure <= 1150);}

    String TempStr(){ return TempValid() ? String(Temp, 1) : EmptyValue;}
    String HumidityStr(){ return HumidityValid() ? String(Humidity, 0) : EmptyValue;}
    String PressureStr(){ return PressureValid() ? String(Pressure, 0) : EmptyValue;}

protected:
    SHT3X sht30;
    Adafruit_BMP280 bme;
    unsigned long DeviceReadTimer;
    double
        _Temp = N2kDoubleNA,
        _Humidity = N2kDoubleNA,
        _Pressure = N2kDoubleNA;
    bool PresureDeviceReinitMust = false;

//================================= FROM BASIC CLASS
    void SetDataSourceConfig(){
        DataSourceConfig = GlobalConfig.env_datasource;
    }

    void SendToN2K(){}

    bool RefreshDataFromDevice(){
        if (millis() > DeviceReadTimer + 5000){
            DeviceReadTimer = millis();

            //Pressure
            if (i2cDeviceAvailable(p_address)){
                if (PresureDeviceReinitMust){
                    bme.begin(p_address);
                    PresureDeviceReinitMust = false;
                }
                _Pressure = bme.readPressure() / 100; //hpa
            }
            else{
                PresureDeviceReinitMust = true;
                _Pressure = N2kDoubleNA;
            }

            //Humidity and Temp
            if(i2cDeviceAvailable(ht_address) && sht30.get()==0){
                _Temp = sht30.cTemp; //C
                _Humidity = sht30.humidity; //%
            }
            else{
                _Temp = N2kDoubleNA;
                _Humidity = N2kDoubleNA;
            }
        }
        Temp = _Temp;
        Humidity = _Humidity;
        Pressure = _Pressure;
        return (TempValid() && HumidityValid() && PressureValid());
    }

    void RefreshDataFromTest(){
        Pressure = MyRandDouble(900, 1100);
        Temp = MyRandDouble(10, 30);
        Humidity = MyRandDouble(30, 99);
    }

    void ResetData(){
        Temp = N2kDoubleNA;
        Humidity = N2kDoubleNA;
        Pressure = N2kDoubleNA;
    }

    void InitHardware(){
        bme.begin(p_address);
    }
};