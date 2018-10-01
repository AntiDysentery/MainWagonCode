//Hello GitHub!! Yea!!


#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>

//#include <ESPAsyncWebServer.h>
//#include <SPIFFSEditor.h>
//#include <StringArray.h>
//#include <WebAuthentication.h>
//#include <WebHandlerImpl.h>
//#include <WebResponseImpl.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#include <FastLED.h>
#include "gradients.h"
#include <WagonClass.h>
#include <ESP8266WiFi.h>

const char* ssid  = "AntiDysentery";
const char* password = "password";
const char* host = "2.2.2.2";

#define LED_PIN     3
#define NUM_LEDS    171
#define COLOR_ORDER GRB
#define COOLING  150
#define SPARKING 120
#define PIN_D0 0
#define PIN_D1 1
#define PIN_D2 2
#define PIN_D3 3




const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
  BlacK_Red_Magenta_Yellow_gp,
  Blue_Cyan_Yellow_gp,
  ib_jul01_gp,
  es_vintage_57_gp,
  es_vintage_01_gp,
  es_rivendell_15_gp,
  rgi_15_gp,
  retro2_16_gp,
  Analogous_1_gp,
  es_pinksplash_08_gp,
  es_pinksplash_07_gp,
  Coral_reef_gp,
  es_ocean_breeze_036_gp,
  es_ocean_breeze_068_gp,
  departure_gp,
  es_landscape_64_gp,
  es_landscape_33_gp,
  rainbowsherbet_gp,
  gr65_hult_gp,
  gr64_hult_gp,
  GMT_drywet_gp,
  ib15_gp,
  Fuschia_7_gp,
  es_emerald_dragon_08_gp,
  lava_gp,
  fire_gp,
  Colorfull_gp,
  Magenta_Evening_gp,
  Pink_Purple_gp,
  Sunset_Real_gp,
  es_autumn_19_gp,
  BlacK_Blue_Magenta_White_gp,
  BlacK_Magenta_Red_gp
};

//Added to Git 

AsyncWebServer server(80);


// Count of how many cpt-city gradients are defined:
const uint8_t gGradientPaletteCount = sizeof( gGradientPalettes) / sizeof( TProgmemRGBGradientPalettePtr );


CRGB leds[NUM_LEDS];
CRGB nfleds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;
uint8_t gCurrentPaletteNumber = 0;// Current palette number from the 'playlist' of color palettes
CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );
CRGBPalette16 targetPalette( PartyColors_p );
enum request {btnSparkle, btnSparkleColor, btnChase, btnChaseColor, btnBackground, btnBackgroundColor, btnFade, btnAux, btnFlame, btnRandom};


extern ChaseClass Chase;
extern SparkleClass Sparkle;
extern BackgroundClass Background;
extern TraceClass Trace;
extern PacManClass PacMan;
extern LedClass Led;
extern WagonGpsClass WGps;


bool hasWifi = false;
bool Fire = false;
bool Fade = false;
bool FadeOverride = false;

unsigned long lastRandom;
unsigned long lastWifi;
unsigned long lastDraw;
unsigned long lastEsp;

int currled = 0;

String serialMegaComm;
String serialMegaFinal;
String newJson;


void setup() {//-------------------------------------------------------
  delay(1000); // power-up safety delay
  Serial.begin(115200);
  Serial.println("Begin Hat Trick - Async");

  FastLED.addLeds<APA102, 12, 13, BGR, DATA_RATE_MHZ(4)>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  Serial.println("Esp8266 Started");

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  Background.value = 50;
  Chase.pvalue = 85;
  Background.pvalue = 170;
  Led.speed = 100;
}

