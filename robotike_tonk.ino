#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <SRF05.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define Rin1 1 
#define Rin2 7
#define Lin1 0 
#define Lin2 10
#define Ren 2
#define Len 7
#define echo_pin 4
#define trig_pin 6

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


RF24 radio(9, 8); // CE, CSN
const byte address[6] = "tank1";
Servo myServo;

struct Controller {
  int potLVal;
  int potRVal;
  byte turretVal;
};

Controller recieve;


void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  myServo.attach(5);
  display.clearDisplay();
  display.display();
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  //Serial.peek();
  //Serial.begin(115200);
  if (radio.available()) {
    radio.read(&recieve, sizeof(Controller)); // Read the whole data and store it into the 'data' structure
    //Serial.println("Radio is available");
  }

  int potLval = map(recieve.potLVal,0,1023,0,255);
  int potRval = map(recieve.potRVal,0,1023,0,255);
  Serial.println(recieve.turretVal);
  myServo.write(recieve.turretVal);
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  //Left track control
  if(potLval < 110) {
    digitalWrite(Lin1, LOW);
    digitalWrite(Lin2, HIGH);
    analogWrite(Len, potLval);
  }
  if(potLval > 140) {
    digitalWrite(Lin1, HIGH);
    digitalWrite(Lin2, LOW);
    analogWrite(Len,potLval);
  }
  if(potLval < 140 && potLval > 110){
    digitalWrite(Len, LOW);
  }

  //Right track control
  if(potRval < 110) {
    digitalWrite(Rin1, LOW);
    digitalWrite(Rin2, HIGH);
    analogWrite(Ren ,potRval);
  }
  if(potRval > 140) {
    digitalWrite(Rin1, HIGH);
    digitalWrite(Rin2, LOW);
    analogWrite(Ren ,potRval);
  }
  if(potRval < 140 && potRval > 110){
    digitalWrite(Ren, LOW);
  }
   
  delay(15);
 const unsigned long duration= pulseIn(echo_pin, HIGH);
 int distance= duration/29/2;
  display.drawRect(84, 11, 24, 42, WHITE);
  display.drawCircle(96, 28, 11, WHITE);
  display.drawLine(96, 28, 96+map(recieve.turretVal-90, -90, 90, -28 ,28), map(abs(90-recieve.turretVal), 0, 90, 0 ,28), WHITE);
  display.setTextSize(1);             
  display.setTextColor(WHITE); 
  display.setCursor(0, 0);
  display.print("LTr: ");
  display.println(recieve.potLVal);
  display.setCursor(0, 10);
  display.print("RTr: ");
  display.println(recieve.potRVal);
  display.setCursor(0, 20);
  display.print("Trt: ");
  display.println(recieve.turretVal);
  display.setCursor(0, 30);
  display.print("Rng: ");
  display.println(distance); display.print(" cm");
  display.display();
  display.clearDisplay();
  //delay(100);
}
