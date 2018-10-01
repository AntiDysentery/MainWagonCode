Getting Git Down.- Ima Noob

#include <ShiftRegister74HC595.h>
#include <TimerOne.h>
#include <WagonClass.h>
#include <ArduinoJson.h>
#include "Wire.h"
#include <LiquidTWI.h>
#include <RotaryEncoder.h>

//#include "WagonMegaConstants.h"
RotaryEncoder encoder(2, 3);

extern LedClass Led;
extern SparkleClass Sparkle;
extern ChaseClass Chase;
extern BackgroundClass Background;
extern TraceClass Trace;
extern PacManClass PacMan;
extern WagonClass Wagon;
extern WagonGpsClass WGps;
extern WagonFlameClass Flame;

LiquidTWI lcdDeviceMain(0);
LiquidTWI lcdDeviceFlame(1);

#define Main_Menu_Max_Root 4
#define Main_Menu_Max_Second 4
#define Main_Menu_Max_Third 1

#define FLAME1_BUTTON 52
#define FLAME2_BUTTON 50
#define FLAME3_BUTTON 48
#define FLAME4_BUTTON 46
#define FLAME5_BUTTON 44
#define RND1_BUTTON 42
#define FTB_BUTTON 40

#define COLOR_BUTTON 38
#define CHASE_BUTTON 36
#define SPARKLE_BUTTON 34
#define NOISE_BUTTON 32

#define LED1_BUTTON 30
#define LED2_BUTTON 28
#define FLAME_BUTTON 26
#define SOUND_BUTTON 24
#define HLIGHT_BUTTON 22

#define COLOR_LED 12
#define CHASE_LED 6
#define SPARKLE_LED 9
#define NOISE_LED 3

#define LED1_LED 11
#define LED2_LED 10
#define FLAME_LED 7
#define SOUND_LED 14
#define HLIGHT_LED 5

#define FLAME1_LED 1
#define FLAME2_LED 13
#define FLAME3_LED 4
#define FLAME4_LED 2
#define FLAME5_LED 0
#define FTB_LED 8
#define RND_LED 15

#define serialDataPin 13
#define serialClockPin 11
#define serialLatchPin 12

#define digitalInputCount 16

#define FLAME_OFF_PIN A0
#define SPEED_PIN A1
#define ROTARY_PIN A4
#define MAX_FLAME_MODE 3

const int digitalInputPin[digitalInputCount] = {52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22};
const int relayBank1[8] = {53, 51, 49, 47, 45, 43, 41, 39};
const int relayBank2[8] = {37, 35, 33, 31, 29, 27, 25, 23};
const int mapled[16] = {12, 6, 9, 3, 11, 10, 7, 14, 5, 1, 13, 4, 2, 0, 8, 15};

//const int ROTARY_PIN = 23;
const String LCDSPACE = "                    ";

HardwareSerial &serialLed = Serial2;  //pins 18+19
HardwareSerial &serialXbee = Serial1; //pins 16+17
HardwareSerial &serialEsp = Serial3;  //pins 15+14   //ESP for sure

ShiftRegister74HC595 sr(2, serialDataPin, serialClockPin, serialLatchPin);

bool espUpdate = false;
bool displayChange = true;
bool relayChange = true;
bool mainMenu = false;
bool wagonAutoStart = false;
bool wagonStartNow = false;
bool EspOk = false;
bool FlameOk = false;
bool LedOk = false;
bool displayColorShift = true;
bool serialFlag = false;
bool readVoltage = false;
bool fireFlame[5];
bool flameReady = false;
bool flameModeFire = false;
bool bottomFlag = false;
bool led1ButtonLS = false;
bool led2ButtonLS = false;
bool flameButtonLS = false;
bool soundButtonLS = false;
bool hlightButtonLS = false;

int flameMode = 0;
int menuItem[4] = {0, 0, 0, 0};
int encoderPos = 0;
int displayMode;
int oldLcdColor;
int oldFlameOff;
int flameLength;
int flameRandom = 0;
int flameCount = 0;
int oldSpeed = 0;

String serialLedComm;
String serialLedFinal;
String serialEspComm;
String serialEspFinal;
String serialXbeeComm;
String serialXbeeFinal;
String lcdMain[4];
String lcdFlame[4];
String flameModeTxt = "Random";

unsigned long voltageTimer = 0;
unsigned long displayTimer = 99999;
unsigned long lastEncoder = 0;
unsigned long threeHundredMillis = 0;
unsigned long lastEsp = 0;
unsigned long lastXbee = 0;
unsigned long lastLed = 0;
unsigned long lastRotaryPress = 0;
unsigned long wagonStartTime = 0;
unsigned long wagonLastStart = 0;
unsigned long wagonStopTime = 0;
unsigned long lastFlame = 0;
unsigned long lastDigitalButton = 0;

uint8_t manbyte0[8] = {14, 4, 0, 14, 21, 4, 10, 10};
uint8_t manbyte1[8] = {14, 4, 0, 31, 4, 4, 10, 10};
uint8_t manbyte2[8] = {14, 4, 17, 14, 4, 4, 10, 10};
uint8_t manbyte3[8] = {24, 24, 0, 0, 0, 0, 0, 0};
uint8_t clichee[8] = {1, 2, 0, 14, 17, 31, 16, 14};
uint8_t manbyte5[8] = {24, 24, 16, 17, 12, 9, 5, 13};

void setup()
{
  sr.setAllLow();

  Background.huevalue = 120;
  Flame.flameon = 25;
  Flame.flameoff = 40;
  oldFlameOff = Flame.flameoff;

  delay(500);
  Serial.begin(115200); // start the serial monitor link
  Serial.println("");
  Serial.println("");
  Serial.println("--==  Main Arduino Mega 2560 Online  ==--");

  serialLed.begin(19200);
  serialEsp.begin(19200);
  serialXbee.begin(9600);

  Timer1.initialize(1500); //1000 micro seconds  - 1 millisecond
  Timer1.attachInterrupt(rotaryCheck);

  lcdDeviceMain.begin(20, 4);
  lcdDeviceFlame.begin(20, 4);

  lcdDeviceMain.createChar(0, manbyte0);
  lcdDeviceMain.createChar(1, manbyte1);
  lcdDeviceMain.createChar(2, manbyte2);
  lcdDeviceMain.createChar(3, manbyte3);
  lcdDeviceMain.createChar(4, clichee);
  lcdDeviceMain.createChar(5, manbyte5);

  lcdDeviceFlame.createChar(0, manbyte0);
  lcdDeviceFlame.createChar(1, manbyte1);
  lcdDeviceFlame.createChar(2, manbyte2);

  lcdDeviceFlame.createChar(3, manbyte3);
  lcdDeviceFlame.createChar(4, clichee);
  lcdDeviceFlame.createChar(5, manbyte5);

  Serial.println("LCDs Enaled");
  pinMode(ROTARY_PIN, INPUT_PULLUP); //rotary button

  for (int x = 0; x < 16; x++)
  {
    pinMode(digitalInputPin[x], INPUT_PULLUP);
  }

  for (int x = 0; x < 8; x++)
  {
    pinMode(relayBank1[x], OUTPUT);
    digitalWrite(relayBank1[x], HIGH);
    pinMode(relayBank2[x], OUTPUT);
    digitalWrite(relayBank2[x], HIGH);
  }

  serialEsp.print("Hello World\r");
  serialLed.print("Hello World\r");

  if (digitalRead(FLAME5_BUTTON) == LOW)
  {
    Serial.println("Starting Welcome Screen");
    welcomeScreen();
  }
  else
  {
    Serial.println("ByPass Welcome Screen");
  }

  readTheVoltage();
  Serial.println("Ending Setup");
  Serial.println("");
}

