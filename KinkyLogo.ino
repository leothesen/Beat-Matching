// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//Defining the pins that will be connected to the correct colours
int redLED = 9;
int greenLED = 10;
int blueLED = 11;

//Defining variables that will be used as time counters
int counter = 0; //will be used to count up to 5000, which indicates a second
int seconds = 0;
int minutes = 0;
int timeout = 1; //number of minutes before the patturn changes

//Variable that will control the state of the patturn being displayed
int pattern = 16;
int numPatterns = 17; //how many patterns are in operation

void setup() {
    // Set ADC to 77khz, max for 10bit
    sbi(ADCSRA,ADPS2);
    cbi(ADCSRA,ADPS1);
    cbi(ADCSRA,ADPS0);

    //Initialzing the pins
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(blueLED, OUTPUT);

    Serial.begin(9600);
    Serial.print("Pattern: ");
    Serial.println(pattern);
}



// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 9.1f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample / 160.f;
    yv[0] = yv[1]; 
    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 7.015f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
}

void clearLEDs(){
  analogWrite(redLED, 0);
  analogWrite(greenLED, 0);
  analogWrite(blueLED, 0);
}

//---------------------------PATTERNS---------------------------

//Red fade with red flashes
void pattern0 (int beatState){
  clearLEDs();
  switch(beatState){
    case 1:
      analogWrite(redLED, 255);
      break;
    case 0:
      if (seconds < 30){
        analogWrite(redLED, 200-seconds*6);
      }else{
        analogWrite(redLED, 26+seconds*3);
      }
      break;
  }
  
}

//Constant red, with white flashes
void pattern1 (int beatState){
    clearLEDs();
    switch(beatState){
      case 1:
        analogWrite(redLED, 255);
        analogWrite(greenLED, 255);
        analogWrite(blueLED, 255);
        break;
      case 0:
        analogWrite(redLED, 206);
        break;
    }
}

//Fade red out into blue. White flashes
void pattern2 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
        analogWrite(redLED, 255);
        analogWrite(greenLED, 255);
        analogWrite(blueLED, 255);
        break;
      case 0:
        if (seconds < 30){
         analogWrite(redLED, 206-seconds*7);
         analogWrite(blueLED, seconds*7);
        }else{
         analogWrite(redLED, 0);
         analogWrite(blueLED, 203);
        }
        break;
    }
}

//fade from blue into green. white flashes
void pattern3 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
        analogWrite(redLED, 255);
        analogWrite(greenLED, 255);
        analogWrite(blueLED, 255);
        break;
      case 0:
        if (seconds < 30){
         analogWrite(blueLED, 206-seconds*7);
         analogWrite(greenLED, seconds*7);
        }else{
         analogWrite(blueLED, 0);
         analogWrite(greenLED, 203);
        }
        break;
    }
}

//constant green, flashes fade to green
void pattern4 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
        if (seconds < 30){
         analogWrite(redLED, 255-seconds*8);
         analogWrite(greenLED, 255);
         analogWrite(blueLED, 255-seconds*8);
        }else{
         analogWrite(greenLED, 255);
        }
        break;
      case 0:
        analogWrite(greenLED, 203); 
        break;
    }
}

//constant green, flashes fade to red
void pattern5 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, seconds*4);
         analogWrite(greenLED, 255-seconds*4);
        break;
      case 0:
        analogWrite(greenLED, 203); 
        break;
    }
}

//fade from green to red, red flashes
void pattern6 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 255); 
        break;
      case 0:
        if (seconds < 30){
         analogWrite(greenLED, 206-seconds*7);
         analogWrite(redLED, seconds*7);
        }else{
         analogWrite(greenLED, 0);
         analogWrite(redLED, 203);
        } 
        break;
    }
}

//constand red, flashes dim red
void pattern7 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 50); 
        break;
      case 0:
          analogWrite(redLED, 255);
        break;
    }
}

//same as before, but fade blue in
void pattern8 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 60);
         analogWrite(blueLED, seconds);
        break;
      case 0:
          analogWrite(redLED, 255);
          analogWrite(blueLED, seconds*4);
        break;
    }
}

//same as before, fade out the red
void pattern9 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 60-seconds);
         analogWrite(blueLED, 60);
        break;
      case 0:
          analogWrite(redLED, 255-seconds*4);
          analogWrite(blueLED, 255);
        break;
    }
}

//same as before, fade in green
void pattern10 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(blueLED, 60);
         analogWrite(greenLED, seconds);
        break;
      case 0:
          analogWrite(blueLED, 255);
          analogWrite(greenLED, seconds*4);
        break;
    }
}

//same as before, fade out blue
void pattern11 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(blueLED, 60-seconds);
         analogWrite(greenLED, 60);
        break;
      case 0:
          analogWrite(blueLED, 255-seconds*4);
          analogWrite(greenLED, 255);
        break;
    }
}

//fade to white, keep flash the same
void pattern12 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 60);
         analogWrite(greenLED, 60);
         analogWrite(blueLED, 60);
        break;
      case 0:
          analogWrite(redLED, seconds*4);
          analogWrite(blueLED, seconds*4);
          analogWrite(greenLED, 255);
        break;
    }
}

