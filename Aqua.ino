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

int leds = 0;
int buzzer = 1;
int button = 2;

boolean autoMode;  //If automation is turned on
boolean hungry;    //If fish is hungry
boolean lightOn;   //If light turned on
byte brightness;   //Brightness of lights (0-255)
unsigned long lastMillis = 0;


void setup() {
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(leds, OUTPUT);

  autoMode = false;
  lightOn = true;
  brightness = 255;
  analogWrite(leds, brightness); 
  TinyTone(Note_C, 4, 200);
  TinyTone(Note_C, 5, 300);

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
    delay(300);
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
  TinyTone(Note_E, 4, 800);   
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
  TinyTone(Note_E, 4, 800); 
  delay(250);
  TinyTone(Note_E, 4, 800); 
  delay(250);
  TinyTone(Note_E, 4, 800); 
  }
}


//******************************************************************
void TinyTone(unsigned char divisor, unsigned char octave, unsigned long duration)
{
  //TCCR1 = 0x90 | (8-octave); // for 1MHz clock
  TCCR1 = 0x90 | (12-octave); // for 1MHz clock
  // TCCR1 = 0x90 | (11-octave); // for 8MHz clock
  OCR1C = divisor-1;         // set the OCR
  delay(duration);
  TCCR1 = 0x90;              // stop the counter
}
