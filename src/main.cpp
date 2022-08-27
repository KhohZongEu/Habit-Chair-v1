#include <Arduino.h>

//-------------------Firebase and WIFI--------------------
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

//---------------------Wifi Info-------------------------
#define WIFI_SSID "CEC"
#define WIFI_PASSWORD "CEC_2018"

//--------------------Firebase Info-------------------------
#define API_KEY "AIzaSyAHmdwFtSJ7YgBuNHSVdR9mTFHPBYi3Ta4"
#define DATABASE_URL "https://habit-chair-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "esp1@gmail.com"
#define USER_PASSWORD "123456"

//----------------------Firebase Init---------------------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

//-------------------------Running LED---------------------
#define Running 2

//--------------------Time switch--------------------------
#define timeSwitch 23
int switchState;
int maxSittingTime;


//------------------------------DF Player Mini------------------------------
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>  //file 0000 is bad posture, 0001 is sitting time, 0002 is left leg, 0003 is right leg
SoftwareSerial mySerial(25,26);//Tx,Rx
DFPlayerMini_Fast myMP3;
uint8_t volume = 30; // max = 30
bool audioPlaying;

//-----------------------------------timer----------------------------------------
unsigned long timePrevMillis;
unsigned long timeCurrentMillis;
unsigned long trickPreventionMillis;

int Seconds = 0;
int Minutes = 0;
int Hours = 0;

int startTime = 0;

//--------------------Posture checking / alert system sytem setup------------------
#define PS1 36
#define PS2 39
#define PS3 34
#define PS4 35
#define PS5 32
#define PS6 33
#define motorPin 5

int warnings = 0;
int States = 0;
int occupancy = 0;
int violations = 0;
const int onlineState = 1;
bool rst = false;

int Sensor1average = 0;
int Sensor2average = 0;
int Sensor3average = 0;
int Sensor4average = 0;
int Sensor5average = 0;
int Sensor6average = 0;

int buttDifference = 0;


void setup() {
  
  //---------------------------Start communication----------------------------
  Serial.begin(115200);
  mySerial.begin(9600);

  //-------------------------------Running LED--------------------------------
  pinMode(Running, OUTPUT);

  //-----------------------timeSwitch---------------------
  pinMode(timeSwitch, INPUT_PULLUP);

  //---------------------Posture Chekcing System pinMode----------------------
  pinMode(PS1,INPUT);
  pinMode(PS2,INPUT);
  pinMode(PS3,INPUT);
  pinMode(PS4,INPUT);
  pinMode(PS5,INPUT);
  pinMode(PS6,INPUT);
  pinMode(Running,HIGH);
  pinMode(motorPin,OUTPUT);
  

  //---------------------------DF Player Mini setup---------------------------
  myMP3.begin(mySerial);
  myMP3.volume(30);

  //-----------------------------Connect to WiFi-------------------------------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

   /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  fbdo.setResponseSize(4096);

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  signupOK = true;

}

