class cLog{

public:
    static String lastBoatData;

    static String boat_data_csv_headers(){
        //IMPORTANT! no spaces in headers. Or web display will not work.
        return String("time,uptime,")
            + "lat,lon,sog,cog,mcog,varia,alt,gps_time,gps_src,avg_sog,"
            + "stw,w_depth,w_temp,avg_stw,"
            + "aws,tws,awa,twa,twd,max_aws,max_tws,"
            + "head_t,head_m,head_var,head_src,"
            + "rud,"
            + "polar_stw,polar_avg,boat_to_polar,boat_to_polar_avg,"
            + "bat_volt,bat_cur,pow_used,bat_temp,bat_charge,bat_charging,"
            + "nav_xte,nav_wp_distance,nav_eta,nav_wp_bearing_t,nav_wp_bearing_m,nav_dest_lat,nav_dest_lon,nav_wp_vmg,"
            + "heel,trim,max_heel,max_trim,att_src,"
            + "air_t,air_h,air_p";
    }

    static String logFiles(){
        File dir = SD.open("/logs");
        if (dir) {
            DynamicJsonDocument _doc(10 * 1024);
            unsigned char l = 0;
            unsigned char d = 0;
            String result;
            while (true) {
                File entry =  dir.openNextFile();
                if (!entry) break;
                if (!entry.isDirectory()) {
                    String _name = String(entry.name());
                    _debug(_name);
                    if (_name.startsWith("/logs/log_") && l < 100){
                        _debug("1");
                        _doc["log"][l++] = _name;
                    }
                    else if (_name.startsWith("/logs/debug_") && d < 100){
                        _debug("2");
                        _doc["debug"][d++] = _name;
                    }
                }
            }
            serializeJson(_doc, result);
            return result;
        }
        else{
            return String("{}");
        }
    }

protected:
    static unsigned long timer;  // Time for log writing
    static String _last_pgns;
    static long Now;
    static cBoatData *Data;
    static cConfig *Config;

    static String boat_data_to_csv(){
        String _result =
        Data->Gps.TimeStr(Now, "%Y-%m-%d %H:%M:%S")+","
            + uptime() + ","
        + Data->Gps.LatitudeDecStr() + ","
            +  Data->Gps.LongitudeDecStr() + ","
            +  Data->Gps.SOGStr() + ","
            +  Data->Gps.COGStr() + ","
            +  Data->Gps.MCOGStr() + ","
            +  Data->Gps.VariationStr() + ","
            +  Data->Gps.AltitudeStr() + ","
            +  Data->Gps.TimeStr("%Y-%m-%d %H:%M:%S") + ","
            +  Data->Gps.Source + ","
            +  Data->Gps.AvgSOGStr() + ","
        + Data->Water.STWStr() + ","
            +  Data->Water.DepthStr() + ","
            +  Data->Water.TempStr() + ","
            +  Data->Water.AvgSTWStr() + ","
        + Data->Wind.AWSStr() + ","
            +  Data->Wind.TWSStr() + ","
            +  Data->Wind.AWAStr() + ","
            +  Data->Wind.TWAStr() + ","
            +  Data->Wind.TWDStr() + ","
            +  Data->Wind.MaxAWSStr() + ","
            +  Data->Wind.MaxTWSStr() + ","
        + Data->Heading.HeadingTStr() + ","
            +  Data->Heading.HeadingMStr() + ","
            +  Data->Heading.VariationStr() + ","
            +  Data->Heading.Source + ","
        + Data->Rudder.PositionStr() + ","
        + Data->Polar.STWStr() + ","
            +  Data->Polar.AvgSTWStr() + ","
            +  Data->Polar.BoatToPolarStr() + ","
            +  Data->Polar.BoatToPolarAvgStr() + ","
        + Data->Battery.VoltageStr() + ","
            +  Data->Battery.CurrentStr() + ","
            +  Data->Battery.PowerConsumedStr() + ","
            +  Data->Battery.TemperatureStr() + ","
            +  Data->Battery.ChargeStr() + ","
            +  String(Data->Battery.Charging()) + ","
        + Data->Navigation.XTEStr() + ","
            + Data->Navigation.DistanceToWaypointStr() + ","
            + Data->Navigation.ETAStr() + ","
            + Data->Navigation.BearingPositionToWPTStr() + ","
            + Data->Navigation.BearingPositionToWPMStr() + ","
            + Data->Navigation.DestinationLatitudeDecStr() + ","
            + Data->Navigation.DestinationLongitudeDecStr() + ","
            + Data->Navigation.VMGToWPStr() + ","
        + Data->Attitude.HeelStr() + ","
            + Data->Attitude.TrimStr() + ","
            + Data->Attitude.MaxHeelStr() + ","
            + Data->Attitude.MaxTrimStr() + ","
            + Data->Attitude.Source + ","
        + Data->Env.TempStr() + ","
            + Data->Env.HumidityStr() + ","
            + Data->Env.PressureStr();

        _result.replace(Data->Gps.EmptyValue, ""); //save some space
        return _result;
    }