void loop()
{ //-------------------------------------------------------
  if (checkSerial())
    handleRequests();
  if (readVoltage)
    readTheVoltage();
  checkSwitches();
  checkTimers();
  if (relayChange)
    setRelays();
  if (displayChange)
  {
    updateDisplay();
    updateButtonLeds();
  }
}

void updateButtonLeds()
{

  sr.set(LED1_LED, Wagon.pwrLed1);
  sr.set(LED2_LED, Wagon.pwrLed2);
  sr.set(FLAME_LED, Wagon.pwrFlame);
  sr.set(SOUND_LED, Wagon.pwrSound);
  sr.set(HLIGHT_LED, Wagon.pwrHeadlight);

  sr.set(CHASE_LED, Chase.on);
  sr.set(COLOR_LED, Background.on);
  sr.set(SPARKLE_LED, Sparkle.on);
  sr.set(NOISE_LED, Led.noise);

  sr.set(FLAME1_LED, Wagon.pwrFlame);
  sr.set(FLAME2_LED, Wagon.pwrFlame);
  sr.set(FLAME3_LED, Wagon.pwrFlame);
  sr.set(FLAME4_LED, Wagon.pwrFlame);
  sr.set(FLAME5_LED, Wagon.pwrFlame);
  sr.set(RND_LED, Wagon.pwrFlame);
  sr.set(FTB_LED, Wagon.pwrFlame);
}

void WagonStart()
{ //-------------------------------------------------------
  if ((millis() - wagonLastStart) > 20000)
  {
    Serial.print("Starting Engine");
    wagonLastStart = millis();
    wagonStartTime = millis();
    Wagon.startRelay = true;
    relayChange = true;
  }
  else
  {
    Serial.println("Wagon Auto Start Too Soon!");
  }
}

void checkTimers()
{ //-------------------------------------------------------
  unsigned long espTimer;

  if (millis() - espTimer) > 1000)
    {
      espTimer = millis();
      serialEsp.print(VarToJson("full"));
    }

  if ((millis() - displayTimer) > 2000)
  { //change display every 3 seconds
    displayMode++;
    displayChange = true;
  }

  if (((millis() - lastEncoder) > 6000) & (menuItem[0] != 0))
  { //return one menu every 6 seconds
    lastEncoder = millis();
    displayChange = true;
    if (menuItem[2] > 0)
      menuItem[2] = 0;
    else if (menuItem[1] > 0)
      menuItem[1] = 0;
    else
      menuItem[0] = 0;
  }

  if ((millis() - voltageTimer) > 30000)
  { //every 30 Seconds
    voltageTimer = millis();
    readVoltage = true;
  }

  if ((millis() - lastLed) > 30000)
  {
    LedOk = false;
    lastLed = millis();
  }

  if ((millis() - lastEsp) > 30000)
  {
    EspOk = false;
    lastEsp = millis();
  }

  if ((millis() - lastXbee) > 30000)
  {
    FlameOk = false;
    lastXbee = millis();
  }

  if (Wagon.pwrFlame)
  {

    if ((millis() - (Flame.flameon + Flame.flameoff)) > lastFlame)
      flameReady = true;

    if ((flameCount > 0) && flameReady)
    {
      switch (flamemode)
      {
      case 0: //random
        flameCount--;
        fireFlame[random(5)] = true;
        break;
      case 1: //Front to Back
        flameCount--;
        fireFlame[(4 - flameCount)] = true;
        break;
      case 2: //Back To Front
        flameCount--;
        fireFlame[flameCount] = true;
        break;
      case 3: //knight rider
        if (flameCount > 4)
        {
          flameCount--;
          fireFlame[(4 - (flameCount - 5))] = true;
        }
        else
        {
          flameCount--;
          fireFlame[flameCount] = true;
        }
        break;
      case 4: //All on
        flameCount = 0;
        fireFlame[0] = true;
        fireFlame[1] = true;
        fireFlame[2] = true;
        fireFlame[3] = true;
        fireFlame[4] = true;
        break;
      }
    }
    for (int i = 0; i < 5; i++)
    {
      if (fireFlame[i] && flameReady)
      { //Turn on flame
        lastFlame = millis();
        flameReady = false;
        digitalWrite(relayBank2[i], LOW);
      }

      if ((millis() > (lastFlame + Flame.flameon)) && fireFlame[i])
      { //turn off flame
        digitalWrite(relayBank2[i], HIGH);
        fireFlame[i] = false;
      }
    }

    if (Wagon.stopRelay)
    { //Wagon Engine kill reset after 5 seconds
      if ((millis() - wagonStopTime) > 5000)
      {
        Wagon.stopRelay = 0;
        relayChange = true;
      }
    }

    if (Wagon.startRelay)
    { //Wagon Engine Start reset after 5 seconds
      if ((millis() - wagonStartTime) > 5000)
      {
        Wagon.startRelay = 0;
        relayChange = true;
      }
    }
  }
}

