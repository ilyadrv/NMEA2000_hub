class cTripDataSource: public cDataSource {
public:
    double
        TripLog=N2kDoubleNA,
        Log=N2kDoubleNA;
    unsigned long
        TripTime = 0,
        TripStartTime = 0;


    bool TripLogValid(){return (0 < TripLog && TripLog < 100000);}
    bool LogValid(){return (0 < Log && Log < 1000000);}
    bool TripTimeValid(){return (0 < TripTime && TripTime < 3600000000);} //less than 1000 hours

    String TripLogStr(){ return TripLogValid() ? String(TripLog, 1) : EmptyValue;}
    String LogStr(){ return LogValid() ? String(Log, 0) : EmptyValue;}
    String TripTimeStr(){
        if (TripTimeValid()){
            char buffer[10];
            int _h = trunc(TripTime/3600000);
            int _m = trunc((TripTime - _h * 3600000)/60000);
            sprintf(buffer, "%d:%02d", _h, _m);
            return String(buffer);
        }
        else{
            return EmptyValue;
        }
    }

    // we want to refresh trip time whatever we got on from N2K or test
    void RefreshData(){
        cDataSource::RefreshData();
        TripTime = millis() - TripStartTime;
    }

protected:
    double TestTripLog = 0;

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.trip_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;
        uint16_t _DaysSince1970;
        double _SecondsSinceMidnight;
        uint32_t
            _TripLog = N2kUInt32NA,
            _Log = N2kUInt32NA;
        if (ParseN2kDistanceLog(cN2K::Message(128275), _DaysSince1970, _SecondsSinceMidnight, _Log, _TripLog) ) {
            TripLog = _TripLog * mToNm;
            Log = _Log * mToNm;
            result = true;
        }
        return result;
    }

    void RefreshDataFromTest(){
        TestTripLog += 0.01;
        TripLog = TestTripLog;
        Log= 125 + TestTripLog;
    }

    void ResetData(){
        TripLog=N2kDoubleNA;
        Log=N2kDoubleNA;
    }
};