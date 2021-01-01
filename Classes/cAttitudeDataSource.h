class cAttitudeDataSource: public cDataSource {
public:
    double
        Heel = N2kDoubleNA,
        Trim = N2kDoubleNA,
        HeelNotSmoothed = N2kDoubleNA, //for compass
        TrimNotSmoothed = N2kDoubleNA,
        MaxHeel = 0,
        MaxTrim = 0;

    bool HeelValid(){ return abs(Heel) <= 180;}
    bool TrimValid(){ return abs(Trim) <= 180;}

    String HeelStr(){ return HeelValid() ? String(Heel, 0) : EmptyValue;}
    String TrimStr(){ return TrimValid() ? String(Trim, 0) : EmptyValue;}
    String MaxHeelStr(){ return MaxHeel > 1 ? String(MaxHeel, 0) : EmptyValue;}
    String MaxTrimStr(){ return MaxTrim > 1 ? String(MaxTrim, 0) : EmptyValue;}

protected:
    const byte DeviceAddress = 0x68; //MPU6886
    bool DeviceReinitMust = false;
    unsigned long DeviceReadTimer;
    double
        _Heel = N2kFloatNA,
        _Trim  = N2kFloatNA,
        _HeelNotSmoothed = N2kDoubleNA, //for compass
        _TrimNotSmoothed = N2kDoubleNA;

    cSmoother HeelSmoother = cSmoother();
    cSmoother TrimSmoother = cSmoother();

//================================= FROM BASIC CLASS
protected:
    void InitHardware(){
        M5.IMU.Init();
    }

    void SetDataSourceConfig(){
        DataSourceConfig = GlobalConfig.attitude_datasource;
    }

    bool RefreshDataFromDevice(){

        //It behaves strange with longer delays. Some buffers probably. Had no time to investigate
        // +10ms gives good "fps" benefit while keeps the performance
        if (millis() > DeviceReadTimer + 10){
            DeviceReadTimer = millis();
            if (i2cDeviceAvailable(DeviceAddress)){
                if (DeviceReinitMust) {
                    M5.IMU.Init();
                    DeviceReinitMust = false;
                }
                float z = N2kFloatNA;
                float _h = N2kFloatNA;
                float _t = N2kFloatNA;

                //we MUST get all the data, otherwise it does not work
                M5.IMU.getGyroData(&z,&z,&z);
                M5.IMU.getAccelData(&z,&z,&z);
                M5.IMU.getAhrsData(&_h,&_t,&z);
                _Heel = _HeelNotSmoothed = _h;
                _Trim = _TrimNotSmoothed = _t;
                HeelSmoother.Smooth(&_Heel);
                TrimSmoother.Smooth(&_Trim);
            }
            else{
                DeviceReinitMust = true;
                _Heel = N2kFloatNA;
                _Trim = N2kFloatNA;
                _HeelNotSmoothed = N2kFloatNA;
                _TrimNotSmoothed = N2kFloatNA;
            }
        }
        Heel = (_Heel + GlobalConfig.heel_offset_device) * GlobalConfig.heel_direction_device;
        Trim = (_Trim + GlobalConfig.trim_offset_device) * GlobalConfig.trim_direction_device;
        HeelNotSmoothed = (_HeelNotSmoothed + GlobalConfig.heel_offset_device) * GlobalConfig.heel_direction_device;
        TrimNotSmoothed = (_TrimNotSmoothed + GlobalConfig.trim_offset_device) * GlobalConfig.trim_direction_device;
        return true;
    }

     void ProcessData(){
        if (TrimValid() && abs(Trim) > MaxTrim) {MaxTrim = abs(Trim);}
        if (HeelValid() && abs(Heel) > MaxHeel) {MaxHeel = abs(Heel);}
     }

    void SendToN2K(){
        //SetN2kAttitude(cN2K::Message(127257));
    }

    bool RefreshDataFromN2k(){
        bool result = false;
        unsigned char _sid;
        double _yaw;
        //pitch <=> trim; roll <=> heel
        if (ParseN2kAttitude(cN2K::Message(127257), _sid, _yaw, Trim, Heel) ) {
            HeelNotSmoothed = Heel = (Heel * radToDeg + GlobalConfig.heel_offset_n2k) * GlobalConfig.heel_direction_n2k;
            TrimNotSmoothed = Trim = (Trim * radToDeg + GlobalConfig.trim_offset_n2k) * GlobalConfig.trim_direction_n2k;
            result = true;
        }
        return result;
    }

    void RefreshDataFromTest(){
        HeelNotSmoothed = Heel = MyRandDouble(-15, 15);
        TrimNotSmoothed = Trim = MyRandDouble(-2, 2);
        ProcessData();
    }

    void ResetData(){
        Heel = N2kDoubleNA;
        Trim = N2kDoubleNA;
        HeelNotSmoothed = N2kDoubleNA;
        TrimNotSmoothed = N2kDoubleNA;
    }
};