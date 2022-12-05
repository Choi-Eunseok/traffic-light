#include <SoftwareSerial.h>
SoftwareSerial unoSerial(4, 5);
int speakerPin = 9;     // 정수현 변수 speakerPin를 선언하고 5를 대입
int length = 50;    // 노래의 총 길이 설정
char notes[] = "ececececececececececececececececececececececececec";             // 음계 설정
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};                            // 해당하는 음이 울리는 길이 설정
int tempo = 350;                                                                                                   // 캐럴이 연주되는 속도
int motorPin = 10;
void playTone(int tone, int duration)
{
  for (long i = 0; i < duration * 2000L; i += tone * 2)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}
void playNote(char note, int duration)
{
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };     //음계 함수 설정
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };      // 음계 톤 설정
  for (int i = 0; i < 8; i++)
  {
    if (names[i] == note)
    {
      playTone(tones[i], duration);
    }
  }
}
void setup() {
  unoSerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  /*while(unoSerial.available()){
    char data = unoSerial.read();
    Serial.println(data);
    }  */
  if (unoSerial.available() > 0) {
    char inChar = unoSerial.read();
    if (inChar == 'B')
    {
      char b = 'C';
      analogWrite(motorPin, 100);
      playNote(b, 200);
      delay(00);
      analogWrite(motorPin, 0);
        delay(200);
    }
    if (inChar == 'A')
    {
      /*음악 나오는 부분 시작!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        for (int i = 0; i < length; i++)
        {
        if (notes[i] == ' ')
        {
        delay(beats[i] * tempo); // rest
        }
        else
        {
        playNote(notes[i], beats[i] * tempo);
        }
        delay(tempo / 10);
        }
        delay(500);

        // 음악 끝나는 부분!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  by  기태*/
      for (int i = 0; i < 6; i++)
      {
        analogWrite(motorPin, 100);
        playNote(notes[i * 2], i * 100);
        delay(i * 100);
        analogWrite(motorPin, 0);
        playNote(notes[i * 2 + 1], i * 100);
        delay(i * 100);
      }
      for (int num = 0; num < 10; num++)
      {
        analogWrite(motorPin, 100);
        playNote(notes[0], 100);
        delay(100);
        analogWrite(motorPin, 0);
        playNote(notes[1], 100);
        delay(100);
      }

      /*analogWrite(motorPin, 100);
        playNote(notes[0], 1 * tempo);
        delay(100);
        analogWrite(motorPin, 0);
        playNote(notes[1], 1 * tempo);
        delay(100);
        analogWrite(motorPin, 100);
        playNote(notes[2], 1 * tempo);
        delay(200);
        analogWrite(motorPin, 0);
        playNote(notes[3], 1 * tempo);
        delay(200);
        analogWrite(motorPin, 100);
        playNote(notes[4], 1 * tempo);
        delay(300);
        analogWrite(motorPin, 0);
        playNote(notes[5], 1 * tempo);
        delay(300);
        analogWrite(motorPin, 100);
        playNote(notes[6], 1 * tempo);
        delay(400);
        analogWrite(motorPin, 0);
        playNote(notes[7], 1 * tempo);
        delay(400);
        analogWrite(motorPin, 100);
        playNote(notes[8], 1 * tempo);
        delay(500);
        analogWrite(motorPin, 0);
        playNote(notes[9], 1 * tempo);
        delay(500);
        Serial.print(inChar);*/
    }
  }
}
