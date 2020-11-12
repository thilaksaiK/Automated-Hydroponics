#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <RTClib.h>

RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x20,16,2);

#define BucketMotorPin 12
#define PlantMotorPin 11
#define MessagePin 13
#define BuzzerPin 6

int BucketMotorDuration = 5;
int BucketMotorFrequency = 15;

uint8_t Fill[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const int tones[4] = {1046, 1174, 1318, 1396};
const int InPins[3] = {0, 1, 2};
const int OutPins[3] = {8, 9, 10};
const int LowP = 7;
byte Level, PrevLevel = 0, DisplayState = 0;
int Minutes;
bool LevelArray[3];
bool BucketMotor, Active, Change, PlantMotor, PrevBucket, PrevPlant;

String PrintCharacter;
const int Threshold = 700;
unsigned long OnTime, PrevTime, PrevUpdate, PrevDisp;
void setup() {
  RTC.begin();
     if (!RTC.isrunning()){
        RTC.adjust(DateTime(__DATE__, __TIME__));
     }
  Wire.begin();
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, Fill);
  lcd.home();
  startup();
  for(int i=0;i<3;i++){
    pinMode(InPins[i], INPUT);
    pinMode(OutPins[i], OUTPUT);
  }
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
  pinMode(PlantMotorPin,OUTPUT);
  pinMode(BucketMotorPin,OUTPUT);
  pinMode(LowP,OUTPUT);
  pinMode(MessagePin,OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  digitalWrite(BucketMotorPin, LOW);
  digitalWrite(LowP, HIGH);
  ReadStatus();
  DisplayAll();
  PlantMotor = 0;
}
void loop(){
  if(Change){
  SendSMS();
  }
  if(millis() - PrevUpdate > 1000){
    PrevUpdate = millis();
    //timeUpdate();
  }
  if(millis() - PrevDisp > 1000){
  DisplayAll();
  PrevDisp = millis();
  }
  ReadStatus();
}
void startup(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Automatic Plant");
  lcd.setCursor(0, 1);
  lcd.print("Watering");
  for(int i = 0;i<=3;i++){
  lcd.print(".");
  delay(100);
  }
  noTone(BuzzerPin);
  lcd.clear();
}
void ReadStatus(){
  PrevLevel = Level;
  Change = 0;
  Level = 0;
  for(int i = 0; i<3; i++){
    //LevelArray[i] = (analogRead(InPins[i]) < Threshold);
    LevelArray[i] = digitalRead(InPins[i]);
    if(LevelArray[i])
      Level = i+1;
  }
  if(PrevLevel != Level)
  Change = 1;
  if(Level == 0){
  BucketMotor = 1;
  }
  if(Level == 3){
  BucketMotor = 0;
  }
  digitalWrite(PlantMotorPin, PlantMotor);
  digitalWrite(BucketMotorPin, BucketMotor);
   if((Minutes)%BucketMotorFrequency < BucketMotorDuration){
    PlantMotor = 1;
}
else
PlantMotor = 0;
Serial.print((Minutes));
Serial.print(" ");
Serial.println((Minutes)%BucketMotorFrequency);
if(PrevBucket != BucketMotor){
  tone(BuzzerPin, 2000);
  if(BucketMotor)
  delay(200);
  else
  delay(500);
  noTone(BuzzerPin);
  PrevBucket = BucketMotor;

}
if(PrevPlant != PlantMotor){
  tone(BuzzerPin, 1000);
  if(BucketMotor)
  delay(200);
  else
  delay(500);
  noTone(BuzzerPin);
  PrevPlant = PlantMotor;
}
}
void DisplayAll(){
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
  switch(DisplayState){
    case 0:
    lcd.write(0);
    for(int l = 0;l<3;l++){
    lcd.setCursor(l+1,0);
    if(LevelArray[l])
    lcd.write(0);
    }
    lcd.setCursor(7,0);
    lcd.print("Level = ");
    lcd.print(Level);
    DisplayState++;
    break;
    case 1:
//    lcd.print("Plant motor ");
//    if(PlantMotor)
//    lcd.print("ON");
//    else
//    lcd.print("OFF");
    DisplayState++;
    break;
    case 2:
    lcd.print("Bucket motor ");
    if(BucketMotor)
    lcd.print("ON");
    else
    lcd.print("OFF");
    DisplayState = 0;
    break;
  }
  for(int i = 0; i<3; i++)
    digitalWrite(OutPins[i], LevelArray[i]);
  PrevTime = millis();
}
void SendSMS(){
  digitalWrite(MessagePin, HIGH);
  lcd.clear(); 
  lcd.print("Sending SMS");
  delay(100);
  tone(BuzzerPin, 1046);
  delay(200);
  tone(BuzzerPin, 2093);
  delay(200);
  noTone(BuzzerPin);
  digitalWrite(MessagePin, LOW);
}
void timeUpdate(){
  DateTime now = RTC.now();
  lcd.setCursor(4,1);
  if(now.hour() < 10)
  lcd.print("0");  
  lcd.print(now.hour());
  lcd.print(":");
  if(now.minute() < 10)
  lcd.print("0");  
  lcd.print(now.minute());
  lcd.print(":");
  if(now.second() < 10)
  lcd.print("0");  
  lcd.print(now.second());
  Minutes = now.minute();
}
