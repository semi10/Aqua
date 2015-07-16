#include <LiquidCrystal.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

typedef struct {
  byte sec;
  byte minute;
  byte hour;
  unsigned int day;
} time;

time sleep;
time awake;
time feed;
 
time current = {0, 0, 0, 0};

int leds = 3;
int buzzer = 11;
int button = 2;

boolean autoMode;  //If automation is turned on
boolean hungry;    //If fish is hungry
boolean lightOn;   //If light turned on
byte brightness;   //Brightness of lights (0-255)
unsigned long lastMillis = 0;


void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(leds, OUTPUT);

  autoMode = false;
  lightOn = true;
  brightness = 255;
  analogWrite(leds, brightness); 

}


void loop() {
  unsigned long millisDiff = millis() - lastMillis;
  millisDiff = abs(millisDiff);
  
  if(millisDiff >= 1000){
    lastMillis += 1000;
    updateTime();
    checkStatus();
  }

  if(digitalRead(button)){
    delay(100);
    unsigned long start = millis();

    while (digitalRead(button)){
      unsigned long now = millis();
      if((now - start) > 3000){
        set();
        return;
      }
    }  

    if(hungry) {
      hungry = false;
      analogWrite(buzzer, 150);
      delay(500);
      analogWrite(buzzer, 0);
    }
    else if(lightOn){
      lightOn = false;
      brightness = 0;
    }
    else{
      lightOn = true;
      brightness = 255;
    }
      analogWrite(leds, brightness);
    }
  }



//******************************************************************
void set(){
  autoMode = true;

  if(lightOn){
    lightOn = false;
    brightness = 0;
    
    sleep.sec = current.sec;
    sleep.minute = current.minute;
    sleep.hour = current.hour;
    
    awake.sec = current.sec;
    awake.minute = current.minute;
    awake.hour = (current.hour + 7) % 24;
    
    feed.sec = current.sec;
    feed.minute = current.minute;
    feed.hour = (current.hour + 20) % 24;
  }
  else{
    lightOn = true;
    brightness = 255; 
    
    sleep.sec = current.sec;
    sleep.minute = current.minute;
    sleep.hour = (current.hour + 17) % 24;
    
    awake.sec = current.sec;
    awake.minute = current.minute;
    awake.hour = current.hour;
    
    feed.sec = current.sec;
    feed.minute = current.minute;
    feed.hour = (current.hour + 13) % 24;
  }
  
  analogWrite(leds, brightness);
  analogWrite(buzzer, 150);
  delay(1500);
  analogWrite(buzzer, 0);
  
  Serial.print("Awake: ");
  Serial.print(awake.hour);
  Serial.print(":");
  Serial.print(awake.minute);
  Serial.print(":");
  Serial.println(awake.sec);
  Serial.print("Feed: ");
  Serial.print(feed.hour);
  Serial.print(":");
  Serial.print(feed.minute);
  Serial.print(":");
  Serial.println(feed.sec);
  Serial.print("Sleep: ");
  Serial.print(sleep.hour);
  Serial.print(":");
  Serial.print(sleep.minute);
  Serial.print(":");
  Serial.println(sleep.sec);
  Serial.println();
}


//******************************************************************
void updateTime(){
  current.sec += 1;
  
  if(current.sec>59){
    current.minute++;
    current.sec = 0;
  }
  if(current.minute>59){
    current.hour++;
    current.minute = 0;
  }
  if(current.hour>23){
    current.day++;
    current.hour = 0;
  }  
  
  lcd.clear();
  lcd.print(current.day);
  lcd.print(" ");
  if (current.hour < 10) lcd.print("0");
  lcd.print(current.hour);
  lcd.print(":");
  if (current.minute < 10) lcd.print("0");
  lcd.print(current.minute);
  lcd.print(":");
  if (current.sec < 10) lcd.print("0");
  lcd.print(current.sec);
  
  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(brightness);
  if(hungry) lcd.print(" H");
  if(autoMode) lcd.print(" A");
  
}

//******************************************************************
void checkStatus(){
  if(current.sec % 15 == 0){
    if(lightOn && brightness < 255){
      brightness++;
    }
    else if (!lightOn && brightness > 0){
    brightness--;
    }
    analogWrite(leds, brightness);
  }
  


  if(current.sec == 0 && autoMode){
    if(current.minute == awake.minute && current.hour == awake.hour){
      lightOn = true;  
    }
    else if(current.minute == sleep.minute && current.hour == sleep.hour){
      lightOn = false;
    }
    else if(current.minute == feed.minute && current.hour == feed.hour){
      hungry = true;
    }
  }
  
  if(hungry && (current.sec == 0) && ((current.minute % 15) == 0)){
    analogWrite(buzzer, 150);
    delay(2000);
    analogWrite(buzzer, 0);
  }
}