void initiateWifi() {//-------------------------------------------------------
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) {
    hasWifi = false;
    Serial.println("Not in range");
    Led.random = true;
    return;
  } else {
    hasWifi = true;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() { //-------------------------------------------------------
  double mill = millis();

  if (((mill - lastWifi) > 15000) & (!hasWifi)) {//if not connected try every n seconds to connect
    lastWifi = mill;
    initiateWifi();
  }

  if (((mill - lastEsp) > 3000 ) & (hasWifi)) {//if connected update hat every n seconds
    lastEsp = mill;
    updateJsonFromEsp();
  }

  if (((mill - lastRandom) > 15000) & (Led.random)) { //random seeder every n seconds
    lastRandom = mill;
    setRandom();
  }

  if ((mill - lastDraw) > 20) { //speed delay
  lastDraw = mill;
  drawLeds();
    if (Led.random) return;
    updateLedMode();
  }

}


void updateJsonFromEsp() {
  String txt = "";

  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    hasWifi = false;
    Led.random = true;
    return;
  }

  String url = "GET /readjson HTTP/1.1\r\nHost: 2.2.2.2\r\nConnection: close\r\n\r\n";
  client.print(url);
  //client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();

  if (client.available()) txt = client.readStringUntil('\r');

  /*   while (client.available() == 0) {
       txt += client.readStringUntil('\r');
       if ((millis() - timeout) > 5000) {
         Serial.println(">>> Client Timeout !");
         Led.random = true;
         client.stop();
         return;
       }
     }
  */

  if (txt.length() == 0) return;

  if (JsonToVar(txt)) {
    Serial.println("Json Parse Successful");
  } else {
    Serial.println(txt);
    Serial.println("Failed to Parse Json");
  }
}

void setRandom() {

  gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
  targetPalette = gGradientPalettes[ gCurrentPaletteNumber ];

  if (random8(2) == 1) Led.fade = random8(30);
  if (random8(2) == 1) Led.speed = random8(100);

  if (random8(2) == 1) Sparkle.on = !Sparkle.on;
  if (random8(2) == 1) Sparkle.fade = !Sparkle.fade;
  Sparkle.colormode = random8(5);
  Sparkle.huevalue = random8(255);
  Sparkle.value = random8(255);

  if (random8(2) == 1) Chase.on = !Chase.on;
  if (random8(2) == 1) Chase.fade = !Chase.fade;
  Chase.huevalue = random8(255);
  Chase.colormode = random8(5);
  Chase.distance = random8(50);

  if (random8(10) == 1) Background.on = !Background.on;
  if (random8(2) == 1) Background.fade = !Background.fade;
  Background.huevalue = random8(255);
  Background.colormode = random8(5);

  if (random8(10) == 1) Led.flame = true; else Led.flame = false;
  if (random8(10) == 1) PacMan.on = true; else PacMan.on = false;

  //Pot1 = random8(20, 200);
  //Pot2 = random8(255);
}

void updateLedMode() {//************************************************************************************************************************************


  Chase.on = false;
  Sparkle.on = false;
  PacMan.on = false;
  Background.on = false;
  Led.flame = false;
  switch (Led.mode)
  {
    case 1:
      Chase.on = true;
      Chase.colormode = 1;
      Chase.distance = 10;
      Chase.colorname = "Solid Chase";
      break;

    case 2:
      Chase.on = true;
      Chase.colormode = 2;
      Chase.distance = 20;
      Chase.colorname = "Random Chase";
      break;

    case 3:
      Chase.on = true;
      Chase.colormode = 3;
      Chase.distance = 30;
      Chase.colorname = "Pallet Chase";
      break;

    case 4:
      Chase.on = true;
      Chase.colormode = 1;
      Chase.distance = 10;
      Chase.colorname = "Solid Chase";
      break;

    case 20:
      Sparkle.on = true;
      Sparkle.colormode = 0;
      break;

    case 21:
      Sparkle.on = true;
      break;

    case 22:
      Sparkle.on = true;
      break;

    case 23:
      Sparkle.on = true;
      break;

    case 30:
      Background.on = true;
      break;

    case 31:
      Background.on = true;
      break;

    case 32:
      Background.on = true;
      break;

    case 33:
      Background.on = true;
      break;

    case 40:
      PacMan.on = true;
      break;

    case 41:
      Led.flame = true;
      break;

    case 50:
      Chase.on = true;
      Sparkle.on = true;
      Background.on = true;
      PacMan.on = true;
      break;


    default:
      Led.mode = random8(50);
  }
}

