#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN            6

#define NUMPIXELS      12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/** 자동차신호등에서 사용하는 LED 핀위치 */
#define pinCarG 12
#define pinCarY 11
#define pinCarR 10
/** 사람이 사용하는 LED 핀위치 */
#define pinHumanG 9
#define pinHumanR 8

int a = 0;
int count = -1;
int b = 0;
int i = 1;

#define car_g 20
#define car_o 3
#define human_g 15
#define human_blink 6

int ledState = 0;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval1 = 500;           // interval at which to blink (milliseconds)

// sets the digital pin 12 as output
const unsigned char wake[24] = {
  0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00
};//wake up NFC module
const unsigned char firmware[9] = {
  0x00, 0x00, 0xFF, 0x02, 0xFE, 0xD4, 0x02, 0x2A, 0x00
};//
const unsigned char tag[11] = {
  0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x01, 0x00, 0xE1, 0x00
};//detecting tag command
const unsigned char std_ACK[25] = {
  0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x0C, \
  0xF4, 0xD5, 0x4B, 0x01, 0x01, 0x00, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x00
};
unsigned char old_id[5];

unsigned char receive_ACK[25];//Command receiving buffer
//int inByte = 0;               //incoming serial byte buffer

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#define print1Byte(args) Serial1.write(args)
#define print1lnByte(args)  Serial1.write(args),Serial1.println()
#else
#include "WProgram.h"
#define print1Byte(args) Serial1.print(args,BYTE)
#define print1lnByte(args)  Serial1.println(args,BYTE)
#endif

void setup() {
  pinMode(pinCarG, OUTPUT);
  pinMode(pinCarY, OUTPUT);
  pinMode(pinCarR, OUTPUT);

  pinMode(pinHumanG, OUTPUT);
  pinMode(pinHumanR, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(40, OUTPUT);
  Serial.begin(9600);  // open serial with PC
  Serial1.begin(115200);  //open serial1 with device
  Serial3.begin(9600);
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  //Serial2.begin(115200);
  wake_card();
  delay(100);
  read_ACK(15);
  delay(100);
  display(15);
}

void loop() {
  digitalWrite(40, LOW);
  unsigned long currentMillis = millis();
  send_tag();
  read_ACK(25);
  delay(100);
  if (!cmp_id ()) {
    if (test_ACK ()) {
      display (25);
      a = 1;
      Serial3.write('B');
      digitalWrite(40, HIGH);
      delay (1);
    }
  }
  copy_id ();
  if (currentMillis - previousMillis > interval1) {
    previousMillis = currentMillis;
    if(a==0)
    {
      wake_card();
  delay(100);
  read_ACK(15);
    }
    if (ledState == 0)
    {
      ledState = 1;
      digitalWrite(13, HIGH);
      count++;
      Serial.println(count);
    }
    else
    {
      ledState = 0;
      digitalWrite(13, LOW);
      count++;
      Serial.println(count);
    }
    if (count == i * 4 + 1 && i < 10)
    {
      pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0));
      i++;
      pixels.show();
    }

    if (count == 44)
    {
      pixels.setPixelColor(10, pixels.Color(0, 0, 0));
      pixels.show();
    }
    if (count == 47)
    {
      pixels.setPixelColor(11, pixels.Color(0, 0, 0));
      for (int num = 0; num < 12; num++)
      {
        pixels.setPixelColor(num, pixels.Color(255, 0, 0));
      }
      i = 1;
      pixels.show();
    }
    if (count > 47 && count - 47 == i * 3 && i < 7)
    {
      pixels.setPixelColor(i - 1, pixels.Color(0, 0, 0));
      i++;
      pixels.show();
    }
    if (count == 65)i = 1;
    if (count > 65 && count - 65 == i * 2 && i < 7)
    {
      pixels.setPixelColor(i + 5, pixels.Color(0, 0, 0));
      i++;
      pixels.show();
    }
    if (count == 0)
    {
      for (int num = 0; num < 10; num++)
      {
        pixels.setPixelColor(num, pixels.Color(0, 255, 0));
      }
      for (int num = 10; num < 12; num++)
      {
        pixels.setPixelColor(num, pixels.Color(255, 153, 0));
      }
      pixels.show();
      digitalWrite(pinCarG, HIGH);
      digitalWrite(pinCarY, LOW);
      digitalWrite(pinCarR, LOW);

      digitalWrite(pinHumanG, LOW);
      digitalWrite(pinHumanR, HIGH);
    }
    else if (count == car_g * 2 + 1)
    {
      pixels.setPixelColor(9, pixels.Color(0, 0, 0));
      pixels.show();
      digitalWrite(pinCarG, LOW);
      digitalWrite(pinCarY, HIGH);
      digitalWrite(pinCarR, LOW);

      digitalWrite(pinHumanG, LOW);
      digitalWrite(pinHumanR, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + 1)
    {
      if (a == 1)
      {
        b = 1;
        digitalWrite(pinCarG, LOW);
        digitalWrite(pinCarY, LOW);
        digitalWrite(pinCarR, HIGH);

        digitalWrite(pinHumanG, HIGH);
        digitalWrite(pinHumanR, LOW);
        Serial3.write('A');
      }
      else
      {
        digitalWrite(pinCarG, LOW);
        digitalWrite(pinCarY, LOW);
        digitalWrite(pinCarR, HIGH);

        digitalWrite(pinHumanG, LOW);
        digitalWrite(pinHumanR, HIGH);

      }
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 1 && b == 1)
    {

      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 2 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 3 && b == 1)
    {
      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 4 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 5 && b == 1)
    {
      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 6 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 7 && b == 1)
    {
      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 8 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 9 && b == 1)
    {
      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 10 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 11 && b == 1)
    {
      digitalWrite(pinHumanG, HIGH);
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 12 && b == 1)
    {
      digitalWrite(pinHumanG, LOW);
      a = 0;
      b = 0;
    }
    else if (count == car_g * 2 + car_o * 2 + human_g * 2 - human_blink * 2 + 13)
    {
      count = -1;
      i = 1;
      delay(100);
  read_ACK(15);
  delay(100);
  display(15);
    }
  }
  copy_id ();
}

