class cWaterDataSource: public cDataSource {
public:
    double
        STW = N2kDoubleNA,
        Depth = N2kDoubleNA,
        Temp = N2kDoubleNA,
        AvgSTW = N2kDoubleNA;

    bool STWValid(){ return (-0.1 < STW && STW < 100);}
    bool AvgSTWValid(){ return (-0.1 < AvgSTW && AvgSTW < 100);}
    bool DepthValid(){ return (-20 < Depth && Depth < 999);} // negative depth because it can be with offsets
    bool TempValid(){ return (-5 < Temp && Temp < 50);}

    String STWStr(){ return STWValid() ? String(STW, 1) : EmptyValue;}
    String AvgSTWStr(){ return AvgSTWValid() ? String(AvgSTW, 1) : EmptyValue;}
    String DepthStr(){ return DepthValid() ? String(Depth, 1) : EmptyValue;}
    String TempStr(){ return TempValid() ? String(Temp, 1) : EmptyValue;}

protected:
    unsigned long int _AvgSTWPoints = 0;

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = &DeviceConfig.water_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;

        unsigned char SID, TempInstance;
        double GroundReferenced;
        tN2kSpeedWaterReferenceType SWRT;
        tN2kTempSource TempSource;
        tN2kHumiditySource HumiditySource;
        double Offset;
        double Range;
        double Humidity, AtmosphericPressure, SetTemperature, Temperature;

        if (ParseN2kBoatSpeed(cN2K::Message(128259), SID, STW, GroundReferenced, SWRT)) {
            STW *= msToKn;
            result = true;
        }

        if (ParseN2kWaterDepth(cN2K::Message(128267), SID, Depth, Offset, Range) ) {
            if (DepthValid()){
                Depth += Offset + DeviceConfig.depth_offset;
            }
            result = true;
        }

        if(ParseN2kPGN130311(cN2K::Message(130311),SID,TempSource,Temperature,HumiditySource,Humidity,AtmosphericPressure) && TempSource==N2kts_SeaTemperature){
            Temp = Temperature + kToC;
        }
        else if(ParseN2kPGN130316(cN2K::Message(130316), SID, TempInstance, TempSource, Temperature, SetTemperature) && TempSource==N2kts_SeaTemperature){
            Temp = Temperature + kToC;
        }
        return result;
    }

    void RefreshDataFromTest(){
        STW = 7.3  + MyRandDouble(-1.5, 1.5);
        Depth = 15.5 + MyRandDouble(5, 5);
        Temp = 25.3 + MyRandDouble(-2, 2);
    }

    void OneSecProcessData(){
        if (STWValid() && DeviceConfig.min_move_speed < STW){
            AvgSTW = (_AvgSTWPoints == 0)
                ? AvgSTW = STW
                : (AvgSTW * _AvgSTWPoints + STW) / (_AvgSTWPoints + 1);
            _AvgSTWPoints++;
        }
    }

//    void InitHardware(){}

    void ResetData(){
        STW = N2kDoubleNA;
        Depth = N2kDoubleNA;
        Temp = N2kDoubleNA;
    }
};