#include <SoftwareSerial.h>

// 블루투스
#define RXD_PIN 10 //아두이노 수신 핀
#define TXD_PIN 11 //아두이노 발신 핀

SoftwareSerial bt(RXD_PIN,TXD_PIN); //Rx/Tx 핀 설정/ 시리얼 통신 객체 생성

int X;  // X좌표
int Y;  // Y좌표
int mode = 0; // 0 : 준비 상태, 1 : 운전 상태, 2 : 복귀 상태
char C = 'E'; //컬러센서 전역변수
int l = 0;
//-----------시간(딜레이) 설정--------------
unsigned long timeval = 0;
unsigned long previousval = 0;

//-----------적외선센서 핀 설정--------------
int Lsensor = A0;
int Rsensor = A1;
int Fsensor = A2;
int Bsensor = A3;
 
//-----------컬러센서 핀 설정------------
const int s0 = 40;
const int s1 = 41;
const int s2 = 42;
const int s3 = 43;
const int out = 7;

//-----------모터 드라이버 핀 설정----------------
//왼쪽바퀴
const unsigned int EN_A = 2;  // 앞 바퀴
const unsigned int IN1_A = 31;
const unsigned int IN2_A = 33;

const unsigned int IN1_B = 35; // 뒷 바퀴
const unsigned int IN2_B = 37;
const unsigned int EN_B = 3;

//오른쪽 바퀴
const unsigned int EN_C = 4;  // 앞 바퀴
const unsigned int IN1_C = 30;
const unsigned int IN2_C = 32;

const unsigned int IN1_D = 34; // 뒷 바퀴
const unsigned int IN2_D = 36;
const unsigned int EN_D = 5;

// 함수들 선언
void Connect(void);
void R_color(void);
void G_color(void);
void Y_color(void);
void Run(void);
void parking(void);
void Return(void);
void Motor_control(int a, int b, int c, int d, int M1_Speed, int M2_Speed, int M3_Speed, int M4_Speed);
void Forward(int Speed);
void Backward(int Speed);
void leftward(int Speed);
void rightward(int Speed);
void diagonal_1(int Speed);
void diagonal_2(int Speed);
void diagonal_3(int Speed);
void diagonal_4(int Speed);
void Stop(void);
void x_Pline(void);
void x_Nline(void);
void y_Pline(void);
void y_Nline(void);

void setup(){
  bt.begin(9600);
  // 컬러센서
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  // 모터
  pinMode(EN_A, OUTPUT);
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN1_B, OUTPUT);
  pinMode(IN2_B, OUTPUT);
  pinMode(EN_C, OUTPUT);
  pinMode(IN1_C, OUTPUT);
  pinMode(IN2_C, OUTPUT);
  pinMode(EN_D, OUTPUT);
  pinMode(IN1_D, OUTPUT);
  pinMode(IN2_D, OUTPUT);
}

void loop(){
  if(mode==0){
    Connect();
  }
  if(mode==1){
    Run();
    parking();
  }
  if(mode==2){
    Return();
  }
}

void Connect(void)  // 연결 시도하는 함수
{
  if(bt.available()){
    String XY = "";
    XY=bt.readStringUntil('e');
    int index1 = XY.indexOf(',');
    int index2 = XY.length();
    X = XY.substring(0, index1).toInt();
    Y = XY.substring(index1+1,index2).toInt();
    mode = 1;
  }
}

void R_color(void) // 빨강 컬러값 확인 함수
{
  int red = 0;
  int green = 0;
  int blue = 0;
  
  //count OUT, pRed, RED
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  red = pulseIn(out, LOW);
  
  //count OUT, pBLUE, BLUE
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  blue = pulseIn(out, LOW);
  
  //count OUT, pGreen, GREEN
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  green = pulseIn(out, LOW);

  if(red > 100 & red < 320 & green > 200 & green < 400 & blue > 170 & blue <350 & red+10 < green & red < blue & blue+10 < green)
  {
    C = 'R';
  }
  else{
    C = 'E';
  }
}

