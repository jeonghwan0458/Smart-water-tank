/*
 * 자동 환수 및 전등 제어 프로그램
 */

#include <ThreeWire.h>  
#include <RtcDS1302.h>

//4핀이 DAT
//5핀이 CLK
//2핀이 CE
ThreeWire myWire(4,5,2); // IO, SCLK, CE

//RTC 라이브러리 생성
RtcDS1302<ThreeWire> Rtc(myWire);

#define DRAIN_PWM 11    //물 흡입 펌프 속도 제어핀
#define DRAIN_1 10      //방향제어
#define DRAIN_2 9       //방향제어
#define SUPPLY_PWM  8   //물 공급 펌프 속도 제어핀
#define SUPPLY_1  7     //방향제어
#define SUPPLY_2  6     //방향제어
#define LIGHT 3


int WeekTime = 0;

void setup () 
{
    Serial.begin(57600);
    //컴파일 시점의 날짜(__DATE__)와 시간(__TIME__)을 시리얼모니터에 표시
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    //RTC 모듈 라이브러리 시작
    Rtc.Begin();

    //RTCDateTime 클래스 생성(컴파일된 시간으로 설정)
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    DateTime(compiled);
    Serial.println();
    
    //RTC모듈에 쓰기 금지 모드인지 확인
    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        //쓰기 금지 모드이면 해제
        Rtc.SetIsWriteProtected(false);
    }
    
    //RTC 모듈이 동작중이 아니라면
    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        // 시작
        Rtc.SetIsRunning(true);
    }

    // RTC 모듈의 현재 시간 얻기
    RtcDateTime now = Rtc.GetDateTime();

    //RTC 모듈하고 위에 만든 RTC클래스(컴파일된 시간을 가지고 있는)와 비교해서
    //RTC 시간이 더 늦다면
    if (now < compiled) 
    {
        //RTC가 컴파일 시간보다 느리다(시간을 갱신한다.)
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        //컴파일 시간을 RTC 모듈에 적용
        Rtc.SetDateTime(compiled);
    }
    //RTC 모듈이 컴파일한 시간보다 빠르다면
    else if (now > compiled) 
    {
        //RTC가 캄파일 시간보다 더 빠르다
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    //RTC와 컴파일한 시간이 같다면
    else if (now == compiled) 
    {
        //RTC와 컴파일 시간이 같다
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    pinMode(DRAIN_PWM, OUTPUT);
    pinMode(DRAIN_1, OUTPUT);
    pinMode(DRAIN_2, OUTPUT);
    pinMode(SUPPLY_PWM, OUTPUT);
    pinMode(SUPPLY_1, OUTPUT);
    pinMode(SUPPLY_2, OUTPUT);
    pinMode(LIGHT, OUTPUT);

    
}

void loop () 
{
    //RTC 모듈의 현재 시간 얻기
    RtcDateTime now = Rtc.GetDateTime();
    

    //시리얼 모니터에 출력
    DateTime(now);
    //줄 바꿈.
    Serial.println();

    
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void Change_water(){
  digitalWrite(DRAIN_1, HIGH);
  digitalWrite(DRAIN_2, LOW);
  analogWrite(DRAIN_PWM, 100);
  delay(300000);
  digitalWrite(DRAIN_1, LOW);
  digitalWrite(DRAIN_2, LOW);
  analogWrite(DRAIN_PWM, 0);

  digitalWrite(SUPPLY_1, HIGH);
  digitalWrite(SUPPLY_2, LOW);
  analogWrite(SUPPLY_PWM, 100);
  delay(300000);
  digitalWrite(SUPPLY_1, LOW);
  digitalWrite(SUPPLY_2, LOW);
  analogWrite(SUPPLY_PWM, 100);
}


//시리얼 모니터에 날짜 시간 표시하는 함수
void DateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);

    if(dt.Hour() == 8 && dt.Minute() == 30 && dt.Second() == 0){
      if(WeekTime == 7){
        Change_water();
        WeekTime = 0;
      }
      digitalWrite(LIGHT, HIGH);
      WeekTime++;
    }
    
}
