#include <Wire.h>
#include <DS3231.h>
#include "LowPower.h"

#define BuzzerPin 6
#define InterruptPin 3

RTCDateTime dt;
DS3231 clock;
bool flag = 0;                                // Flag for interrupt.

void setup() {
  clock.begin();
  //clock.setDateTime(2019, 06, 30, 12, 59, 56); // For manual testing. 
  //clock.setDateTime(__DATE__, __TIME__);    // Uncomment it to set RTC time according
                                              // time fetched while compiling. Make
                                              // sure to comment it out again after uploading the code.

  clock.armAlarm1(false);                     // Confirm both alarms are disarmed
  clock.armAlarm2(false);                     //  on starup.

  clock.clearAlarm1();                        // Clear Alarm1 & Alarm2 on startup,
  clock.clearAlarm2();                        // if arduino resets after power loss.

  clock.setAlarm2(0, 0, 0, DS3231_MATCH_M);   // Enable Hourly Alarm.
  clock.enable32kHz(false);                   // Disable 32KHz Output on RTC.
  clock.enableOutput(false);                  // Disable square wave (SQW) output.

  pinMode(BuzzerPin, OUTPUT);
  digitalWrite(BuzzerPin, LOW);
  delay(1000);                                // Wait for everything to settle down just to avoid erroneous interrupt.

  pinMode(InterruptPin, INPUT_PULLUP);        // Enable internal pullup resistors as SQW pin is open drain.
  attachInterrupt(digitalPinToInterrupt(InterruptPin), InterruptTrigger, FALLING); // Enable interrupt trigger if 
}

void InterruptTrigger() {
  flag = 1; // Set flag to 1 if interrupt is recived.
}

void loop() {
  if (flag == 1)                              // Check if interrupt is really recived, with a flag.
    {
    dt = clock.getDateTime();
    if (dt.hour <= 22 && dt.hour >= 7)        // Do not chime if current time is in between 22 and 7, i.e. night time.
      { 
        if((hour12(dt.hour)%3)==0){
          beep(3, 300);
        } else if((hour12(dt.hour)%2)==0){
          beep(2, 500);
        } else{
          beep(1, 1000);
        }
      }
      flag = 0;
      clock.clearAlarm2();
      clock.clearAlarm1();
  } else {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); // Enter low power state.
  }
}

int hour12(int hour24){ //Convert to 12 Hour
  if (hour24 == 0)
    {
        return 12;
    }

  if (hour24 > 12)
    {
       return (hour24 - 12);
    }

  return hour24;
}

void beep(int times, int duration){
  for (int i=0; i<times; i++){
        digitalWrite(BuzzerPin, HIGH);
        delay(duration);
        digitalWrite(BuzzerPin, LOW);
        delay(duration);
  }
}