void handleMainPress(String request)
{ //-------------------------------------------------------

  relayChange = true;
  request = request.substring(2);
  Serial.println("Recieved Mega Command :" + request);

  if (request == "btnMainLed1")
    Wagon.pwrLed1 = !Wagon.pwrLed1;
  if (request == "btnMainLed2")
    Wagon.pwrLed2 = (Wagon.pwrLed2) ? 0 : 1;
  if (request == "btnMainFlame")
    Wagon.pwrFlame = (Wagon.pwrFlame) ? 0 : 1;
  if (request == "btnMainSound")
    Wagon.pwrSound = (Wagon.pwrSound) ? 0 : 1;
  if (request == "btnMainHeadlight")
    Wagon.pwrHeadlight = (Wagon.pwrHeadlight) ? 0 : 1;
  if (request == "btnMainAux1")
    Wagon.pwrAux = (Wagon.pwrAux) ? 0 : 1;
  if (request == "btnMainAllOff")
  {
    Wagon.pwrLed1 = false;
    Wagon.pwrLed2 = false;
    Wagon.pwrFlame = false;
    Wagon.pwrSound = false;
    Wagon.pwrHeadlight = false;
    Wagon.pwrAux = false;
    Wagon.startRelay = false;
    Wagon.stopRelay = false;
  }

  if (request == "btnMainStart")
  {
    WagonStart();
  }

  if (request == "btnMainStop")
  {
    Wagon.startRelay = 0;
    Wagon.stopRelay = 1;
    wagonStopTime = millis();
  }

  serialEsp.print(VarToJson("full"));
}

void HandleFlameRequest(String request)
{
  Serial.println("Flame Request: " + request);
  if (request.startsWith("F,"))
  {
    request = request.substring(2);

    if (request == "btnRandom")
    {
      flameMode = 0 flameCount = 4;
      return;
    }
    if (request == "btnFireFtB")
    {
      flameMode = 1;
      flameCount = 4;
      return;
    }

    if (request == "btnFireCannon1")
    {
      fireFlame[0] = true;
      return;
    }
    if (request == "btnFireCannon2")
    {
      fireFlame[1] = true;
      return;
    }
    if (request == "btnFireCannon3")
    {
      fireFlame[2] = true;
      return;
    }
    if (request == "btnFireCannon4")
    {
      fireFlame[3] = true;
      return;
    }
    if (request == "btnFireCannon5")
    {
      fireFlame[4] = true;
      return;
    }

    int temp = request.indexOf(',', 0);          //finds location of first ,
    temp = (request.substring(0, temp).toInt()); //captures data String
    request = request.substring(temp + 1);

    if (request == "rngFlameOn")
    {
      Flame.flameon = temp;
      flameLength = Flame.flameon + Flame.flameoff;
      return;
    }
    if (request == "rngFlameOn")
    {
      Flame.flameon = temp;
      flameLength = Flame.flameon + Flame.flameoff;
      return;
    }
  }

  if (JsonToVar(request))
  {
    Serial.println("Successful");
    serialEsp.print(request + "\r");
  }
  else
  {
    Serial.println("Failed");
  }
}

void updateDisplay()
{ //-------------------------------------------------------
  String txt = "";
  displayChange = false;
  displayTimer = millis();

  if (displayColorShift)
    lcdColor(Background.huevalue, 2);

  if (menuItem[0] != 0)
  {
    lcdCenter(menuItemPrint(0), 0, 1);
    lcdPrint(menuItemPrint(1), 1, 1);
    lcdPrint(menuItemPrint(2), 2, 1);
    lcdPrint(menuItemPrint(3), 3, 1);
  }
  else
  {
    lcdClear(2);
    lcdCenter("AntiDysentery 2018", 0, 1);
    switch (displayMode)
    {
    case 0:
      lcdPrint("Led Mode:", 1, 1);
      lcdCenter(String(Led.mode), 2, 1);
    case 1:
      lcdCenter("JaNkyTown", 1, 1);
      lcdCenter("6:15 + Gort", 2, 1);
      break;
    case 2:
      lcdPrint("Speed: ", 1, 1);
      lcdCenter(String(WGps.speed), 2, 1);
      break;
    case 3:
      lcdPrint("Latitude: ", 1, 1);
      lcdCenter(String(WGps.lat, 6), 2, 1);
      break;
    case 4:
      lcdPrint("Longitute: ", 1, 1);
      lcdCenter(String(WGps.lon, 6), 2, 1);
      break;
    case 5:
      EspOk ? txt = "ESP - " : txt = "esp - ";
      LedOk ? txt += "LED - " : txt += "led - ";
      FlameOk ? txt += "XBEE" : txt += "xbee";
      lcdCenter(txt, 3, 1);
      break;
    case 6:
      lcdPrint("Tracking:", 1, 1);
      txt = WGps.sat;
      txt = txt + " Satellites";
      lcdCenter(txt, 2, 1);
      break;

    case 7:
      lcdCenter("Fuck Your Burn", 1, 1);
      break;
    case 8:
      lcdCenter("No Really...", 1, 1);
      break;
    case 9:
      lcdCenter("Fuck Your Burn", 2, 1);
      break;
    default:
      displayMode = 0;
      displayChange = true;
      break;
    }
    //      case 5:
    //        lcdPrint("Main Voltage: ", 1, 1);
    //        lcdCenter(String(Wagon.mainVoltage, 2), 2, 1);
    //        break;
    //      case 6:
    //        lcdPrint("Aux Voltage: ", 1, 1);
    //        lcdCenter(String(Wagon.auxVoltage, 2), 2, 1);
    //        break;
    //
    //      case 7:
    //          lcdCenter("Auto Start", 1, 1);
    //          lcdCenter(wagonAutoStart ? "Active" : "Disabled", 2, 1);
    //          break;

    if (WGps.locstring == "")
      WGps.locstring = "Earth";
    lcdCenter(WGps.locstring, 3, 1);

    if (Wagon.pwrFlame)
    {
      lcdCenter("Fire Poofer Armed", 0, 0);
      lcdColor(0, 0);

      txt = "On Timing:  ";
      txt += Flame.flameon;
      txt += "ms";
      lcdPrint(txt, 1, 0);

      txt = "Off Timing: ";
      txt += Flame.flameoff;
      txt += "ms";
      lcdPrint(txt, 2, 0);

      txt = "Mode: ";
      txt += flameModeTxt;
      lcdPrint(txt, 3, 0);
    }
    else
    {

      lcdCenter("Flame Poofer", 0, 0);
      lcdCenter("Disarmed", 1, 0);
      lcdCenter(WGps.locstring, 3, 0);
    }
  }

  updateLcd(2);
}

