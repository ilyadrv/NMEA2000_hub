//!!!! ATTENTION KEY LENGTH MUST BE <=15
void load_state() {
   //LOAD from flash
   DeviceState.cur_page = prefs.getInt("s.cur_page", DeviceState.cur_page);
   DeviceState.loop_pages = prefs.getBool("s.loop_pages", DeviceState.loop_pages);
   DeviceState.lcd_brightness = prefs.getInt("s.lcd_bright", DeviceState.lcd_brightness);
   if (DeviceState.cur_page > MainDisplay.LastPage()){
        DeviceState.cur_page = 0;
   }

   tMyGpsData _position;
   _position.Latitude = ((double) prefs.getLong("bp.lat", -185000000)) / 1000000;
   _position.Longitude = ((double) prefs.getLong("bp.lon", -185000000)) / 1000000;
   _position.Time = prefs.getLong("bp.time", _position.Time);
   if (BoatData.Gps.PositionValid(_position)){
        BoatData.LastShutdownPosition = _position;
        BoatData.Gps._LastShutdownPosition = _position; //for tests
   }
}

void save_state() {
    //Save to flash to flash
    prefs.putInt("s.cur_page", DeviceState.cur_page);
    prefs.putBool("s.loop_pages", DeviceState.loop_pages);
    prefs.putInt("s.lcd_bright", DeviceState.lcd_brightness);

    if (BoatData.Gps.CoordinatesValid() && BoatData.Gps.TimeValid()){
        prefs.putLong("bp.lat", (long)(BoatData.Gps.Latitude * 1000000));
        prefs.putLong("bp.lon", (long)(BoatData.Gps.Longitude * 1000000));
        prefs.putLong("bp.time", BoatData.Gps.Time());
    }
}

//===================== Take Off
double _stop_start_distance_max = 80/1000; //80m
double _mooring_distance_min = 100/1000; //100m
double _mooring_distance_max = 200/1000; //200m
int _time_since_stop_max = 8 * 3600;

void takeoff(){
    //general checks
    if (BoatData.TakeoffHappend){return;}
    if (BoatData.Gps.TimeValid(DeviceState.Time)) { return; }
    if (!BoatData.Gps.CoordinatesValid() || !BoatData.Gps.PositionValid(BoatData.LastShutdownPosition) || !BoatData.Gps.PositionValid(BoatData.StartupPosition) ) { return; }

    //if last shutdown less than 8 hours
    if (BoatData.Gps.Time() - BoatData.LastShutdownPosition.Time < _time_since_stop_max){
        return;
    }

    //if last mooring and startup is not the same (less than 80m) - skip
    if (haversine(BoatData.LastShutdownPosition, BoatData.StartupPosition, true) > _stop_start_distance_max){
        return;
    }

    //if last mooring distance not in 100-200m range - then skip
    double _distanceFromLastMooring = haversine(BoatData.LastShutdownPosition, BoatData.Gps.Data(), true);
    if (_distanceFromLastMooring < _mooring_distance_min || _distanceFromLastMooring > _mooring_distance_max){
        return;
    }

    BoatData.TakeoffHappend = true;
    //@TODO e.g. play takeoff music
}

