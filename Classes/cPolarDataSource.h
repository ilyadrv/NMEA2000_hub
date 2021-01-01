//@TODO do not calc polars under engine
class cPolarDataSource: public cDataSource {
public:
    double
        STW=N2kDoubleNA,
        AvgSTW=N2kDoubleNA;
    unsigned short
        BoatToPolar = N2kUInt8NA, //percentage relation boat speed tp polar predict
        BoatToPolarAvg = N2kUInt8NA;

    bool STWValid(){ return (GlobalConfig.polar_stw_min < STW && STW < 100);}
    bool AvgSTWValid(){ return (GlobalConfig.polar_stw_min < AvgSTW && AvgSTW < 100);}
    bool BoatToPolarValid(){ return (0 < BoatToPolar && BoatToPolar < 200);} //if we are 2 times faster polars that's bad STW or bad polars
    bool BoatToPolarAvgValid(){ return (0 < BoatToPolarAvg && BoatToPolarAvg < 200);}

    String STWStr(){ return STWValid() ? String(STW, 1) : EmptyValue;}
    String AvgSTWStr(){ return AvgSTWValid() ? String(AvgSTW, 1) : EmptyValue;}
    String BoatToPolarStr(){ return BoatToPolarValid() ? String(BoatToPolar) : EmptyValue;}
    String BoatToPolarAvgStr(){ return BoatToPolarAvgValid() ? String(BoatToPolarAvg) : EmptyValue;}

    void InitDataSource(cConfig &Config, cWindDataSource *_wind, cWaterDataSource *_water){
        cDataSource::InitDataSource(Config);
        Wind = _wind;
        Water = _water;
        LoadPolars();
    }

protected:
    cWindDataSource *Wind;
    cWaterDataSource *Water;

    unsigned long int AvgSTWPoints = 0;

    static const int polar_twa_min = 20;
    static const int polar_twa_max = 180;
    static const int polar_tws_min = 4;
    static const int polar_tws_max = 35;

    struct tPolarPoint {
        int twa = 0;
        double stw = 0;
    };
    struct tPolarLine{
        int tws = 0, points_num = 0;
        tPolarPoint points[polar_twa_max - polar_twa_min + 1];
    };
    struct tPolars{
        tPolarLine lines[polar_tws_max - polar_tws_min + 1];
        int lines_num = 0;
    };
    tPolars polars;

    void CalcPolarSTW(){
        if (polars.lines_num == 0 || (Wind->TWS < polar_tws_min || Wind->TWS > polar_tws_max || Wind->TWA < polar_twa_min || Wind->TWA > polar_twa_max)){
            STW = 0;
            return;
        }

        // StwLine - is a line on the Polar diagram. The line is f(TWA)=STWpolar for certain wind speed.
        // Usually there aer sme number of lines for different TWS.
        // In this algorithm we find HI and LO lines and build the proper one for current TWS - LineWeighted.
        //
        // finding LO and HI STW lines for current TWS
        // if less than low then lo is empty
        // if greater than hi, than hi and low are the same
        tPolarLine StwLineLo, StwLineHi;
        for (int i = 0; i < polars.lines_num; i++){
            StwLineHi = polars.lines[i];
            if (polars.lines[i].tws > Wind->TWS){break;}
            StwLineLo = polars.lines[i];
            if (polars.lines[i].tws == Wind->TWS){break;}
        }

        // finding left and right borders for the current TWA on the HI STW LINE
        // if less than the first one then left is empty
        // if greater than the last one then right and left are the same
        tPoint pointLeft, pointRight;
        for (int i = 0; i < StwLineHi.points_num; i++){
            pointRight.x = StwLineHi.points[i].twa;
            pointRight.y = StwLineHi.points[i].stw;
            if (StwLineHi.points[i].twa > Wind->TWA){break;}
            pointLeft.x = StwLineHi.points[i].twa;
            pointLeft.y = StwLineHi.points[i].stw;
            if (StwLineHi.points[i].twa == Wind->TWA){break;}
        }

        tLine LineLo, LineHi, LineWeighted;
        LineHi = getLine(pointLeft, pointRight);

        if (StwLineLo.tws == StwLineHi.tws){
            //if StwLineLo and StwLineHi is the same line we consider this is also the LineWeighted
            LineWeighted = LineHi;
        }
        else{
            //if StwLineLo exists, then finding left and right borders for the current TWA
            // as we did previously for StwLineHi.
            //Else StwLineLo will be y = 0x+0;
            if (StwLineLo.tws != 0){
                pointLeft.x = 0; pointLeft.y = 0;
                pointRight.x = 0; pointRight.y = 0;;
                for (int i = 0; i < StwLineLo.points_num; i++){
                    pointRight.x = StwLineLo.points[i].twa;
                    pointRight.y = StwLineLo.points[i].stw;
                    if (StwLineLo.points[i].twa > Wind->TWA){break;}
                    pointLeft.x = StwLineLo.points[i].twa;
                    pointLeft.y = StwLineLo.points[i].stw;
                    if (StwLineLo.points[i].twa == Wind->TWA){break;}
                }
            }
            LineLo = getLine(pointLeft, pointRight);

            double ratio = getRatio(StwLineLo.tws, StwLineHi.tws, Wind->TWS);
            LineWeighted = getWeightedLine(LineLo, LineHi, ratio);
        }
        STW = Fx(LineWeighted, Wind->TWA);
    }

    void LoadPolars() {
        DynamicJsonDocument json_buffer(3 * 1024);
        File file = SD.open("/polars.json");
        if (file && file.size() < 3 * 1024){
            DeserializationError error = deserializeJson(json_buffer, file);
            if (!error){
                for (int line = 0; line <= polar_tws_max - polar_tws_min; line++){
                    if (!json_buffer["polars"][line][0]){break;} //reach the end of lines
                    polars.lines_num++;
                    polars.lines[line].tws = (int) json_buffer["polars"][line][0];
                    for (int point = 0; point <= polar_twa_max - polar_twa_min; point++){
                        if (!json_buffer["polars"][line][1][point][0]){break;} //reach the end of point in line
                        if (json_buffer["polars"][line][1][point][1] > 0.5){ // because this is floats we assuming empty everything less 0.5
                            polars.lines[line].points[point].twa = (int) json_buffer["polars"][line][1][point][0];
                            polars.lines[line].points[point].stw = (double) json_buffer["polars"][line][1][point][1];
                            polars.lines[line].points_num++;
                        }
                    }
                }
            }
            else{
                _debug("Bad polars.json");
                _debug(error.f_str());
            }
            file.close();
        }
        else{
           _debug("polars.json does not exist or it is too big");
        }
    }


//================================= FROM BASIC CLASS
protected:
    void SetDataSourceConfig(){
        DataSourceConfig = GlobalConfig.polar_datasource;
    }

    bool RefreshDataFromDevice(){
        CalcPolarSTW();
        return true;
    }

    void ProcessData(){
        BoatToPolar = STWValid() ? round(Water->STW/STW * 100) : 0;
        if (STWValid()) {
            AvgSTW = (AvgSTWPoints == 0) ? STW : (AvgSTW * AvgSTWPoints + STW) / (AvgSTWPoints + 1);
            AvgSTWPoints++;
        }
        if (AvgSTWValid() && Water->AvgSTWValid()){
            BoatToPolarAvg = (AvgSTW > 0.1) ? round(Water->AvgSTW/AvgSTW * 100) : 0;
        }
    }

    void RefreshDataFromTest(){
        RefreshDataFromDevice();
    }

    void ResetData(){
        STW=N2kDoubleNA;
        BoatToPolar = N2kUInt8NA;
    }
};