void setRelays()
{ //-------------------------------------------------------
  Serial.println("Relay Change");
  relayChange = false;
  if (Wagon.pwrLed1)
  {
    digitalWrite(relayBank1[0], LOW);
    digitalWrite(relayBank1[1], LOW);
    sr.set(LED1_LED, 1);
  }
  else
  {
    digitalWrite(relayBank1[0], HIGH);
    digitalWrite(relayBank1[1], HIGH);
    sr.set(LED1_LED, 0);
  }

  if (Wagon.pwrLed2)
  {
    digitalWrite(relayBank1[2], LOW);
    sr.set(LED2_LED, 1);
  }
  else
  {
    digitalWrite(relayBank1[2], HIGH);
    sr.set(LED2_LED, 0);
  }

  if (Wagon.pwrFlame)
  {
    digitalWrite(relayBank1[3], LOW);
    sr.set(FLAME_LED, 1);
  }
  else
  {
    digitalWrite(relayBank1[3], HIGH);
    sr.set(FLAME_LED, 0);
  }

  if (Wagon.pwrSound)
  {
    digitalWrite(relayBank1[4], LOW);
    sr.set(SOUND_LED, HIGH);
  }
  else
  {
    digitalWrite(relayBank1[4], HIGH);
    sr.set(SOUND_LED, LOW);
  }
  if (Wagon.pwrHeadlight)
  {
    digitalWrite(relayBank1[5], LOW);
    sr.set(HLIGHT_LED, 1);
  }
  else
  {
    digitalWrite(relayBank1[5], HIGH);
    sr.set(HLIGHT_LED, 0);
  }
}

void checkSwitches()
{ //-------------------------------------------------------
  int x = 0;

  if ((millis() - threeHundredMillis) > 300)
  { //Run timer every 300 millis
    threeHundredMillis = millis();

    checkEncoderPosition(); //poll encoder

    x = analogRead(SPEED_PIN); //check Led Speed Delay Pot
    x = map(x, 0, 1028, 0, 255);
    if ((x > oldSpeed + 2) || (x < oldSpeed - 2))
    {
      oldSpeed = x;
      String txt;
      txt = "L,";
      txt = txt + x;
      txt = txt + ",rngSpeed";
      serialEspFinal = txt;
      handleRequests();
    }
  }

  x = analogRead(FLAME_OFF_PIN); //check Flame Off Pot
  x = map(x, 0, 1028, 0, 255);
  if ((x > oldFlameOff + 2) || (x < oldFlameOff - 2))
  {
    oldFlameOff = x;
    Flame.flameoff = x;
  }

  if ((millis() - lastRotaryPress) > 500)
  { // wait x milliseconds between rotary presses
    if (digitalRead(ROTARY_PIN) == LOW)
    {
      rotaryPress();
    }
  }

  for (int x = 0; x < 16; x++)
  { //debounce first 11 pins
    if (digitalRead(digitalInputPin[x]) == HIGH)
    { // if pushed
      if (debouncePin(digitalInputPin[x]))
      {
        switch (x)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
          fireFlame[x] = true;
          flameCount = 0;
          break;
        case 5: //Flame Mode
          flameMode++;
          switch (flameMode)
          {
          case 1:
            flameModeTxt = "Front To Back";
            break;
          case 2:
            flameModeTxt = "Back to Front";
            break;
          case 3:
            flameModeTxt = "Knight Rider";
            break;
          case 4:
            flameModeTxt = "All On";
            break;
          default:
            flameModeTxt = "Random";
            flameMode = 0;
          }
          break;
        }
        if (millis() < (lastDigitalButton + 500))
          return;
        switch (x)
        {
        case 6: //Fire Mode
          switch (flameMode)
          {
          case 0:
            flameCount = 5;
            break;
          case 1:
            flameCount = 5;
            break;
          case 2:
            flameCount = 5;
            break;
          case 3:
            flameCount = 9;
            break;
          case 4:
            flameCount = 1;
            break;
          }
          break;
        case 7: //Color
          serialLed.print("L,btnBackground\r");
          break;
        case 8: //Chase
          serialLed.print("L,btnChase\r");
          break;
        case 9: //Sparkle
          serialLed.print("L,btnSparkle\r");
          break;
        case 10: //Noise
          serialLed.print("L,btnNoise\r");
          break;
        case 11:
          relayChange = true;
          Wagon.pwrLed1 = !Wagon.pwrLed1;
          led1ButtonLS = Wagon.pwrLed1;
          break;
        case 12:
          relayChange = true;
          Wagon.pwrLed2 = !Wagon.pwrLed2;
          led2ButtonLS = Wagon.pwrLed2;
          break;
        case 13:
          relayChange = true;
          Wagon.pwrFlame = !Wagon.pwrFlame;
          flameButtonLS = Wagon.pwrFlame;
          break;
        case 14:
          relayChange = true;
          Wagon.pwrSound = !Wagon.pwrSound;
          soundButtonLS = Wagon.pwrSound;
          break;
        case 15:
          relayChange = true;
          Wagon.pwrHeadlight = !Wagon.pwrHeadlight;
          hlightButtonLS = Wagon.pwrHeadlight;
          break;
        }
        lastDigitalButton = millis();

      } //end debounce verify
    }   //end if pushed
  }     //end 1- 16 button check
} //end function

void checkEncoderPosition()
{ //-------------------------------------------------------
  int x = encoder.getPosition();
  if (x != encoderPos)
  {
    int y;
    String txt;
    encoder.setPosition(0);
    encoderPos = x;
    displayChange = true;

    if (menuItem[0] != 0)
    {
      if (menuItem[1] == 0)
      {
        menuItem[0] = menuItem[0] + encoderPos;
        if (menuItem[0] > Main_Menu_Max_Root)
          menuItem[0] = Main_Menu_Max_Root;
        if (menuItem[0] < 1)
          menuItem[0] = 1;
      }
      else if (menuItem[2] == 0)
      {
        menuItem[1] += encoderPos;
        if (menuItem[1] > Main_Menu_Max_Second)
          menuItem[1] = Main_Menu_Max_Second;
        if (menuItem[1] < 1)
          menuItem[1] = 1;
      }
      else if (menuItem[3] > 1)
      {
        menuItem[3] = 1;
      }
    }

    if (menuItem[2] == 1)
    {
      switch (menuItem[0])
      {
      case 1:
        switch (menuItem[1])
        {
        case 1:
          txt = "L,btnSparkle\r";
          break;
        case 2:
          txt = "L,";
          txt += (Sparkle.value + encoderPos);
          txt += ",rngSparkleValue\r";
          break;
        case 3:
          txt = "L,";
          txt += (Sparkle.huevalue + encoderPos);
          txt += ",rngSparkleHue\r";
          break;
        case 4:
          txt = "L,btnSparkleColor\r";
          break;
        }
        serialLed.print(txt);
        break;

      case 2:
        switch (menuItem[1])
        {
        case 1:
          txt = "L,btnChase\r";
          break;
        case 2: //distance
          txt = "L,";
          txt += Chase.distance + encoderPos;
          txt += ",rngChaseDistance\r";
          break;
        case 3: //hue
          txt = "L,";
          txt += Chase.huevalue + encoderPos;
          txt += ",rngChaseHue\r";
          break;
        case 4:
          txt = "L,btnChaseColor\r";
          break;
        }
        serialLed.print(txt);
        break;

      case 3:
        switch (menuItem[1])
        {
        case 1:
          txt = "L,btnBackground\r";
          break;
        case 2:
          txt = "L,";
          y = (Background.value + (encoderPos)) % 255;
          if (y < 0)
            y = 255;
          txt += y;
          txt += ",rngBackground\r";
          break;
        case 3:
          txt = "L,";
          y = (Background.huevalue + (encoderPos * 3)) % 255;
          if (y < 0)
            y = 255;
          txt += y;
          txt += ",rngBackgroundHue\r";
          break;
        case 4:
          txt = "L,btnBackgroundColor\r";
          break;
        }
        serialLed.print(txt);
        break;

      case 4:
        switch (menuItem[1])
        {
        case 1:
          WGps.homelat = WGps.lat;
          WGps.homelon = WGps.lon;
          break;
        case 2:
          wagonAutoStart = !wagonAutoStart;
          break;
        case 3:
          Wagon.startVoltage = Wagon.startVoltage + encoderPos;
          break;
        case 4:
          displayColorShift = !displayColorShift;
          break;
        }
        break;
      }
    }
    else
    { // Menu is off, Change preset display mode
      txt = "L,";
      txt += Led.mode + encoderPos;
      txt += ",rngMode\r";
      serialLed.print(txt);
      displayMode = 0;
    }
  }
}