//constant white, fade flash to blue
void pattern13 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 60-seconds);
         analogWrite(greenLED, 60-seconds);
         analogWrite(blueLED, 60);
        break;
      case 0:
          analogWrite(redLED, 255);
          analogWrite(blueLED, 255);
          analogWrite(greenLED, 255);
        break;
    }
}

//consant white, fade flash to green
void pattern14 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 0);
         analogWrite(greenLED, seconds);
         analogWrite(blueLED, 60-seconds);
        break;
      case 0:
          analogWrite(redLED, 255);
          analogWrite(blueLED, 255);
          analogWrite(greenLED, 255);
        break;
    }
}

//constant white, fade flash to red
void pattern15 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, seconds);
         analogWrite(greenLED, 60-seconds);
         analogWrite(blueLED, 0);
        break;
      case 0:
          analogWrite(redLED, 255);
          analogWrite(blueLED, 255);
          analogWrite(greenLED, 255);
        break;
    }
}

//fade background to red, keep flash at dim red
void pattern16 (int beatState){
  clearLEDs();
    switch(beatState){
      case 1:
         analogWrite(redLED, 60);
        break;
      case 0:
          analogWrite(redLED, 255-seconds);
          analogWrite(blueLED, 240-seconds*4);
          analogWrite(greenLED, 240-seconds*4);
        break;
    }
}
//----------------------------------------------------------

void loop() {

    
    
    unsigned long time = micros(); // Used to track rate
    float sample, value, envelope, beat, thresh;
    unsigned char i;

    for(i = 0;;++i){

      //The following code allows for the patturn on the LEDs to change after a predetermined amount of time
      counter++;   
      if (counter == 5000){
       counter = 0;
        seconds++;
        Serial.println(seconds);
        if (seconds == 60){
          seconds = 0;
          minutes++;
          if (minutes == timeout){
            pattern++;
            minutes = 0;
            if (pattern == numPatterns){
              pattern = 0;
            }
            Serial.print("Pattern: ");
            Serial.println(pattern);
          }
        }
      }
      
        // Read ADC and center so +-512
        sample = (float)analogRead(0)-503.f;

        // Filter only bass component
        value = bassFilter(sample);

        // Take signal amplitude and filter
        if(value < 0)value=-value;
        envelope = envelopeFilter(value);

        // Every 200 samples (25hz) filter the envelope 
        if(i == 200) {
                // Filter out repeating bass sounds 100 - 180bpm
                beat = beatFilter(envelope);

                // Threshold it based on potentiometer on AN1
                thresh = 0.02f * (float)analogRead(1);


//------------------------------------------------------------------------
                  //this code controls the initiation of patterns
                  if(beat > thresh){
                    //beat has occured. beatState = 1;

                    switch (pattern){
                      case 0:
                        pattern0(1);
                        break;
                      case 1:
                        pattern1(1);
                        break;
                      case 2:
                        pattern2(1);
                        break;
                      case 3:
                        pattern3(1);
                        break;
                      case 4:
                        pattern4(1);
                        break;
                      case 5:
                        pattern5(1);
                        break;
                      case 6:
                        pattern6(1);
                        break;
                      case 7:
                        pattern7(1);
                        break;
                      case 8:
                        pattern8(1);
                        break;
                      case 9:
                        pattern9(1);
                        break;
                      case 10:
                        pattern10(1);
                        break;
                      case 11:
                        pattern11(1);
                        break;
                      case 12:
                        pattern12(1);
                        break;
                      case 13:
                        pattern13(1);
                        break;
                      case 14:
                        pattern14(1);
                        break;
                      case 15:
                        pattern15(1);
                        break;
                      case 16:
                        pattern16(1);
                        break;
                    }
                    
                  }
                  else{
                    //no beat occured. beatState = 0;

                    switch (pattern){
                      case 0:
                        pattern0(0);
                        break;
                      case 1:
                        pattern1(0);
                        break;
                      case 2:
                        pattern2(0);
                        break;
                      case 3:
                        pattern3(0);
                        break;
                      case 4:
                        pattern4(0);
                        break;
                      case 5:
                        pattern5(0);
                        break;
                      case 6:
                        pattern6(0);
                        break;
                      case 7:
                        pattern7(0);
                        break;
                      case 8:
                        pattern8(0);
                        break;
                      case 9:
                        pattern9(0);
                        break;
                      case 10:
                        pattern10(0);
                        break;
                      case 11:
                        pattern11(0);
                        break;
                      case 12:
                        pattern12(0);
                        break;
                      case 13:
                        pattern13(0);
                        break;
                      case 14:
                        pattern14(0);
                        break;
                      case 15:
                        pattern15(0);
                        break;
                      case 16:
                        pattern16(0);
                        break;
                    }
                    
                  }
//------------------------------------------------------------------------

                //Reset sample counter
                i = 0;
        }

        // Consume excess clock cycles, to keep at 5000 hz
        for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
    }  
}
