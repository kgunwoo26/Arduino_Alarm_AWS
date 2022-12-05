e
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include "time.h"
#include <WiFi.h>

#include "arduino_secrets.h"
#include <ArduinoJson.h>

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;
 String inString = "22:53:00"; //현재 시간
 String sleepTime = "";
 String alarm = "22:53:15"; //알람 시간
int seg_pin[8] = {13,12,11,10,9,8,7,6};
int digit_pin[4] ={5,4,3,2};
long jlastMillis = 0;
long plastMillis = 0;
int interruptPin = 0;
int led = 1;
int buzzerPin = 14;
int a_value =0;
int seg_value[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

boolean state_toggle = false;

extern volatile unsigned long timer0_millis = millis();
unsigned long readTime;
unsigned long setTime;
unsigned long tTime, pTime;
long bTime=0;
int hour, mins, sec;
int a_hour, a_mins, a_sec;
int t=true;
int count;
int bDelay=0;


WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;


void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH);  
  pinMode(interruptPin, INPUT_PULLUP); 
  pinMode(16, INPUT_PULLUP); 
  pinMode(17, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(interruptPin), switchFn, RISING);
  attachInterrupt(digitalPinToInterrupt(16), sleepButton, RISING);
  attachInterrupt(digitalPinToInterrupt(17), wakeupButton, RISING);
  pinMode(led,OUTPUT);
  for(int i=0; i<10; i++){
    pinMode(seg_pin[i], OUTPUT);
  }
  for(int j=0;j<4; j++){
    pinMode(digit_pin[j], OUTPUT);
    digitalWrite(digit_pin[j], HIGH);
  }




  int a_index =alarm.indexOf(':');
  int a_index2 =alarm.indexOf(':',a_index+1);
  int a_index3 = alarm.length();

  a_hour = alarm.substring(0, a_index).toInt();
  a_mins = alarm.substring(a_index+1,a_index2).toInt();
  a_sec = alarm.substring(a_index2+1,a_index3).toInt();


  while (!Serial);

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);
  
}

void loop() {
   
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 5 seconds.
if ( millis() - pTime >=1000 ){
      pTime = millis();
      count+=1000;
 }

if(state_toggle == false){
        
     
      int index1 =inString.indexOf(':');
      int index2 =inString.indexOf(':',index1+1);
      int index3 = inString.length();

      hour = inString.substring(0, index1).toInt();
      mins = inString.substring(index1+1,index2).toInt();
      sec = inString.substring(index2+1,index3).toInt();

      timer0_millis = ((long)hour*3600+mins*60+sec)*1000;


      // if(timer0_millis+millis()>=86400000) {
      //   timer0_millis =0;
      // }
      
        timer0_millis += count;
        count=0;
        readTime = (timer0_millis)/1000;
        sec = readTime%60;
        mins = (readTime/60)%60;
        hour = (readTime/(60*60)%24);
        inString=(String(hour)+":"+String(mins)+":"+String(sec));
        if(inString.equals(alarm)){
          a_value =1;
        } 
        
        
    

      if(millis()-tTime >1000){
        if(a_value == 1 && !sleepTime.equals("")){
          if(bDelay ==0){
             digitalWrite(buzzerPin, LOW);  
            bDelay = 1;
          }
          else {
             digitalWrite(buzzerPin, HIGH);  
            bDelay = 0;
          }
        }
        else {
          digitalWrite(buzzerPin, HIGH);  
        }
        Serial.println("time:"+inString);
        t=!t;
        tTime = millis();

      }
      segOutput(3,mins%10,0);
      segOutput(2,mins/10,0);
      if(t) segOutput(1,hour%10,1);
      else segOutput(1,hour%10,0);
      segOutput(0,hour/10,0);

  
  }
  else {

      setTime = ((long)a_hour*3600+a_mins*60+a_sec)*1000;
      if(setTime >=86400000) {
        setTime =0;
      }
      readTime = (setTime)/1000;
      sec = readTime%60;
      mins = (readTime/60)%60;
      hour = (readTime/(60*60)%24);

      readInput();
    
      segOutput(3,mins%10,0);
      segOutput(2,mins/10,0);
      segOutput(1,hour%10,1);
      segOutput(0,hour/10,0);
  }

}

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void buzzer(){
  Serial.println("buzzer");
  digitalWrite(buzzerPin, LOW);
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();
}

void setState_sleep(char* payload) {
  char time[20] ={0};
  alarm.toCharArray(time,alarm.length());
  
  sprintf(payload,"{\"state\":{\"reported\":{\"type\":\"%s\",\"setTime\":\"%s\"}}}","sleep",time);
}

void setState_sleep_sh(char* payload) {
  char time[20] ={0};
  alarm.toCharArray(time,alarm.length());
    sprintf(payload,"{\"state\":{\"reported\":{\"light\":\"%s\",\"blind\":\"%s\"}}}","OFF" ,"OFF" ); 
}

