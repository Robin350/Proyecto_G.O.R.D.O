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
#include <Timer.h>

#define RIGHT 12
#define LEFT 10
#define BRAKE_LIGHT 11
#define FRONT_LIGHT 13
#define DATA_R 4

#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3

#define NULL_PIN 8

RH_ASK driver(2000,DATA_R,2,10);

char blinker = '0';
bool lights = false;
bool brake = false; 
int LED;

volatile unsigned int count = 0;
bool state = false;

uint8_t buf[5];
uint8_t buflen = sizeof(buf);

bool fr,br,bl, thereIsMessage;

void setup()
{
  
    Serial.begin(9600); // Debugging only
    driver.init();
    pinMode(LEFT,OUTPUT);
    pinMode(RIGHT,OUTPUT);
    pinMode(BRAKE_LIGHT,OUTPUT);
    pinMode(FRONT_LIGHT,OUTPUT);
    pinMode(NULL_PIN,OUTPUT);

    digitalWrite(LEFT,LOW);
    digitalWrite(RIGHT,LOW);
    digitalWrite(BRAKE_LIGHT,LOW);
    digitalWrite(FRONT_LIGHT,LOW);

    SREG = (SREG & 0b01111111); //Desabilitar interrupciones
    TIMSK2 = TIMSK2|0b00000001; //Habilita la interrupcion por desbordamiento
    TCCR2B = 0b00000111; //Configura preescala para que FT2 sea de 7812.5Hz
    SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones //Desabilitar interrupciones

    thereIsMessage= false;
    
    Serial.print("hola");
}


ISR(TIMER2_OVF_vect){

  if(driver.available()){
    Serial.println("Hay un santo mensaje");
    driver.recv(buf, &buflen);
    thereIsMessage = true;
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

void Blink(){
    
  if(bl){
    if(blinker == 'l'){
      digitalWrite(LEFT,HIGH);
      digitalWrite(RIGHT,LOW);
    }
    else if(blinker == 'r'){
      digitalWrite(RIGHT,HIGH);
      digitalWrite(LEFT,LOW);
    }
    else if(blinker == '0'){
      digitalWrite(LEFT,LOW);
      digitalWrite(RIGHT,LOW);
      blinker = 'x';
    }
    bl = false;
  }
}

void updateLights(){
  if(fr)
    Front();
  if(br)
    Brake();
  if(bl);
    Blink();
}

void newMessage(){
  int num = atoi((const char*) buf);
  switch(num){
     case RIGHT_BLINKER:          
      if(blinker == '0' || blinker == 'x'){
        blinker = 'r';
      }
      else{
        blinker = '0';
      }
      bl =true;
      break;

     case LEFT_BLINKER:
      if(blinker == '0' || blinker == 'x'){
        blinker = 'l';
      }
      else{
        blinker = '0';
      }
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
  }
}

void loop()
{
    fr=br=bl=false;
    if(thereIsMessage){
      cli();
      thereIsMessage = false;
      newMessage();
      sei();
    }

    updateLights();
    delay(50);

}
