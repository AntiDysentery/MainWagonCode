#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>
#include <FS.h>

#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WagonClass.h>

const double MAN_LAT =  40.7864;
const double MAN_LON =  -119.2065;

extern LedClass Led;
extern SparkleClass Sparkle;
extern ChaseClass Chase;
extern BackgroundClass Background;
extern TraceClass Trace;
extern PacManClass PacMan;

extern WagonClass Wagon;
extern WagonGpsClass WGps;
extern WagonFlameClass Flame;

TinyGPSPlus gps;
double lastKnownCourse;

HardwareSerial GpsSerial(1);
HardwareSerial serialMega(2);

const char* ssid = "AntiDysentery";
const char* password = "password";
const IPAddress apIP(2, 2, 2, 2);

int pinCount;
String oldJson;
String newJson;

unsigned long saveTimer;
unsigned long fileBackupTimer;
unsigned long pinPerSecond;
unsigned long lastRequest;
unsigned long lastGPS;

unsigned long test1;
unsigned long test2;

bool debug = true;

String serialMegaFinal;
String serialMegaComm;

AsyncWebServer server(80);

/*
  pin0 =
  pin1 =
  pin2 = chip resets
  pin3 = requests per minute
  pin4 = total requests

*/
void setup(void) { //--------------------------------------------------------------------------
  delay(1000);
  pinMode(23, INPUT_PULLUP);  //strange esp32 shit
  //  rtc.begin();
  Serial.begin(115200);
  Serial.println("Starting Esp");
  GpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  serialMega.begin(19200, SERIAL_8N1, 27, 15);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(100);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.print("Connected! IP address: "); Serial.println(WiFi.softAPIP());
  if (SD.begin(33)) {   //SD card CS is GPIO 33 on the ESP32
    Serial.println("SD Card initialized.");
  }
  else {
    Serial.println("SD Card Failed.");
  }

  if (!readFile("/save.wgn")) {
    Serial.println("!!!Loading Backup!!!");
    if (!readFile("/save.bak")) {
      Serial.println("Backup Failed!");
    }
  }

  server.onNotFound([](AsyncWebServerRequest * request) {
    request-> send(200, "text/plain", "Your at Burnign Man \nGet Off Your Fucking Phone");
  });

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hello World");
  });

  server.on("/index.htm", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/index.htm", "text/html");
  });

  server.on("/go", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/index.htm", "text/html");
  });

  server.on("/app.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/app.css", "text/css");
  });

  server.on("/smoothie.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/smoothie.js", "application/javascript");
  });

  server.on("/jmin.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/jmin.js", "application/javascript");
  });

  server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/app.js", "application/javascript");
  });

  server.on("/fire.gif", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/fire.gif", "image/gif");
  });

  server.on("/font.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SD, "/font.ttf", "application/font");
  });

  server.on("/status.jsn", HTTP_GET, [](AsyncWebServerRequest * request) {
    updatePins() ;
    request->send(200, "application/json", VarToJson("full"));
  });

  server.on("/hattrick", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Request From Hat");
    updatePins() ;
    request->send(200, "application/json", VarToJson("led"));
  });

  server.on("/readjson", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println();
    Serial.println("Sending Json to Web");
    String txt = "<html><body><pre>";
    txt += VarToJson("full");
    txt += "</pre></body></html>";
    request->send(200, "text/html", txt);
  });


  server.on("/btnRequest", HTTP_POST, [](AsyncWebServerRequest * request) {

    request->send(200, "text/plain", "ass");
    updatePins() ;
    Serial.print("Button Request: ");
    Serial.print(request->argName(0).c_str());
    Serial.print(" - ");
    Serial.println(request->arg(request->argName(0)) );

    String led = "";
    if (request->argName(0) == "main") led = "M,";
    if (request->argName(0) == "flame") led = "F,";
    if (request->argName(0) == "led") led = "L,";
    led += request->arg(request->argName(0));
    led += "\r";
    serialMega.print(led);
  });

  server.on("/rngRequest", HTTP_POST, [](AsyncWebServerRequest * request) {
    int args = request->args();
    for (int i = 0; i < args; i++) {
      Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }

    request->send(200, "text/plain", "ass");

    updatePins() ;
    String led = "";

    Serial.print("Range Request: ");
    Serial.print(request->argName(0).c_str());
    Serial.print(" - ");
    Serial.println(request->arg(request->argName(1)) );

    led = "L," + request->arg(request->argName(1)) + "," + request->arg(request->argName(0));
    if (request->argName(0) == "rngFlameOn") led = "F," + request->argName(0) + "," + request->argName(0);
    if (request->argName(0) == "rngFlameOff") led = "F," + request->argName(0) + "," + request->argName(0);
    led += "\r";

    Serial.println(led);
    serialMega.print(led);
  });

  server.begin();
  Serial.println("HTTP server started");
  Wagon.chipResets++;

}

void loop(void) { //--------------------------------------------------------------------------

  if (GpsSerial.available()) gps.encode(GpsSerial.read());
  if (checkSerial()) handleRequests();

  if ((millis() - saveTimer) > 15000) {
    saveTimer = millis();
    saveFile("/save.wgn");
  }

  if ((millis() - fileBackupTimer) > 60000) {
    fileBackupTimer = millis();
    saveFile("/save.bak");
  }

  if ((millis() - lastGPS) > 10000) {
    updateGPS();
    lastGPS = millis();
    String txt = VarToJson("gps");
    serialMega.print(txt);
    Serial.println(txt);
  }
}

bool checkSerial() { //-------------------------------------------------------
  bool flag = false;

  if (serialMega.available()) {
    char in = serialMega.read();
    if (in == 13) {
      serialMegaFinal = serialMegaComm;
      serialMegaComm = "";
      flag = true;
    } else {
      serialMegaComm += in;
    }
  }
  return flag;
}

