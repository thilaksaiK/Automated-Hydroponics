/* 
  Function : Automatic Plant Watering for Hydroponics
  Author : K S Sai Thilak
  Email : tilaksai007@gmail.com
  Date : 28/12/2020
*/

//Including Libraries
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <RTClib.h>
#include <avr/wdt.h>

//Uncomment the below line for Serial debugging
//#define Serial_Debug

//Uncomment the below line to enable watch-dog timer
#define Watchdog_reset

//Defining the RTC clock
RTC_DS1307 RTC;

//Defining The I2C LiquidCrystal Display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Defining Pins
#define AnalogOut 5
#define BuzzerPin 6
#define Lowpin 7
#define PlantMotorPin 11
#define BucketMotorPin 12
#define TestPin 13

//Input and Output Level Arrays
const uint8_t Outpins[3] = {8, 9, 10};
const uint8_t Inpins[3] = {A1, A2, A3};

//Defining Parameters
#define SensorThreshold 700
#define BucketMotorFrequency 15
#define BucketMotorDuration 5
#define TestFrequency 10

//Global Variables
bool PlantMotor;
bool BucketMotor;
bool levelarray[3];
byte Level;
uint8_t Fill[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
byte Minutes;
byte DisplayState;
byte TestFreq;
bool timeUpdate_flag;
bool readStatus_flag;
bool DisplayScreen_flag;

void readStatus();
void startupLCD();
void PinDeclarations();
void timeUpdate();
void DisplayScreen();
void SetupTimer();

//TimerONE Interrupt for every second
ISR(TIMER1_OVF_vect)
{
  TCNT1 = 3036;
  //Updates the time every second
  timeUpdate_flag = 1;
  //Reads the status for the specified test Frequncy

  if (TestFreq++ >= TestFrequency)
  {
    readStatus_flag = 1;
    TestFreq = 1;
  }
  //Updates the display for every two seconds
  if (TestFreq % 2 == 0)
  {
    DisplayScreen_flag = 1;
  }
}

void setup()
{
#ifdef Serial_Debug
  Serial.begin(9600);
#endif
  //Setting Time - Uncomment the below line to set Time.
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  PinDeclarations();
  RTC.begin();
  Wire.begin();
  startupLCD();
  delay(2000);
  lcd.clear();
#ifdef Watchdog_reset
  wdt_enable(WDTO_4S);
#endif
  SetupTimer();
}

void loop()
{
  if (timeUpdate_flag)
  {
    timeUpdate_flag = 0;
    timeUpdate();
  }
  if (readStatus_flag)
  {
    readStatus_flag = 0;
    readStatus();
  }
  if (DisplayScreen_flag)
  {
    DisplayScreen_flag = 0;
    DisplayScreen();
  }
#ifdef Watchdog_reset
  wdt_reset();
#endif
}

//Defining Functions
void startupLCD()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, Fill);
  lcd.home();
  lcd.clear();
#ifdef Serial_Debug
  Serial.print("Automatic Plant Watering ...");
#endif
  lcd.print("Automatic Plant");
  lcd.setCursor(0, 1);
  lcd.print("Watering");
}

// Declaring the Input Output Pins
void PinDeclarations()
{
#ifdef Serial_Debug
  Serial.print("Declaraing pins.....");
#endif
  for (int i = 0; i < 3; i++)
  {
    pinMode(Inpins[i], INPUT);
    pinMode(Outpins[i], OUTPUT);
  }
  pinMode(PlantMotorPin, OUTPUT);
  pinMode(BucketMotorPin, OUTPUT);
  pinMode(Lowpin, OUTPUT);
  pinMode(TestPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(AnalogOut, OUTPUT);
#ifdef Serial_Debug
  Serial.println("Done!");
#endif
  digitalWrite(Lowpin, HIGH);
  digitalWrite(PlantMotorPin, LOW);
  digitalWrite(BucketMotorPin, LOW);
  PlantMotor = 0;
}

// Initial Setup of TimerONE
void SetupTimer()
{
#ifdef Serial_Debug
  Serial.print("Setting up timer.....");
#endif
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 3036;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
#ifdef Serial_Debug
  Serial.println("Done!");
#endif
}

// Reads the level and time to toggle Output Motors
void readStatus()
{
#ifdef Serial_Debug
  Serial.println("Reading Status....");
#endif
  digitalWrite(TestPin, HIGH);
  digitalWrite(AnalogOut, HIGH);
  Level = 0;
  for (int i = 0; i < 3; ++i)
  {
    levelarray[i] = (analogRead(Inpins[i]) < SensorThreshold);
    if(levelarray[i])
      Level = i+1;
  }
  digitalWrite(AnalogOut, LOW);
  if (Level == 0 && BucketMotor == 0)
  {
    tone(BuzzerPin, 1000, 200);
    BucketMotor = 1;
  }
  if (Level == 3 && BucketMotor == 1)
  {
    tone(BuzzerPin, 1000, 400);
    BucketMotor = 0;
  }
  if (((Minutes) % BucketMotorFrequency < BucketMotorDuration) && PlantMotor == 0){
    tone(BuzzerPin, 2000, 200);
    PlantMotor = 1;
  }
   if (((Minutes) % BucketMotorFrequency >= BucketMotorDuration) && PlantMotor == 1){
     tone(BuzzerPin, 2000, 400);
     PlantMotor = 0;
   }
  digitalWrite(TestPin, LOW);
#ifdef Serial_Debug
  Serial.println(Minutes);
  for (int i = 0; i < 3; i++)
  {
    Serial.print(analogRead(Inpins[i]));
    Serial.print("\t");
    Serial.println(levelarray[i]);
  }
  Serial.println("Done!");
  Serial.print("Bucket Motor ");
  Serial.println(BucketMotor);
  Serial.print("Plant Motor ");
  Serial.println(PlantMotor);
  Serial.print("PlantMotor condition ");
  Serial.println((Minutes) % BucketMotorFrequency < BucketMotorDuration);
#endif
  digitalWrite(PlantMotorPin, PlantMotor);
  digitalWrite(BucketMotorPin, BucketMotor);
}

//Displays the status on the LCD Screem
void DisplayScreen()
{
#ifdef Serial_Debug
  Serial.println("Changing Display");
#endif
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  switch (DisplayState)
  {
  case 0:
    lcd.write(0);
    for (int l = 0; l < 3; l++)
    {
      lcd.setCursor(l + 1, 0);
      if (levelarray[l])
        lcd.write(0);
    }
    lcd.setCursor(7, 0);
    lcd.print("Level = ");
    lcd.print(Level);
    DisplayState++;
    break;
  case 1:
    lcd.print("Plant motor ");
    if (PlantMotor)
      lcd.print("ON");
    else
      lcd.print("OFF");
    DisplayState++;
    break;
  case 2:
    lcd.print("Bucket motor ");
    if (BucketMotor)
      lcd.print("ON");
    else
      lcd.print("OFF");
    DisplayState = 0;
    break;
  }
  for (int i = 0; i < 3; ++i)
    digitalWrite(Outpins[i], levelarray[i]);
#ifdef Serial_Debug
  Serial.println("Done!");
#endif
}

// Updates the time on the screen
void timeUpdate()
{
#ifdef Serial_Debug
  Serial.println("Updating time");
#endif
  DateTime now = RTC.now();
  lcd.setCursor(4, 1);
  if (now.hour() < 10)
    lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10)
    lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10)
    lcd.print("0");
  lcd.print(now.second());
  Minutes = now.minute();
#ifdef Serial_Debug
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.println("\t \t Done!");
#endif
}
