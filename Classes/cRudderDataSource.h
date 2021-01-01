class cRudderDataSource: public cDataSource {
public:
    double
        Position = N2kDoubleNA;

    bool PositionValid(){ return (-90 < Position && Position < 90);}

    String PositionStr(){ return PositionValid() ? String(Position, 1) : EmptyValue;}

//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = GlobalConfig.rudder_datasource;
    }

    bool RefreshDataFromN2k(){
        bool result = false;
        unsigned char Instance;
        tN2kRudderDirectionOrder RudderDirectionOrder;
        double AngleOrder;
        if (ParseN2kRudder(cN2K::Message(127245), Position, Instance, RudderDirectionOrder, AngleOrder)) {
            Position *= radToDeg;
            result = true;
        }
        return result;
    }

    void RefreshDataFromTest(){
        Position = MyRandDouble(-10, 10);
    }

    void ResetData(){
        Position = N2kDoubleNA;
    }
};