String menuItemPrint(int row)
{ //-------------------------------------------------------
  String txt;

  if (menuItem[1] == 0)
  {
    if ((menuItem[0] > 3) & (row != 0))
      row = row + 3;
    switch (row)
    {
    case 0:
      txt = "--Main Menu--";
      return txt;
      break;
    case 1:
      txt = "Sparkle";
      break;
    case 2:
      txt = "Chase";
      break;
    case 3:
      txt = "Background";
      break;
    case 4:
      txt = "Wagon";
      break;
    }
    if (row != 0)
    {
      if (menuItem[0] == row)
      {
        txt = "> " + txt;
      }
      else
      {
        txt = "  " + txt;
      }
    }
    return txt;
  }

  if (menuItem[2] == 0)
  {
    if ((menuItem[1] > 3) & (row != 0))
      row = row + 3;
    switch (menuItem[0])
    {
    case 1: // //Sparkle menu
      switch (row)
      {
      case 0:
        txt = "--Sparkle Menu--";
        break;
      case 1:
        txt = "Sparkle: ";
        txt = Sparkle.on ? "On" : "Off";
        break;
      case 2:
        txt = "Intensity: ";
        txt += Sparkle.value;
        break;
      case 3:
        txt = "Hue Value: ";
        txt += Sparkle.huevalue;
        break;
      case 4:
        txt = "Mode: ";
        txt += Sparkle.colorname;
        break;
      }
      break;
    case 2:
      switch (row)
      {
      case 0:
        txt = "--Chase Menu--";
        break;
      case 1:
        txt = "Chase: ";
        txt += (Chase.on) ? "On" : "Off";
        break;
      case 2:
        txt = "Distance: ";
        txt += Chase.distance;
        break;
      case 3:
        txt = "Hue Value: ";
        txt += Chase.huevalue;
        break;
      case 4:
        txt = "Mode: ";
        txt += Chase.colorname;
        break;
      }
      break;
    case 3:
      switch (row)
      {
      case 0:
        txt = "--Background Menu--";
        return txt;
        break;
      case 1:
        txt = "Background: ";
        txt += Background.on ? "On" : "Off";
        break;
      case 2:
        txt = "Brightness: ";
        txt += Background.value;
        break;
      case 3:
        txt = "Hue Value: ";
        txt += Background.huevalue;
        break;
      case 4:
        txt = "Mode: ";
        txt += Background.colorname;
        break;
      }
      break;
    case 4:
      switch (row)
      {
      case 0:
        txt = "--Wagon Settings--";
        return txt;
        break;
      case 1:
        txt = "Set Gps Home";
        break;
      case 2:
        txt = "Auto Start: ";
        txt += wagonAutoStart ? "Active" : "Disabled";
        break;
      case 3:
        txt = "Auto Start Voltage";
        break;
      case 4:
        txt = "Color Shift: ";
        txt += displayColorShift ? "On" : "Off";
        break;
      }
      break;
    }
    if (row != 0)
    {
      if (menuItem[1] == row)
      {
        txt = "> " + txt;
      }
      else
      {
        txt = "  " + txt;
      }
    }
    return txt;
  }

  if (menuItem[3] == 0)
  {
    switch (menuItem[0])
    {
    case 1: // //Sparkle menu--)
      if (row == 0)
        txt = "--Sparkle Menu--";
      if (row != 3)
        break;
      switch (menuItem[1]) //Sparkle Submenu
      {
      case 1:
        txt = (Sparkle.on) ? "Sparkle: On" : "Sparkle: Off";
        break;
      case 2:
        txt = "Value: " + (String)Sparkle.value;
        break;
      case 3:
        txt = "Color of Hue: " + (String)Sparkle.huevalue;
        break;
      case 4:
        txt = "Color Mode: " + Sparkle.colormode;
        break;
      }
      break;

    case 2:
      if (row == 0)
        txt = "--Chase Menu--";
      if (row != 3)
        break;
      switch (menuItem[1]) // Submenu
      {
      case 1:
        txt = (Chase.on) ? "Chase: On" : "Chase: Off";
        break;
      case 2:
        txt = "Distance: " + (String)Chase.distance;
        break;
      case 3:
        txt = "Color of Hue: " + (String)Chase.huevalue;
        break;
      case 4:
        txt = "Mode: " + Chase.colormode;
        break;
      }
      break;

    case 3:
      if (row == 0)
        txt = "--Background Menu--";
      if (row != 3)
        break;
      switch (menuItem[1]) // Submenu
      {
      case 1:
        txt = (Background.on) ? "Background: On" : "Background: Off";
        break;
      case 2:
        txt = "Brightness: " + (String)Background.value;
        break;
      case 3:
        txt = "Color of Hue: " + (String)Background.huevalue;
        break;
      case 4:
        txt = "Mode: ";
        txt += Background.colorname;
        break;
      }
      break;

    case 4:
      if (row == 0)
        txt = "--Wagon Settings--";
      if (row != 3)
        break;
      switch (menuItem[1]) // Submenu
      {
      case 1:
        txt = "Set GPS Home: ";
        txt = txt + WGps.lat;
        txt = txt + " - ";
        txt = txt + WGps.lon;
        break;
      case 2:
        txt = (wagonAutoStart) ? "Auto Start Engine: On" : "Auto Start Engine: Off";
        break;
      case 3:
        txt = "Auto Start Voltage: " + (String)Wagon.startVoltage;
        break;
      case 4:
        txt = (displayColorShift) ? "Color Shift: On" : "Shift: Off";
        break;
      }
      break;
    }
  }
  return txt;
}

