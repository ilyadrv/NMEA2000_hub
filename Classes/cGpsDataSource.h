class cGpsDataSource: public cDataSource {
public:
    uint16_t
        DaysSince1970 = 0;
    double
        SecondsSinceMidnight = N2kDoubleNA,
        Latitude = N2kDoubleNA,
        Longitude = N2kDoubleNA,
        Altitude = N2kDoubleNA,
        Variation = N2kDoubleNA,
        SOG = N2kDoubleNA,
        COG = N2kDoubleNA,
        MCOG = N2kDoubleNA,
        AvgSOG = N2kDoubleNA;


    bool LatitudeValid(){ return LatValid(Latitude);}
    bool LongitudeValid(){ return LonValid(Longitude);}
    bool CoordinatesValid(){ return (LatitudeValid() && LongitudeValid());}
    bool AltitudeValid(){ return (-1000 < Altitude && Altitude < 1000);}
    bool VariationValid(){ return (-90 < Variation && Variation < 90);}
    bool SOGValid(){ return (-0.1 < SOG && SOG < 100);}
    bool AvgSOGValid(){ return (-0.1 < AvgSOG && AvgSOG < 100);}
    bool COGValid(){ return (-0.1 < COG && COG < 360.1);}
    bool MCOGValid(){ return (-0.1 < MCOG && MCOG < 360.1);}
    bool TimeValid(){ return (18200 < DaysSince1970 && DaysSince1970 < 62118 && 0 <= SecondsSinceMidnight && SecondsSinceMidnight <= 86400);} // after 1 Dec 2020 but before 2140
    bool TimeValid(long time){ return (1607000000 < time && time < 5616000000);} // after Dec 2020 but before 2140 something
    bool PositionValid(tMyGpsData p){return (-90.1 < p.Latitude && p.Latitude < 90.1) && (-180.1 < p.Longitude && p.Longitude < 180.1) && TimeValid(p.Time);}

    String LatitudeStr(){ return LatStr(Latitude);}
    String LongitudeStr(){return LonStr(Longitude);}
    String LatitudeDecStr(){ return LatitudeValid() ? String(Latitude, 6) : EmptyValue;}
    String LongitudeDecStr(){ return LongitudeValid() ? String(Longitude, 6) : EmptyValue;}
    String AltitudeStr(){ return AltitudeValid() ? String(Altitude, 0) : EmptyValue;}
    String VariationStr(){ return VariationValid() ? String(Variation, 1) : EmptyValue;}
    String SOGStr(){ return SOGValid() ? String(SOG, 1) : EmptyValue;}
    String AvgSOGStr(){ return AvgSOGValid() ? String(AvgSOG, 1) : EmptyValue;}
    String COGStr(){ return COGValid() ? String(COG, 0) : EmptyValue;}
    String MCOGStr(){ return MCOGValid() ? String(MCOG, 0) : EmptyValue;}
    String TimeStr(long t, const char* format = "%d.%m.%Y %H:%M:%S"){return (TimeValid(t)) ? SecToTime(t, format) :EmptyValue;}
    String TimeStr(const char* format = "%d.%m.%Y %H:%M:%S"){return TimeStr(Time(), format);}

    long Time(){
        return TimeValid()
            ? DaysSince1970 * 3600 * 24 + SecondsSinceMidnight + DeviceConfig.gps_time_offset_minutes * 60
            : 0;
    }

    tMyGpsData Data(){
        tMyGpsData result;
            result.DaysSince1970 = DaysSince1970,
            result.SecondsSinceMidnight = SecondsSinceMidnight,
            result.Time = Time();
            result.Latitude = Latitude,
            result.Longitude = Longitude,
            result.Altitude = Altitude,
            result.Variation = Variation,
            result.SOG = SOG,
            result.COG = COG;
            result.Source = Source;
        return result;
    }

    tMyGpsData _LastShutdownPosition;

protected:
    tMyGpsData testData;
    unsigned long int _AvgSOGPoints = 0;
    TinyGPSPlus gps;
    HardwareSerial ss = HardwareSerial(2);

    uint32_t _DeviceLastCharsProcessed;
    unsigned long  _DeviceLastCharsProcessedTimer = 0;
    bool _DeviceUpdated = false;

    void smartDelay(unsigned long ms) {
        unsigned long start = millis();
        do {
            while (ss.available()) gps.encode(ss.read());
        } while (millis() - start < ms);
    }

    uint16_t getDaysSince1970(uint8_t d, uint8_t m, uint16_t y){
        struct tm a = {0,0,0,1,1,70};
        struct tm b = {0,0,0,d,m,y-1900};
        time_t ta = mktime(&a);
        time_t tb = mktime(&b);
        return difftime(tb, ta) / (60 * 60 * 24);
    }

    TinyGPSCustom GeoidSeparation = TinyGPSCustom(gps, "GNGGA", 11);

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.gps_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;

        //stuff
        unsigned char _SID;
        tN2kGNSStype GNSStype;
        tN2kGNSSmethod GNSSmethod;
        unsigned char nSatellites;
        double HDOP;
        double PDOP;
        double GeoidalSeparation;
        double _COG;
        unsigned char nReferenceStations;
        tN2kGNSStype ReferenceStationType;
        uint16_t ReferenceSationID;
        double AgeOfCorrection;
        tN2kHeadingReference HeadingReference;

        //GNSS
        if (ParseN2kGNSS(cN2K::Message(129029), _SID, DaysSince1970, SecondsSinceMidnight, Latitude, Longitude, Altitude, GNSStype, GNSSmethod,
                        nSatellites, HDOP, PDOP, GeoidalSeparation,
                        nReferenceStations, ReferenceStationType, ReferenceSationID, AgeOfCorrection)
        ){
            Altitude -= GeoidalSeparation; //fix altitude
            result = true;
        }

        //Lat/lon rapid because this is just rapid we do not consider it as a positive result;
        ParseN2kPGN129025(cN2K::Message(129025), Latitude, Longitude);

        //variation
        tN2kMagneticVariation _Source;
        uint16_t _DaysSince1970;
        if (ParseN2kMagneticVariation(cN2K::Message(127258), _SID, _Source, _DaysSince1970, Variation)){
            Variation *= radToDeg;
        }

        //COG SOG N2khr_true=0, N2khr_magnetic=1,
        if (ParseN2kCOGSOGRapid(cN2K::Message(129026), _SID, HeadingReference, _COG, SOG)) {
            if (HeadingReference == N2khr_magnetic) {
              MCOG = NormalizeDegree(_COG * radToDeg);
              if (VariationValid()) COG = NormalizeDegree(MCOG - Variation);
            }
            else if (HeadingReference == N2khr_true) {
              COG = NormalizeDegree(_COG * radToDeg);
              if (VariationValid()) MCOG = NormalizeDegree(COG - Variation);
            }
            SOG *= msToKn;
        }

        return result;
    }

    void SendToN2K(){
        tN2kMsg N2kMsg;
        unsigned char SID = _SID();

        SetN2kGNSS(N2kMsg, SID,
            DaysSince1970, SecondsSinceMidnight, Latitude, Longitude, Altitude,
            N2kGNSSt_GPSGLONASS, N2kGNSSm_noGNSS, 10, 0, 0, 0, 10, N2kGNSSt_GPS, 0, 0
        );
        SendN2KMessage(N2kMsg);

        SetN2kMagneticVariation(N2kMsg, SID, N2kmagvar_Manual, DaysSince1970, Variation/radToDeg);
        SendN2KMessage(N2kMsg);
    }

    void SendToN2KRapid(){
        tN2kMsg N2kMsg;
        unsigned char SID = _SID();

        //coordinates - rapid
        SetN2kPGN129025(N2kMsg, Latitude, Longitude);
        SendN2KMessage(N2kMsg);

        SetN2kCOGSOGRapid(N2kMsg, SID, N2khr_true, COG/radToDeg, SOG/msToKn);
        SendN2KMessage(N2kMsg);
    }

    void RefreshDataFromTest(){
        unsigned long _time;
        if (!LatitudeValid()){
            if (PositionValid(testData)){ //from last test data
                Latitude=testData.Latitude;
                Longitude=testData.Longitude;
                _time = testData.Time - DeviceConfig.gps_time_offset_minutes * 60;
            }
            else if(PositionValid(_LastShutdownPosition)){
                Latitude=_LastShutdownPosition.Latitude;
                Longitude=_LastShutdownPosition.Longitude;
                _time = _LastShutdownPosition.Time - DeviceConfig.gps_time_offset_minutes * 60;
            }
            else{ //from scratch
                Latitude= 52.514903;
                Longitude=5.215997;
                _time = 20000 + 14*3600;
            }
        }
        else{
            _time = DeviceState.Time - DeviceConfig.gps_time_offset_minutes * 60;
        }
        Latitude +=  0.000021;
        Longitude += 0.000010;
        Altitude = 54.1 + MyRandDouble(-3, 3);
        Variation = 2.5 + MyRandDouble(-0.5, 0.5);
        SOG = 6.1 + MyRandDouble(-1.5, 1.5);
        COG = 320.6  + MyRandDouble(-5, 5);
        MCOG = COG - 1.2;
        DaysSince1970 = trunc(_time/24/3600);
        SecondsSinceMidnight = _time - DaysSince1970 * 24 *3600;
        Source = (MyRandInt(1, 100) < 90) ? "n2k" : "device"; //emulate switching from n2k to device data in 90% cases
        testData = Data();//save test data
    }

    bool RefreshDataFromDevice(){
        bool result = false;
        //invalidate data after 5 seconds
        if (_DeviceLastCharsProcessedTimer + 5000 < millis() && gps.charsProcessed() >= 10){
            _DeviceLastCharsProcessedTimer = millis();
            _DeviceUpdated = (_DeviceLastCharsProcessed != gps.charsProcessed());
            _DeviceLastCharsProcessed = gps.charsProcessed();
        }

        if (_DeviceUpdated) {
            DaysSince1970 = getDaysSince1970(gps.date.day(), gps.date.month(), gps.date.year());
            SecondsSinceMidnight = gps.time.hour() * 3600 + gps.time.minute() * 60 + gps.time.second();
            Latitude = gps.location.lat();
            Longitude = gps.location.lng();
            Altitude = gps.altitude.meters() - TinyGPSPlus::parseDecimal(GeoidSeparation.value()) / 100; //for 6558 GeoidSeparation is always 0 (((
            SOG = gps.speed.knots();
            COG = gps.course.deg();
            result = true;

            //DOPs
            //TinyGPSCustom hdop = gps.hdop.value()
            //TinyGPSCustom pdop = TinyGPSCustom(gps, "GPGSA", 15); // $GPGSA sentence, 15th element
            //TinyGPSCustom hdop = TinyGPSCustom(gps, "GPGSA", 16) // $GPGSA sentence, 16th element
            //TinyGPSCustom vdop = TinyGPSCustom(gps, "GPGSA", 17); // $GPGSA sentence, 17th element
            //Variation = mVariation.value(); // unit value is empty TinyGPSCustom mVariation = TinyGPSCustom(gps, "GPGSA", 10)
            //MCOG = N2kDoubleNA,
        }
        smartDelay(0); //it is not very smart. we'll keep it zero
        return result;
    }

    void ProcessData(){
        if (SOG < DeviceConfig.sog_ignore){
            COG = N2kDoubleNA;
            MCOG = N2kDoubleNA;
        }
    }

    void OneSecProcessData(){
        if (SOGValid() && SOG > DeviceConfig.min_move_speed){
            AvgSOG = (_AvgSOGPoints == 0)
                ? AvgSOG = SOG
                : (AvgSOG * _AvgSOGPoints + SOG) / (_AvgSOGPoints + 1);
            _AvgSOGPoints++;
        }
    }

    void InitHardware(){
        ss.begin(9600);
    }

    void ResetData(){
        DaysSince1970 = 0;
        SecondsSinceMidnight = 0;
        Latitude = -1000;
        Longitude = -1000;
        Altitude = -1000;
        Variation = -1000;
        SOG = -1000;
        COG = -1000;
        MCOG = -1000;
    }
};