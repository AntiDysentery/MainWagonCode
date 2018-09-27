
#include "WagonClass.h"
#include <ArduinoJson.h>

LedClass Led;
ChaseClass Chase;
SparkleClass Sparkle;
BackgroundClass Background;
WagonClass Wagon;
WagonGpsClass WGps;
WagonFlameClass Flame;
TraceClass Trace;
PacManClass PacMan;



bool JsonToVar(String data) {
  int wcHue = 0;

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);
  if (!root.success()) {
    return false;
  }

  if (root.containsKey("GpsSpeed")) WGps.speed = root["GpsSpeed"];
  if (root.containsKey("GpsDir")) WGps.dir = root["GpsDir"];
  if (root.containsKey("GpsAlt")) WGps.alt = root["GpsAlt"];
  if (root.containsKey("GpsLat")) WGps.lat = root["GpsLat"];
  if (root.containsKey("GpsLon")) WGps.lon = root["GpsLon"];
  if (root.containsKey("GpsSat")) WGps.sat = root["GpsSat"];
  if (root.containsKey("GpsHomeLat")) WGps.homelat = root["GpsHomeLat"];
  if (root.containsKey("GpsHomeLon")) WGps.homelon = root["GpsHomeLon"];
  if (root.containsKey("GpsDirToHome")) WGps.dirtohome = root["GpsDirToHome"];
  if (root.containsKey("GpsManFeet")) WGps.manfeet = root["GpsManFeet"];
  if (root.containsKey("GpsLocString")) {
    WGps.locstring = "";
    root["GpsLocString"].printTo(WGps.locstring);
    WGps.locstring.replace("\"", "");
  }

  if (root.containsKey("mainVoltage")) Wagon.mainVoltage = root["mainVoltage"];
  if (root.containsKey("auxVoltage")) Wagon.auxVoltage = root["auxVoltage"];
  if (root.containsKey("startVoltage")) Wagon.startVoltage = root["startVoltage"];
  if (root.containsKey("autoStart")) Wagon.autoStart = root["autoStart"];

  if (root.containsKey("pwrLed1")) Wagon.pwrLed1 = root["pwrLed1"];
  if (root.containsKey("pwrLed2")) Wagon.pwrLed2 = root["pwrLed2"];
  if (root.containsKey("pwrHeadlight")) Wagon.pwrHeadlight = root["pwrHeadlight"];
  if (root.containsKey("pwrFlame")) Wagon.pwrFlame = root["pwrFlame"];
  if (root.containsKey("pwrSound")) Wagon.pwrSound = root["pwrSound"];
  if (root.containsKey("pwrAux")) Wagon.pwrAux = root["pwrAux"];

  if (root.containsKey("startRelay")) Wagon.stopRelay = root["startRelay"];
  if (root.containsKey("stopRelay")) Wagon.stopRelay = root["stopRelay"];

  if (root.containsKey("chipResets")) Wagon.chipResets = root["chipResets"];
  if (root.containsKey("totalRequests")) Wagon.totalRequests = root["totalRequests"];
  if (root.containsKey("requestsPerMinute")) Wagon.requestsPerMinute = root["requestsPerMinute"];

  if (root.containsKey("Sparkle")) Sparkle.on = root["Sparkle"];
  if (root.containsKey("SparkleColorMode")) {
    Sparkle.colormode = root["SparkleColorMode"];
    Sparkle.colorname = "";
    root["SparkleColorName"].printTo(Sparkle.colorname);
    Sparkle.colorname.replace("\"", "");
  }
  if (root.containsKey("SparkleValue")) Sparkle.value = root["SparkleValue"];
  if (root.containsKey("SparkleHue")) {
    wcHue = root["SparkleHue"];
    Sparkle.huevalue = map(wcHue, 0, 360, 0, 255);
  }
  if (root.containsKey("SparkleFade")) Sparkle.fade = root["SparkleFade"];

  if (root.containsKey("Chase")) Chase.on = root["Chase"];
  if (root.containsKey("ChaseFade")) Chase.fade = root["ChaseFade"];
  if (root.containsKey("ChaseColorMode")) Chase.colormode = root["ChaseColorMode"];
  if (root.containsKey("ChaseColorName")) root["ChaseColorName"].printTo(Chase.colorname);
  if (root.containsKey("ChaseColorMode")) {
    Chase.colormode = root["ChaseColorMode"];
    Chase.colorname = "";
    root["ChaseColorName"].printTo(Chase.colorname);
    Chase.colorname.replace("\"", "");
  }
  if (root.containsKey("ChaseDistance")) Chase.distance = root["ChaseDistance"];
  if (root.containsKey("ChaseHue")) {
    wcHue = root["ChaseHue"];
    Chase.huevalue = map(wcHue, 0, 360, 0, 255);
  }

  if (root.containsKey("Background")) Background.on = root["Background"];
  if (root.containsKey("BackgroundFade")) Background.fade = root["BackgroundFade"];
  if (root.containsKey("BackgroundColorMode")) {
    Background.colormode = root["BackgroundColorMode"];
    Background.colorname = "";
    root["BackgroundColorName"].printTo(Background.colorname);
    Background.colorname.replace("\"", "");
  }
  if (root.containsKey("BackgroundValue")) Background.value = root["BackgroundValue"];
  if (root.containsKey("BackgroundHue")) {
    wcHue = root["BackgroundHue"];
    Background.huevalue = map(wcHue, 0, 360, 0, 255);
  }

  if (root.containsKey("LedFade")) Led.fade = root["LedFade"];
  if (root.containsKey("LedFadeVal")) Led.fadevalue = root["LedFadeVal"];
  if (root.containsKey("LedRandom")) Led.random = root["LedRandom"];
  if (root.containsKey("LedFlame")) Led.flame = root["LedFlame"];
  if (root.containsKey("LedSpeed")) Led.speed = root["LedSpeed"];
  if (root.containsKey("LedMode")) Led.mode = root["LedMode"];
  if (root.containsKey("LedNoise")) Led.noise = root["LedNoise"];
  if (root.containsKey("LedGps")) Led.gps = root["LedGps"];

  if (root.containsKey("FlameOn")) Flame.flameon = root["FlameOn"];
  if (root.containsKey("FlameOff")) Flame.flameoff = root["FlameOff"];
  if (root.containsKey("FlameMode")) Flame.flamemode = root["FlameMode"];

  return true;
}

