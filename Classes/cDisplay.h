class cDisplay{
protected:
    unsigned char _alarmDamper = 0;
    struct tDisplayPage {
        String tiles[15]; //max of 15 tiles.
        int tiles_count = 0;
        bool loop_exclude = false;
    };
    const unsigned char _maxDisplayPages = 32;
    tDisplayPage *DisplayPages = (tDisplayPage *) malloc(_maxDisplayPages * sizeof(tDisplayPage)); //allocate memory runtime because of compile time memory lack

    struct tLast {
        String
            Gps_SOG, Gps_AvgSOG, Gps_COG, Gps_MCOG, Gps_Latitude, Gps_Longitude, Gps_Variation, Gps_Altitude,
            Battery_Charge, Battery_Voltage, Battery_Current, Battery_PowerConsumed,
            Water_STW, Water_AvgSTW, Water_Depth, Water_Temp,
            Wind_AWS, Wind_TWS, Wind_MaxAWS, Wind_MaxTWS, Wind_AWA, Wind_TWA, Wind_TWD,
            Heading_HeadingT,Heading_HeadingM,
            Polar_STW, Polar_AvgSTW, Rudder_Position,
            Trip_TripLog, Trip_Log,Trip_TripTime,
            XTE, WpDistance, WpETA, WpBearingT, WpBearingM, DestLat, DestLon, WpVMG,
            Heel, Trim, MaxHeel, MaxTrim,
            AirTemp, Humidity, Pressure;
    } _last;

    struct tTile {
      int X, Y, w, h, textSize, textSmallSize, num, layout, valueTopOffset;
       uint16_t textColor, textBgColor, valueBgColor;
    };

    int prevPage = -1; // page displayed before current
    int last_page = -1; // last (maximum) page id
    const int mainFont = 2;
    bool _alarmsHappens = false;
    bool _alarmsUpdated = false;
    bool _heartBeatState = true;

    /*
    uint16_t
        bg_main = 0x439,
        bg_status = 0x1E9F,
        bg_alarm = RED,
        bg_good = GREEN,
        txt_col_main = WHITE,
        grid_col = WHITE;
    */

    //RGB565
    uint16_t
        c_red = 0x8800,
        c_green = 0x0320,
        bg_main = BLACK,
        bg_status = 0x7BEF,
        bg_alarm = 0x8800,
        bg_good = 0x0320,
        bg_highlight = 0x3186,
        txt_col_main = 0xC618,
        grid_col = 0xC618;

    int get_page_layout(){
        int tiles_count = DisplayPages[DeviceState.cur_page].tiles_count;
        if (tiles_count <= 4){return tiles_count;}
        if (5 <= tiles_count && tiles_count <= 6){return 6;}
        if (7 <= tiles_count && tiles_count <= 8){return 8;}
        if (tiles_count == 9){return 9;}
        if (10 <= tiles_count && tiles_count <= 12){return 12;}
        if (13 <= tiles_count && tiles_count <= 15){return 15;}
        return 1;
    }

    //====================== Labels
    String indexLabel(String index){
        if(index == "WaterTemperature") return "Temp water";
        if(index == "SOG") return "SOG";
        if(index == "AvgSOG") return "avg SOG";
        if(index == "COG") return "COG t";
        if(index == "MCOG") return "COG m";
        if(index == "Heading") return "Heading t";
        if(index == "HeadingM") return "Heading m";
        if(index == "Log") return "Log";
        if(index == "TripLog") return "Trip";
        if(index == "STW") return "STW";
        if(index == "AvgSTW") return "avg STW";
        if(index == "COORD") return "Lat/Lon";
        if(index == "AWS") return "AWS";
        if(index == "TWS") return "TWS";
        if(index == "AWA") return "AWA";
        if(index == "TWA") return "TWA";
        if(index == "MaxAws") return "max AWS";
        if(index == "MaxTws") return "max TWS";
        if(index == "TWD") return "TWD";
        if(index == "RudderPosition") return "Rudder";
        if(index == "Variation") return "Variation";
        if(index == "Altitude") return "Altitude";
        if(index == "PolarSTW") return "Polar STW";
        if(index == "TripTime") return "Trip time";
        if(index == "AvgPolarSTW") return "Polar avg";
        if(index == "BatteryCharge") return "Battery";
        if(index == "BatteryVolt") return "Battery";
        if(index == "BatteryCurrent") return "Current";
        if(index == "PowerConsumed") return "Power used";
        if(index == "WaterDepth") {
            if((DeviceConfig.depth_offset < -0.1))return "Depth surf";
            else if(DeviceConfig.depth_offset > 0.1) return "Depth keel";
            else return "Depth sens";
        }
        if(index == "XTE") return "XTE";
        if(index == "WpDistance") return "Wp dist";
        if(index == "WpETA") return "Wp ETA";
        if(index == "WpBearingT") return "Wp bear t";
        if(index == "WpBearingM") return "Wp bear m";
        if(index == "DestCoord") return "Destination";
        if(index == "WpVMG") return "Wp VMG";
        if(index == "Heel") return "Heel";
        if(index == "Trim") return "Trim";
        if(index == "MaxHeel") return "Heel max";
        if(index == "MaxTrim") return "Trim max";
        if(index == "AirTemp") return "Temp air";
        if(index == "Humidity") return "Humidity air";
        if(index == "Pressure") return "Pressure air";
        return "";
    }

    String indexUnit(String index){
        if(index == "WaterTemperature") return "c";
        if(index == "SOG") return "kn";
        if(index == "AvgSOG") return "kn";
        if(index == "COG") return "o";
        if(index == "MCOG") return "o";
        if(index == "Heading") return "o";
        if(index == "HeadingM") return "o";
        if(index == "Log") return "nm";
        if(index == "TripLog") return "nm";
        if(index == "STW") return "kn";
        if(index == "AvgSTW") return "kn";
        if(index == "COORD") return "o";
        if(index == "AWS") return "kn";
        if(index == "TWS") return "kn";
        if(index == "AWA") return "o";
        if(index == "TWA") return "o";
        if(index == "MaxAws") return "kn";
        if(index == "MaxTws") return "kn";
        if(index == "TWD") return "o";
        if(index == "RudderPosition") return "o";
        if(index == "Variation") return "o";
        if(index == "Altitude") return "m";
        if(index == "PolarSTW") return "kn";
        if(index == "TripTime") return "h:m";
        if(index == "AvgPolarSTW") return "kn";
        if(index == "BatteryCharge") return "%";
        if(index == "BatteryVolt") return "v";
        if(index == "BatteryCurrent") return "a";
        if(index == "PowerConsumed") return "w/h";
        if(index == "WaterDepth") return "m";
        if(index == "XTE") return "m";
        if(index == "WpDistance") return "nm";
        if(index == "WpETA") return "h:m";
        if(index == "WpBearingT") return "o";
        if(index == "WpBearingM") return "o";
        if(index == "DestCoord") return "o";
        if(index == "WpVMG") return "kn";
        if(index == "Heel") return "o";
        if(index == "Trim") return "o";
        if(index == "MaxHeel") return "o";
        if(index == "MaxTrim") return "o";
        if(index == "AirTemp") return "c";
        if(index == "Humidity") return "%";
        if(index == "Pressure") return "hpa";
        return "";
    }

    //=============== Alarms
    bool alarm_stw(){return (BoatData.Water.STW < DeviceConfig.stw_alarm_min || BoatData.Water.STW > DeviceConfig.stw_alarm_max);}
    bool alarm_avg_stw(){return (BoatData.Water.AvgSTW < DeviceConfig.stw_alarm_min || BoatData.Water.AvgSTW > DeviceConfig.stw_alarm_max);}
    bool alarm_water_temp(){return (BoatData.Water.Temp < DeviceConfig.water_temp_alarm_min);}
    bool alarm_depth(){return (BoatData.Water.Depth < DeviceConfig.depth_alarm_min);}
    bool alarm_avg_sog(){return (BoatData.Gps.AvgSOG < DeviceConfig.sog_alarm_min || BoatData.Gps.AvgSOG > DeviceConfig.sog_alarm_max);}
    bool alarm_aws(){return (BoatData.Wind.AWS < DeviceConfig.aws_alarm_min || BoatData.Wind.AWS > DeviceConfig.aws_alarm_max);}
    bool alarm_tws(){return (BoatData.Wind.TWS < DeviceConfig.tws_alarm_min || BoatData.Wind.TWS > DeviceConfig.tws_alarm_max);}
    bool alarm_awa(){return abs(BoatData.Wind.AWA) < DeviceConfig.upwind_awa_alarm || abs(BoatData.Wind.AWA) > 180 - DeviceConfig.downwind_awa_alarm;}
    bool alarm_max_aws(){return (BoatData.Wind.MaxAWS > DeviceConfig.aws_alarm_max);}
    bool alarm_max_tws(){return (BoatData.Wind.MaxTWS > DeviceConfig.tws_alarm_max);}
    bool alarm_polar_stw(){return (BoatData.Polar.STWValid() && BoatData.Polar.BoatToPolarValid() && BoatData.Polar.BoatToPolar <= DeviceConfig.stw_to_polar_bad);}

    bool good_polar_stw(){return (BoatData.Polar.STWValid() && BoatData.Polar.BoatToPolarValid()  && BoatData.Polar.BoatToPolar >= DeviceConfig.stw_to_polar_good);}
    bool good_polar_avg(){return (BoatData.Polar.AvgSTWValid() && BoatData.Polar.BoatToPolarAvgValid()  && BoatData.Polar.BoatToPolarAvg >= DeviceConfig.stw_to_polar_good);}
    bool good_wp_vmg(){
        return (
            BoatData.Navigation.DestinationLatitudeValid() && BoatData.Navigation.VMGToWPValid() && abs(BoatData.Navigation.VMGToWP) > DeviceConfig.sog_ignore &&
            BoatData.Gps.SOGValid() && BoatData.Gps.SOG > DeviceConfig.sog_ignore &&
            BoatData.Navigation.VMGToWP/BoatData.Gps.SOG * 100 >= DeviceConfig.sog_to_wp_vmg_good
        );
    }

    //-------------------------------

    tTile get_tile(int num, int layout, String index){
        tTile _result;
        _result.textBgColor = bg_main;
        _result.textColor = txt_col_main;
        _result.textSize = 4;
        _result.textSmallSize = 3;
        _result.Y = 0;
        _result.w = 320;
        _result.h = 210;
        _result.num = num;
        _result.layout = layout;
        _result.valueTopOffset = 15;
        _result.valueBgColor = bg_main;

        //@TODO - automation based on col/row
        if (layout == 2){
            _result.textSize = 3;
            _result.textSmallSize = 2;
            _result.w = 320;
            _result.h = 105;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 0;_result.Y = 106;}
        }

        if (layout == 3){
            _result.textSize = 3;
            _result.textSmallSize = 1;
            _result.w = 320;
            _result.h = 70;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 0;_result.Y = 70;}
            if (num == 3){_result.X = 0;_result.Y = 140;}
        }

        if (layout == 4){
            _result.textSize = 3;
            _result.textSmallSize = 0;
            _result.w = 160;
            _result.h = 105;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 161;_result.Y = 0;}

            if (num == 3){_result.X = 0;_result.Y = 106;}
            if (num == 4){_result.X = 161;_result.Y = 106;}
        }

        if (layout == 6){
            _result.textSize = 2;
            _result.textSmallSize = 0;
            _result.w = 160;
            _result.h = 70;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 161;_result.Y = 0;}

            if (num == 3){_result.X = 0;_result.Y = 71;}
            if (num == 4){_result.X = 161;_result.Y = 71;}

            if (num == 5){_result.X = 0; _result.Y = 141;}
            if (num == 6){_result.X = 161;_result.Y = 141;}
        }

        if (layout == 8){
            _result.textSize = 2;
            _result.textSmallSize = 0;
            _result.w = 160;
            _result.h = 52;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 161;_result.Y = 0;}

            if (num == 3){_result.X = 0;_result.Y = 53;}
            if (num == 4){_result.X = 161;_result.Y = 53;}

            if (num == 5){_result.X = 0; _result.Y = 105;}
            if (num == 6){_result.X = 161;_result.Y = 105;}

            if (num == 7){_result.X = 0;_result.Y = 157;}
            if (num == 8){_result.X = 161;_result.Y = 157;}
        }

        if (layout == 9){
            _result.textSize = 2;
            _result.textSmallSize = 0;
            _result.w = 106;
            _result.h = 70;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 106;_result.Y = 0;}
            if (num == 3){_result.X = 213;_result.Y = 0;}

            if (num == 4){_result.X = 0;_result.Y = 71;}
            if (num == 5){_result.X = 106;_result.Y = 71;}
            if (num == 6){_result.X = 213;_result.Y = 71;}

            if (num == 7){_result.X = 0;_result.Y = 141;}
            if (num == 8){_result.X = 106;_result.Y = 141;}
            if (num == 9){_result.X = 213;_result.Y = 141;}
        }

        if (layout == 12){
            _result.textSize = 1;
            _result.textSmallSize = 0;
            _result.w = 106;
            _result.h = 52;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 106;_result.Y = 0;}
            if (num == 3){_result.X = 213;_result.Y = 0;}

            if (num == 4){_result.X = 0;_result.Y = 53;}
            if (num == 5){_result.X = 106;_result.Y = 53;}
            if (num == 6){_result.X = 213;_result.Y = 53;}

            if (num == 7){_result.X = 0;_result.Y = 105;}
            if (num == 8){_result.X = 106;_result.Y = 105;}
            if (num == 9){_result.X = 213;_result.Y = 105;}

            if (num == 10){_result.X = 0;_result.Y = 157;}
            if (num == 11){_result.X = 106;_result.Y = 157;}
            if (num == 12){_result.X = 213;_result.Y = 157;}
        }

        if (layout == 15){
            _result.textSize = 1;
            _result.textSmallSize = 0;
            _result.w = 106;
            _result.h = 48;
            if (num == 1){_result.X = 0;_result.Y = 0;}
            if (num == 2){_result.X = 106;_result.Y = 0;}
            if (num == 3){_result.X = 213;_result.Y = 0;}

            if (num == 4){_result.X = 0;_result.Y = 48;}
            if (num == 5){_result.X = 106;_result.Y = 48;}
            if (num == 6){_result.X = 213;_result.Y = 48;}

            if (num == 7){_result.X = 0;_result.Y = 96;}
            if (num == 8){_result.X = 106;_result.Y = 96;}
            if (num == 9){_result.X = 213;_result.Y = 96;}

            if (num == 10){_result.X = 0;_result.Y = 144;}
            if (num == 11){_result.X = 106;_result.Y = 144;}
            if (num == 12){_result.X = 213;_result.Y = 144;}

            if (num == 13){_result.X = 0;_result.Y = 192;}
            if (num == 14){_result.X = 106;_result.Y = 192;}
            if (num == 15){_result.X = 213;_result.Y = 192;}
        }

        return _result;
    }

    //compare values only considering one floating sign
    bool _the_same(double current, double last){
        return ((int)trunc(current * 10) == (int)trunc(last * 10));
    }

    bool _the_same_precise(double current, double last){
        return ((int)trunc(current * 1000000) == (int)trunc(last * 1000000));
    }

    void _print_index_value(String index, tTile tile){
        bool _same = false;
        String _val = "";
        if (index == "COORD"){_same = (BoatData.Gps.LatitudeStr() == _last.Gps_Latitude && BoatData.Gps.LongitudeStr() == _last.Gps_Longitude);}
        else if (index == "Variation"){_val = BoatData.Gps.VariationStr(); _same = (_val == _last.Gps_Variation);}
        else if (index == "Altitude"){_val = BoatData.Gps.AltitudeStr(); _same = (_val == _last.Gps_Altitude);}
        else if (index == "COG"){_val = BoatData.Gps.COGStr(); _same = (_val == _last.Gps_COG);}
        else if (index == "MCOG"){_val = BoatData.Gps.MCOGStr(); _same = (_val == _last.Gps_MCOG);}
        else if (index == "SOG"){_val = BoatData.Gps.SOGStr(); _same = (_val == _last.Gps_SOG);}
        else if (index == "AvgSOG"){_val = BoatData.Gps.AvgSOGStr(); _same = (_val == _last.Gps_AvgSOG);}

        else if (index == "Heading"){_val = BoatData.Heading.HeadingTStr(); _same = (_val == _last.Heading_HeadingT);}
        else if (index == "HeadingM"){_val = BoatData.Heading.HeadingMStr(); _same = (_val == _last.Heading_HeadingM);}

        else if (index == "STW"){_val = BoatData.Water.STWStr(); _same = (_val == _last.Water_STW);}
        else if (index == "WaterDepth"){_val = BoatData.Water.DepthStr(); _same = (_val == _last.Water_Depth);}
        else if (index == "WaterTemperature"){_val = BoatData.Water.TempStr(); _same = (_val == _last.Water_Temp);}
        else if (index == "AvgSTW"){_val = BoatData.Water.AvgSTWStr(); _same = (_val == _last.Water_AvgSTW);}

        else if (index == "BatteryCharge"){_val = BoatData.Battery.ChargeStr(); _same = (BoatData.Battery.VoltageStr() == _last.Battery_Voltage);} //here we checking _same by voltage
        else if (index == "BatteryVolt"){_val = BoatData.Battery.VoltageStr(); _same = (_val == _last.Battery_Voltage);}
        else if (index == "BatteryCurrent"){_val = BoatData.Battery.CurrentStr(); _same = (_val == _last.Battery_Current);}
        else if (index == "PowerConsumed"){_val = BoatData.Battery.PowerConsumedStr(); _same = (_val == _last.Battery_PowerConsumed);}

        else if (index == "TripLog"){_val = BoatData.Trip.TripLogStr(); _same = (_val == _last.Trip_TripLog);}
        else if (index == "Log"){_val = BoatData.Trip.LogStr(); _same = (_val == _last.Trip_Log);}
        else if (index == "TripTime"){_val = BoatData.Trip.TripTimeStr(); _same = (_val == _last.Trip_TripTime);}

        else if (index == "AWS"){_val = BoatData.Wind.AWSStr(); _same = (_val == _last.Wind_AWS);}
        else if (index == "TWS"){_val = BoatData.Wind.TWSStr(); _same = (_val == _last.Wind_TWS);}
        else if (index == "AWA"){_val = BoatData.Wind.AWAStr(); _same = (_val == _last.Wind_AWA);}
        else if (index == "TWA"){_val = BoatData.Wind.TWAStr(); _same = (_val == _last.Wind_TWA);}
        else if (index == "MaxAws"){_val = BoatData.Wind.MaxAWSStr(); _same = (_val == _last.Wind_MaxAWS);}
        else if (index == "MaxTws"){_val = BoatData.Wind.MaxTWSStr(); _same = (_val == _last.Wind_MaxTWS);}
        else if (index == "TWD"){_val = BoatData.Wind.TWDStr(); _same = (_val == _last.Wind_TWD);}

        else if (index == "RudderPosition"){_val = BoatData.Rudder.PositionStr(); _same = (_val == _last.Rudder_Position);}

        else if (index == "PolarSTW"){_val = BoatData.Polar.STWStr(); _same = (_val == _last.Polar_STW);}
        else if (index == "AvgPolarSTW"){_val = BoatData.Polar.AvgSTWStr(); _same = (_val == _last.Polar_AvgSTW);}

        else if (index == "XTE"){_val = BoatData.Navigation.XTEStr(); _same = (_val == _last.XTE);}
        else if (index == "WpDistance"){_val = BoatData.Navigation.DistanceToWaypointStr(); _same = (_val == _last.WpDistance);}
        else if (index == "WpETA"){_val = BoatData.Navigation.ETAStr(); _same = (_val == _last.WpETA);}
        else if (index == "WpBearingT"){_val = BoatData.Navigation.BearingPositionToWPTStr(); _same = (_val == _last.WpBearingT);}
        else if (index == "WpBearingM"){_val = BoatData.Navigation.BearingPositionToWPMStr(); _same = (_val == _last.WpBearingM);}
        else if (index == "WpVMG"){_val = BoatData.Navigation.VMGToWPStr(); _same = (_val == _last.WpVMG);}
        else if (index == "DestCoord"){_same = (BoatData.Navigation.DestinationLatitudeStr() == _last.DestLat && BoatData.Navigation.DestinationLongitudeStr() == _last.DestLon);}

        else if (index == "Heel"){_val = BoatData.Attitude.HeelStr(); _same = (_val == _last.Heel);}
        else if (index == "Trim"){_val = BoatData.Attitude.TrimStr(); _same = (_val == _last.Trim);}
        else if (index == "MaxHeel"){_val = BoatData.Attitude.MaxHeelStr(); _same = (_val == _last.MaxHeel);}
        else if (index == "MaxTrim"){_val = BoatData.Attitude.MaxTrimStr(); _same = (_val == _last.MaxTrim);}

        else if (index == "AirTemp"){_val = BoatData.Env.TempStr(); _same = (_val == _last.AirTemp);}
        else if (index == "Humidity"){_val = BoatData.Env.HumidityStr(); _same = (_val == _last.Humidity);}
        else if (index == "Pressure"){_val = BoatData.Env.PressureStr(); _same = (_val == _last.Pressure);}

        // do not reprint the same values on the same page
        //force reprint last row if alarms
        if(DeviceState.cur_page == prevPage && _same && !(tile.num >= 12 && _alarmsUpdated)){
            return;
        }

        //reduce font for coordinates
        if ((index == "COORD" || index == "DestCoord")){
            tile.textSize = tile.textSmallSize;
        }

        M5.Lcd.setTextColor(tile.textColor); //M5.Lcd.setTextColor(tile.textColor, tile.textBgColor);
        int char_h = 11;
        if (tile.textSize == 1) {
            char_h = 16;
            M5.Lcd.setTextSize(1);
            M5.Lcd.setFont(FSS12);
        }
        else if (tile.textSize == 2) {
            M5.Lcd.setTextSize(1);
            M5.Lcd.setFont(FSS18);
            char_h = 28;
        }
        else if (tile.textSize == 3) {
            char_h = 40;
            M5.Lcd.setTextSize(1);
            M5.Lcd.setFont(FSS24);
        }
        else if (tile.textSize == 4) {
            M5.Lcd.setTextSize(2);
            M5.Lcd.setFont(FSS24);
            char_h = 40;
        }
        else{
            char_h = 11;
            M5.Lcd.setTextSize(1);
            M5.Lcd.setTextFont(mainFont);
        }

        // set values to be in the center
        int center_X = round(tile.X +  tile.w/2);
        int center_Y = round(tile.Y + tile.valueTopOffset/2 + tile.h/2);

        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.fillRect(
            tile.X+1,
            tile.Y + tile.valueTopOffset,
            tile.w-1,
            tile.h - tile.valueTopOffset - 1,
            tile.valueBgColor
        );
        if (index == "COORD"){
            M5.Lcd.drawString(BoatData.Gps.LatitudeStr(), center_X, center_Y - round(char_h/2) - 2);
            M5.Lcd.drawString(BoatData.Gps.LongitudeStr(), center_X, center_Y +  round(char_h/2) + 2);
        }
        else if(index == "DestCoord" && BoatData.Navigation.DestinationLatitudeValid()){
            M5.Lcd.drawString(BoatData.Navigation.DestinationLatitudeStr(), center_X, center_Y - round(char_h/2) - 2);
            M5.Lcd.drawString(BoatData.Navigation.DestinationLongitudeStr(), center_X, center_Y +  round(char_h/2) + 2);
        }
        else{
            M5.Lcd.drawString(_val, center_X, center_Y);
        }
    }

    bool _index_alarm(String index){
        if (index == "COORD") return !BoatData.Gps.CoordinatesValid();
        if (index == "Variation") return !BoatData.Gps.VariationValid();
        if (index == "Altitude") return !BoatData.Gps.AltitudeValid();
        if (index == "SOG") return (BoatData.Gps.SOG > DeviceConfig.sog_ignore && (BoatData.Gps.SOG < DeviceConfig.sog_alarm_min || BoatData.Gps.SOG > DeviceConfig.sog_alarm_max));
        if (index == "COG") return !BoatData.Gps.COGValid() && BoatData.Gps.SOG > DeviceConfig.sog_ignore;
        if (index == "AvgSOG") return alarm_avg_sog();

        if (index == "STW") return (!BoatData.Water.STWValid() || alarm_stw());
        if (index == "WaterDepth") return (!BoatData.Water.DepthValid() || alarm_depth());
        if (index == "WaterTemperature") return (!BoatData.Water.TempValid() || alarm_water_temp());
        if (index == "AvgSTW") return alarm_avg_stw();

        if (index == "BatteryCharge" || index == "BatteryVolt") return (!BoatData.Battery.SourceValid() || BoatData.Battery.Low());
        if (index == "BatteryCurrent") return (!BoatData.Battery.CurrentValid() || BoatData.Battery.Current >= DeviceConfig.high_current_alarm);
        if (index == "PowerConsumed") return (!BoatData.Battery.PowerConsumedValid() || BoatData.Battery.PowerConsumed >= DeviceConfig.power_consumed_alarm);

        if (index == "Heading") return !BoatData.Heading.HeadingTValid();
        if (index == "HeadingM") return !BoatData.Heading.HeadingMValid();

        if (index == "Log") return !BoatData.Trip.LogValid();
        if (index == "TripLog") return !BoatData.Trip.TripLogValid();

        if (index == "AWS") return (!BoatData.Wind.AWSValid() || alarm_aws());
        if (index == "TWS") return (!BoatData.Wind.TWSValid() || alarm_tws());
        if (index == "AWA") return (!BoatData.Wind.AWAValid() || alarm_awa());
        if (index == "TWA") return !BoatData.Wind.TWAValid();
        if (index == "MaxAws") return (!BoatData.Wind.MaxAWSValid() || alarm_max_aws());
        if (index == "MaxTws") return (!BoatData.Wind.MaxTWSValid() || alarm_max_tws());
        if (index == "TWD") return !BoatData.Wind.TWDValid();

        if (index == "RudderPosition") return !BoatData.Rudder.PositionValid();

        if (index == "PolarSTW") return alarm_polar_stw();

        if (BoatData.Navigation.DestinationLatitudeValid()){
            if (index == "XTE") return (abs(BoatData.Navigation.XTE) > DeviceConfig.xte_alarm);
            if (index == "WpDistance") return !BoatData.Navigation.DistanceToWaypointValid();
            if (index == "WpETA") return !BoatData.Navigation.ETAValid();
            if (index == "WpBearingT") return !BoatData.Navigation.BearingPositionToWPTValid();
            if (index == "WpBearingM") return !BoatData.Navigation.BearingPositionToWPMValid();
            if (index == "DestCoord") return !(BoatData.Navigation.DestinationLatitudeValid() && BoatData.Navigation.DestinationLongitudeValid());
            if (index == "WpVMG") return !BoatData.Navigation.VMGToWPValid();
        }

        if (BoatData.Attitude.SourceEnabled()){
            if (index == "Heel") return abs(BoatData.Attitude.Heel) > DeviceConfig.heel_alarm;
            if (index == "Trim") return !BoatData.Attitude.TrimValid();
            if (index == "MaxHeel") return abs(BoatData.Attitude.MaxHeel) > DeviceConfig.heel_alarm;
        }

        return false;
    }

    bool _index_good(String index){
        if (index == "PolarSTW") return good_polar_stw();
        if (index == "AvgPolarSTW") return good_polar_avg();
        if (index == "BatteryCharge" || index == "BatteryVolt") return BoatData.Battery.Charging();
        if (index == "WpVMG") return good_wp_vmg();
        return false;
    }

    //if both datasource available we would like to know from which the data comes
    bool _highlight_local_index(String index){
        if (DeviceConfig.highlight_local_gps &&
            (index == "SOG" || index == "COG" || index == "MCOG" || index == "COORD" || index == "Variation" || index == "Altitude") &&
            BoatData.Gps.SourceDevice()
        ) return true;

        if (DeviceConfig.highlight_local_attitude && (index == "Heel" || index == "Trim") && BoatData.Attitude.SourceDevice()) return true;

        if (DeviceConfig.highlight_local_heading && (index == "Heading" || index == "HeadingM") && BoatData.Heading.SourceDevice()) return true;

        return false;
    }

    void _print_tile(String index, int num, int layout){
        tTile tile = get_tile(num, layout, index);
        //order is important
        if(_index_good(index)){
            tile.valueBgColor = bg_good;
        }
        if(_highlight_local_index(index)){
            tile.valueBgColor = bg_highlight;
        }
        if(_index_alarm(index)){
            tile.valueBgColor = bg_alarm;
        }

        if (DeviceState.cur_page != prevPage){
            M5.Lcd.setTextColor(txt_col_main);
            M5.Lcd.setTextSize(1);
            M5.Lcd.setTextFont(mainFont);

            //LABEL
            M5.Lcd.setTextDatum(TL_DATUM);
            M5.Lcd.drawString(indexLabel(index), tile.X + 5, tile.Y, mainFont);

            //UNIT
            M5.Lcd.setTextDatum(TR_DATUM);
            M5.Lcd.drawString(indexUnit(index), tile.X + tile.w - 5, tile.Y, mainFont);
        }

        //if there were alarms we need to redraw tile if it no status bar
        _print_index_value(index, tile);
    }

    void drawLayout(int layout){

       if (DeviceState.cur_page != prevPage){
            if (layout == 15){
                M5.Lcd.fillRect(0, 0, 320, 240, bg_main); //main are cleanup
            }
            else{
                M5.Lcd.fillRect(0, 0, 320, 210, bg_main); //main are cleanup
            }
       }
       //if there were alarms we need to redraw grid for layout #15
       if (DeviceState.cur_page != prevPage || (layout == 15 && _alarmsUpdated)){
            if (layout == 2){
                M5.Lcd.drawLine(0,105, 320,105,grid_col); //horizontal
            }
            if (layout == 3){
                M5.Lcd.drawLine(0,70, 320,70,grid_col); //horizontal
                M5.Lcd.drawLine(0,140, 320,140,grid_col); //horizontal
            }
            if (layout == 4){
                M5.Lcd.drawLine(0,105, 320,105,grid_col); //horizontal
                M5.Lcd.drawLine(160, 0, 160, 210,grid_col); //vertical
            }
            if (layout == 6){
                M5.Lcd.drawLine(0,70, 320,70,grid_col); //horizontal
                M5.Lcd.drawLine(0,140, 320,140,grid_col); //horizontal

                M5.Lcd.drawLine(160, 0, 160, 210,grid_col); //vertical
            }
            if (layout == 8){
                M5.Lcd.drawLine(0,53, 320,53,grid_col); //horizontal
                M5.Lcd.drawLine(0,105, 320,105,grid_col); //horizontal
                M5.Lcd.drawLine(0,157, 320,157,grid_col); //horizontal

                M5.Lcd.drawLine(160, 0, 160, 210,grid_col); //vertical
            }
            if (layout == 9){
                M5.Lcd.drawLine(0,70, 320,70,grid_col); //horizontal
                M5.Lcd.drawLine(0,140, 320,140,grid_col); //horizontal

                M5.Lcd.drawLine(106, 0, 106, 210,grid_col); //vertical
                M5.Lcd.drawLine(213, 0, 213, 210,grid_col); //vertical
            }
            if (layout == 12){
                M5.Lcd.drawLine(0,53, 320,53,grid_col); //horizontal
                M5.Lcd.drawLine(0,105, 320,105,grid_col); //horizontal
                M5.Lcd.drawLine(0,157, 320,157,grid_col); //horizontal

                M5.Lcd.drawLine(106, 0, 106, 210,grid_col); //vertical
                M5.Lcd.drawLine(213, 0, 213, 210,grid_col); //vertical
            }
            if (layout == 15){
                M5.Lcd.drawLine(0,48, 320,48,grid_col); //horizontal
                M5.Lcd.drawLine(0,96, 320,96,grid_col); //horizontal
                M5.Lcd.drawLine(0,144, 320,144,grid_col); //horizontal
                M5.Lcd.drawLine(0,192, 320,192,grid_col); //horizontal

                M5.Lcd.drawLine(106, 0, 106, 240,grid_col); //vertical
                M5.Lcd.drawLine(213, 0, 213, 240,grid_col); //vertical
            }
       }
    }

    void DisplayAlarms(){
        String _alarms = "";
        if (DeviceConfig.gps_warn && !BoatData.Gps.SourceValid()){
            _alarms += "No GPS; ";
        }
        if (DeviceConfig.depth_warn && (!BoatData.Water.DepthValid() || alarm_depth())){
            _alarms += "Depth ";
            if (!BoatData.Water.DepthValid()){_alarms += BoatData.Water.EmptyValue;}
            else {_alarms += BoatData.Water.DepthStr();}
            _alarms += "; ";
        }
        if (!SD_Present) _alarms += " No SD card; "; else if (LogFileError)_alarms += " No log file; ";

        //With this trick we show alarms once in 3 page refreshes.
        //To allow data beneath it being seen.
        //That's fine because alarms these are long lasting so there are not many chances to miss it
        if (_alarms.length() > 0 && _alarmDamper > 2){
                int val_w = _alarms.length() * 8; //real width for the value
                M5.Lcd.setTextColor(txt_col_main);
                M5.Lcd.fillRect(1, 211, val_w, 28, bg_alarm);
                M5.Lcd.setTextFont(mainFont);
                M5.Lcd.setTextSize(1);
                M5.Lcd.setCursor(10, 220, mainFont);
                M5.Lcd.print(_alarms);
                _alarmsHappens = true;
                _alarmsUpdated = true;
                _alarmDamper = 0;
        }
        else{
            //we need this to redraw alarm zone after alarm canceled
            _alarmsUpdated = _alarmsHappens;
            _alarmsHappens = false;
            if (_alarms.length() > 0) _alarmDamper++;
        }
    }

    void StatusBar() {
        char buffer[50];

        M5.Lcd.setTextColor(txt_col_main);
        String _str_time = "------------";
        if(BoatData.Gps.TimeValid()){
            _str_time = BoatData.Gps.TimeStr();
            if (BoatData.Gps.SourceDevice()) _str_time += "*";
        }
        else if(BoatData.Gps.TimeValid(DeviceState.Time)) _str_time = BoatData.Gps.TimeStr(DeviceState.Time) + "**";
        else M5.Lcd.setTextColor(bg_alarm);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setFont(FSS9);
        M5.Lcd.setTextDatum(BL_DATUM);
        M5.Lcd.fillRect(0, 210, 320, 30, bg_status);
        M5.Lcd.drawString(_str_time, 5, 235);

        if (BoatData.Battery.SourceEnabled()){
            if(BoatData.Battery.Charging()) M5.Lcd.setTextColor(c_green);
            else if (BoatData.Battery.Low()) M5.Lcd.setTextColor(c_red);
            else M5.Lcd.setTextColor(txt_col_main);
            String _str = BoatData.Battery.ChargeStr() + "%";
            M5.Lcd.setTextDatum(BR_DATUM);
            M5.Lcd.drawString(_str, 315, 235);
        }
    }

    void HearBeat(){
        if (_heartBeatState) M5.Lcd.fillCircle(316, 236, 4, bg_alarm);
            else M5.Lcd.fillCircle(316, 236, 4, bg_main);
        _heartBeatState = !_heartBeatState;
    }

    void _saveLastDisplayedData(){
        prevPage = DeviceState.cur_page;
        _last.Gps_SOG = BoatData.Gps.SOGStr();
        _last.Gps_AvgSOG = BoatData.Gps.AvgSOGStr();
        _last.Gps_COG = BoatData.Gps.COGStr();
        _last.Gps_MCOG = BoatData.Gps.MCOGStr();
        _last.Gps_Latitude = BoatData.Gps.LatitudeStr();
        _last.Gps_Longitude = BoatData.Gps.LongitudeStr();
        _last.Gps_Variation = BoatData.Gps.VariationStr();
        _last.Gps_Altitude = BoatData.Gps.AltitudeStr();

        _last.Water_STW = BoatData.Water.STWStr();
        _last.Water_AvgSTW = BoatData.Water.AvgSTWStr();
        _last.Water_Depth = BoatData.Water.DepthStr();
        _last.Water_Temp = BoatData.Water.TempStr();

        _last.Battery_Charge = BoatData.Battery.ChargeStr();
        _last.Battery_Voltage = BoatData.Battery.VoltageStr();
        _last.Battery_Current = BoatData.Battery.CurrentStr();
        _last.Battery_PowerConsumed = BoatData.Battery.PowerConsumedStr();

        _last.Wind_AWS = BoatData.Wind.AWSStr();
        _last.Wind_TWS = BoatData.Wind.TWSStr();
        _last.Wind_MaxAWS = BoatData.Wind.MaxAWSStr();
        _last.Wind_MaxTWS = BoatData.Wind.MaxTWSStr();
        _last.Wind_AWA = BoatData.Wind.AWAStr();
        _last.Wind_TWA = BoatData.Wind.TWAStr();
        _last.Wind_TWD = BoatData.Wind.TWDStr();

        _last.Heading_HeadingT = BoatData.Heading.HeadingTStr();
        _last.Heading_HeadingM = BoatData.Heading.HeadingMStr();

        _last.Polar_STW = BoatData.Polar.STWStr();
        _last.Polar_AvgSTW = BoatData.Polar.AvgSTWStr();

        _last.Rudder_Position = BoatData.Rudder.PositionStr();

        _last.Trip_TripLog =  BoatData.Trip.TripLogStr();
        _last.Trip_Log =  BoatData.Trip.LogStr();
        _last.Trip_TripTime = BoatData.Trip.TripTimeStr();

        _last.XTE = BoatData.Navigation.XTEStr();
        _last.WpDistance = BoatData.Navigation.DistanceToWaypointStr();
        _last.WpETA = BoatData.Navigation.ETAStr();
        _last.WpBearingT = BoatData.Navigation.BearingPositionToWPTStr();
        _last.WpBearingM = BoatData.Navigation.BearingPositionToWPMStr();
        _last.DestLat = BoatData.Navigation.DestinationLatitudeStr();
        _last.DestLon = BoatData.Navigation.DestinationLongitudeStr();
        _last.WpVMG = BoatData.Navigation.VMGToWPStr();

        _last.Heel = BoatData.Attitude.HeelStr();
        _last.Trim = BoatData.Attitude.TrimStr();
        _last.MaxHeel = BoatData.Attitude.MaxHeelStr();
        _last.MaxTrim = BoatData.Attitude.MaxTrimStr();

        _last.AirTemp = BoatData.Env.TempStr();
        _last.Humidity = BoatData.Env.HumidityStr();
        _last.Pressure = BoatData.Env.PressureStr();

    }

    void LoadPages() {
        DynamicJsonDocument json_buffer(3 * 1024);
        File file = SD.open("/pages.json");
        if (file && file.size() < 3 * 1024){
            DeserializationError error = deserializeJson(json_buffer, file);
            if (!error){
                tDisplayPage _default_page;
                if (json_buffer["pages"]){
                    for (int _page=0; _page < _maxDisplayPages; _page++){
                        if (!json_buffer["pages"][_page]){break;}
                        last_page++;
                        DisplayPages[_page] = _default_page; //initialize, because memory allocation did not set default values
                        for(int _tile=0; _tile < 15; _tile++){
                            if (!json_buffer["pages"][_page][_tile]){;break;}
                            DisplayPages[_page].tiles_count++;
                            DisplayPages[_page].tiles[_tile] = json_buffer["pages"][_page][_tile].as<String>();
                        }
                    }
                }
                if (json_buffer["loop_exclude"]){
                    for (int i=0; i < json_buffer["loop_exclude"].size(); i++){
                        if (json_buffer["loop_exclude"][i] < 31) DisplayPages[(int)json_buffer["loop_exclude"][i]].loop_exclude = true; //check if not out of range;
                    }
                }
            }
            else{
                _debug("Bad pages.json");
                _debug(error.f_str());
            }
            file.close();
        }
        else{
           _debug("pages.json is too big");
        }
    }

