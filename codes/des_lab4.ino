#include <SD.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <DallasTemperature.h>


Adafruit_BMP280 bmp;


int cs = 10; 
File dane;
char fileName[] = "dane.txt";

bool sdWorks = false;

//#define led0 1
#define pinA 2
#define pinB 3
#define pinC 4
#define pinD 5
#define pinE 6
#define pinF 7
#define pinG 8
#define pinDP 9
#define DS18B20_PIN A0
int digits[] = {23, 24, 25, 26};

int buttonPins[3] = {A1, A2, A3};

int buttonState[4];

OneWire oneWire(DS18B20_PIN);
DallasTemperature ds(&oneWire);
DeviceAddress tempDeviceAddress;
bool dsworks = false;

void setup() {
//  pinMode(led0, OUTPUT);

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinDP, OUTPUT);
  pinMode(digits[0], OUTPUT);
  pinMode(digits[1], OUTPUT);
  pinMode(digits[2], OUTPUT);
  pinMode(digits[3], OUTPUT);
  //pinMode(DS18B20_PIN, INPUT_PULLUP);

  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  bmp.begin(0x76);
  ds.begin();
  if(ds.getAddress(tempDeviceAddress, 0)){
    dsworks = true;
    ds.setResolution(tempDeviceAddress, 12);
    ds.requestTemperatures();
  }
  Serial.begin(9600);

  
  //digitalWrite(led0, LOW);
  
}

String msg = "0000";
int mode = 0;
float temp = 0;
float pressure = 0;
float tempwire = 0;

void loop() {
  for (int i = 0; i < 3; i++) {
    int state = digitalRead(buttonPins[i]);
    
    if (i == 0 && state == LOW && buttonState[i] != LOW) {
      initializeSD();
    }
    if (i == 1 && state == LOW && buttonState[i] != LOW) {
      writeToSD();
    }
    if (i == 2 && state == LOW && buttonState[i] != LOW) {
      if(mode == 2)
        mode = 0;
      else 
        mode++;
    }


    buttonState[i] = state;
  }

 

  readSensor(mode);

  if(mode == 0){
    msg = String(temp);
  }
  if(mode == 1){
    msg = String(pressure);
  }
  if(mode == 2){
    msg = String(tempwire);
  }

  draw7seg(msg);

}
unsigned long previousMillis = 0;

void readSensor(int choice){
    temp = bmp.readTemperature();
    pressure = bmp.readPressure()/100;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      if(dsworks){
        ds.requestTemperatures();
        float tempC = ds.getTempC(tempDeviceAddress);
        if (tempC == DEVICE_DISCONNECTED_C)
        {
          tempwire = 222;
        }
        else tempwire = tempC;
      } else{
        tempwire = 1234;
      }
        Serial.print(temp);
        Serial.print(",");
        Serial.print(pressure);
        Serial.print(",");
        Serial.println(tempwire);
    }
  
}

void initializeSD(){
  pinMode(cs, OUTPUT);
  pinMode(SS, OUTPUT);

  if (!SD.begin(cs)) {
//    Serial.println("SD did not initiliaze");
    sdWorks = false;
  } else{
//    Serial.println("SD initialized.");
    //digitalWrite(led0, HIGH);
    sdWorks = true;
  }
  
}

void writeToSD(){
  if (!sdWorks)
    return;

  disableDisplay();
  
  dane = SD.open(fileName, FILE_WRITE);
  if(mode == 0){
    dane.println("T = " + msg + " C"); 
  }
  if(mode == 1){
    dane.println("p = " + msg + " hPa");
  }
  if(mode == 2){
    dane.println("T sonda = " + msg + " C"); 
  }
  dane.flush();
  dane.close();
  
}

void disableDisplay() {
  for (int i = 0; i < 4; i++) digitalWrite(digits[i], HIGH);
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}


void draw7seg(String input){
  char ch[4];
  if (input.charAt(2) != '.') {
    ch[0] = input.charAt(3);
    ch[1] = input.charAt(2);
    ch[2] = input.charAt(1);
    ch[3] = input.charAt(0);
  } else {
    ch[0] = 'z';
    ch[1] = input.charAt(3);
    ch[2] = input.charAt(1);
    ch[3] = input.charAt(0);
  }

  for(int i = 0; i <= 3; i++){
    digitalWrite(digits[i], LOW);
    digitalWrite(pinDP, HIGH);
    if(i == 2 && input.charAt(2) == '.'){
      digitalWrite(pinDP, LOW);
    }
    
    switch(ch[i]){
      case '0':
        draw0();
        break;
      case '1':
        draw1();
        break;
      case '2':
        draw2();
        break;
      case '3':
        draw3();
        break;
      case '4':
        draw4();
        break;
      case '5':
        draw5();
        break;
      case '6':
        draw6();
        break;
      case '7':
        draw7();
        break;
      case '8':
        draw8();
        break;
      case '9':
        draw9();
        break;
      default:
        drawBlank();
        break;
    }
    delay(1);
    digitalWrite(digits[i], HIGH);
    delay(2);
  }
  
}

void drawBlank() {
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);

}

void draw0() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, HIGH);

}

void draw1() {
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);

}

void draw2() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, LOW);

}

void draw3() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, LOW);

}

void draw4() {
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);

}

void draw5() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);
}

void draw6() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);

}

void draw7() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);

}

void draw8() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);

}

void draw9() {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);

}