String VarToJson(String type) {
  int wcHue = 0;
  String txt;

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  if ((type == "gps") || (type == "full")) {
    root["GpsSpeed"] = WGps.speed;
    root["GpsDir"] = WGps.dir;
    root["GpsDirToHome"] = WGps.dirtohome;
    root["GpsAlt"] = WGps.alt;
    root["GpsLat"] = WGps.lat;
    root["GpsLon"] = WGps.lon;
    root["GpsSat"] = WGps.sat;
    root["GpsHomeLon"] = WGps.homelon;
    root["GpsHomeLat"] = WGps.homelat;
    root["GpsDirToHome"] = WGps.dirtohome;
    root["GpsManFeet"] = WGps.manfeet;
    root["GpsLocString"] = WGps.locstring;
  }
  if ((type == "flame") || (type == "full")) {
    root["FlameOn"] = Flame.flameon;
    root["FlameOff"] = Flame.flameoff;
    root["FlameMode"] = Flame.flamemode;
  }
  if ((type == "led") || (type == "full")) {
    root["Sparkle"] = Sparkle.on;
    root["SparkleFade"] = Sparkle.fade;
    root["SparkleColorMode"] = Sparkle.colormode;
    root["SparkleColorName"] = Sparkle.colorname;
    root["SparkleValue"] = Sparkle.value;
    wcHue = map(Sparkle.huevalue, 0, 255, 0, 360);
    root["SparkleHue"] = wcHue;
    root["Chase"] = Chase.on;
    root["ChaseFade"] = Chase.fade;
    root["ChaseColorMode"] = Chase.colormode;
    root["ChaseColorName"] = Chase.colorname;
    root["ChaseDistance"] = Chase.distance;
    wcHue = map(Chase.huevalue, 0, 255, 0, 360);
    root["ChaseHue"] = wcHue;
    root["Background"] = Background.on;
    root["BackgroundFade"] = Background.fade;
    root["BackgroundColorMode"] = Background.colormode;
    root["BackgroundColorName"] = Background.colorname;
    root["BackgroundValue"] = Background.value;
    wcHue = map(Background.huevalue, 0, 255, 0, 360);
    root["BackgroundHue"] = wcHue;
    root["LedFade"] = Led.fade;
    root["LedFadeVal"] = Led.fadevalue;
    root["LedRandom"] = Led.random;
    root["LedFlame"] = Led.flame;
    root["LedSpeed"] = Led.speed;
    root["LedMode"]  = Led.mode;
    root["LedNoise"] = Led.noise;
    root["LedGps"] = Led.gps;
  }
  if ((type == "wagon") || (type == "full")) {

    root["chipResets"] = Wagon.chipResets;
    root["totalRequests"] = Wagon.totalRequests;
    root["requestsPerMinute"] = Wagon.requestsPerMinute;

    root["pwrLed1"]  = Wagon.pwrLed1;
    root["pwrLed2"]  = Wagon.pwrLed2;
    root["pwrFlame"]  = Wagon.pwrFlame;
    root["pwrSound"]  = Wagon.pwrSound;
    root["pwrHeadlight"]  = Wagon.pwrHeadlight;
    root["pwrAux"]  = Wagon.pwrAux;

    root["mainVoltage"] = Wagon.mainVoltage;
    root["auxVoltage"] = Wagon.auxVoltage;
    root["startVoltage"] = Wagon.startVoltage;
    root["autoStart"] = Wagon.autoStart;
  }
  
  root.printTo(txt);
  txt = txt + "\r";
  return txt;
}