void G_color(void) // 초록 컬러값 확인 함수
{
  int red = 0;
  int green = 0;
  int blue = 0;
  
  //count OUT, pRed, RED
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  red = pulseIn(out, LOW);
  
  //count OUT, pBLUE, BLUE
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  blue = pulseIn(out, LOW);
  
  //count OUT, pGreen, GREEN
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  green = pulseIn(out, LOW);

  if(red > 125 & red < 310 & green > 100 & green < 250 & blue > 140 & blue < 310 & green+10 < red & green+20 < blue)
  {
    C = 'G';
  }
  else{
    C = 'E';
  }
}

void Y_color(void) // 노랑 컬러값 확인 함수
{
  int red = 0;
  int green = 0;
  int blue = 0;
  
  //count OUT, pRed, RED
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  red = pulseIn(out, LOW);
  
  //count OUT, pBLUE, BLUE
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  blue = pulseIn(out, LOW);
  
  //count OUT, pGreen, GREEN
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  green = pulseIn(out, LOW);

  if(red > 50 & red < 190 & green > 50 & green < 220 & blue > 90 & blue < 240 & red <= green & red+30 < blue & green+10 < blue)
  {
    C = 'Y';
  }
  else{
    C = 'E';
  }
}

void Run(void)  // 운행하는 함수
{
  int x = (X%2==0) ? X/2 : X/2+1;
  int y = Y;
  int i = 0;
  int j = 0;

  while(i<x){
    x_Pline();
    R_color();
    timeval = millis();
    if(C=='R' && timeval-previousval >= 1000){
      Stop();
      previousval = millis();
      i++;
    }
  }
  Backward(40);
  delay(300);
  leftward(70);
  delay(200);

  while(j<y){
    G_color();
    y_Pline();
    timeval = millis();
    if(C=='G' && timeval-previousval >= 1000){
      Stop();
      previousval = millis();
      j++;
    }
  }
  Stop();
  delay(200);
  rightward(60);
  delay(200);
}

void parking(void)  // 주차하는 함수
{ 
  Stop();
  delay(200);
  //주차공간으로 들어가기
  if(X%2==1){ // x좌표가 홀수 일 경우
    while(!(C=='Y')){ //노랑 찾을때 까지
      Y_color();
      x_Nline(); //후진
    }
    Forward(50);
    delay(200);
  }
  if(X%2==0){ // x좌표가 짝수 일 경우
    while(!(C=='Y')){
      Y_color();
      x_Pline(); //전진
    }
    Backward(50);
    delay(200);
  }
  if(C=='Y'){ // 노랑찾으면 정지
      Stop();
      delay(3000);
      mode=2;
    }
}

void Return(void) // 돌아가는 함수
{
  // 여기는 추자공간에서 빠져나오기
  if(X%2==1){
    Forward(50);
    delay(200);
    while(!(C=='G')){ 
      G_color();
      x_Pline();
    }
    Stop();
    delay(300);
    Backward(40);
    delay(200);
  }
  if(X%2==0){
    Backward(50);
    delay(200);
    while(!(C=='G')){
      G_color();
      x_Nline();
    }
    Stop();
    delay(300);
    Forward(40);
    delay(200);
  }

  // 여기는 y축으로 이동
  while(1){
    R_color();
    y_Nline();
    if(C=='R') break;
  }
  Stop();
  delay(300);
  leftward(40);
  delay(200);

  // 여기는 x축으로 이동
  while(1){
    Y_color();
    x_Nline();
    if(C=='Y') break;
  }
  Stop();
  delay(300);
  mode = 0; //모드 초기화
}

