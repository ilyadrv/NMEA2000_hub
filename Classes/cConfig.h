class cConfig {
protected:
    bool ConfigFileExists = false;
public:
    struct tDataSourceConfig{
        String Source = ""; //source in Config n2k | device | n2k_device | device_n2k; empty - mean disabled
        int N2kSendInterval = 1000;
        unsigned char N2kPriority = 0; //0 means no data will be sent to N2K
    };

    static const unsigned char BatteryChargeMaxTableSize = 10;
    struct tBatteryChargeTable{
        double volts[BatteryChargeMaxTableSize] = {11.8,12.0,12.3,12.6,12.8};
        unsigned char percents[BatteryChargeMaxTableSize] = {0,25,50,75,100};
        unsigned char TableSize=5;
        double offset = 0.0; //if we need to consider static voltage drop
        double chargingVoltage = 13.0; //voltage which indicates it is charging
    };

    int
        page_refresh_speed = 1000, //Page refresh interval, ms
        loop_page_speed = 10, //Sec How long each page displayed
        downwind_awa_alarm = 30, //angel
        upwind_awa_alarm = 35, //angel
        aws_alarm_min = 5, //kn
        aws_alarm_max = 30,  //kn
        tws_alarm_min = 5,   //kn
        tws_alarm_max = 30,   //kn
        xte_alarm = 200, //meters
        gps_time_offset_minutes = 0, //minutes to be added to GPS time e.g. to fix DST
        water_temp_alarm_min = 5, //C deg
        stw_to_polar_bad = 50, //if Polar/STW considered bad, %
        stw_to_polar_good = 80, //if Polar/STW  considered good, %
        sog_to_wp_vmg_good = 80, //if Polar/STW  considered good, %
        data_log_speed = 10, //How often boat data will be logged, sec. !!!ATTENTION 0 means log disabled
        n2k_msg_ttl = 5000, //for how long n2k messages considered to be valid, ms. Keep it high for readings stability. This does not affect precision if data available;
        power_consumed_alarm = 12000,// power consume alarm, watt
        heel_direction_n2k = 1, //to set up if heel to S or to P [-1,1]
        heel_direction_device = 1, // ---//---
        trim_direction_n2k = 1, //to set up if trim to bow or to aft [-1,1]
        trim_direction_device = 1, // ---//---
        heel_offset_device = 0,
        heel_offset_n2k = 0,
        trim_offset_device = 0,
        trim_offset_n2k = 0;
    unsigned char
        battery_low = 20, // min battery charge alarm, %
        n2k_address = 15; //n2k address in the bus do not change it unless there are device conflict
    double
        sog_alarm_min = -0.1,//kn
        sog_alarm_max = 12.0,//kn
        stw_alarm_min = 1.5,//kn
        stw_alarm_max = 12.0,//kn
        min_move_speed = 1.5, //kn - to determine if we are moving. Also is used to filter out average SOG, STW
        depth_alarm_min = 2.1, //Meters from sensor
        polar_stw_min = 1.5, //below this polar stw is ignored
        depth_offset = 0.0, //M
        sog_ignore = 1.0, // SOG below which sog will be ignored and COG will be set to N/A. This is also applied to true wind calculations
        ameter_shunt_ratio = 1.0, //ampermeter must be connected through shunt and you ned to put ration here. For shunt 50A-75mV value is 65
        high_current_alarm = 5.0, // high current usage alarm
        heel_alarm = 20.0, // when heel abs value greater that alarm
        heading_compass_device_offset_roll = 0.0, //will be added to pitch of compass tilt  compensation, degree, more that +- 10 not recommended
        heading_compass_device_offset_pitch = 0.0, //will be added to pitch of compass tilt compensation, degree, more that +- 10 not recommended
        heading_compass_device_offset_yam = 0.0; //will be added to result compass heading, degree
    timezone
        tz = {0,0}; //timezone to set up device local time
    String
        boat_name = ""; //used for the log file name. Do not put weird things here
    bool
        gps_warn = true, //enable GPS alarm in status bar
        depth_warn = true,  //enable DEPTH alarm in status bar
        highlight_local_gps = false, //highlight field if GPS data is from local device, not from N2K
        highlight_local_attitude = false,
        highlight_local_heading = false,
        test=false,
        debug=false,
        heading_compass_calibration_reset = false,
        heading_compass_calibrate = false;
    tBatteryChargeTable
        batteries[2]; //maximum 2 batteries
    tDataSourceConfig
        gps_datasource, battery_datasource, water_datasource, wind_datasource, heading_datasource, trip_datasource,
        rudder_datasource, navigation_datasource, attitude_datasource, polar_datasource, env_datasource;

    /*
    Supported types *char, long, double, bool
    */
    void load() {
        File file = SD.open("/config.json");
        if (file && file.size() < 5 * 1024){
            DynamicJsonDocument json_buffer(5 * 1024);
            DeserializationError error = deserializeJson(json_buffer, file);
            if (!error){
                ConfigFileExists = true;
                boat_name = json_buffer["boat_name"].as<String>();
                test = (bool) json_buffer["test"];
                debug = (bool) json_buffer["debug"];
                if (json_buffer["n2k_msg_ttl"]){n2k_msg_ttl = (int)json_buffer["n2k_msg_ttl"];}
                if (json_buffer["n2k_address"]){n2k_address = (unsigned char)json_buffer["n2k_address"];}
                if (json_buffer["page_refresh_speed"]){page_refresh_speed = (int)json_buffer["page_refresh_speed"];}
                if (json_buffer["loop_page_speed"]){loop_page_speed = (int)json_buffer["loop_page_speed"];}
                if (json_buffer["downwind_awa_alarm"]){downwind_awa_alarm = (int)json_buffer["downwind_awa_alarm"];}
                if (json_buffer["upwind_awa_alarm"]){upwind_awa_alarm = (int)json_buffer["upwind_awa_alarm"];}
                if (json_buffer["aws_alarm_min"]){aws_alarm_min = (int)json_buffer["aws_alarm_min"];}
                if (json_buffer["aws_alarm_max"]){aws_alarm_max = (int)json_buffer["aws_alarm_max"];}
                if (json_buffer["tws_alarm_min"]){tws_alarm_min = (int)json_buffer["tws_alarm_min"];}
                if (json_buffer["tws_alarm_max"]){tws_alarm_max = (int)json_buffer["tws_alarm_max"];}
                if (json_buffer["gps_time_offset_minutes"]){gps_time_offset_minutes = (int)json_buffer["gps_time_offset_minutes"];}
                if (json_buffer["sog_alarm_min"]){sog_alarm_min = (double)json_buffer["sog_alarm_min"];}
                if (json_buffer["sog_alarm_max"]){sog_alarm_max = (double)json_buffer["sog_alarm_max"];}
                if (json_buffer["stw_alarm_min"]){stw_alarm_min = (double)json_buffer["stw_alarm_min"];}
                if (json_buffer["stw_alarm_max"]){stw_alarm_max = (double)json_buffer["stw_alarm_max"];}
                if (json_buffer["min_move_speed"] > 0.1){min_move_speed = (double)json_buffer["min_move_speed"];}
                if (json_buffer["water_temp_alarm_min"]){water_temp_alarm_min = (int)json_buffer["water_temp_alarm_min"];}
                if (json_buffer["depth_alarm_min"]){depth_alarm_min = (double)json_buffer["depth_alarm_min"];}
                if (json_buffer["stw_to_polar_bad"]){stw_to_polar_bad = (int)json_buffer["stw_to_polar_bad"];}
                if (json_buffer["stw_to_polar_good"]){stw_to_polar_good = (int)json_buffer["stw_to_polar_good"];}
                if (json_buffer["sog_to_wp_vmg_good"]){sog_to_wp_vmg_good = (int)json_buffer["sog_to_wp_vmg_good"];}
                if (json_buffer["polar_stw_min"] > 0.1){polar_stw_min = (double)json_buffer["polar_stw_min"];}
                if (json_buffer["sog_ignore"]){sog_ignore = (double)json_buffer["sog_ignore"];}
                if(sog_ignore < 0.1) sog_ignore = 0.1; //to avoid zero divisions
                if (json_buffer["depth_offset"]){depth_offset = (double)json_buffer["depth_offset"];}
                gps_warn = (bool) json_buffer["gps_warn"];
                depth_warn = (bool) json_buffer["depth_warn"];
                if (json_buffer["data_log_speed"]){data_log_speed = (int)json_buffer["data_log_speed"];}
                if (json_buffer["battery_low"]){battery_low = (int)json_buffer["battery_low"];}
                if (json_buffer["ameter_shunt_ratio"]){ameter_shunt_ratio = (double)json_buffer["ameter_shunt_ratio"];}
                if (json_buffer["high_current_alarm"]){high_current_alarm = (double)json_buffer["high_current_alarm"];}
                if (json_buffer["power_consumed_alarm"]){power_consumed_alarm = (int)json_buffer["power_consumed_alarm"];}
                if (json_buffer["heel_alarm"]){heel_alarm = (double)json_buffer["heel_alarm"];}
                highlight_local_gps = (bool) json_buffer["highlight_local_gps"];
                highlight_local_attitude = (bool) json_buffer["highlight_local_attitude"];
                highlight_local_heading = (bool) json_buffer["highlight_local_heading"];
                if (json_buffer["heel_offset_device"]){heel_offset_device = (int)json_buffer["heel_offset_device"];}
                if (json_buffer["heel_offset_n2k"]){heel_offset_n2k = (int)json_buffer["heel_offset_n2k"];}
                if (json_buffer["trim_offset_device"]){trim_offset_device = (int)json_buffer["trim_offset_device"];}
                if (json_buffer["trim_offset_n2k"]){trim_offset_n2k = (int)json_buffer["trim_offset_n2k"];}
                heel_direction_n2k = ((int)json_buffer["heel_direction_n2k"] == -1) ? -1 : 1;
                heel_direction_device = ((int)json_buffer["heel_direction_device"] == -1) ? -1 : 1;
                trim_direction_n2k = ((int)json_buffer["trim_direction_n2k"] == -1) ? -1 : 1;
                trim_direction_device = ((int)json_buffer["trim_direction_device"] == -1) ? -1 : 1;
                if (json_buffer["heading_compass_device_offset_roll"]){heading_compass_device_offset_roll = (double)json_buffer["heading_compass_device_offset_roll"];}
                if (json_buffer["heading_compass_device_offset_pitch"]){heading_compass_device_offset_pitch = (double)json_buffer["heading_compass_device_offset_pitch"];}
                if (json_buffer["heading_compass_device_offset_yam"]){heading_compass_device_offset_yam = (double)json_buffer["heading_compass_device_offset_yam"];}
                heading_compass_calibration_reset = (bool) json_buffer["heading_compass_calibration_reset"];
                heading_compass_calibrate = (bool) json_buffer["heading_compass_calibrate"];

                for (int i = 0; i<2; i++){
                    if (json_buffer["battery"] && json_buffer["battery"][i]){
                        tBatteryChargeTable _bat;
                        if(0 < json_buffer["battery"][i]["charge_v"].size() && json_buffer["battery"][i]["charge_v"].size() <= BatteryChargeMaxTableSize &&
                            json_buffer["battery"][i]["charge_v"].size() == json_buffer["battery"][i]["charge_p"].size()
                        ){
                            for (int j=0; j < json_buffer["battery"][i]["charge_v"].size(); j++){
                                batteries[i].volts[j] = (double)json_buffer["battery"][i]["charge_v"][j];
                                batteries[i].percents[j] = (int)json_buffer["battery"][i]["charge_p"][j];
                            }
                            batteries[i].TableSize = json_buffer["battery"][i]["charge_v"].size();
                        }
                        if(json_buffer["battery"][i]["offset"]){
                            batteries[i].offset = (double)json_buffer["battery"][i]["offset"];
                        }
                        if(json_buffer["battery"][i]["charging_v"]){
                            batteries[i].chargingVoltage = (double)json_buffer["battery"][i]["charging_v"];
                        }
                    }
                }

                //datasources
                if (ConfigFileExists){
                    datasource_config(json_buffer["battery_datasource"], battery_datasource);
                    datasource_config(json_buffer["gps_datasource"], gps_datasource);
                    datasource_config(json_buffer["water_datasource"], water_datasource);
                    datasource_config(json_buffer["wind_datasource"], wind_datasource);
                    datasource_config(json_buffer["heading_datasource"], heading_datasource);
                    datasource_config(json_buffer["trip_datasource"], trip_datasource);
                    datasource_config(json_buffer["rudder_datasource"], rudder_datasource);
                    datasource_config(json_buffer["navigation_datasource"], navigation_datasource);
                    datasource_config(json_buffer["attitude_datasource"], attitude_datasource);
                    datasource_config(json_buffer["polar_datasource"], polar_datasource);
                    datasource_config(json_buffer["env_datasource"], env_datasource);
                }
            }
            else{
                _debug("Bad config.json");
                _debug(error.f_str());
            }
            file.close();
        }
        else{
           _debug("config.json is missing or too big");
        }
        // if no sd we still want to have datasources as emergency
        if (!ConfigFileExists){
            battery_datasource.Source = "n2k_device";
            gps_datasource.Source = "n2k_device";
            water_datasource.Source = "n2k_device";
            wind_datasource.Source = "n2k_device";
            heading_datasource.Source = "n2k_device";
            trip_datasource.Source = "n2k_device";
            rudder_datasource.Source = "n2k_device";
            navigation_datasource.Source = "n2k_device";
            attitude_datasource.Source = "n2k_device";
            polar_datasource.Source = "n2k_device";
            env_datasource.Source = "n2k_device";
        }
    }

protected:
    void datasource_config(const JsonObject &node, tDataSourceConfig &ConfigNode){
        if(node["source"]) {
            char buffer[20];
            strlcpy(buffer, node["source"],  sizeof(buffer));
            ConfigNode.Source = String(buffer);
            if(node["n2k_send_intrerval"]) ConfigNode.N2kSendInterval = (int)node["n2k_send_intrerval"];
            if(node["n2k_priority"]) ConfigNode.N2kPriority = (int)node["n2k_priority"];
        }
    }
};
cConfig DeviceConfig;
