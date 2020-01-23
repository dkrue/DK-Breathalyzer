int digit1 = 16;
int digit2 = 14; 
int digit3 = 9; 
int digit4 = 2;

int segA = 10;
int segB = 8;
int segC = 4;
int segD = 6;
int segE = 7;
int segF = 15;
int segG = 3;
int dec  = 5;

#define SENSOR_PIN  A2
#define BAC_START   100     // Beginning ADC value of BAC chart
#define BAC_END     550     // Last ADC value in BAC chart - display EEEE over this value

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH
#define DISPLAY_BRIGHTNESS  500

const uint8_t bac_chart[] = 
{2, 3, 4, 5, 6, 6, 7, 8, 8, 9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 15, 16, 
16, 16, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 
23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 
26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 
29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 
31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 
33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 
35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 
38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42, 
42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 
45, 46, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 49, 49, 
49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 
53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56, 56, 
56, 57, 57, 57, 57, 58, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 
61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 
67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 
76, 76, 77, 78, 78, 79, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 87, 87, 88, 
89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 
106, 107, 108, 109, 111, 112, 113, 114, 116, 117, 118, 120, 121, 122, 124, 125, 
127, 128, 130, 131, 133, 135, 136, 138, 140, 142, 143, 145, 147, 149, 151, 153, 
155, 157, 159, 161, 163, 165, 168, 170, 172, 174, 177, 179, 182, 184, 187, 189, 
192, 194};

// Global variables
char temp_str[10];
int sensor_read;
int sensor_refresh_counter;
uint8_t bac;


void setup() {   
               
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  pinMode(dec, OUTPUT);

  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {

  // Read sensor if we are due for a refresh
  if(sensor_refresh_counter == 0) {
    
    sensor_read = analogRead(SENSOR_PIN);
    Serial.println(sensor_read);
  }

  // Increment refresh counter, don't refresh every time to stabilize display
  sensor_refresh_counter++;
  if(sensor_refresh_counter > 10) {
    sensor_refresh_counter = 0;
  }

  // Calculate ppm. Regression fitting from MQ-3 datasheet.
  // Equation using 5V max ADC and RL = 4.7k. "v" is voltage.
  // PPM = 150.4351049*v^5 - 2244.75988*v^4 + 13308.5139*v^3 - 
  //       39136.08594*v^2 + 57082.6258*v - 32982.05333
  // Calculate BAC. See BAC/ppm chart from page 2 of:
  // http://sgx.cdistore.com/datasheets/sgx/AN4-Using-MiCS-Sensors-for-Alcohol-Detection1.pdf
  // All of this was put into the lookup table in BAC_Lookup.h
  if ( sensor_read < BAC_START ) {
    displayNumber(0);
  } else if ( sensor_read > BAC_END ) {
    displayError();
  } else {
    displayNumber(bac_chart[sensor_read - BAC_START]);
  }
}

//Given a number, we display 10:22
//After running through the 4 numbers, the display is left turned off

//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//If each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)

void displayNumber(int toDisplay) {

  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, HIGH);
      digitalWrite(dec, SEGMENT_ON); // static decimal after the first digit
      break;
    case 2:
      digitalWrite(digit2, HIGH);
      break;
    case 3:
      digitalWrite(digit3, HIGH);
      break;
    case 4:
      digitalWrite(digit4, HIGH);
      break;
    }

    //Turn on the right segments for this digit
    lightNumber(toDisplay % 10);
    toDisplay /= 10;

    delayMicroseconds(DISPLAY_BRIGHTNESS); //Display this digit for a fraction of a second (between 1us and 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, LOW);
    digitalWrite(digit2, LOW);
    digitalWrite(digit3, LOW);
    digitalWrite(digit4, LOW);
    digitalWrite(dec, SEGMENT_OFF);
  }

  while( (millis() - beginTime) < 10) ; //Wait for 10ms to pass before we paint the display again
}

void displayError() {

  long beginTime = millis();

  for(int digit = 4 ; digit > 0 ; digit--) {

    //Turn on a digit for a short amount of time
    switch(digit) {
    case 1:
      digitalWrite(digit1, HIGH);
      break;
    case 2:
      digitalWrite(digit2, HIGH);
      break;
    case 3:
      digitalWrite(digit3, HIGH);
      break;
    case 4:
      digitalWrite(digit4, HIGH);
      break;
    }

    //Display letter 'E' for this digit
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);

    delayMicroseconds(DISPLAY_BRIGHTNESS); //Display this digit for a fraction of a second (between 1us and 5000us, 500 is pretty good)

    //Turn off all segments
    lightNumber(10); 

    //Turn off all digits
    digitalWrite(digit1, LOW);
    digitalWrite(digit2, LOW);
    digitalWrite(digit3, LOW);
    digitalWrite(digit4, LOW);
  }

  while( (millis() - beginTime) < 10) ; //Wait for 10ms to pass before we paint the display again
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void lightNumber(int numberToDisplay) {

  switch (numberToDisplay){

  case 0:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 1:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 2:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 3:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 4:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 5:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 6:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 7:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;

  case 8:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_ON);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 9:
    digitalWrite(segA, SEGMENT_ON);
    digitalWrite(segB, SEGMENT_ON);
    digitalWrite(segC, SEGMENT_ON);
    digitalWrite(segD, SEGMENT_ON);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_ON);
    digitalWrite(segG, SEGMENT_ON);
    break;

  case 10:
    digitalWrite(segA, SEGMENT_OFF);
    digitalWrite(segB, SEGMENT_OFF);
    digitalWrite(segC, SEGMENT_OFF);
    digitalWrite(segD, SEGMENT_OFF);
    digitalWrite(segE, SEGMENT_OFF);
    digitalWrite(segF, SEGMENT_OFF);
    digitalWrite(segG, SEGMENT_OFF);
    break;
  }
}

