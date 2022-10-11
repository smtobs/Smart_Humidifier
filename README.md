# Smart_Humidifier
* 스마트 가습기 (작업 중)
 : 주요 사항으로 기구 작업, 부저 드라이버, UI 제작 등이 필요.
 
 개발 환경
  : 언어 C, C++
  : OS : FreeRTOS
  : IDE : CubeIDE
 
- 디스플레이 : TFT LCD
  : touchGFX 라이브러리 사용

- 센서 : DHT11, BUZZER

- 인터페이스 : Wi-Fi(ESP8266)
 : UART 통신 이용, AT Command 사용

- 프로토콜 : MQTT(PAHO)
 : 센서 값 -> MQTT 브로커 보낼떄는 publisher 사용
   MQTT 브로커 -> 수신 받을 때는 subscriber 사용
