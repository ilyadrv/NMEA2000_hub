#include "cN2K.h"
#include "cDataSource.h"

//order maters
#include "cGpsDataSource.h"
#include "cWaterDataSource.h"
#include "cBatteryDataSource.h"
#include "cTripDataSource.h"
#include "cRudderDataSource.h"
#include "cAttitudeDataSource.h"
#include "cEnvDataSource.h"
#include "cHeadingDataSource.h"
#include "cWindDataSource.h"
#include "cNavigationDataSource.h"
#include "cPolarDataSource.h"

class cBoatData{
public:
    unsigned long
        TimeRefreshN2K = 0,
        TimeRefreshGps = 0,
        TimeRefreshHeading = 0,
        TimeRefreshWater = 0,
        TimeRefreshWind = 0,
        TimeRefreshBattery = 0,
        TimeRefreshTrip = 0,
        TimeRefreshRudder = 0,
        TimeRefreshNavigation = 0,
        TimeRefreshAttitude = 0,
        TimeRefreshPolar = 0,
        TimeRefreshEnv = 0,
        TimeSendGps = 0,
        TimeSendBattery = 0,
        TimeSendAttitude = 0,
        TimeSendHeading = 0,
        TimeSendEnv = 0;

    bool TakeoffHappend = false;
    tMyGpsData LastShutdownPosition, StartupPosition;

    //=========== Add DataSources here
    cGpsDataSource Gps;
    cHeadingDataSource Heading;
    cWaterDataSource Water;
    cWindDataSource Wind;
    cTripDataSource Trip;
    cRudderDataSource Rudder;
    cNavigationDataSource Navigation;
    cAttitudeDataSource Attitude;
    cPolarDataSource Polar;
    cBatteryDataSource Battery;
    cEnvDataSource Env;

    //must be called only once at startup
    void Init(){
        //must go first. This sets up N2K "client", known messages, callbacks etc
        cN2K::Init();
        //this init each data source; order maters
        Gps.InitDataSource();
        Heading.InitDataSource(&Gps, &Attitude);
        Water.InitDataSource();
        Wind.InitDataSource(&Gps, &Heading);
        Battery.InitDataSource(1);
        Trip.InitDataSource();
        Rudder.InitDataSource();
        Navigation.InitDataSource(&Gps);
        Attitude.InitDataSource();
        Polar.InitDataSource(&Wind, &Water);
        Env.InitDataSource();
    }

    // must be called in the loop. This should go fast - without timer.
    // Otherwise we data from N2K will be missed and data for N2K will be updated too slow
    void Refresh(){
        //must go first. This actually updates global var SavedN2kMessages.
        TimeRefreshN2K = millis(); NMEA2000.ParseMessages(); TimeRefreshN2K = millis() - TimeRefreshN2K;
        // actually will parse saved messages; order maters
        TimeRefreshGps = millis(); Gps.RefreshData(); TimeRefreshGps = millis() - TimeRefreshGps;
        TimeRefreshHeading = millis(); Heading.RefreshData(); TimeRefreshHeading = millis() - TimeRefreshHeading;
        TimeRefreshWater = millis(); Water.RefreshData(); TimeRefreshWater = millis() - TimeRefreshWater;
        TimeRefreshWind = millis(); Wind.RefreshData(); TimeRefreshWind = millis() - TimeRefreshWind;
        TimeRefreshBattery = millis(); Battery.RefreshData(); TimeRefreshBattery = millis() - TimeRefreshBattery;
        TimeRefreshTrip = millis(); Trip.RefreshData(); TimeRefreshTrip = millis() - TimeRefreshTrip;
        TimeRefreshRudder = millis(); Rudder.RefreshData(); TimeRefreshRudder = millis() - TimeRefreshRudder;
        TimeRefreshNavigation = millis(); Navigation.RefreshData(); TimeRefreshNavigation = millis() - TimeRefreshNavigation;
        TimeRefreshAttitude = millis(); Attitude.RefreshData(); TimeRefreshAttitude = millis() - TimeRefreshAttitude;
        TimeRefreshPolar = millis(); Polar.RefreshData(); TimeRefreshPolar = millis() - TimeRefreshPolar;
        TimeRefreshEnv = millis(); Env.RefreshData(); TimeRefreshEnv = millis() - TimeRefreshEnv;
    }

    //must be called in the loop
    void SendToN2K(){
       //depending on the datasource config may send data to N2K
       //it also has independent timers inside. So GPS may be sent with short intervals, while environment once in some seconds
        TimeSendGps = millis(); Gps.SendDataToN2K(); TimeSendGps = millis() - TimeSendGps;
        TimeSendBattery = millis(); Battery.SendDataToN2K(); TimeSendBattery = millis() - TimeSendBattery;
        TimeSendAttitude = millis(); Attitude.SendDataToN2K(); TimeSendAttitude = millis() - TimeSendAttitude;
        TimeSendHeading = millis(); Heading.SendDataToN2K(); TimeSendHeading = millis() - TimeSendHeading;
        TimeSendEnv = millis(); Env.SendDataToN2K(); TimeSendEnv = millis() - TimeSendEnv;
    }

};

cBoatData BoatData; //define global var