void handleRequests() { //-------------------------------------------------------
  Serial.print("Request from Mega: ");
  if (serialMegaFinal == "Hello Led") {
    serialMega.print(VarToJson("full"));
  }

  if (JsonToVar(serialMegaFinal)) {
    Serial.println("Successful");
  } else {
    Serial.println("Fail");
    Serial.println(serialMegaFinal);
  }
  serialMegaFinal = "";
}

bool readFile(String fileName) {//--------------------------------------------------------------------------
  String var = "";
  int fileSize = 0;

  File mySD = SD.open(fileName);
  Serial.print("Loading - "); Serial.print(fileName); Serial.print(": ");
  Serial.print(mySD.size()); Serial.println("b");
  char data[mySD.size()];

  if (mySD) {
    for (int x = 0; x < mySD.size(); x++) {
      if (mySD.available()) {
        data[x] = mySD.read();
        fileSize++;
      } else {
        data[x] = 0;
      }
    }
  }
  mySD.close();

  Serial.print("Size of Read: "); Serial.println(fileSize);
  Serial.println("_________________");
  Serial.println(data);
  Serial.println("_________________");
  JsonToVar(data);
  return true;
}

void saveFile(String sdFile) { //--------------------------------------------------------------------------
  SD.remove(sdFile);
  File mySD = SD.open(sdFile, FILE_WRITE);
  if (mySD) {
    mySD.print (VarToJson("full"));
    mySD.close();
  }
}

void updatePins() { //--------------------------------------------------------------------------
  lastRequest = millis();
  pinCount++;
  Wagon.totalRequests ++;   //total number of requests

  if ((pinPerSecond + 10000) <= millis()) {   //Json Calls per Minute
    pinPerSecond = millis();
    Wagon.requestsPerMinute = (pinCount * 6);   //requests per minute
    pinCount = 0;
  }
}

void updateGPS() { //--------------------------------------------------------------------------
  double p;
  double q;

  p = gps.speed.mph();
  if (p < 2) {
    p = 0;
    q = lastKnownCourse;
  } else {
    q = gps.course.deg();
    lastKnownCourse = q;
  }

  WGps.speed = p;
  WGps.dir = q;
  WGps.alt = gps.altitude.feet();
  WGps.lat = gps.location.lat();
  WGps.lon = gps.location.lng();
  WGps.sat = gps.satellites.value();

  WGps.dirtohome = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), WGps.homelat, WGps.homelon);
  double streetFt = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), MAN_LAT, MAN_LON);
  streetFt = streetFt * 3.28084; //convert m to feet
  int sDeg = TinyGPSPlus::courseTo(MAN_LAT, MAN_LON, gps.location.lat(), gps.location.lng());
  String txt = "";
  Serial.print("Feet: ");
  Serial.println(streetFt);
  WGps.manfeet = streetFt;
  int hr; int mn;
  sDeg = ((sDeg - 45) % 360);

  Serial.println(sDeg);
  double sDegd = ((double)sDeg) / 30;

  Serial.println(sDegd);

  hr = sDegd;
  sDegd = sDegd - hr;
  if (hr == 0) hr = 12;
  Serial.println(sDegd);
  mn = 60 * sDegd;
  Serial.println(mn);

  txt = hr;
  txt += ":";
  if (mn < 10)txt += "0";
  txt += mn;
  txt += " & ";

  Serial.println("===");

  WGps.locstring = txt;

  txt = "Inner Playa";
  if (streetFt > 2420) txt = "Esplinade";  //Esplinade is at 2520
  if (streetFt > 2860) txt = "Algorithm";          //A is at  2960  //240 ft between streets
  if (streetFt > 3100) txt = "Bender";          //B is at  3200
  if (streetFt > 3340) txt = "Cylon";          //C is at  3440
  if (streetFt > 3580) txt = "Dewey";          //D is at  3680
  if (streetFt > 3820) txt = "Electro";          //E is at  3920
  if (streetFt > 4060) txt = "Fribo";          //F is at  4160
  if (streetFt > 4300) txt = "Gort";          //G is at  4400
  if (streetFt > 4540) txt = "Hal";          //H is at  4640
  if (streetFt > 4780) txt = "Iron Giant";          //I is at  4880
  if (streetFt > 5020) txt = "Johny 5";          //J is at  5120
  if (streetFt > 5260) txt = "Kinoshita";          //K is at  5360
  if (streetFt > 5500) txt = "Leon";          //L is at  5600
  if (streetFt > 5700) txt = "Outer Playa";
  if (streetFt > 10000) txt = "Not on Playa";

  WGps.locstring += txt;
  Serial.println("Location: " + WGps.locstring);

  if (WGps.lon != 0) {
    Serial.println("GPS Data Valid...  Saving");
    String fileName = ""; 
    int gpsDay = gps.date.day();
    if (gpsDay ==0) gpsDay = 69;    
    fileName = "gps";
    fileName += (String)gpsDay;
    fileName += ".dat";
    if (fileName.length() > 12) Serial.println("ERROR - FILENAME TOO LONG: *" + fileName + "*");
    File mySD = SD.open(fileName, FILE_WRITE);
    if (mySD) {
      Serial.println("File Open: " + fileName);
      mySD.print (gps.location.rawLat().negative ? "-" : "+");
      mySD.print (gps.location.lat(), 4);
      mySD.print (",");
      mySD.print (gps.location.rawLng().negative ? "-" : "+");
      mySD.print (gps.location.lng(), 4);
      mySD.print (",");
      mySD.print (gps.altitude.feet());
    }
    mySD.close();
  }
}



