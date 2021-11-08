#define DEBUG


#include <Arduino.h>
#include <math.h>

const uint8_t led = 4 ;
const uint8_t buzzer = 5;
const uint8_t sensor = A0;

// calibration resistance of the sensor
const double RL = 11e3;
// sensing resistance for H2 at 1000ppm (1%)
double Ro = 21261.4446;
const float cleanAirRatio = 9.83f;
// const unsigned sensorThreshhold = 350;

// defining profiles for different gases
// format is: { x1, y1, m }
// where x1, y1 is a poing on the graph representing ppm and RS/Ro ratio respectively; and m is the logarithmic graph slope
/*
const double CO[3] = {
    2.3, 0.72,
    -0.34
};
const double LPG[3] = {
    2.3, 0.21,
    -0.47
};
const double Smoke[3] = {
    2.3, 0.53,
    -0.44
};
*/

const double CO[2] = { 1.51, -0.34 };
const double LPG[2] = { 1.25, -0.47 };
const double Smoke[2] = { 1.62, -0.44 };

void alarm();
double getRS(const unsigned &reading);
double getPpmValue(const double &RS, const double gasProfile[3]);


void setup()
{
    #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("#### Serial comms initiated ####");
    #endif

    pinMode(led, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(sensor, INPUT);

    Ro = getRS(51) / cleanAirRatio;
}

void loop()
{
    unsigned reading = analogRead(sensor);

    double RS = getRS(reading);

    double ppmCO = getPpmValue(RS, CO);
    double ppmLPG = getPpmValue(RS, LPG);
    double ppmSmoke = getPpmValue(RS, Smoke);

    #ifdef DEBUG
    Serial.print("CO: "); Serial.print(ppmCO); Serial.print("ppm    ");
    Serial.print("LPG: "); Serial.print(ppmLPG); Serial.print("ppm    ");
    Serial.print("Smoke: "); Serial.print(ppmSmoke); Serial.println("ppm");
    #endif


    // obtaining sensing resistance derived from voltage divider formula
    // raw readings are used to calculate this as it's not necessary to them convert to voltage values

    if (ppmCO > 800) 
        alarm();
}

void alarm()
{
    #ifdef DEBUG
    Serial.println("Aire contaminado!!!");
    #endif

    digitalWrite(led, HIGH);
    tone(buzzer, 1000, 500);
    digitalWrite(led, LOW);
    delay(500);
}

double getRS(const unsigned &reading)
{
    return RL * (1024 - reading) / reading;
}

double getPpmValue(const double &RS, const double gasProfile[2])
{
    /*
    double x1 = gasProfile[0];
    double y1 = gasProfile[1];
    double m = gasProfile[2];
    */
    double b = gasProfile[0];
    double m = gasProfile[1];

    return pow(10, (log10(RS/Ro) - b) / m);
    // return pow(10, (log10(RS / Ro) - y1)/m + x1);
}