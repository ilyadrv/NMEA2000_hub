class cN2K{
public:
    struct tSavedN2kMessage{
        unsigned long PGN = 0;
        unsigned long Time = 0;
        tN2kMsg Msg;
    };

protected:
    static tSavedN2kMessage *SavedMessages;

public:
    static unsigned long lastCollect;
    static String allN2kPGNs;

    static void Init(){
        // INIT KNOWN PGNS. Max 32 fro now
        int i=0;
        SavedMessages[i++].PGN = 126992UL;//System date/time
        SavedMessages[i++].PGN = 127245UL; //Rudder
        SavedMessages[i++].PGN = 127250UL; //heading
        SavedMessages[i++].PGN = 127257UL; //attitude
        SavedMessages[i++].PGN = 127258UL; //variation
        SavedMessages[i++].PGN = 128259UL; //boat speed
        SavedMessages[i++].PGN = 128267UL; //Water depth
        SavedMessages[i++].PGN = 128275UL; //Distance log
        SavedMessages[i++].PGN = 129025UL; //Lat Lon rapid
        SavedMessages[i++].PGN = 129026UL; //COG SOG rapid
        SavedMessages[i++].PGN = 129029UL; //GNSS Position Data
        SavedMessages[i++].PGN = 129283UL; //Cross Track Error
        SavedMessages[i++].PGN = 129284UL; //Navigation info
        SavedMessages[i++].PGN = 129285UL; //Waypoint list
        SavedMessages[i++].PGN = 129539UL; //GNSS DOP data
        SavedMessages[i++].PGN = 129540UL; //GNSS Sats in View
        SavedMessages[i++].PGN = 130306UL; //Wind
        SavedMessages[i++].PGN = 130310UL; //Environmental parameters - old
        SavedMessages[i++].PGN = 130311UL; //Environmental parameters
        SavedMessages[i++].PGN = 130316UL; //Temperature extended range
        SavedMessages[i++].PGN = 0; //!!!PGN=0 is the sign of the array end

        // INIT NMEA
        uint8_t chipid[6];
        uint32_t id = 0;
        esp_efuse_read_mac(chipid);
        for (int i = 0; i < 6; i++) id += (chipid[i] << (7 * i));

        // Set product information
        NMEA2000.SetProductInformation("ID00001", //Model serial
                                     123, //product code
                                     "M5 Display",  //Model ID
                                     "1.0.0 (2021-01-01)",  //Software version
                                     "1.0.0 (2021-01-01)" //Model version
                                    );
        // Set device information
        NMEA2000.SetDeviceInformation(id, // Unique number. Use e.g. Serial number. Id is generated from MAC-Address
                                    132, // Device function=Analog to NMEA 2000 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                    25, // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                    2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                   );

        NMEA2000.SetN2kCANSendFrameBufSize(250);
        NMEA2000.SetN2kCANReceiveFrameBufSize(250);

        NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, DeviceConfig.n2k_address); //the second param is device address in N2K
        NMEA2000.EnableForward(false);
        NMEA2000.SetMsgHandler(CollectMessages);
        NMEA2000.Open();
    }

    //Save N2K messages to be parsed later
    // if message in missing in N2K the previous obe will be kept
    // to stabilize data aquizition
    static void CollectMessages(const tN2kMsg &N2kMsg) {
        cN2K::lastCollect = millis();
        int i = 0;
        for (i=0; SavedMessages[i].PGN!=0 && !(N2kMsg.PGN==SavedMessages[i].PGN); i++);
        if (SavedMessages[i].PGN!=0) {
            SavedMessages[i].Msg = N2kMsg;
            SavedMessages[i].Time = millis();
        }
        //collecting all known PGNs for debug
        String _pgn = String(N2kMsg.PGN) + "|";
        if (allN2kPGNs.indexOf(_pgn) == -1){ allN2kPGNs += _pgn;}
    }

    //get message by PGN respecting expiration time
    static tN2kMsg Message(unsigned long PGN) {
        int i = 0;
        for (i = 0; SavedMessages[i].PGN!=0 && !(PGN==SavedMessages[i].PGN); i++);
        if (SavedMessages[i].PGN != 0 && SavedMessages[i].Time + DeviceConfig.n2k_msg_ttl > millis()){
            return SavedMessages[i].Msg;
        }
        else{
            tN2kMsg _blankMessage;
            return _blankMessage;
        }
    }

    static bool MessageValid(unsigned long PGN) {
        int i = 0;
        for (i = 0; SavedMessages[i].PGN!=0 && !(PGN==SavedMessages[i].PGN); i++);
        return (SavedMessages[i].PGN != 0 && SavedMessages[i].Time + DeviceConfig.n2k_msg_ttl > millis());
    }

};
//init static
String cN2K::allN2kPGNs = "";
cN2K::tSavedN2kMessage *cN2K::SavedMessages = (cN2K::tSavedN2kMessage *) malloc(32 * sizeof(cN2K::tSavedN2kMessage));
unsigned long cN2K::lastCollect = millis();