# 아두이노 스마트 알람 시계 (AWS 연동)








## 소스 파일 폴더
 - [Iot_Smart_Clock](#Iot_Smart_Clock)
 - [connect_home](#connect_home)
 - [RecordingDeviceDataLambdaJavaProject](#RecordingDeviceDataLambdaJavaProject)
 
 
 ## Iot_Smart_Clock ( 아두이노 디바이스 - 알람 시계 )
  - Iot_Smart_Clock.ino (아두이노 프로그램 파일)
  - arduino_secrets.h (와이파이 정보, aws 디바이스 정보 등)
  
![image](https://user-images.githubusercontent.com/56541313/205848769-183c87d9-50bc-47ca-ac8b-356284157ca4.png)

  
  
  
  
  
  
  
  ## connect_home (아두이노 디바이스 - 스마트홈 구현 예시)
   - connect_home.ino (아두이노 프로그램 파일)
   - devices.cpp (디바이스 클래스 파일)
   - devices.h ( 디바이스 헤더 파일)
   - arduino_secrets.h (와이파이 정보, aws 디바이스 정보 등)

![image](https://user-images.githubusercontent.com/56541313/205848825-a53651ff-158f-42cf-ac83-719e5a53eb59.png)



## RecordingDeviceDataLambdaJavaProject (아두이노의 정보를 dynamoDB에 전송해주는 lambda 함수)
 - RecordingDeviceInfoHandler.java (아두이노 디바이스의 정보를 받아 테이블로 작성) -
 계정에 존재하는 Iot_cloud Table에 전송
 ![image](https://user-images.githubusercontent.com/56541313/205850596-685aa827-e5fe-4ed8-ac02-b6c7ea3a4bdd.png)
