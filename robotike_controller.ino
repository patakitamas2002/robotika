#define potL 0
#define potR 1
#define btnL 3
#define btnR 2

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>


RF24 radio(9, 8); // CE, CSN
const byte address[6] = "tank1";
byte turretVal = 90;
int btnLState = 0;
int btnRState = 0;

struct Data {
  int potLVal = 0;
  int potRVal = 0;
  byte turretVal = 0;
};

Data trans;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(potL, INPUT);
  pinMode(potR, INPUT);
  pinMode(btnR, INPUT);
  pinMode(btnL, INPUT);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  btnLState = digitalRead(btnL);
  btnRState = digitalRead(btnR);
  if(btnLState == HIGH && turretVal > 0){
      turretVal-=3;
      Serial.println("Pressed L button");
      delay(100);
      //Serial.println(turretVal);
    }
  if(btnRState == HIGH && turretVal < 180){
      turretVal+=3;
      Serial.println("Pressed R button");
      delay(100);
    }
  trans.turretVal = turretVal;
  trans.potRVal = analogRead(potR);
  trans.potLVal = analogRead(potL);
  //Serial.println(trans.turretVal);
  radio.write(&trans, sizeof(Data));
  Serial.println(turretVal);  
  delay(100);
}