void copy_id (void) {//save old id
  int ai, oi;
  for (oi=0, ai=19; oi<5; oi++,ai++) {
    old_id[oi] = receive_ACK[ai];
  }
}
 
char cmp_id (void){//return true if find id is old
  int ai, oi;
  for (oi=0,ai=19; oi<5; oi++,ai++) {
    if (old_id[oi] != receive_ACK[ai])
      return 0;
  }
  return 1;
}
 
int test_ACK (void) {// return true if receive_ACK accord with std_ACK
  int i;
  for (i=0; i<19; i++) {
    if (receive_ACK[i] != std_ACK[i])
      return 0;
  }
  return 1;
}
 
void send_id (void) {//send id to PC
  int i;
  Serial.print ("ID: ");
  for (i=19; i<= 23; i++) {
    Serial.print (receive_ACK[i], HEX);
  }
}
 
void UART1_Send_Byte(unsigned char command_data){//send byte to device
  print1Byte(command_data);
#if defined(ARDUINO) && ARDUINO >= 100
  Serial1.flush();// complete the transmission of outgoing serial data 
#endif
} 
 
void UART_Send_Byte(unsigned char command_data){//send byte to PC
  Serial.print(command_data,HEX);
} 
 
void read_ACK(unsigned char temp){//read ACK into reveive_ACK[]
  unsigned char i;
  for(i=0;i<temp;i++) {
    receive_ACK[i]= Serial1.read();
  }
}
 
void wake_card(void){//send wake[] to device
  unsigned char i;
  for(i=0;i<24;i++){ //send command
    UART1_Send_Byte(wake[i]);
  }
}
 
void firmware_version(void){//send fireware[] to device
  unsigned char i;
  for(i=0;i<9;i++) //send command
    UART1_Send_Byte(firmware[i]);
}
 
void send_tag(void){//send tag[] to device
  unsigned char i;
  for(i=0;i<11;i++) //send command
    UART1_Send_Byte(tag[i]);
}
 
void display(unsigned char tem){//send receive_ACK[] to PC 
  unsigned char i;
  for(i=0;i<tem;i++){ //send command
    UART_Send_Byte(receive_ACK[i]);
  }
}