bool debouncePin(int pin)
{ //-------------------------------------------------------
  int count = 0;
  bool thebool = false;
  unsigned long val = 0;
  val = millis();

  while (millis() < (val + 30))
  {
    if (digitalRead(pin) == HIGH)
      count++;
  }
  if (count > 500)
    thebool = true;
  if (count == 0)
    delay(10);
  return thebool;
}

void rotaryPress()
{ //-------------------------------------------------------
  displayChange = true;
  Serial.println("click");
  lastEncoder = millis();
  lastRotaryPress = millis();

  if (menuItem[0] == 0)
  {
    menuItem[0] = 1;
    return;
  }
  if (menuItem[1] == 0)
  {
    menuItem[1] = 1;
    return;
  }
  if (menuItem[2] == 0)
  {
    menuItem[2] = 1;
    return;
  }
  menuItem[2] = 0;
}

void readTheVoltage()
{ //-------------------------------------------------------
  readVoltage = false;
  voltageTimer = millis();
  float volt0;
  float volt1;
  volt0 = analogRead(A0);
  volt1 = analogRead(A1);
  volt0 = map(volt0, 0, 1028, 0, 1600);
  volt1 = map(volt1, 0, 1028, 0, 1600);
  volt0 = (volt0 / 100);
  volt1 = (volt1 / 100);
  Wagon.mainVoltage = (volt0);
  Wagon.auxVoltage = (volt1);
  if (wagonAutoStart & (Wagon.mainVoltage < Wagon.startVoltage))
    WagonStart();
}

void welcomeScreen()
{ //-------------------------------------------------------
  btnCircle(30);
  lcdCenter("AntiDysentery", 0, 2);
  updateLcd(2);
  for (int x = 0; x < 255; x++)
  {
    lcdColor(x, 2);
    delay(4);
  }
  btnFTB(50);
  lcdCenter("JankyTown", 1, 2);
  updateLcd(2);
  for (int x = 0; x < 255; x++)
  {
    lcdColor(x, 2);
    delay(3);
  }
  btnRandom(40);
  lcdCenter("Burning Man", 2, 2);
  updateLcd(2);
  for (int x = 0; x < 255; x++)
  {
    lcdColor(x, 2);
    delay(2);
  }
  btnCenterOut(20);
  lcdCenter("2018", 3, 2);
  updateLcd(2);
  for (int y = 0; y < 10; y++)
  {
    for (int x = 0; x < 255; x++)
    {
      lcdColor(x, 2);
      delay(1);
    }
  }
  for (int x = 0; x < 3; x++)
  {
    btnCenterOut(40);
  }
  lcdClear(2);
}

int parseVar(String var, int loop)
{ //-------------------------------------------------------
  int ind1 = -1;
  String sretInt = "";
  int temp = 0;
  bool imax = false;
  for (int x = 0; x <= loop; x++)
  {
    if (imax)
      return 0;
    temp = var.indexOf(',', ind1 + 1);       //finds location of first ,
    sretInt = var.substring(ind1 + 1, temp); //captures data String
    ind1 = temp;
    if (temp == -1)
      imax = true;
  }
  return sretInt.toInt();
}

void lcdColor(int hue, int device)
{ //-------------------------------------------------------
  if (hue == oldLcdColor)
    return;
  oldLcdColor = hue;
  int colors[3];
  hue = map(hue, 0, 255, 0, 359); // hue is a number between 0 and 360

  H2R_HSBtoRGB(hue, 255, 255, colors);

  if (device == 1 || device == 2)
  {                                    //main
    analogWrite(5, (255 - colors[1])); // red value in index 0 of rgb_colors array
    analogWrite(6, (255 - colors[2])); // green value in index 1 of rgb_colors array
    analogWrite(7, (255 - colors[0])); // blue value in index 2 of rgb_colors array
  }
  if (device == 0 || device == 2)
  {                                     //flame
    analogWrite(8, (255 - colors[1]));  // red value in index 0 of rgb_colors array
    analogWrite(9, (255 - colors[2]));  // green value in index 1 of rgb_colors array
    analogWrite(10, (255 - colors[0])); // blue value in index 2 of rgb_colors array
  }
}

void H2R_HSBtoRGB(int hue, int sat, int bright, int *colors)
{

  // constrain all input variables to expected range
  hue = constrain(hue, 0, 360);
  sat = constrain(sat, 0, 100);
  bright = constrain(bright, 0, 100);

  // define maximum value for RGB array elements
  float max_rgb_val = 255.0;

  // convert saturation and brightness value to decimals and init r, g, b variables
  float sat_f = float(sat) / 100.0;
  float bright_f = float(bright) / 100.0;
  int r, g, b;

  // If brightness is 0 then color is black (achromatic)
  // therefore, R, G and B values will all equal to 0
  if (bright <= 0)
  {
    colors[0] = 0;
    colors[1] = 0;
    colors[2] = 0;
  }

  // If saturation is 0 then color is gray (achromatic)
  // therefore, R, G and B values will all equal the current brightness
  if (sat <= 0)
  {
    colors[0] = bright_f * max_rgb_val;
    colors[1] = bright_f * max_rgb_val;
    colors[2] = bright_f * max_rgb_val;
  }

  // if saturation and brightness are greater than 0 then calculate
  // R, G and B values based on the current hue and brightness
  else
  {

    if (hue >= 0 && hue < 120)
    {
      float hue_primary = 1.0 - (float(hue) / 120.0);
      float hue_secondary = float(hue) / 120.0;
      float sat_primary = (1.0 - hue_primary) * (1.0 - sat_f);
      float sat_secondary = (1.0 - hue_secondary) * (1.0 - sat_f);
      float sat_tertiary = 1.0 - sat_f;
      r = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
      g = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
      b = (bright_f * max_rgb_val) * sat_tertiary;
    }

    else if (hue >= 120 && hue < 240)
    {
      float hue_primary = 1.0 - ((float(hue) - 120.0) / 120.0);
      float hue_secondary = (float(hue) - 120.0) / 120.0;
      float sat_primary = (1.0 - hue_primary) * (1.0 - sat_f);
      float sat_secondary = (1.0 - hue_secondary) * (1.0 - sat_f);
      float sat_tertiary = 1.0 - sat_f;
      r = (bright_f * max_rgb_val) * sat_tertiary;
      g = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
      b = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
    }

    else if (hue >= 240 && hue <= 360)
    {
      float hue_primary = 1.0 - ((float(hue) - 240.0) / 120.0);
      float hue_secondary = (float(hue) - 240.0) / 120.0;
      float sat_primary = (1.0 - hue_primary) * (1.0 - sat_f);
      float sat_secondary = (1.0 - hue_secondary) * (1.0 - sat_f);
      float sat_tertiary = 1.0 - sat_f;
      r = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
      g = (bright_f * max_rgb_val) * sat_tertiary;
      b = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
    }

    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
  }
}

