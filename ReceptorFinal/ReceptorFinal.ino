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

#define RIGHT 4
#define LEFT 3
#define BRAKE_LIGHT 2
#define FRONT_LIGHT 1
#define DATA_R 5

#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3

RH_ASK driver(2000,DATA_R,2,10);

bool left = false;
bool right = false;
bool b_on = false;
bool lights = false;
bool brake = false;

void setup()
{
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    pinMode(LED_BUILTIN,OUTPUT);
}

void Blinkers(int LED){
    (b_on)?digitalWrite(LED,HIGH):digitalWrite(LED,LOW);
}

void Blink(){
  if(right){
    Blinkers(RIGHT);
    if(left)
      digitalWrite(LEFT,LOW);
  }
  else if(left){
    Blinkers(LEFT);
    if(right)
      digitalWrite(RIGHT,LOW);
  }
  else{
    digitalWrite(RIGHT,LOW);
    digitalWrite(LEFT,LOW);
  }
    
}

void Brake(){
    if(brake){
      digitalWrite(BRAKE_LIGHT,HIGH);
    }
    else if(lights){
      analogWrite(BRAKE_LIGHT,50);
    }
    else{
      digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void Front(){
    if(lights){
      digitalWrite(FRONT_LIGHT,HIGH);
      analogWrite(BRAKE_LIGHT,50);
    }
    else{
      digitalWrite(FRONT_LIGHT,LOW);
      if(!brake)
        digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void updateLights(bool bl, bool fr, bool br){
  b_on=!b_on;
  if(bl)
    Blink();
  if(fr)
    Front();
  if(br)
    Brake();
}

void loop()
{
    uint8_t buf[5];
    uint8_t buflen = sizeof(buf);
    bool bl,fr,br;
    bl=fr=br=false;
    if(driver.available()){
        Serial.println("There is a message");
    
        driver.recv(buf, &buflen); // Non-blocking
        int i;
        Serial.print("Message: ");
        Serial.println((char*)buf);

        switch(buf[0]){
           case RIGHT_BLINKER:
            right = !right;
            bl = true;
            break;

           case LEFT_BLINKER:
            left = !left;
            bl = true;
            break;

           case LIGHTS:
            lights = !lights;
            fr = true;
            break;

           case BRAKE:
            brake = !brake;
            br = true;
            break;

           default:
            Serial.println("Vaya basura acabo de recibir");
            break;
        }
    }

    updateLights(bl,fr,br);
    delay(50);
}
