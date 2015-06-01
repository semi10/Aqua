
const int Note_C  = 239;
const int Note_CS = 225;
const int Note_D  = 213;
const int Note_DS = 201;
const int Note_E  = 190;
const int Note_F  = 179;
const int Note_FS = 169;
const int Note_G  = 159;
const int Note_GS = 150;
const int Note_A  = 142;
const int Note_AS = 134;
const int Note_B  = 127;

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
time carry = {0, 0, 0, 0};
time real = {0, 0, 0, 0};

const int button = 2;
const int leds = 0;
const int autoModeLed = 3;
const int hungryLed = 4;
const int buzzer = 1;
boolean autoMode;  //If automation is turned on
boolean hungry;    //If fish is hungry
boolean lightOn;   //If light turned on
byte brightness;   //Brightness of lights (0-255)
byte lastSec = 0;


void setup() {
  //Serial.begin(9600);
  autoMode = false;
  lightOn = true;
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT); 
  pinMode(autoModeLed, OUTPUT);
  pinMode(hungryLed, OUTPUT);
  analogWrite(leds, 255); 
  TinyTone(Note_C, 4, 200);
  TinyTone(Note_C, 5, 300);
  delay(3000);
}

void loop() {
  if(millis() < 2000){
    time current = {0, 0, 0, 0};
    time carry = {real.sec, real.minute, real.hour, real.day};
  }

  current.sec = millis()/1000 - 60*(current.day*24*60 + (current.hour*60 + current.minute));

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

  real.sec = current.sec + carry.sec;
  if (real.sec > 59){
    real.sec-=60; 
    real.minute++; 
  }

  real.minute = current.minute + carry.minute;
  if (real.minute > 59){
    real.minute -=60;
    real.hour++;
  }

  real.hour = current.hour + carry.hour;
  if (real.hour > 23){
    real.hour -=24;
    real.day++;
  }

  real.day = current.day + carry.day;

  if(lastSec != real.sec){
    lastSec = real.sec;
    if(autoMode) checkStatus();
  }


  if(digitalRead(button)){
    delay(100);
    unsigned long start = millis();

    while (digitalRead(button)){
      unsigned long now = millis();
      if(abs(now - start) > 3000){
        set();
        return;
      }
    }  

    if(hungry) {
      hungry = false;
      TinyTone(Note_B, 4, 250);
      delay(50);
      TinyTone(Note_B, 4, 250);
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
    
    sleep.sec = real.sec;
    sleep.minute = real.minute;
    sleep.hour = real.hour;
    
    awake.sec = real.sec;
    awake.minute = real.minute;
    awake.hour = (real.hour + 7) % 24;
    
    feed.sec = real.sec;
    feed.minute = real.minute;
    feed.hour = (real.hour + 20) % 24;
  }
  else{
    lightOn = true;
    brightness = 255; 
    
    sleep.sec = real.sec;
    sleep.minute = real.minute;
    sleep.hour = (real.hour + 17) % 24;
    
    awake.sec = real.sec;
    awake.minute = real.minute;
    awake.hour = real.hour;
    
    feed.sec = real.sec;
    feed.minute = real.minute;
    feed.hour = (real.hour + 13) % 24;
  }
  
  /*
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
  analogWrite(leds, brightness);
  */
  analogWrite(leds, brightness);
  TinyTone(Note_E, 4, 800);   
}


//******************************************************************
void checkStatus(){
  if(real.sec % 15 == 0){
    if(lightOn && brightness < 255){
      brightness++;
    }
    else if (!lightOn && brightness > 0){
      brightness--;
    }
    analogWrite(leds, brightness);
  }

  if(real.sec == 0){
    if(real.minute == awake.minute && real.hour == awake.hour){
      lightOn = true;  
    }
    else if(real.minute == sleep.minute && real.hour == sleep.hour){
      lightOn = false;
    }
    else if(real.minute == feed.minute && real.hour == feed.hour){
      hungry = true;
    }
  }
  
  (autoMode)?(digitalWrite(autoModeLed,HIGH)):(digitalWrite(autoModeLed,LOW));
  (hungry)?(digitalWrite(hungryLed,HIGH)):(digitalWrite(hungryLed,LOW));
  
  if(hungry && (real.sec == 0) && ((real.minute % 15) == 0)){
    TinyTone(Note_E, 4, 300);
    delay(100);
    TinyTone(Note_E, 4, 300);
  }

  /*
  Serial.print("Current: ");
  Serial.print(real.day);
  Serial.print(" ");
  Serial.print(real.hour);
  Serial.print(":");
  Serial.print(real.minute);
  Serial.print(":");
  Serial.print(real.sec);
  Serial.print(" Brightness:");
  Serial.print(brightness);
  Serial.println(" ");
  Serial.println(); */
}



//******************************************************************
void TinyTone(unsigned char divisor, unsigned char octave, unsigned long duration)
{
  TCCR1 = 0x90 | (8-octave); // for 1MHz clock
  // TCCR1 = 0x90 | (11-octave); // for 8MHz clock
  OCR1C = divisor-1;         // set the OCR
  delay(duration);
  TCCR1 = 0x90;              // stop the counter
}