void lcdPrint(String txt, int row, int device)
{ //-------------------------------------------------------
  txt.remove(20);
  int add = (20 - txt.length());
  for (int x = 0; x < add; x++)
  {
    txt += " ";
  }
  if ((device == 0) || (device == 2))
    lcdFlame[row] = txt;
  if ((device == 1) || (device == 2))
    lcdMain[row] = txt;
}

void lcdCenter(String txt, int row, int device)
{ //-------------------------------------------------------
  if (txt.length() > 20)
    txt = txt.substring(0, 20);
  int col = ((20 - txt.length()) / 2);
  String temp;
  temp = LCDSPACE.substring(0, col);
  txt = temp + txt + temp;
  if (txt.length() == 19)
    txt = txt + " ";
  if (device == 0 || device == 2)
    lcdFlame[row] = txt;
  if (device == 1 || device == 2)
    lcdMain[row] = txt;
}

void updateLcd(int device)
{ //-------------------------------------------------------
  if ((device == 0) || (device == 2))
  {
    if (lcdFlame[0].length() == 0)
      lcdFlame[0] = "                    ";
    if (lcdFlame[1].length() == 0)
      lcdFlame[1] = "                    ";
    if (lcdFlame[2].length() == 0)
      lcdFlame[2] = "                    ";
    if (lcdFlame[3].length() == 0)
      lcdFlame[3] = "                    ";
    lcdDeviceFlame.setCursor(0, 0);
    lcdDeviceFlame.print(lcdFlame[0]);
    lcdDeviceFlame.setCursor(0, 1);
    lcdDeviceFlame.print(lcdFlame[1]);
    lcdDeviceFlame.setCursor(0, 2);
    lcdDeviceFlame.print(lcdFlame[2]);
    lcdDeviceFlame.setCursor(0, 3);
    lcdDeviceFlame.print(lcdFlame[3]);
  }
  if ((device == 1) || (device == 2))
  {
    if (lcdMain[0].length() == 0)
      lcdMain[0] = "                    ";
    if (lcdMain[1].length() == 0)
      lcdMain[1] = "                    ";
    if (lcdMain[2].length() == 0)
      lcdMain[2] = "                    ";
    if (lcdMain[3].length() == 0)
      lcdMain[3] = "                    ";
    lcdDeviceMain.setCursor(0, 0);
    lcdDeviceMain.print(lcdMain[0]);
    lcdDeviceMain.setCursor(0, 1);
    lcdDeviceMain.print(lcdMain[1]);
    lcdDeviceMain.setCursor(0, 2);
    lcdDeviceMain.print(lcdMain[2]);
    lcdDeviceMain.setCursor(0, 3);
    lcdDeviceMain.print(lcdMain[3]);
  }
}

void lcdClear(int device)
{

  if ((device == 0) || (device == 2))
  {
    lcdFlame[0] = "";
    lcdFlame[1] = "";
    lcdFlame[2] = "";
    lcdFlame[3] = "";
  }
  if ((device == 1) || (device == 2))
  {
    lcdMain[0] = "";
    lcdMain[1] = "";
    lcdMain[2] = "";
    lcdMain[3] = "";
  }
}

void getRGB(double h, double s, double v, byte rgb[])
{
  double r = 0.00;
  double g = 0.00;
  double b = 0.00;

  int i = int(h * 6);
  double f = h * 6 - i;
  double p = v * (1 - s);
  double q = v * (1 - f * s);
  double t = v * (1 - (1 - f) * s);

  switch (i % 6)
  {
  case 0:
    r = v, g = t, b = p;
    break;
  case 1:
    r = q, g = v, b = p;
    break;
  case 2:
    r = p, g = v, b = t;
    break;
  case 3:
    r = p, g = q, b = v;
    break;
  case 4:
    r = t, g = p, b = v;
    break;
  case 5:
    r = v, g = p, b = q;
    break;
  }

  rgb[0] = r * 255;
  rgb[1] = g * 255;
  rgb[2] = b * 255;
  Serial.println(rgb[0]);
  Serial.println(rgb[1]);
  Serial.println(rgb[2]);
  Serial.println("");
}

void hsvToRgb(double h, double s, double v, byte rgb[])
{
  double r, g, b;

  int i = int(h * 6);
  double f = h * 6 - i;
  double p = v * (1 - s);
  double q = v * (1 - f * s);
  double t = v * (1 - (1 - f) * s);

  switch (i % 6)
  {
  case 0:
    r = v, g = t, b = p;
    break;
  case 1:
    r = q, g = v, b = p;
    break;
  case 2:
    r = p, g = v, b = t;
    break;
  case 3:
    r = p, g = q, b = v;
    break;
  case 4:
    r = t, g = p, b = v;
    break;
  case 5:
    r = v, g = p, b = q;
    break;
  }

  rgb[0] = r * 255;
  rgb[1] = g * 255;
  rgb[2] = b * 255;
}

void rotaryCheck()
{ //-------------------------------------------------------
  encoder.tick();
}

bool checkSerial()
{ //-------------------------------------------------------
  bool flag = false;

  if (serialLed.available())
  {
    char inLed = serialLed.read();
    if (inLed == 13)
    {
      serialLedFinal = serialLedComm;
      serialLedComm = "";
      flag = true;
    }
    else
    {
      serialLedComm += inLed;
    }
  }

  if (serialEsp.available())
  {
    char inEsp = serialEsp.read();
    if (inEsp == 13)
    {
      serialEspFinal = serialEspComm;
      serialEspComm = "";
      flag = true;
    }
    else
    {
      serialEspComm += inEsp;
    }
  }

  if (serialXbee.available())
  {
    char inFlame = serialXbee.read();
    if (inFlame == 13)
    {
      serialXbeeFinal = serialXbeeComm;
      serialXbeeComm = "";
      flag = true;
    }
    else
    {
      serialXbeeComm += inFlame;
    }
  }

  return flag;
}