    static void log_debug(){
        if (!Config->debug) return;
        String filename = String("/logs/debug_") + Data->Gps.TimeStr(Now, "%Y-%m-%d") + ".csv";
        bool _fileExists = SD.exists(filename);
        File _debug_file = SD.open(filename, FILE_WRITE);
        if (_fileExists){
             _debug_file.seek(_debug_file.size());  //append
        }
        else{
             _debug_file.println(
                String("dev_time,uptime,pseudo_fps,free_heap,min_free_heap,pgns,") +
                "refresh_n2k,refresh_gps,refresh_heading,refresh_water,refresh_wind,refresh_battery,refresh_trip," +
                "refresh_rudder,refresh_navigation,refresh_attitude,refresh_polar,refresh_env,send_gps,send_battery," +
                "send_attitude,send_heading,send_env"
            ); //csv headers
        }
        String _line = Data->Gps.TimeStr(Now, "%Y-%m-%d %H:%M:%S") + ","
        + uptime() + ","
        + String(_pseudo_fps) + ","
        + String(ESP.getFreeHeap()) + ","
        + String(esp_get_minimum_free_heap_size())+ ",";
        if (cN2K::allN2kPGNs != _last_pgns){ //if strings are equal save some space in the log
            _line += cN2K::allN2kPGNs;
            _last_pgns = cN2K::allN2kPGNs;
        }
        _line += ","
            + String(Data->TimeRefreshN2K) + ","
            + String(Data->TimeRefreshGps) + ","
            + String(Data->TimeRefreshHeading) + ","
            + String(Data->TimeRefreshWater) + ","
            + String(Data->TimeRefreshWind) + ","
            + String(Data->TimeRefreshBattery) + ","
            + String(Data->TimeRefreshTrip) + ","
            + String(Data->TimeRefreshRudder) + ","
            + String(Data->TimeRefreshNavigation) + ","
            + String(Data->TimeRefreshAttitude) + ","
            + String(Data->TimeRefreshPolar) + ","
            + String(Data->TimeRefreshEnv) + ","
            + String(Data->TimeSendGps) + ","
            + String(Data->TimeSendBattery) + ","
            + String(Data->TimeSendAttitude) + ","
            + String(Data->TimeSendHeading) + ","
            + String(Data->TimeSendEnv) + ",";
        _debug_file.println(_line);
        _debug_file.close();
    }

public:
    static void Init(cBoatData *_data, cConfig *_config){
        Data = _data;
        Config = _config;
    }

    // The file is one per day.
    // No hot swap possible. After card inserted device must be restarted
    static bool Log(long now){
        Now = now;
//        Now = 1607000001; // this can be used for debugging
        //write log only if there is Device time and respecting time intervals
        //we need a device time to address log
        //it will be nice if we can detect under motor in future
        //data_log_speed == 0 means disabled
        bool result = true;
        if (Data->Gps.TimeValid(Now) && Config->data_log_speed > 0
            && millis() > timer + Config->data_log_speed * 1000
        ) {
            //reset timer
            timer = millis();
            //get date based file name
            //file per day strategy
            String filename = String("/logs/log_") + Data->Gps.TimeStr(Now, "%Y-%m-%d") + ".csv";

            bool _fileExists = SD.exists(filename);
            File _log_file = SD.open(filename, FILE_WRITE);
            int _bytes_written = 0;
            if (_fileExists){
                _log_file.seek(_log_file.size());  //append
            }
            else{
                _log_file.println(boat_data_csv_headers()); //csv headers
            }
            lastBoatData = boat_data_to_csv();
            _bytes_written = _log_file.println(lastBoatData);
            _log_file.close();
            result = (_bytes_written > 5); //some bytes written?
            log_debug();
        }
        return result;
    }
};
unsigned long cLog::timer = 0;
String cLog::_last_pgns = "";
String cLog::lastBoatData = "";
long cLog::Now = 0;
cBoatData *cLog::Data = (cBoatData *) malloc(sizeof(cBoatData));
cConfig *cLog::Config = (cConfig *) malloc(sizeof(cConfig));