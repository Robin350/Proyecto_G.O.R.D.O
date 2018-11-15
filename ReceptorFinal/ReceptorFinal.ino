/*
 * Coded by: 
 *  Robin Costas del Moral 
 *  Antonio Jaimez Jimenez
 *  
 *    TE < ETSIIT < UGR
 *    
 */
 
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#define RIGHT 10
#define LEFT 11
#define BRAKE_LIGHT 9
#define FRONT_LIGHT 8
#define DATA_R 4

#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3

RH_ASK driver(2000,DATA_R,2,10);

char sblink = '0';
bool left = false;
bool right = false;
bool b_on = false;
bool lights = false;
bool brake = false;
const long interval = 50;
unsigned long previousMillis = 0;        

void setup()
{
  
    //Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    pinMode(LEFT_BLINKER,OUTPUT);
    pinMode(RIGHT_BLINKER,OUTPUT);
    pinMode(BRAKE_LIGHT,OUTPUT);
    pinMode(FRONT_LIGHT,OUTPUT);

    digitalWrite(LEFT_BLINKER,LOW);
    digitalWrite(RIGHT_BLINKER,LOW);
    digitalWrite(BRAKE_LIGHT,LOW);
    digitalWrite(FRONT_LIGHT,LOW);
    
    Serial.print("hola");

    TIMSK2 = (TIMSK2 & B11111110) | 0x01;
    TCCR2B = (TCCR2B & B11111000) | 0x07;
}

void Blinkers(int LED){
    (b_on)?digitalWrite(LED,HIGH):digitalWrite(LED,LOW);
}

void Blink(){
  if(sblink == 'r'){
    Blinkers(RIGHT);
    if(left){
      digitalWrite(LEFT,LOW);
      left = false;
    }
  }
  else if(sblink == 'l'){
    Blinkers(LEFT);
    if(right){
      digitalWrite(RIGHT,LOW);
      right = false;
    }
  }
  else{
    right=left=false;
    digitalWrite(RIGHT,LOW);
    digitalWrite(LEFT,LOW);
  }
    
}

void Brake(){
    if(brake){
      digitalWrite(BRAKE_LIGHT,HIGH);
    }
    /*else if(lights){
      analogWrite(BRAKE_LIGHT,50);
    }*/
    else{
      digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void Front(){
    if(lights){
      digitalWrite(FRONT_LIGHT,HIGH);
      //analogWrite(BRAKE_LIGHT,50);
    }
    else{
      digitalWrite(FRONT_LIGHT,LOW);
      if(!brake)
        digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void updateLights(bool bl, bool fr, bool br){
  Blink();
  if(fr)
    Front();
  if(br)
    Brake();
}

ISR(TIMER2_OVF_vect){
   b_on=!b_on;
}

void loop()
{
    uint8_t buf[5];
    uint8_t buflen = sizeof(buf);
    bool bl,fr,br;
    unsigned long currentMillis = millis();
    bl=fr=br=false;
    
    if(driver.available()){
    
        driver.recv(buf, &buflen); // Non-blocking
        int num = atoi((const char*) buf);
        switch(num){
           case RIGHT_BLINKER:
            right = !right;
            if(right)
              sblink = 'r';
            else
              sblink = 'o';
            break;

           case LEFT_BLINKER:
            left = !left;
            if(left)
              sblink = 'l';
            else
              sblink = 'o';
            break;

           case LIGHTS:
            lights = !lights;
            fr = true;
            break;

           case BRAKE:
            brake = !brake;
            br = true;
            break;
        }
    }

    updateLights(bl,fr,br);
}
