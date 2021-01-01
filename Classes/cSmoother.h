class cSmoother{
protected:
    unsigned char Power;
    int Interval;
    double maxDelta; //difference betww
    double LastValue; //last value
    double *LastValues; //array of last values
    double LastResult = 0;
    byte index=0;
    bool Ready = false;
    unsigned long  Timer = 0;

public:
    cSmoother(unsigned char p = 50, int i = 100, double d = 100) {
        Power = p;
        Interval = i;
        maxDelta = d;
        LastValues = (double *) malloc(Power * sizeof(double));
    }
    void Smooth(double *Parameter) {
        //if too huge delta then reinit all the vector. This happens e.g. when radial data comes from 360 to 0
        if(abs(*Parameter - LastValue) > maxDelta){
            index = 0;
            Ready = false;
        }
        LastValue = *Parameter;
        //initial values array fill without delays
        if (!Ready){
            LastValues[index] = *Parameter;
            index++;
            if (index>=Power) {
                index = 0;
                Ready = true;
            }
            return;
        }
        if (millis() > Timer + Interval){
            Timer = millis();
            LastResult = 0;
            LastValues[index] = *Parameter;
            index++;
            if (index>=Power) index = 0;
            for (byte i=0; i<Power; i++) {
                LastResult += LastValues[i];
            }
            LastResult /= Power;
        }
        *Parameter = LastResult;
    }
};