void drawLeds()//-------------------------------------------------------
{
  nblendPaletteTowardPalette(currentPalette, targetPalette, 1);     //Palette blend and speed

  if (Chase.distance < 10) Chase.distance = 10;
  Chase.pvalue = (Chase.pvalue + 1) % 255;
  Chase.value = (Chase.value - 1) % Chase.distance;

  PacMan.value = (PacMan.value + 1) % NUM_LEDS;
  Sparkle.pvalue = (Sparkle.pvalue + 1) % 255;
  Background.pvalue = (Background.pvalue + 1) % 255;
  Trace.value  ++;
  if (Trace.value > NUM_LEDS) {
    Trace.value = 0;
    Trace.num = (Trace.num + 1) % NUM_LEDS;
  }

  if (Led.flame) {
    DrawFire();   //Fire animation must stay out of mail loop
  } else {

    for (currled = 0; currled < NUM_LEDS; currled++)   //Main Led Loop =================================================
    {

      leds[currled].fadeToBlackBy(Led.fadevalue);
      nfleds[currled] = CRGB::Black;

      if (Background.on) DrawBackGround();
      if (Led.noise) DrawNoise();
      if (Sparkle.on) DrawSparkle();
      if (Chase.on) DrawChase();
      if (PacMan.on) DrawPacMan();
      if (Led.gps) DrawGps();

      leds[currled] = leds[currled] + nfleds[currled];
      //leds[currled] = nfleds[currled];
    }
  }//end main led lood  ==============================================================================================

  leds[Trace.value] = CRGB::Red;
  leds[Trace.num] = CHSV((currled % 255), 255, 50);
  if (Led.random) leds[0] = CHSV(150, 255, 50);

  FastLED.show();
}
void DrawGps() { //-------------------------------------------------------
  int gpsHue;
  gpsHue = map(WGps.dirtohome, 0, 360, 0, 255);
  nfleds[currled] = CHSV(gpsHue, 255, 40);
}

void DrawBackGround() {//-------------------------------------------------------
  //nfleds[currled] = CHSV(0, 255, Background.value);
  switch (Background.colormode) {
    case 0: nfleds[currled] = CHSV(0, 255, Background.value); break;
    case 1: nfleds[currled] = CHSV(Background.huevalue, 255, Background.value); break;
    case 2: nfleds[currled] = ColorFromPalette(currentPalette, random8(255), Background.value, currentBlending); break;
    case 3: nfleds[currled] = CHSV(((Background.huevalue + currled) % 255), 255, Background.value); break;
    case 4: nfleds[currled] = ColorFromPalette(currentPalette, ((Background.pvalue + currled) % 255), Background.value, currentBlending);      break;
    default: Background.colormode = 0; break;
  }
}

void DrawSparkle() {//-------------------------------------------------------
  if (random16(Sparkle.value * 10) == 1)
  {
    if (Sparkle.fade) {
      switch (Sparkle.colormode) {
        case 0: nfleds[currled] = CRGB::White; break;
        case 1: nfleds[currled] = CHSV(Sparkle.huevalue, 255, 255); break;
        case 2: nfleds[currled] = CHSV(random8(255), 255, 255); break;
        case 3: nfleds[currled] = CHSV(((Sparkle.pvalue) % 255), 255, 255); break;
        case 4: nfleds[currled] = ColorFromPalette(currentPalette, ((Sparkle.pvalue + currled) % 255), 255, currentBlending); break;
        default: Sparkle.colormode = 0;
      }
    } else {
      switch (Sparkle.colormode) {
        case 0: leds[currled] = CRGB::White; break;
        case 1: leds[currled] = CHSV(Sparkle.huevalue, 255, 255); break;
        case 2: leds[currled] = CHSV(random8(255), 255, 255); break;
        case 3: leds[currled] = CHSV(((Sparkle.pvalue) % 255), 255, 255); break;
        case 4: leds[currled] = ColorFromPalette(currentPalette, ((Sparkle.pvalue + currled) % 255), 255, currentBlending); break;
        default: Sparkle.colormode = 0; break;
      }

    }
  }
}

