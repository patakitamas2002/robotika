#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define button 4
#define Rin1 1 
#define Rin2 2
#define Lin1 3 
#define Lin2 5
#define Ren 6
#define Len 7



RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"tonk1", "tonk2"};
Servo myServo;
byte turret = 0;

struct Controller {
  byte potLVal = 0;
  byte potRVal= 0;
  byte turretVal = 0;
};
struct Tank {
  byte dist = 0;
  int turret = -90;
};

Controller recieve;
Tank trans;


void setup() {
  pinMode(button, INPUT);
  myServo.attach(5);
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00001
  radio.openReadingPipe(1, addresses[1]); // 00002
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  
  delay(15);
  radio.startListening();
  if (radio.available()) {
    radio.read(&recieve, sizeof(Controller)); // Read the whole data and store it into the 'data' structure
  }  

  
  myServo.write(recieve.turretVal);

  //Left track control
  if(recieve.potLVal < 110) {
    digitalWrite(Lin1, LOW);
    digitalWrite(Lin2, HIGH);
    analogWrite(Len, recieve.potLVal);
  }
  if(recieve.potLVal > 140) {
    digitalWrite(Lin1, HIGH);
    digitalWrite(Lin2, LOW);
    analogWrite(Len, recieve.potLVal);
  }
  if(recieve.potLVal < 140 && recieve.potLVal > 110){
    digitalWrite(Len, LOW);
  }

  //Right track control
  if(recieve.potRVal < 110) {
    digitalWrite(Rin1, LOW);
    digitalWrite(Rin2, HIGH);
    analogWrite(Ren ,recieve.potRVal);
  }
  if(recieve.potRVal > 140) {
    digitalWrite(Rin1, HIGH);
    digitalWrite(Rin2, LOW);
    analogWrite(Ren ,recieve.potRVal);
  }
  if(recieve.potRVal < 140 && recieve.potRVal > 110){
    digitalWrite(Ren, LOW);
  }
   
  delay(15);
  radio.stopListening();
  
  
}
