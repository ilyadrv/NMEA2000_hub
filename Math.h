const double radToDeg = 57.2957795; // 180.0/M_PI;
const double msToKn = 1.9438445; //3600.0/1852.0;
const double kToC = -273.15;
const double mToNm = 1/1852.0;
struct tPoint {double x=0, y=0;};
struct tLine {double k=0, b=0;};

// ========  calculate haversine distance for linear distance
double haversine(double lat1, double long1, double lat2, double long2, bool km = false){
    double dlong = (long2 - long1)/radToDeg;
    double dlat = (lat2 - lat1)/radToDeg;
    double a = pow(sin(dlat/2.0), 2) + cos(lat1/radToDeg) * cos(lat2/radToDeg) * pow(sin(dlong/2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return (km)
        ? 6367 * c
        : 3956 * c;
}

double haversine(tMyGpsData p1, tMyGpsData p2, bool km = false){
    return haversine(p1.Latitude, p1.Longitude,p2.Latitude, p2.Longitude, km);
}

double NormalizeDegree(double degree){
    degree = degree - trunc(degree/360) * 360;
    if (degree<0) degree += 360;
    return degree;
}

double Degree360to180(double degree){
    return (degree > 180) ? degree - 360 : degree;
}

double Degree180to360(double degree){
    return (degree < 0) ? 360 + degree : degree;
}

//============= Some MATH

double MyRandDouble(double min, double max){
    return rand() / (RAND_MAX / (max - min)) + min; // scale = RAND_MAX / (max - min);
}
int MyRandInt(double min, double max){
  return round(MyRandDouble(min,max));
}

//=============== lines and coordinates

tLine getLine(tPoint p1, tPoint p2){
    tLine result;
    if (p1.x == p2.x){ //if p1 = p2 then constant 0x + b
        result.k = 0;
        result.b = p1.y;
    }
    else{
        result.k = (p1.y - p2.y) / (p1.x - p2.x);
        result.b = p2.y - result.k * p2.x;
    }
    return result;
}

tLine getWeightedLine(tLine line1, tLine line2, double ratio){
    tLine result;
    result.k = (line2.k - line1.k) * ratio  + line1.k;
    result.b = (line2.b - line1.b) * ratio  + line1.b;
    return result;
}

double getRatio(double valMin, double valMax, double val){
    return (val - valMin) / (valMax - valMin);
}

double Fx(tLine line, double x){
    return line.k * x + line.b;
}