void DrawChase () {
  if (((currled + Chase.value) % Chase.distance) == 0)
  {
    if (!Chase.fade) {
      switch (Chase.colormode) {
        case 0: leds[currled] = CRGB::Red; break;
        case 1: leds[currled] = CHSV(Chase.huevalue, 255, 255); break;
        case 2: leds[currled] = CHSV(random16(255), 255, 255); break;
        case 3: leds[currled] = CHSV(((Chase.pvalue + currled) % 255), 255, 255); break;
        case 4: leds[currled] = ColorFromPalette(currentPalette, ((Chase.pvalue + currled) % 255), 255,  currentBlending); break;
        default: Chase.colormode = 0; break;
      }
    } else {
      switch (Chase.colormode) {
        case 0: nfleds[currled] = CRGB::Red; break;
        case 1: nfleds[currled] = CHSV(Chase.huevalue, 255, 255); break;
        case 2: nfleds[currled] = CHSV(random16(255), 255, 255); break;
        case 3: nfleds[currled] = CHSV(((Chase.pvalue + currled) % 255), 255, 255); break;
        case 4: nfleds[currled] = ColorFromPalette(currentPalette, ((Chase.pvalue + currled) % 255), 255,  currentBlending); break;
        default: Chase.colormode = 0; break;
      }
    }
  }
}

void DrawPacMan ()
{
  if ( ((PacMan.value + 2) < NUM_LEDS) && (currled == PacMan.value) )
  {
    leds[currled] = CRGB::Yellow;
    if (currled & 0x10)
    {
      leds[currled + 1] = CRGB::Yellow;
      leds[currled + 2] = CRGB::Yellow;
    }
  }
  if (((currled % 10) == 0) && (currled > PacMan.value) && (currled <= (PacMan.value + 100)))
  {
    leds[currled] = CRGB::White;
  }
}

void DrawFire()
{
  Sparkle.on = false;
  Chase.on = false;

  static byte heat[NUM_LEDS];
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 100) / NUM_LEDS) + 2));
  }
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  if ( random8() < SPARKING ) {
    int y = random8(7);

    for (int q = 0; q < (NUM_LEDS - 50); q = q + 50)
    {
      heat[y + q] = qadd8( heat[y], random8(160, 255) );
    }
  }
  for ( int j = 0; j < NUM_LEDS; j++)
  {
    leds[j] = HeatColor( heat[j]);
  }
}

void DrawNoise() {                                            // moves a noise up and down while slowly shifting to the side

  uint16_t scale = 500;                                      // the "zoom factor" for the noise

  //for (uint16_t i = 0; i < NUM_LEDS; currled+) {

  uint16_t shift_x = beatsin8(5);                           // the x position of the noise field swings @ 17 bpm
  uint16_t shift_y = millis() / 100;                        // the y position becomes slowly incremented


  uint16_t real_x = (currled + shift_x) * scale;                  // the x position of the noise field swings @ 17 bpm
  uint16_t real_y = (currled + shift_y) * scale;                  // the y position becomes slowly incremented
  uint32_t real_z = millis() * 20;                          // the z position becomes quickly incremented

  uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;   // get the noise data and scale it down

  uint8_t index = sin8(noise * 3);                         // map LED color based on noise data
  uint8_t bri   = noise;

  leds[currled] = ColorFromPalette(currentPalette, index, Background.value, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  //leds[currled] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  //leds[currled] = CHSV(index, index, bri);
}



bool parseBool(int x) {//-------------------------------------------------------
  bool b = false;
  if (parseVar(serialMegaFinal, x) == 1) b = true;
  return b;
}

int parseVar(int x) {//-------------------------------------------------------
  return parseVar(serialMegaFinal, x);
}

int parseVar(String var, int loop) {//-------------------------------------------------------
  int ind1 = -1;
  String sretInt = "";
  int temp = 0;
  bool imax = false;
  for (int x = 0; x <= loop; x++) {
    if (imax) return 0;
    temp = var.indexOf(',', ind1 + 1);  //finds location of first ,
    sretInt = var.substring(ind1 + 1, temp);  //captures data String
    ind1 = temp;
    if (temp == -1) imax = true;
  }
  return sretInt.toInt();
}


