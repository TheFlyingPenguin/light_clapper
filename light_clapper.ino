#define THRESHOLD1 100  //volume threshold of the 1st clap
#define THRESHOLD2 80  //volume threshold of the 2nd clap

#define STATE_STANDBY 0
#define STATE_FIRST_CLAP 1
#define STATE_CLAP_DELAY 2
#define STATE_SECOND_CLAP 3
#define STATE_SWITCH_DELAY 4

//duration of each state (ms)
#define TIME_FIRST_CLAP 50    //length of the first clap
#define TIME_CLAP_DELAY 80     //gap between claps
#define TIME_SECOND_CLAP 500   //second clap
#define TIME_SWITCH_DELAY 1000 //minimum delay between light switching

//DC pins
#define PUSHBUTTON 2
#define RELAY 4

int count;
int clapCount;
int state;

boolean buttonPushed;
boolean lightOn;

boolean validClap(int claps) {
    return (1 < claps && claps < 20);
}

//TODO code to switch light on
void lightswitch() {
  Serial.println("Light switched");
  
  if (lightOn) digitalWrite (RELAY, LOW);
  else         digitalWrite (RELAY, HIGH);
  
  lightOn = !lightOn;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  count = 0;
  state = STATE_STANDBY;
  
  pinMode(PUSHBUTTON, INPUT);
  buttonPushed = false;
  
  pinMode(RELAY, OUTPUT);
  lightOn = false;
}

// the loop routine runs over and over again forever:
void loop() {
  
  if (digitalRead(PUSHBUTTON) == 1) {
    if (!buttonPushed) {
      lightswitch();
      buttonPushed = true;
    }
  } else {
    buttonPushed = false;
  }
  
  // read the input on analog pin 0:  
  int sensorValue = analogRead(A0);
  count++;
  
  switch (state) {
    case STATE_STANDBY:
      count = 0;
      if (sensorValue > THRESHOLD1) {
        state = STATE_FIRST_CLAP;
        Serial.println("STATE FIRST CLAP");
        clapCount = 1;
      }
      break;
      
    case STATE_FIRST_CLAP:
      if (sensorValue > THRESHOLD1) clapCount++;
      if (count > TIME_FIRST_CLAP) {
        if (validClap(clapCount)) {
          count = 0;
          clapCount = 0;
          state = STATE_CLAP_DELAY;
          Serial.println("STATE CLAP DELAY");
        }
        else {
          state = STATE_STANDBY;
          Serial.println("STATE STANDBY");
        }
      }
      break;
      
    case STATE_CLAP_DELAY:
      count++;
      if (sensorValue > THRESHOLD2) {
        state = STATE_STANDBY;
        Serial.println("STATE STANDBY");
      }
      if (count > TIME_CLAP_DELAY) {
        count = 0;
        state = STATE_SECOND_CLAP;
        Serial.println("STATE SECOND CLAP");
      }
      break;
    
    case STATE_SECOND_CLAP:
      if (sensorValue > THRESHOLD2) clapCount++;
      
      if (validClap(clapCount)) {
        count = 0;
        lightswitch();
        state = STATE_SWITCH_DELAY;
        Serial.println("STATE SWITCH DELAY");
      }
      else if (count > TIME_SECOND_CLAP) {
          state = STATE_STANDBY;
          Serial.println("STATE STANDBY");
      }
      break;
      
    case STATE_SWITCH_DELAY:
      if (count > TIME_SWITCH_DELAY) {
        state = STATE_STANDBY;
        Serial.println("STATE STANDBY");
      }
      break;
      
    default:
      Serial.println("State error!");
      return;
  }
  
  delay(1);        // delay in between reads for stability
}
