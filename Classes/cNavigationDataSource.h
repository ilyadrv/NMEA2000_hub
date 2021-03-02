class cNavigationDataSource: public cDataSource {
public:
    double
        XTE = N2kDoubleNA,
        DistanceToWaypoint = N2kDoubleNA,
        ETATime = N2kDoubleNA,
        BearingPositionToWPT = N2kDoubleNA,
        BearingPositionToWPM = N2kDoubleNA,
        DestinationLatitude = N2kDoubleNA,
        DestinationLongitude = N2kDoubleNA,
        VMGToWP = N2kDoubleNA;
    int16_t
        ETADate = N2kInt16NA;

    bool XTEValid(){ return (abs(XTE) < 10000);} //xte more than 10km is bad
    bool DistanceToWaypointValid(){ return (-0.1 < DistanceToWaypoint && DistanceToWaypoint < 10000);}
    bool ETAValid(){ return (18200 < ETADate && ETADate < 62118 && 0 <= ETATime && ETATime <= 86400);} // after 1 Dec 2020 but before 2140
    bool BearingPositionToWPTValid(){ return (-0.1 < BearingPositionToWPT && BearingPositionToWPT < 360.1);}
    bool BearingPositionToWPMValid(){ return (-0.1 < BearingPositionToWPM && BearingPositionToWPM < 360.1);}
    bool DestinationLatitudeValid(){ return LatValid(DestinationLatitude);}
    bool DestinationLongitudeValid(){ return LonValid(DestinationLongitude);}
    bool VMGToWPValid(){ return (abs(VMGToWP) < 100);}

    String XTEStr(){ return XTEValid() ? String(XTE, 0) : EmptyValue;} //value in meters
    String DistanceToWaypointStr(){ return DistanceToWaypointValid() ? String(DistanceToWaypoint, 1) : EmptyValue;}
    String ETAStr(){return (ETAValid()) ? SecToTime(ETASec() ,"%H:%M") : EmptyValue;}
    String BearingPositionToWPTStr(){ return BearingPositionToWPTValid() ? String(BearingPositionToWPT, 0) : EmptyValue;}
    String BearingPositionToWPMStr(){ return BearingPositionToWPMValid() ? String(BearingPositionToWPM, 0) : EmptyValue;}
    String DestinationLatitudeStr(){ return LatStr(DestinationLatitude);}
    String DestinationLongitudeStr(){return LonStr(DestinationLongitude);}
    String DestinationLatitudeDecStr(){ return DestinationLatitudeValid() ? String(DestinationLatitude, 6) : EmptyValue;}
    String DestinationLongitudeDecStr(){ return DestinationLongitudeValid() ? String(DestinationLongitude, 6) : EmptyValue;}
    String VMGToWPStr(){ return VMGToWPValid() ? String(VMGToWP, 1) : EmptyValue;}

    long ETASec(){ return ETAValid() ? ETADate * 3600 * 24 + ETATime + DeviceConfig.gps_time_offset_minutes * 60 : 0;}

    void InitDataSource(cGpsDataSource *_gps){
        cDataSource::InitDataSource();
        Gps = _gps;
    }

protected:
     cGpsDataSource *Gps;

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.navigation_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;

        unsigned char _SID;
        tN2kXTEMode _XTEMode;
        bool _NavigationTerminated;
        if(ParseN2kXTE(cN2K::Message(129283), _SID, _XTEMode, _NavigationTerminated, XTE)) {
            result = true;
        }

        tN2kHeadingReference _BearingReference;
        bool _PerpendicularCrossed, _ArrivalCircleEntered;
        tN2kDistanceCalculationType _CalculationType;
        double _BearingOriginToDestinationWaypoint, _BearingPositionToWP;
        uint8_t _OriginWaypointNumber, _DestinationWaypointNumber;
        if (ParseN2kNavigationInfo(cN2K::Message(129284), _SID, DistanceToWaypoint, _BearingReference,
            _PerpendicularCrossed, _ArrivalCircleEntered, _CalculationType,
            ETATime, ETADate, _BearingOriginToDestinationWaypoint, _BearingPositionToWP,
            _OriginWaypointNumber, _DestinationWaypointNumber,
            DestinationLatitude, DestinationLongitude, VMGToWP))
        {
            //if no point data - escape
            if (!DestinationLatitudeValid() || !DestinationLongitudeValid()) {
                ResetData();
                return false;
            }
            if (_BearingReference == N2khr_magnetic) { //true heading from magnetic
                BearingPositionToWPM = NormalizeDegree(_BearingPositionToWP * radToDeg);
                if (Gps->VariationValid()) BearingPositionToWPT = NormalizeDegree(BearingPositionToWPM - Gps->Variation);
            }
            else if (_BearingReference == N2khr_true) {
                BearingPositionToWPT = NormalizeDegree(_BearingPositionToWP * radToDeg);
                if (Gps->VariationValid()) BearingPositionToWPM = NormalizeDegree(BearingPositionToWPT - Gps->Variation);
            }
            //both magnetic and true
            VMGToWP *= msToKn;
            DistanceToWaypoint *= mToNm;
            result = true;
        }
        return result;
    }

    double
        _DistanceToWaypoint = N2kDoubleNA,
        _ETATime = N2kDoubleNA,
        _DestinationLatitude = N2kDoubleNA,
        _DestinationLongitude = N2kDoubleNA;

    void RefreshDataFromTest(){
        if (!(0.1 < _DistanceToWaypoint && _DistanceToWaypoint < 110)){
            _DistanceToWaypoint = 109;
            _ETATime = 23 * 3600;
            _DestinationLatitude = 65.514903;
            _DestinationLongitude = 12.215997;
        }
        _DistanceToWaypoint -= 0.0013; //109/(23*3600)
        _ETATime -= 1;
        _DestinationLatitude +=  0.000021;
        _DestinationLongitude += 0.000010;

        XTE = MyRandDouble(-50, 50);
        DistanceToWaypoint = _DistanceToWaypoint;
        ETATime = _ETATime;
        ETADate = 18250;
        DestinationLatitude = _DestinationLatitude;
        DestinationLongitude = _DestinationLongitude;
        BearingPositionToWPT = NormalizeDegree(MyRandDouble(-10, 10));
        BearingPositionToWPM = NormalizeDegree(BearingPositionToWPT - 3.4);
        VMGToWP = MyRandDouble(-1, 5.5);
    }

    void ResetData(){
        XTE = N2kDoubleNA;
        DistanceToWaypoint = N2kDoubleNA;
        ETATime = N2kDoubleNA;
        BearingPositionToWPT = N2kDoubleNA;
        BearingPositionToWPM = N2kDoubleNA;
        DestinationLatitude = N2kDoubleNA;
        DestinationLongitude = N2kDoubleNA;
        VMGToWP = N2kDoubleNA;
        ETADate = N2kInt16NA;
    }

};