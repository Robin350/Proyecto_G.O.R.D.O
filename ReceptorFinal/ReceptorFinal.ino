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

#define RIGHT 12
#define LEFT 10
#define BRAKE_LIGHT 11
#define FRONT_LIGHT 13
#define DATA_R 4

#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3

RH_ASK driver(2000,DATA_R,2,10);

bool left = false;
bool right = false;
bool b_on = true;
bool lights = false;
bool brake = false;       

void setup()
{
  
    Serial.begin(19200); // Debugging only
    driver.init();
    pinMode(LEFT_BLINKER,OUTPUT);
    pinMode(RIGHT_BLINKER,OUTPUT);
    pinMode(BRAKE_LIGHT,OUTPUT);
    pinMode(FRONT_LIGHT,OUTPUT);

    digitalWrite(LEFT_BLINKER,LOW);
    digitalWrite(RIGHT_BLINKER,LOW);
    digitalWrite(BRAKE_LIGHT,LOW);
    digitalWrite(FRONT_LIGHT,LOW);
    
    Serial.print("hola");
}

void Blinkers(int LED){
    (b_on)?digitalWrite(LED,HIGH):digitalWrite(LED,LOW);
}

void Blink(){
  if(right){
    Blinkers(RIGHT);
    if(left){
      digitalWrite(LEFT,LOW);
      left = false;
    }
  }
  else if(left){
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
    else{
      digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void Front(){
    if(lights){
      digitalWrite(FRONT_LIGHT,HIGH);
    }
    else{
      digitalWrite(FRONT_LIGHT,LOW);
      if(!brake)
        digitalWrite(BRAKE_LIGHT,LOW);
    }
}

void updateLights(bool fr, bool br){
  if(fr)
    Front();
  if(br)
    Brake();
}

void loop()
{
    uint8_t buf[5];
    uint8_t buflen = sizeof(buf);
    bool fr,br;
    fr=br=false;
    
    if(driver.available()){
    
        driver.recv(buf, &buflen); // Non-blocking
        int num = atoi((const char*) buf);
        switch(num){
           case RIGHT_BLINKER:
            
            right = !right;
            if(right){
              Serial.print("Encendemos derecha\n");
              digitalWrite(RIGHT,HIGH);
            }
            else{
              Serial.print("Apagamos derecha\n");
              digitalWrite(RIGHT, LOW);
            }
            break;

           case LEFT_BLINKER:
            left = !left;
            
            if(left){
              digitalWrite(LEFT,HIGH);
            }
            else{
              digitalWrite(LEFT, LOW);
            }
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

    updateLights(fr,br);
}
