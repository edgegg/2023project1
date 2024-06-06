#include <SoftwareSerial.h>

//스위치
#define sw 4

// 조도센서
const int cds[][1] = {{A0}, {A1}, {A2}, {A3}}; // [라인][칸??] 주차공간의 위치표현(x,y)

const int x = sizeof(cds)/sizeof(cds[0]); // 라인 개수
const int y = sizeof(cds[0])/sizeof(int); // 칸 개수

// 블루투스
#define RXD_PIN 2 //아두이노 수신 핀
#define TXD_PIN 3 //아두이노 발신 핀

SoftwareSerial bt(RXD_PIN,TXD_PIN); //Rx/Tx 핀 설정/ 시리얼 통신 객체 생성
/* 아두이노 수신 핀에는 HC-05의 발신핀(TX)연결
 *  아두이노 발신 핀에는 HC-05의 수신핀(RX)연결*/

// 함수 선언
int order(void);

void setup() {
  pinMode(sw, INPUT);
  bt.begin(9600); //SoftwareSerial 통신 속도 및 시작
}

void loop() {
  if(digitalRead(sw)){  // 스위치가 눌리고 로봇이 시작지점에 있으면
    order();
    delay(3000);
  }
}

int order(void) // 명령할 좌표값 설정 함수
{
  for(int i=0; i<x; i++){
    for(int j=0; j<y; j++){
      if(analogRead(cds[i][j])<600){ // 밝으면
        bt.print(i+1);
        delay(2);
        bt.write(',');
        delay(2);
        bt.print(j+1);
        delay(2);
        bt.write('e');  // 명령의 끝을 알리기 위한 문자
        return 0;
      }
    }
  }
}