/*
  } else {

    if ((type == "gps") || (type == "full")) {
      if (oldWGps.speed != WGps.speed) {
        root["GpsDir"] = WGps.speed;
        oldWGps.speed = WGps.speed;
      }
      if (oldWGps.dir != WGps.dir) {
        root["GpsDir"] = WGps.dir;
        oldWGps.dir = WGps.dir;
      }
      if (oldWGps.alt != WGps.alt) {
        root["GpsAlt"] = WGps.alt;
        oldWGps.alt = WGps.alt;
      }
      if (oldWGps.lat != WGps.lat) {
        root["GpsLat"] = WGps.lat;
        oldWGps.lat = WGps.lat;
      }
      if (oldWGps.lon != WGps.lon) {
        root["GpsLon"] = WGps.lon;
        oldWGps.lon = WGps.lon;
      }
      if (oldWGps.sat != WGps.sat) {
        root["GpsSat"] = WGps.sat;
        oldWGps.sat = WGps.sat;
      }
      if (oldWGps.homelon != WGps.homelon) {
        root["GpsHomeLon"] = WGps.homelon;
        oldWGps.homelon = WGps.homelon;
      }
      if (oldWGps.homelat != WGps.homelat) {
        root["GpsHomeLon"] = WGps.homelat;
        oldWGps.homelat = WGps.homelat;
      }
      if (oldWGps.dirtohome != WGps.dirtohome) {
        root["GpsDirToHome"] = WGps.dirtohome;
        oldWGps.dirtohome = WGps.dirtohome;
      }
      if (oldWGps.locstring = WGps.locstring) {
        root["GpsLocString"] = WGps.locstring;
        oldWGps.locstring = WGps.locstring;
      }
      if (oldWGps.manfeet = WGps.manfeet) {
        root["GpsManFeet"] = WGps.manfeet;
        oldWGps.manfeet = WGps.manfeet;
      }
    }
    if ((type == "flame") || (type == "full")) {

      if (oldFlame.flameon != oldFlame.flameon) {
        root["FlameOn"] = Flame.flameon;
        oldFlame.flameon = Flame.flameon;
      }
      if (oldFlame.flameoff != Flame.flameoff) {
        root["FlameOff"] = Flame.flameoff;
        oldFlame.flameoff = Flame.flameoff;
      }
    }
    if ((type == "led") || (type == "full")) {
      if (oldSparkle.on != Sparkle.on) {
        root["Sparkle"] = Sparkle.on;
        oldSparkle.on  = Sparkle.on;
      }
      if (oldSparkle.fade != Sparkle.fade) {
        root["SparkleFade"] = Sparkle.fade;
        oldSparkle.fade  = Sparkle.fade;
      }
      if (oldSparkle.colormode != Sparkle.colormode) {
        root["SparkleColorMode"] = Sparkle.colormode;
        root["SparkleColorName"] = Sparkle.colorname;
        oldSparkle.colormode  = Sparkle.colormode;
      }
      if (oldSparkle.value != Sparkle.value) {
        root["SparkleValue"] = Sparkle.value;
        oldSparkle.value  = Sparkle.value;
      }
      if (oldSparkle.huevalue != Sparkle.huevalue) {
        wcHue = map(Sparkle.huevalue, 0, 255, 0, 360);
        root["SparkleHue"] = wcHue;
        oldSparkle.huevalue  = Sparkle.huevalue;
      }


      if (oldChase.on != Chase.on) {
        root["Chase"] = Chase.on;
        oldChase.on  = Chase.on;
      }
      if (oldChase.fade != Chase.fade) {
        root["ChaseFade"] = Chase.fade;
        oldChase.fade  = Chase.fade;
      }
      if (oldChase.colormode != Chase.colormode) {
        root["ChaseColorMode"] = Chase.colormode;
        root["ChaseColorName"] = Chase.colorname;
        oldChase.colormode  = Chase.colormode;
      }
      if (oldChase.distance != Chase.distance) {
        root["ChaseDistance"] = Chase.distance;
        oldChase.distance  = Chase.distance;
      }
      if (oldChase.huevalue != Chase.huevalue) {
        wcHue = map(Chase.huevalue, 0, 255, 0, 360);
        root["ChaseHue"] = wcHue;
        oldChase.huevalue  = Chase.huevalue;
      }

      if (oldBackground.on != Background.on) {
        root["Background"] = Background.on;
        oldBackground.on  = Background.on;
      }
      if (oldBackground.fade != Background.fade) {
        root["BackgroundFade"] = Background.fade;
        oldBackground.fade  = Background.fade;
      }
      if (oldBackground.colormode != Background.colormode) {
        root["BackgroundColorMode"] = Background.colormode;
        root["BackgroundColorName"] = Background.colorname;
        oldBackground.colormode  = Background.colormode;
      }
      if (oldBackground.value != Background.value) {
        root["BackgroundValue"] = Background.value;
        oldBackground.value  = Background.value;
      }
      if (oldBackground.huevalue != Background.huevalue) {
        wcHue = map(Background.huevalue, 0, 255, 0, 360);
        root["BackgroundHue"] = wcHue;
        oldBackground.huevalue  = Background.huevalue;
      }


      if (oldLed.fade != Led.fade) {
        root["LedFade"] = Led.fade;
        oldLed.fade  = Led.fade;
      }

      if (oldLed.fadevalue != Led.fadevalue) {
        root["LedFadeVal"] = Led.fadevalue;
        oldLed.fadevalue  = Led.fadevalue;
      }

      if (oldLed.random != Led.random) {
        root["LedRandom"] = Led.random;
        oldLed.random  = Led.random;
      }

      if (oldLed.flame != Led.flame) {
        root["LedFlame"] = Led.flame;
        oldLed.flame  = Led.flame;
      }

      if (oldLed.speed != Led.speed) {
        root["LedSpeed"] = Led.speed;
        oldLed.speed  = Led.speed;
      }

      if (oldLed.mode != Led.mode) {
        root["LedMode"]  = Led.mode;
        oldLed.mode  = Led.mode;
      }

      if (oldLed.noise != Led.noise) {
        root["LedNoise"]  = Led.noise;
        oldLed.noise  = Led.noise;
      }
      if (oldLed.gps != Led.gps) {
        root["LedGps"]  = Led.gps;
        oldLed.gps  = Led.gps;
      }
    }
    if ((type == "wagon") || (type == "full")) {
      if (oldWagon.chipResets != Wagon.chipResets) {
        root["chipResets"]  = Wagon.chipResets;
        oldWagon.chipResets  = Wagon.chipResets;
      }
      if (oldWagon.totalRequests != Wagon.totalRequests) {
        root["totalRequests"]  = Wagon.totalRequests;
        oldWagon.totalRequests  = Wagon.totalRequests;
      }
      if (oldWagon.requestsPerMinute != Wagon.requestsPerMinute) {
        root["requestsPerMinute"]  = Wagon.requestsPerMinute;
        oldWagon.requestsPerMinute  = Wagon.requestsPerMinute;
      }

      if (oldWagon.pwrLed1 != Wagon.pwrLed1) {
        root["pwrLed1"]  = Wagon.pwrLed1;
        oldWagon.pwrLed1  = Wagon.pwrLed1;
      }
      if (oldWagon.pwrLed2 != Wagon.pwrLed2) {
        root["pwrLed2"]  = Wagon.pwrLed2;
        oldWagon.pwrLed2  = Wagon.pwrLed2;
      }
      if (oldWagon.pwrFlame != Wagon.pwrFlame) {
        root["pwrFlame"]  = Wagon.pwrFlame;
        oldWagon.pwrFlame  = Wagon.pwrFlame;
      }
      if (oldWagon.pwrSound != Wagon.pwrSound) {
        root["pwrSound"]  = Wagon.pwrSound;
        oldWagon.pwrSound  = Wagon.pwrSound;
      }
      if (oldWagon.pwrHeadlight != Wagon.pwrHeadlight) {
        root["pwrHeadlight"]  = Wagon.pwrHeadlight;
        oldWagon.pwrHeadlight  = Wagon.pwrHeadlight;
      }
      if (oldWagon.pwrAux != Wagon.pwrAux) {
        root["pwrAux"]  = Wagon.pwrAux;
        oldWagon.pwrAux  = Wagon.pwrAux;
      }
      if (oldWagon.mainVoltage != Wagon.mainVoltage) {
        root["mainVoltage"] = Wagon.mainVoltage;
        oldWagon.mainVoltage = Wagon.mainVoltage;
      }
      if (oldWagon.auxVoltage != Wagon.auxVoltage) {
        root["auxVoltage"] = Wagon.auxVoltage;
        oldWagon.auxVoltage = Wagon.auxVoltage;
      }
      if (oldWagon.startVoltage != Wagon.startVoltage) {
        root["startVoltage"] = Wagon.startVoltage;
        oldWagon.startVoltage = Wagon.startVoltage;
      }
      if (oldWagon.autoStart != Wagon.autoStart) {
        root["autoStart"] = Wagon.autoStart;
        oldWagon.autoStart = Wagon.autoStart;
      }

*/

