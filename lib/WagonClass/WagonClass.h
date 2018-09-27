#ifndef WagonClass_h
#define WagonClass_h
#include <Arduino.h>

bool JsonToVar(String data);
String VarToJson(String type);


class ChaseClass
{
  public:
    bool on;
    bool fade;
    int colormode;
    int huevalue;
    int distance;

    String colorname;
    int value;
    int pvalue;
};
class SparkleClass
{
  public:
    bool on;
    bool fade;
    int colormode;
    int huevalue;
    int value;

    String colorname;
    int pvalue;
};
class BackgroundClass
{
  public:
    bool on;
    bool fade;
    int colormode;
    int value;

    String colorname;
    int huevalue;
    int pvalue;
};
class TraceClass
{
  public:
    int tdirection;
    int value;
    int num;
};
class PacManClass
{
  public:
    bool on;
    int value;
};
class WagonGpsClass
{
  public:
    int speed;
    int dir;
    int alt;
    double lat;
    double lon;
    int sat;
    double homelon;
    double homelat;
    int dirtohome;
    String locstring;
    int manfeet;
};

class WagonFlameClass
{
  public:
//    bool flame;
    int flameon;
    int flameoff;
    int flamemode;
//    int btnFlame[6];
//    int lcdcolor;
};

class LedClass {
  public:
    bool fade;
    int fadevalue;
    bool random;
    bool flame;
    bool pacman;
    int speed;
    int mode;
    bool noise;
    bool gps;

};

class WagonClass
{
  public:
    int chipResets;
    double totalRequests;
    int requestsPerMinute;

    bool pwrLed1;
    bool pwrLed2;
    bool pwrFlame;
    bool pwrSound;
    bool pwrHeadlight;
    bool pwrAux;

    bool startRelay;
    bool stopRelay;

    float mainVoltage;
    float auxVoltage;
    int autoStart;
    float startVoltage;
};

#endif