void postureCheck(){

    for (int i1=0; i1 < 5; i1++) {
     Sensor1average = Sensor1average + analogRead(PS1);
    }
    Sensor1average = Sensor1average/5;

    for (int i2=0; i2 < 5; i2++) {
     Sensor2average = Sensor2average + analogRead(PS2);
    }
    Sensor2average = Sensor2average/5;

    for (int i3=0; i3 < 10; i3++) {
     Sensor3average = Sensor3average + analogRead(PS3);
    }
    Sensor3average = Sensor3average/10;    

    for (int i4=0; i4 < 10; i4++) {
     Sensor4average = Sensor4average + analogRead(PS4);
    }
    Sensor4average = Sensor4average/10;

    for (int i5=0; i5 < 10; i5++) {
     Sensor5average = Sensor5average + analogRead(PS5);
    }
    Sensor5average = Sensor5average/10;

    for (int i6=0; i6 < 10; i6++) {
     Sensor6average = Sensor6average + analogRead(PS6);
    }
    Sensor6average = Sensor6average/10;

    
    buttDifference = Sensor1average - Sensor2average;
    
    if(buttDifference < 0){
      buttDifference = buttDifference * -1;
      
    }else{
      buttDifference = buttDifference * 1;
    }

    audioPlaying = myMP3.isPlaying();
  
  if ((Sensor1average > 2800) && (Sensor2average > 2900) && (buttDifference > 0) && (buttDifference   < 400) && (Sensor4average > 100) && (Sensor3average > 100) && (Sensor6average == 0) && (Sensor5average == 0)) {
    Serial.println("posture correct");
    occupancy = true;
    States = 1;
    

  } else if ((Sensor4average == 0) && (Sensor3average > 100)) {
    Serial.println("right leg up");
    occupancy = true;
    States = 2;

  } else if ((Sensor3average == 0) && (Sensor4average > 100)) {
    Serial.println("left leg up");
    occupancy = true;
    States = 3;

  } else if ((Sensor1average < 200) && (Sensor2average < 100) && (Sensor3average == 0) && (Sensor4average == 0) && (Sensor5average < 50) && (Sensor6average < 50)) {
    Serial.println("not occupied");
    occupancy = false;
    States = 1;
    
  }else if((Sensor5average > 0) && (Sensor6average > 0)){ 
    Serial.println("Back lean");
    occupancy = true;
    States = 5;
    
  }else {
    Serial.println("posture incorrect");
    occupancy = true;
    States = 5;
    
    
  }

    switch(States){
      case 1:
      
        break;

      case 2:
        if(audioPlaying == false){
          myMP3.volume(30);
          myMP3.play(2);
          warnings++;
        }
          
        break;
      case 3:
        if(audioPlaying == false){
          myMP3.volume(30);
          myMP3.play(3);
          warnings++;
        }
        
        break;
      case 4:
      
        break;
      case 5:
        if(audioPlaying ==false){
          myMP3.volume(30);
          myMP3.play(1);
          warnings++;
        }
        break;
    }

    
}

//-----------------------Backup Vibrator Alert System------------------
void vibratorControl(){
  if (warnings > 5){
    for(int motor=0; motor < 10;motor++){
      digitalWrite(motorPin, HIGH);
      delay(500);
      digitalWrite(motorPin,LOW);
      delay(500);
      
    }
    warnings = 0;
  }
}

//------------------------------Sitting Time---------------------------------
void extendedSeating(){
  switchState = digitalRead(timeSwitch);
  if(switchState == 0){
    maxSittingTime = 35;
  }
  if(switchState == 1){
    maxSittingTime = 50;
  }

  if(Minutes > maxSittingTime){
    if(audioPlaying == false){
      myMP3.volume(30);
      myMP3.play(1);
    }
    if((occupancy == true) && ((Minutes - maxSittingTime) > 2)){
      warnings++;
    }else if((occupancy = false) && ((Minutes - maxSittingTime) < 2)){
      if((Minutes - maxSittingTime) > 5){
      Minutes = 0;
      }
    }
  }
}

// -----------------------------Timer----------------------------------
void Timer(){
  if(occupancy == true){
    timeCurrentMillis = millis();
    if((timeCurrentMillis - timePrevMillis) > 1000){
      Seconds++;
      timePrevMillis = millis();
    }
    if(Seconds > 59){
      Minutes++;
      Seconds = 0;
    }
    if(Minutes > 59){
      Hours++;
      Minutes = 0;
    }
  }
}

//--------------------------Firebase Send Data------------------------------

void sendDataToFirebase(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    Firebase.RTDB.setInt(&fbdo, "data/time/hours", Hours);
    Firebase.RTDB.setInt(&fbdo, "data/time/minutes", Minutes);
    Firebase.RTDB.setInt(&fbdo, "data/time/seconds", Seconds);
    Firebase.RTDB.setInt(&fbdo, "data/violations/times", violations);  
    Firebase.RTDB.setInt(&fbdo, "data/online/state", onlineState);
    Firebase.RTDB.setInt(&fbdo, "data/occupancy/status", occupancy);
}

}


void loop(){ 
  digitalWrite(Running,HIGH);
    
//--------------------------Run postureCheck() -------------------------------
  postureCheck();

//---------------------Run Backup Vibrator Alert System-----------------------
  vibratorControl();

//----------------------------Send Data to Firebase-----------------------------
  sendDataToFirebase();

//------------------------sitting Time----------------
  extendedSeating();
}