void setState_wakeup(char* payload) {
    char time[20] ={0};
    char stime[20] = {0};
    alarm.toCharArray(time,alarm.length());
    sleepTime.toCharArray(stime, sleepTime.length());
    sprintf(payload,"{\"state\":{\"reported\":{\"sleep\":\"%s\",\"awake\":\"%s\"}}}",stime ,time);
    
}

void setState_wakeup_sh(char* payload) {
    sprintf(payload,"{\"state\":{\"reported\":{\"light\":\"%s\",\"blind\":\"%s\"}}}","ON" ,"ON" ); 
}


void sendMessage(char* payload) {
  char TOPIC_NAME[]= "$aws/things/Arduino_alarm/shadow/update";
  Serial.print("Publishing send message:");
  Serial.println(payload);
    mqttClient.subscribe("$aws/things/Arduino_alarm/shadow/update/delta");
    mqttClient.beginMessage(TOPIC_NAME);
    mqttClient.print(payload);
    mqttClient.endMessage();
    sleepTime = "";
  
}

void sendMessage_sh(char* payload) {
  char TOPIC_NAME[]= "$aws/things/smart_home/shadow/update";
  Serial.print("Publishing send message:");
  Serial.println(payload);
    mqttClient.subscribe("$aws/things/smart_home/shadow/update/delta");
    mqttClient.beginMessage(TOPIC_NAME);
    mqttClient.print(payload);
    mqttClient.endMessage();
    sleepTime = "";
  
}

void sleepButton(){
  char payload[512];
  setState_sleep_sh(payload);
  sendMessage_sh(payload);
  sleepTime = (String(hour)+":"+String(mins)+":"+String(sec));
  Serial.println("sleep... "+ sleepTime);
  digitalWrite (led, LOW);  
}

void wakeupButton(){
  a_value =0;
  if(!sleepTime.equals("")){
  char payload[512];
  setState_wakeup(payload);
  sendMessage(payload);
  setState_wakeup_sh(payload);
  sendMessage_sh(payload);
  digitalWrite (led, HIGH);
  Serial.println("wakeUp!");
  }
}

void readInput()
{
    long currentMillis = millis();
    if(currentMillis - jlastMillis > 300){
    jlastMillis = currentMillis;
    if (analogRead(5) > 1000) {
       Serial.println("Up"); 
      if(a_mins == 59) 
          a_mins = 0;
      else
          a_mins += 1;
    }
    
    else if (analogRead(5) < 300) { 
      Serial.println("Down"); 
      if(a_mins == 0) 
        a_mins =  59;
      else
        a_mins -= 1;
    }
    else if (analogRead(6) > 1000) { 
      Serial.println("Left"); 
      if(a_hour == 0) 
        a_hour =  23;
      else
        a_hour -= 1;
    }
    else if (analogRead(6) < 300) { 
       Serial.println("Right"); 
       if(a_hour == 23) 
          a_hour = 0;
      else
          a_hour += 1;
      }
       
     
  }

}

void switchFn(){
      long currentMillis = millis();
    if(currentMillis - plastMillis > 300){
        plastMillis = currentMillis;
        if(state_toggle == true){
          Serial.println("alarm_time:"+alarm);
          alarm=(String(a_hour)+":"+String(a_mins)+":"+String(a_sec));
          Serial.println("alarm_time:"+alarm);
          update_alarm();
          // String temp = String(a_hour)+":"+String(a_mins)+":"+String(a_sec);
          // Serial.println("alarm_time:"+temp);
        }
        state_toggle = !state_toggle;
        
        Serial.println(state_toggle);
        delay(1000);
    }
}

void update_alarm(){
  int a_index =alarm.indexOf(':');
  int a_index2 =alarm.indexOf(':',a_index+1);
  int a_index3 = alarm.length();

  a_hour = alarm.substring(0, a_index).toInt();
  a_mins = alarm.substring(a_index+1,a_index2).toInt();
  a_sec = 0;
}

void segClear(){
  for(int i=0;i<8;i++){
    digitalWrite(seg_pin[i],LOW);
  }
}

void blink (){
     digitalWrite (led, HIGH); // 5번핀을 HIGH(약 5V)로 출력
    delay (1000); // 딜레이 설정(2000=2초)
    digitalWrite (led, LOW); // 5번핀을 LOW(약 0V)로 출력
    delay (1000); // 딜레이 설정(2000=2초)
}

void segOutput(int d, int Number, int dp){
  segClear();
  digitalWrite(digit_pin[d],LOW);
  for(int i=0; i<7; i++){
    digitalWrite(seg_pin[i], seg_value[Number][i]);
  }

  digitalWrite(seg_pin[7],dp);
  delayMicroseconds(3000);
  digitalWrite(digit_pin[d],HIGH);
}
