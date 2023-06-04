#define potL 0
#define potR 1
#define btnL 3
#define btnR 2

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define led 12

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"tonk1", "tonk2"};
byte turretVal = 90;
int btnLState = 0;
int btnRState = 0;

struct Controller {
  byte potLVal = 0;
  byte potRVal = 0;
  byte turretVal = 0;
};
struct Tank {
  byte dist = 0;
  int turret = 0;
};

Controller trans;
Tank recieve;

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.display();
  pinMode(potL, INPUT);
  pinMode(potR, INPUT);
  pinMode(btnR, INPUT);
  pinMode(btnL, INPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
}



void loop()  {
  delay(50);

  radio.stopListening();

  btnLState = digitalRead(btnL);
  btnRState = digitalRead(btnR);
  
  //Turret control
  if(btnLState == HIGH && turretVal < 180){
      turretVal+=3;
    }
  if(btnRState == HIGH && turretVal > 0){
      turretVal-=3;
    }
   trans.turretVal = turretVal;
   //Track control
  trans.potLVal = map(analogRead(potL), 0, 1023, 0, 255);
  trans.potRVal = map(analogRead(potR), 0, 1023, 0, 255);
  Serial.println(trans.potLVal);
  Serial.println(trans.potRVal);
  Serial.println(turretVal);
  radio.write(&trans, sizeof(trans));
  
  delay(50);
  radio.startListening();
  if (radio.available()) {
    radio.read(&recieve, sizeof(Tank)); // Read the whole data and store it into the 'data' structure
  }  
  //Display for turret rotation  
  display.drawRect(84, 11, 24, 42, WHITE);
  display.drawCircle(96, 28, 11, WHITE);
  display.drawLine(96, 28, 96+map(turretVal-90, -90, 90, -28 ,28), map(abs(90-turretVal), 0, 90, 0 ,28), WHITE);
  display.setTextSize(1);             
  display.setTextColor(WHITE); 
  display.setCursor(0, 0);
  display.println(trans.potLVal);
  display.setCursor(0, 10);
  display.println(trans.potRVal);
  display.setCursor(0, 20);
  display.println(turretVal);
  display.setCursor(0, 30);
  display.println(recieve.dist);
  display.display();
  display.clearDisplay();
  
}
