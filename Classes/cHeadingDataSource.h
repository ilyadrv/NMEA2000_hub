class cHeadingDataSource: public cDataSource {
public:
    double
        HeadingT = N2kDoubleNA,
        HeadingM = N2kDoubleNA,
        Variation = N2kDoubleNA;

    bool HeadingTValid(){return (0 <= HeadingT && HeadingT < 360);}
    bool HeadingMValid(){return (0 <= HeadingM && HeadingM < 360);}
    bool VariationValid(){return (-90 < Variation && Variation < 90);}

    String HeadingTStr(){ return HeadingTValid() ? Degree360String(HeadingT, 0) : EmptyValue;}
    String HeadingMStr(){ return HeadingMValid() ? Degree360String(HeadingM, 0) : EmptyValue;}
    String VariationStr(){ return VariationValid() ? String(Variation, 1) : EmptyValue;}

    void InitDataSource(cGpsDataSource *_gps, cAttitudeDataSource *_attitude){
        cDataSource::InitDataSource();
        Gps = _gps;
        Attitude = _attitude;
        
        //load calibration
        if (DeviceConfig.heading_compass_calibration_reset){
            prefs.remove("h.mag_max.x");
            prefs.remove("h.mag_max.y");
            prefs.remove("h.mag_max.z");
            prefs.remove("h.mag_min.x");
            prefs.remove("h.mag_min.y");
            prefs.remove("h.mag_min.z");
        }
        mag_max.x = prefs.getFloat("h.mag_max.x", -2000);
        mag_max.y = prefs.getFloat("h.mag_max.y", -2000);
        mag_max.z = prefs.getFloat("h.mag_max.z", -2000);
        mag_min.x = prefs.getFloat("h.mag_min.x", 2000);
        mag_min.y = prefs.getFloat("h.mag_min.y", 2000);
        mag_min.z = prefs.getFloat("h.mag_min.z", 2000);
    }

protected:
    cGpsDataSource *Gps;
    cAttitudeDataSource *Attitude;
    unsigned long DeviceReadTimer;
    bool DeviceReinitMust = false;
    double _HeadingM = N2kDoubleNA;

    //Lets have smoothers for raw data so all variables are smoothed in sync;
    // if smoother sync breaks then magnetic data does not fit heel/trim and it breaks the result
    // We set smoother intervat to be lower than compass refresh interval
    // So we get not 50x10 but 50x50 = 2500 ms
    cSmoother HeelSmoother = cSmoother(50, 10);
    cSmoother TrimSmoother = cSmoother(50, 10);
    cSmoother XSmoother = cSmoother(50, 10);
    cSmoother YSmoother = cSmoother(50, 10);
    cSmoother ZSmoother = cSmoother(50, 10);

private:
    bmm150_mag_data mag_min, mag_max;
    BMM150 bmm = BMM150();

//================================= FROM BASIC CLASS
protected:
    double
        _Heel = N2kDoubleNA,
        _Trim = N2kDoubleNA;

    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.heading_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;
        unsigned char SID;
        tN2kHeadingReference ref;
        double
            _Deviation = 0,
            _Heading = N2kDoubleNA,
            _Variation = N2kDoubleNA;

        if (ParseN2kHeading(cN2K::Message(127250), SID, _Heading, _Deviation, _Variation, ref)) {
            if (!N2kIsNA(_Variation)) Variation = _Variation * radToDeg;
            if (ref == N2khr_magnetic) { //true heading from magnetic
                HeadingM = NormalizeDegree(_Heading * radToDeg);
                if (VariationValid()) HeadingT = NormalizeDegree(HeadingM - Variation);
                result = true;
            }
            else if (ref == N2khr_true) {
                HeadingT = NormalizeDegree(_Heading * radToDeg);
                if (VariationValid()) HeadingM = NormalizeDegree(HeadingT - Variation);
                result = true;
            }
        }
        return result;
    }

    void SendToN2K(){
        tN2kMsg N2kMsg;
        unsigned char SID = _SID();

        if (HeadingMValid()){
            double _Deviation=N2kDoubleNA;
            double _Variation=N2kDoubleNA;
            if (VariationValid()) {
                _Variation = Variation/radToDeg;
            }
            SetN2kMagneticHeading(N2kMsg, SID, HeadingM/radToDeg, _Deviation, _Variation);
            SendN2KMessage(N2kMsg);
        }

        /*
        This is possible but it confuses some plotters. So magnetic heading + variation is enough
        if (HeadingTValid()){
           SetN2kTrueHeading(N2kMsg, SID, HeadingT/radToDeg);
           SendN2KMessage(N2kMsg);
        }
        */
    }

    void RefreshDataFromTest(){
        HeadingT = 15 + MyRandInt(-5, 5);
        Variation = 2.5  + MyRandDouble(-0.5, 0.5);
        HeadingM = HeadingT - Variation;
        Source = (MyRandInt(1, 100) < 90) ? "n2k" : "device"; //emulate switching from n2k to device data in 90% cases
    }

    bool RefreshDataFromDevice(){
        if (millis() > DeviceReadTimer + 50){
            DeviceReadTimer = millis();

            if (i2cDeviceAvailable(BMM150_I2C_Address)){
                if (DeviceReinitMust){
                    InitHardware();
                    DeviceReinitMust = false;
                }

                //get raw data
                bmm150_mag_data value;
                bmm.read_mag_data();
                value.x = bmm.raw_mag_data.raw_datax;
                value.y = bmm.raw_mag_data.raw_datay;
                value.z = bmm.raw_mag_data.raw_dataz;                
                //It makes no sense without  tilt compensation
                _Heel = Attitude->HeelNotSmoothed;
                _Trim = Attitude->TrimNotSmoothed;

                //auto calibrate
                if(DeviceConfig.heading_compass_calibrate && value.x && value.y && value.z){
                    if (value.x < mag_min.x){ mag_min.x = value.x; prefs.putFloat("h.mag_min.x", mag_min.x);}
                    if (value.x > mag_max.x){ mag_max.x = value.x; prefs.putFloat("h.mag_max.x", mag_max.x);}
                    if (value.y < mag_min.y){ mag_min.y = value.y; prefs.putFloat("h.mag_min.y", mag_min.y);}
                    if (value.y > mag_max.y){ mag_max.y = value.y; prefs.putFloat("h.mag_max.y", mag_max.y);}
                    if (value.z < mag_min.z){ mag_min.z = value.z; prefs.putFloat("h.mag_min.z", mag_min.z);}
                    if (value.z > mag_max.z){ mag_max.z = value.z; prefs.putFloat("h.mag_max.z", mag_max.z);}
                }

                //apply calibration
                //and swap axis - device specific
                double x = value.y - (mag_max.y + mag_min.y) / 2; //good offset 2
                double y = value.x - (mag_max.x + mag_min.x) / 2; //good offset -13
                double z = value.z - (mag_max.z + mag_min.z) / 2; //good offset 2

                //smooth raw data
                HeelSmoother.Smooth(&_Heel);
                TrimSmoother.Smooth(&_Trim);
                XSmoother.Smooth(&x);
                YSmoother.Smooth(&y);
                ZSmoother.Smooth(&z);

                //if we have huge heel or trim the heading data will be bad so we just exit;
                //however we still calibrate the device so this shild be after calibration
                if (abs(_Heel) > 40 || abs(_Trim) > 40) {
                    _HeadingM = N2kDoubleNA;
                }
                else{
                    //tilt compensation - without it it makes no sense
                    //pitch <=> trim; roll <=> heel
                    // also apply magnetic sensor mount position offsets
                    double r = (_Heel + DeviceConfig.heading_compass_device_offset_roll) / radToDeg; //heel aka roll radians
                    double p = (_Trim + DeviceConfig.heading_compass_device_offset_pitch) / radToDeg; //trim aka pitch radians
                    x = x*cos(p) + z*sin(p);
                    y = x*sin(r)*sin(p) + y*cos(r) - z*sin(r)*cos(p);
                    _HeadingM = NormalizeDegree(atan2(y, x) * radToDeg + DeviceConfig.heading_compass_device_offset_yam);
                }
            }
            else{
                DeviceReinitMust = true;
                _HeadingM = N2kDoubleNA;
            }
        }
        HeadingM = _HeadingM;
        if (Gps->VariationValid()) {
            Variation = Gps->Variation;
            HeadingT = NormalizeDegree(HeadingM - Gps->Variation);
        }
        return (HeadingMValid());
    }

    void ResetData(){
        HeadingT = N2kDoubleNA;
        HeadingM = N2kDoubleNA;
        Variation = N2kDoubleNA;
    }

    // https://wiki.seeedstudio.com/Grove-3-Axis_Digitial_Compass_v2.0/
    void InitHardware(){
        bmm.initialize();
        if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
            _debug("Chip ID can not read!");
            DeviceReinitMust = true;
        } else {
            _debug("Initialize done!");
        }
    }

};