void handleRequests()
{ //-------------------------------------------------------

  if (serialLedFinal != "")
  {
    lastLed = millis();
    LedOk = true;
    Serial.print("Incoming Led: ");
    Serial.println(serialLedFinal);

    if (serialLedFinal == "Hello Mega")
    {
      serialEsp.print("Hello Led\r");
      serialLedFinal = "";
      return;
    }

    if (JsonToVar(serialLedFinal))
    {
      Serial.println("Successful");
      serialEsp.print(serialLedFinal + "\r");
      updateButtonLeds();
    }
    else
    {
      Serial.println("Led Fail");
    }
    serialLedFinal = "";
  }

  if (serialXbeeFinal != "")
  {
    lastXbee = millis();
    FlameOk = true;
    lcdCenter(serialXbeeFinal, 3, 0);
    updateLcd(0);
    Serial.print("Incoming Flame: ");
    Serial.println(serialXbeeFinal);
    HandleFlameRequest(serialXbeeFinal);
    serialXbeeFinal = "";
  }

  if (serialEspFinal != "")
  {
    lastEsp = millis();
    EspOk = true;
    //lcdCenter(serialEspFinal, 3, 1);
    Serial.print("Incoming Esp: ");
    Serial.println(serialEspFinal);
    if (serialEspFinal.startsWith("L,"))
    {
      serialLed.print(serialEspFinal + "\r");
      serialEspFinal = "";
      return;
    }
    if (serialEspFinal.startsWith("F,"))
    {
      HandleFlameRequest(serialEspFinal);
      serialEspFinal = "";
      return;
    }
    if (serialEspFinal.startsWith("M,"))
    {
      handleMainPress(serialEspFinal);
      serialEspFinal = "";
      return;
    }

    if (JsonToVar(serialEspFinal))
    {
      String txt;
      txt = serialEspFinal + "\r";
      serialLed.print(txt);
      Serial.println(serialEspFinal);
      Serial.println("Successful Json from Esp, sent to Led");
    }
    else
    {
      Serial.println("Esp Json Fail");
    }
  }
}

void btnRandom(int del)
{
  bool btn[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  bool flag = false;
  int rnd = 0;
  while (flag == false)
  {
    flag = true;
    rnd = random(16);
    sr.set(mapled[rnd], HIGH);
    delay(del);
    btn[rnd] = true;
    for (int x = 0; x < 16; x++)
    {
      if (btn[x] == false)
        flag = false;
    }
  }
  flag = false;
  while (flag == false)
  {
    flag = true;
    rnd = random(16);
    sr.set(mapled[rnd], LOW);
    delay(del);
    btn[rnd] = false;
    for (int x = 0; x < 16; x++)
    {
      if (btn[x] == true)
        flag = false;
    }
  }
}

void btnCircle(int del)
{
  sr.set(mapled[4], HIGH);
  delay(del);
  sr.set(mapled[4], LOW);
  sr.set(mapled[5], HIGH);
  delay(del);
  sr.set(mapled[5], LOW);
  sr.set(mapled[6], HIGH);
  delay(del);
  sr.set(mapled[6], LOW);
  sr.set(mapled[7], HIGH);
  delay(del);
  sr.set(mapled[7], LOW);
  sr.set(mapled[8], HIGH);
  delay(del);
  sr.set(mapled[8], LOW);
  sr.set(mapled[14], HIGH);
  delay(del);
  sr.set(mapled[14], LOW);
  sr.set(mapled[15], HIGH);
  delay(del);
  sr.set(mapled[15], LOW);
  sr.set(mapled[13], HIGH);
  delay(del);
  sr.set(mapled[13], LOW);
  sr.set(mapled[12], HIGH);
  delay(del);
  sr.set(mapled[12], LOW);
  sr.set(mapled[11], HIGH);
  delay(del);
  sr.set(mapled[11], LOW);
  sr.set(mapled[10], HIGH);
  delay(del);
  sr.set(mapled[10], LOW);
  sr.set(mapled[9], HIGH);
  delay(del);
  sr.set(mapled[9], LOW);
  sr.set(mapled[3], HIGH);
  delay(del);
  sr.set(mapled[3], LOW);
  sr.set(mapled[2], HIGH);
  delay(del);
  sr.set(mapled[2], LOW);
  sr.set(mapled[1], HIGH);
  delay(del);
  sr.set(mapled[1], LOW);
  sr.set(mapled[0], HIGH);
  delay(del);
  sr.set(mapled[0], LOW);
}

void btnCenterOut(int del)
{
  for (int x = 0; x < 9; x++)
  {
    switch (x)
    {
    case 0:
      sr.set(mapled[6], HIGH);
      break;
    case 1:
      sr.set(mapled[5], HIGH);
      sr.set(mapled[7], HIGH);
      break;
    case 2:
      sr.set(mapled[4], HIGH);
      sr.set(mapled[8], HIGH);
      break;
    case 3:
      sr.set(mapled[3], HIGH);
      sr.set(mapled[9], HIGH);
      break;
    case 4:
      sr.set(mapled[2], HIGH);
      sr.set(mapled[10], HIGH);
      break;
    case 5:
      sr.set(mapled[1], HIGH);
      sr.set(mapled[11], HIGH);
      break;
    case 6:
      sr.set(mapled[0], HIGH);
      sr.set(mapled[12], HIGH);
      sr.set(mapled[14], HIGH);
      break;
    case 7:
      sr.set(mapled[13], HIGH);
      sr.set(mapled[15], HIGH);
      break;
    }
    delay(del);
  }
  for (int x = 0; x < 8; x++)
  {
    switch (x)
    {
    case 0:
      sr.set(mapled[6], LOW);
      break;
    case 1:
      sr.set(mapled[5], LOW);
      sr.set(mapled[7], LOW);
      break;
    case 2:
      sr.set(mapled[4], LOW);
      sr.set(mapled[8], LOW);
      break;
    case 3:
      sr.set(mapled[3], LOW);
      sr.set(mapled[9], LOW);
      break;
    case 4:
      sr.set(mapled[2], LOW);
      sr.set(mapled[10], LOW);
      break;
    case 5:
      sr.set(mapled[1], LOW);
      sr.set(mapled[11], LOW);
      break;
    case 6:
      sr.set(mapled[0], LOW);
      sr.set(mapled[12], LOW);
      sr.set(mapled[14], LOW);
      break;
    case 7:
      sr.set(mapled[13], LOW);
      sr.set(mapled[15], LOW);
      break;
    }
    delay(del);
  }
}

void btnFTB(int del)
{
  for (int x = 0; x < 14; x++)
  {
    sr.set(mapled[x], HIGH);
    delay(del);
    sr.set(mapled[x], LOW);
  }
}