// 모터 제어 함수
void Motor_control(int a, int b, int c, int d, int M1_Speed, int M2_Speed, int M3_Speed, int M4_Speed) 
{
  if(a==1)
  {
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    analogWrite(EN_A, M1_Speed);
  }
  
  if(a==0)
  {
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    analogWrite(EN_A, M1_Speed);
  }

  if(b==1)
  {
    digitalWrite(IN1_B, HIGH);
    digitalWrite(IN2_B, LOW);
    analogWrite(EN_B, M2_Speed);
  }

  if(b==0)
  {
    digitalWrite(IN1_B, LOW);
    digitalWrite(IN2_B, HIGH);
    analogWrite(EN_B, M2_Speed);
  }

  if(c==1)
  {
    digitalWrite(IN1_C, HIGH);
    digitalWrite(IN2_C, LOW);
    analogWrite(EN_C, M3_Speed);
  }

  if(c==0)
  {
    digitalWrite(IN1_C, LOW);
    digitalWrite(IN2_C, HIGH);
    analogWrite(EN_C, M3_Speed);
  }

  if(d==1)
  {
    digitalWrite(IN1_D, HIGH);
    digitalWrite(IN2_D, LOW);
    analogWrite(EN_D, M4_Speed);
  }

  if(d==0)
  {
    digitalWrite(IN1_D, LOW);
    digitalWrite(IN2_D, HIGH);
    analogWrite(EN_D, M4_Speed);
  }
}

// 전진하는 함수
void Forward(int Speed){
  Motor_control(1,1,1,1,Speed,Speed,Speed,Speed); //전진
}

// 후진하는 함수
void Backward(int Speed){
  Motor_control(0,0,0,0,Speed,Speed,Speed,Speed); //후진
}

// 좌측으로 움직이는 함수
void leftward(int Speed){
  Motor_control(0,1,1,0,Speed,Speed,Speed,Speed); //좌측이동
}

// 우측으로 움직이는 함수
void rightward(int Speed){
  Motor_control(1,0,0,1,Speed,Speed,Speed,Speed);
}

// 1사분면 이동 함수
void diagonal_1(int Speed){
  Motor_control(1,0,0,1,Speed,0,0,Speed);
}

// 2사분면 이동 함수
void diagonal_2(int Speed){
  Motor_control(0,1,1,0,0,Speed,Speed,0);
}

// 3사분면 이동 함수
void diagonal_3(int Speed){
  Motor_control(0,1,1,0,Speed,0,0,Speed);
}

// 4사분면 이동 함수
void diagonal_4(int Speed){
  Motor_control(1,0,0,1,0,Speed,Speed,0);
}

// 모터 정지
void Stop(void){
  Motor_control(0,0,0,0,0,0,0,0);
}

// x축 +라인트레이싱
void x_Pline(void){
  if(analogRead(Lsensor) < 196 && analogRead(Rsensor) < 209){
    Forward(40);
  }
  else if(analogRead(Lsensor) > 196){
    diagonal_2(50);
  }
  else if(analogRead(Rsensor) > 209){
    diagonal_1(50);
  }
  else{
    Stop();
  }
}

// x축 -라인트레이싱
void x_Nline(void){
  if(analogRead(Lsensor) < 196 && analogRead(Rsensor) < 209){
    Backward(40);
  }
  else if(analogRead(Lsensor) > 196){
    diagonal_3(50);
  }
  else if(analogRead(Rsensor) > 209){
    diagonal_4(50);
  }
  else{
    Stop();
  }
}

// y축 +라인트레이싱
void y_Pline(void){
  if((analogRead(Fsensor) < 150 && analogRead(Bsensor) < 150))
  {
      if(analogRead(Lsensor) > 300 && analogRead(Rsensor) > 300){
        leftward(90);
      }
      leftward(50);
  }
  else if(analogRead(Fsensor) > 150) //
  {
    diagonal_2(50);
    l=2; //우회전 할것
  }
  else if(analogRead(Bsensor) > 150) //
  {
    diagonal_3(50);
    l=1; //좌회전 할것
  }
  else
  {
    Stop();
  }
}

// y축 -라인트레이싱
void y_Nline(void){
  if((analogRead(Fsensor) < 150 && analogRead(Bsensor) < 150))
  {
      if(analogRead(Lsensor) > 300 && analogRead(Rsensor) > 300){
        rightward(90);
      }
      rightward(50);
  }
  else if(analogRead(Bsensor) > 150) //
  {
    diagonal_4(50);
    l=1; //좌회전 할것
  }
  else if(analogRead(Fsensor) > 150) //
  {
    diagonal_1(50);
    l=2; //우회전 할것
  }
  else
  {
    Stop();
  }
}
