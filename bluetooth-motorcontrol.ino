#include<Servo.h> //서보모터를 쉽게 제어하기 위해 라이브러리를 추가한다.
#include<SoftwareSerial.h>

SoftwareSerial BTSerial(2,3); //RXD = 2, RXD = 3

int enablePin = 11;
int in1Pin = 10;
int in2Pin = 9;
int LED = 13;
byte buffer[100] = {0}; //블루투스로부터의 데이터를 읽어 저장해두기위한 배열

int velocity = 0;
int radian = 60;
String aa = "";

Servo myservo; //서보모터를 제어하기위한 인스턴스 생성

void setup() {
  // put your setup code here, to run once:
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(LED, OUTPUT);
  myservo.attach(6); //서보모터와 연결된 핀을 6번으로 설정한다.

  Serial.begin(9600); //디버깅을 위해 시리얼로 컴퓨터와 연결해 변수의 값을 확인할 것이다.
  BTSerial.begin(38400); //아두이노와 블루투스 모듈의 통신 시리얼
}

void loop() {
  // put your main code here, to run repeatedly:
  char c;
  int i = 0;
  
  if(BTSerial.available() > 0) //수신한 데이터가 있다면
  {
    aa = "";
    digitalWrite(LED,HIGH); //데이터를 받는동안 led가 켜진다.
    do{
      c = BTSerial.read();
      Serial.println(c);
      buffer[i++] = c;
      aa = aa + c;
      //delay(15);
    }while((c != '\n') && (c != '\r')); //줄바꿈 문자를 만날때 까지
    buffer[--i]= NULL;
    digitalWrite(LED,LOW);
    Serial.println(aa); //받은 문자열을 컴퓨터 시리얼 모니터에 보내준다.
  }

  int* result = readcommand(buffer);
  switch(result[0])
  {
    case 0:
      velocity = result[1];
      break;
      
    case 1:
      radian = result[1];
      break;
  }

  setMotor(velocity, radian);
  
  //speed:  velocity  rad:  radian  result[0] result[1] aa
  Serial.print("speed:\t");
  Serial.print(velocity);
  Serial.print('\t');
  Serial.print("rad:\t");
  Serial.print(radian);
  Serial.print('\t');
  Serial.print(result[0]);
  Serial.print('\t');
  Serial.print(result[1]);
  Serial.print('\t');
  Serial.println(aa);
}

int* readcommand(char* str) //명령어를 해석하는 함수
{
  int result[2] = {-1, 0};
  if(isSame(str, "v=", 2)) 
  { //v= ~~~같은 식의 문자열이 들어왔다면, velocity에 ~~~를 대입하라는 의미를 가진 데이터를 반환한다.
    result[0] = 0;
    result[1] = atoi(str + 2);
  }
  if(isSame(str, "r=", 2))
  { //r= ~~~같은 식의 문자열이 들어왔다면, radian에 ~~~를 대입하라는 의미를 가진 데이터를 반환한다.
    result[0] = 1;
    result[1] = atoi(str + 2);
  }
  delay(15);
  return result;
}

bool isSame(char* str1, char* str2, int i) //i번째 자리까지 str1과 str2가 같은지 판별하는 함수
{
  for(int j = 0;j < i;j++)
  {
    if(str1[j] != str2[j])
    {
      return false;
    }
  }
  return true;
}

void setMotor(int v, int rad) //모터를 조작하는 함수
{
  if(0 <= v) //모터의 속도와 회전방향 제어
  {
    if(255 < v) v = 255; //v의 값이 255 이상이면 잘라준다.
    analogWrite(enablePin, v);
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
  if(v < 0)
  {
    if(v < -255) v = -255; //v의 값이 -255 이상이면 잘라준다.
    analogWrite(enablePin, -v);
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  }

  //서보모터 회전각 제어
  if(rad < 0) rad = 0; //r의 값이 0 이상 120 이하가 아니면 잘라준다.
  if(rad > 120) rad = 120;
  myservo.write(rad);
}

