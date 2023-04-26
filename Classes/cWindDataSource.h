class cWindDataSource: public cDataSource {
public:
    double
        AWS=N2kDoubleNA,
        TWS=N2kDoubleNA,
        AWA=N2kDoubleNA,
        TWA=N2kDoubleNA,
        TWD=N2kDoubleNA,
        MaxAWS=0,
        MaxTWS=0,
        AWD=N2kDoubleNA; //who cares of AWD?!?

    bool AWSValid(){return (0 <= AWS && AWS <= 100);}
    bool TWSValid(){return (0 <= TWS && TWS <= 100);}
    bool AWAValid(){return (abs(AWA) <= 180);}
    bool TWAValid(){return (abs(TWA) <= 180);}
    bool TWDValid(){return (0 <= TWD && TWD <= 360);}
    bool MaxAWSValid(){return (0 <= MaxAWS && MaxAWS <= 100);}
    bool MaxTWSValid(){return (0 <= MaxTWS && MaxTWS <= 100);}

    String AWSStr(){ return AWSValid() ? String(AWS, 1) : EmptyValue;}
    String TWSStr(){ return TWSValid() ? String(TWS, 1) : EmptyValue;}
    String AWAStr(){ return AWAValid() ? String(AWA, 0) : EmptyValue;}
    String TWAStr(){ return TWAValid() ? String(TWA, 0) : EmptyValue;}
    String TWDStr(){ return TWDValid() ? String(TWD, 0) : EmptyValue;}
    String MaxAWSStr(){ return MaxAWSValid() ? String(MaxAWS, 1) : EmptyValue;}
    String MaxTWSStr(){ return MaxTWSValid() ? String(MaxTWS, 1) : EmptyValue;}

    void InitDataSource(cGpsDataSource *_gps, cHeadingDataSource *_heading){
        cDataSource::InitDataSource();
        Gps = _gps;
        Heading = _heading;
    }

protected:
    cGpsDataSource *Gps;
    cHeadingDataSource *Heading;
    cSmoother AWASmoother = cSmoother();
    cSmoother AWSSmoother = cSmoother();

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.wind_datasource;
    }

    void WindFromApparent(){
        double x, y;
        if (AWAValid() && Heading->HeadingTValid()){ //who cares of AWD?!?
            AWD=NormalizeDegree(Degree180to360(AWA) + Heading->HeadingT);
        }
        // Calculate and send TWS/TWA if have adequate SOG and COG
        if (Gps->SOGValid()) {
            if (Gps->SOG >= DeviceConfig.sog_ignore){
                x = AWS * cos(AWA/radToDeg);
                y = AWS * sin(AWA/radToDeg);
                TWA = Degree360to180(NormalizeDegree(atan2(y, x - Gps->SOG) * radToDeg));
                TWS = sqrt(y*y + (x-Gps->SOG)*(x-Gps->SOG));
            }
            else{
                //e.g if SOG is about 0 or very small we consider True are the same as Apparent
                TWA = AWA;
                TWS = AWS;
            }
        }

        //If SOG is below sog_ignore COG will be set to N/A and so we do no have twd
        if (TWAValid() && Gps->COGValid()){
            TWD=NormalizeDegree(Degree180to360(TWA) + Gps->COG);
        }

    }

    bool RefreshDataFromN2k(){
        bool result = false;
        unsigned char SID;
        tN2kWindReference WindReference;
        double WindAngle, WindSpeed;
        if (ParseN2kWindSpeed(cN2K::Message(130306), SID, WindSpeed, WindAngle, WindReference)){
            if (WindReference == N2kWind_Apparent) {
                AWA= NormalizeDegree(WindAngle * radToDeg);
                AWS=WindSpeed * msToKn;
                AWASmoother.Smooth(&AWA);
                AWSSmoother.Smooth(&AWS);
                AWA=Degree360to180(AWA);
                WindFromApparent();
                result = true;
            }
        }
        return result;
    }

    void RefreshDataFromTest(){
        //TWS = 16.5 + MyRandDouble(-5, 5);
        //TWD = 105  + MyRandDouble(-10, 10);
        //TWA = 90 + MyRandDouble(-10, 10);
        AWS = 16.5 + MyRandDouble(-10, 10);
        AWA = 62 + MyRandDouble(-15, 15);
        WindFromApparent();
    }

    void ProcessData(){
        if (AWSValid() && AWS > MaxAWS) MaxAWS = AWS;
        if (TWSValid() && TWS > MaxTWS) MaxTWS = TWS;
    }

    void ResetData(){
        AWD=N2kDoubleNA,
        AWS=N2kDoubleNA,
        TWS=N2kDoubleNA,
        AWA=N2kDoubleNA,
        TWA=N2kDoubleNA,
        TWD=N2kDoubleNA;
    }
};