public:
    bool LogFileError = false;

    void Init(){
        LoadPages();
        if (last_page < 0){ //if no pages loaded from config - init default pages
            initDefaultPages();
        }
    }

    int LastPage(){return  last_page;}

    void DrawPage(){
        int _layout = get_page_layout();
        drawLayout(_layout);
        for(int i = 0; i < DisplayPages[DeviceState.cur_page].tiles_count; i++){
            _print_tile(DisplayPages[DeviceState.cur_page].tiles[i],(i+1),_layout);
        }

        _saveLastDisplayedData();

        if(_layout <= 12) {StatusBar();}
        DisplayAlarms();
        if(_layout > 12){HearBeat();}
    }

    void NextPage(){
        DeviceState.cur_page++;
        if (DeviceState.cur_page > last_page) {DeviceState.cur_page = 0;}
        if (DeviceState.loop_pages && DisplayPages[DeviceState.cur_page].loop_exclude){
            NextPage();
        }
    }

    void PrevPage(){
        DeviceState.cur_page--;
        if (DeviceState.cur_page < 0) {DeviceState.cur_page = last_page;}
    }

    void initDefaultPages(){
        tDisplayPage _default_page;

        last_page = 0;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 1;
        DisplayPages[last_page].tiles[0] = "COORD";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 2;
        DisplayPages[last_page].tiles[0] = "AWS";
        DisplayPages[last_page].tiles[1] = "COORD";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 3;
        DisplayPages[last_page].tiles[0] = "AWA";
        DisplayPages[last_page].tiles[1] = "TWA";
        DisplayPages[last_page].tiles[2] = "COORD";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 4;
        DisplayPages[last_page].tiles[0] = "BatteryCharge";
        DisplayPages[last_page].tiles[1] = "BatteryVolt";
        DisplayPages[last_page].tiles[2] = "BatteryCurrent";
        DisplayPages[last_page].tiles[3] = "PowerConsumed";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 4;
        DisplayPages[last_page].tiles[0] = "AWS";
        DisplayPages[last_page].tiles[1] = "TWS";
        DisplayPages[last_page].tiles[2] = "AWA";
        DisplayPages[last_page].tiles[3] = "COORD";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 6;
        DisplayPages[last_page].tiles[0] = "TWA";
        DisplayPages[last_page].tiles[1] = "STW";
        DisplayPages[last_page].tiles[2] = "TWS";
        DisplayPages[last_page].tiles[3] = "PolarSTW";
        DisplayPages[last_page].tiles[4] = "AvgPolarSTW";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 6;
        DisplayPages[last_page].tiles[0] = "WaterTemperature";
        DisplayPages[last_page].tiles[1] = "TripLog";
        DisplayPages[last_page].tiles[2] = "COORD";
        DisplayPages[last_page].tiles[5] = "Log";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 8;
        DisplayPages[last_page].tiles[0] = "AWA";
        DisplayPages[last_page].tiles[1] = "WaterDepth";
        DisplayPages[last_page].tiles[2] = "COORD";
        DisplayPages[last_page].tiles[3] = "COG";
        DisplayPages[last_page].tiles[4] = "SOG";
        DisplayPages[last_page].tiles[5] = "AvgSOG";
        DisplayPages[last_page].tiles[6] = "STW";
        DisplayPages[last_page].tiles[7] = "AvgSTW";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 9;
        DisplayPages[last_page].tiles[0] = "Heading";
        DisplayPages[last_page].tiles[1] = "COG";
        DisplayPages[last_page].tiles[2] = "SOG";
        DisplayPages[last_page].tiles[3] = "COORD";
        DisplayPages[last_page].tiles[4] = "RudderPosition";
        DisplayPages[last_page].tiles[5] = "Altitude";
        DisplayPages[last_page].tiles[6] = "Variation";
        DisplayPages[last_page].tiles[8] = "Log";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 12;
        DisplayPages[last_page].tiles[0] = "STW";
        DisplayPages[last_page].tiles[1] = "WaterDepth";
        DisplayPages[last_page].tiles[2] = "COORD";
        DisplayPages[last_page].tiles[3] = "WaterDepth";
        DisplayPages[last_page].tiles[4] = "COG";
        DisplayPages[last_page].tiles[5] = "TripTime";
        DisplayPages[last_page].tiles[11] = "Log";

        last_page++;
        DisplayPages[last_page] = _default_page;
        DisplayPages[last_page].tiles_count = 15;
        DisplayPages[last_page].tiles[0] = "AWA";
        DisplayPages[last_page].tiles[1] = "TWA";
        DisplayPages[last_page].tiles[2] = "AWS";
        DisplayPages[last_page].tiles[3] = "TWS";
        DisplayPages[last_page].tiles[4] = "TWD";

        DisplayPages[last_page].tiles[5] = "SOG";
        DisplayPages[last_page].tiles[6] = "AvgSOG";
        DisplayPages[last_page].tiles[7] = "STW";
        DisplayPages[last_page].tiles[8] = "AvgSTW";

        DisplayPages[last_page].tiles[9] = "COG";
        DisplayPages[last_page].tiles[10] = "Heading";

        DisplayPages[last_page].tiles[11] = "WaterDepth";

        DisplayPages[last_page].tiles[12] = "MaxAws";
        DisplayPages[last_page].tiles[13] = "MaxTws";

        DisplayPages[last_page].tiles[14] = "COORD";
    }
};
cDisplay MainDisplay;