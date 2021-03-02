#include "cSmoother.h";

class cDataSource{

protected:
    cConfig::tDataSourceConfig *DataSourceConfig = (cConfig::tDataSourceConfig *) malloc(sizeof(cConfig::tDataSourceConfig));
    bool test = false;
    unsigned long N2kSendTimer = 0, N2kSendRapidTimer = 0, OneSecTimer = 0, TestDataTimer = 0;
    unsigned char _sid = 0; //Sequence ID - messages of the same SID from one device are grouped together like they were sent in the same time
    unsigned char _SID(){if (_sid == 255) {_sid = 0;} return ++_sid;}

public:
    const String EmptyValue = "---";

    //============= public data fields ==================
    String Source = ""; // source of received data: n2k | device | n2k_device | device_n2k

    //================================== public functions
    void InitDataSource(){
        SetDataSourceConfig();
        test = DeviceConfig.test;
        InitHardware();
    }

    //check if data received from the source
    bool SourceValid(){return (Source == "n2k" || Source == "device");}

    //check if source was defined in the config
    bool SourceEnabled(){return (DataSourceConfig->Source == "n2k" || DataSourceConfig->Source == "device" || DataSourceConfig->Source == "n2k_device" || DataSourceConfig->Source == "device_n2k");}

    void RefreshData(){
        if (test){
            if (TestDataTimer + DeviceConfig.page_refresh_speed < millis()){ //we are using page refresh timer settings to mutate test data rarely so it does not jump like hell
                TestDataTimer = millis();
                unsigned char _r = MyRandInt(1, 100);
                if(_r <= 5) { //emulate data loss in 3% of cases and it will last for a few iterations, because next RefreshDataFromTest will arrive only in 40% cases
                    Source = "";
                    ResetData();
                }
                else if(_r < 60){ //mutate data only in 40% cases
                    // the order is important; this allows us to change Source of test data runtime
                    //by default we set source = device
                    Source = "device";
                    RefreshDataFromTest();
                }
            }
        }
        else{
            //on production we reset data and source each time.
            //We are buffering N2K messages so we do not need to do it here
            Source = "";
            ResetData();
            //N2K priority
            if (DataSourceConfig->Source == "n2k" || DataSourceConfig->Source == "n2k_device"){
                if (RefreshDataFromN2k()) Source = "n2k";
                else if(DataSourceConfig->Source == "n2k_device" && RefreshDataFromDevice()) Source = "device";
            }
            //Device priority
            else if (DataSourceConfig->Source == "device" || DataSourceConfig->Source == "device_n2k"){
                if (RefreshDataFromDevice()) Source = "device";
                else if(DataSourceConfig->Source == "device_n2k" && RefreshDataFromN2k()) Source = "n2k";
            }
        }
        if (SourceValid()){
            ProcessData();
            if (OneSecTimer + 1000 < millis()){
                OneSecTimer = millis();
                OneSecProcessData();
            }
        }
    }

    void SendDataToN2K(){
        //if it is in test mode we want to send data to N2K regardless it's source and configuration.
        if(test){
            DataSourceConfig->N2kSendInterval = 1000;
            DataSourceConfig->N2kPriority = 6;
        }
        if (DataSourceConfig->N2kPriority > 0 && (Source == "device" || test)){
            if (N2kSendTimer + DataSourceConfig->N2kSendInterval < millis()){
                N2kSendTimer = millis();
                SendToN2K();
            }
            if (N2kSendRapidTimer + DataSourceConfig->N2kSendInterval/10 < millis()){
                N2kSendRapidTimer = millis();
                SendToN2KRapid();
            }
        }
    }

    bool SourceN2k(){ return (Source == "n2k");}
    bool SourceDevice(){ return (Source == "device");}

protected:
    void SendN2KMessage(tN2kMsg N2kMsg){
        N2kMsg.Priority = DataSourceConfig->N2kPriority;
        NMEA2000.SendMsg(N2kMsg);
    }

//=====================================================
//================ to implement =======================
    virtual void SetDataSourceConfig(){} //e.g. DataSourceConfig = &DeviceConfig.battery_config
    virtual void ResetData(){}

    virtual void SendToN2K(){}
    virtual void SendToN2KRapid(){}

    virtual bool RefreshDataFromN2k(){ return false;} //must return true if data from device received.
    virtual bool RefreshDataFromDevice(){return false;}//must return true if data from device received.
    virtual void RefreshDataFromTest(){}

    virtual void ProcessData(){}
    virtual void OneSecProcessData(){} // data to be processed slow - 1 sec timer. E.g. averages

    virtual void InitHardware(){}

//=====================================================
//=====================================================

public:
    static bool LatValid(double lat){ return (abs(lat) < 90.1);}
    static bool LonValid(double lon){ return (abs(lon) < 180.1);}
    static String LatStr(double lat){
        if(!LatValid(lat)) return String("------");
        return String((lat > 0 ) ? "N " : "S ")
            + String(trunc(lat), 0) + " "
            + String(((lat - trunc(lat)) / 100 * 60 * 100), 3);
    }
    static String LonStr(double lon){
        if(!LonValid(lon)) return String("------");
        return String((lon > 0 ) ? "E " : "W ")
            + String(trunc(lon), 0) + " "
            + String(((lon - trunc(lon)) / 100 * 60 * 100), 3);
    }

     static String SecToTime(long sec, const char* format = "%d.%m.%Y %H:%M:%S"){
        struct tm  ts;
        char buffer[50];
        ts = *localtime(&sec);
        strftime(buffer, sizeof(buffer), format, &ts);
        return String(buffer);
    }

    static bool i2cDeviceAvailable(byte _addr){
        Wire.beginTransmission(_addr);
        return (Wire.endTransmission() == 0);
    }

    static String Degree360String(double degree, unsigned char prec){
        String result = String(NormalizeDegree(degree), prec);
        return (result == "360") ? String(0.0, prec) : result;

    }

};