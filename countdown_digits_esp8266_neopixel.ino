// The Dogg 2021
// Date countdown timer on 7 segment digit using ESP8266 wifi 
// The following calculates the remaining days until Nov 27th 2021

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


// if you just want to use DateTime:
#include <DateTime.h>
#include <TimeLib.h>

#define PIXELS_PER_SEGMENT  1     // Number of LEDs in each Segment
#define PIXELS_DIGITS       2     // Number of connected Digits
#define PIXELS_PIN          D2     // GPIO Pin
#define LOGO_PIN            D1     // GPIO Pin for logo
#define LOGO_NUMBER_PIXEL   4     // Number of LED for logo

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "password";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.au.pool.ntp.org");

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}


Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS_PER_SEGMENT * 7 * PIXELS_DIGITS, PIXELS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel logostrip = Adafruit_NeoPixel(LOGO_NUMBER_PIXEL, LOGO_PIN, NEO_GRB + NEO_KHZ800);

//Pixel Arrangement
/*
          b
        a   c
          g
        f   d
          e
*/

// Segment array
byte segments[7] = {
  //abcdefg
  0b0000001,     // Segment g
  0b0000010,     // Segment f
  0b0000100,     // Segment e
  0b0001000,     // Segment d
  0b0010000,     // Segment c
  0b0100000,     // Segment b
  0b1000000      // Segment a
};

//Digits array
byte digits[10] = {
  //abcdefg
  0b1111110,     // 0
  0b0011000,     // 1
  0b0110111,     // 2
  0b0111101,     // 3
  0b1011001,     // 4
  0b1101101,     // 5
  0b1101111,     // 6
  0b0111000,     // 7
  0b1111111,     // 8
  0b1111001      // 9
};

//Clear all the Pixels
void clearDisplay() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    //logostrip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  logostrip.show();
}

void setup() {
  Serial.begin(115200);
  initWiFi();

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(39600);
  
  strip.begin();
  logostrip.begin();
  
  getCurrentTime();

}

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
 
//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void loop() {
   
  // display LED in logo strip
  logoRainbow(10);


  // calculate remaining days to Nov 27th 2021
  int REM = calDays();

  // split two digits into single digits
  int one = REM / 10;
  int two = REM % 10;
  
  // display LED digits
  disp_Digit1(one);
  disp_Digit2(two);

}


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void logoRainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<logostrip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / logostrip.numPixels());
      logostrip.setPixelColor(i, logostrip.gamma32(logostrip.ColorHSV(pixelHue)));
    }
    logostrip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// powers the logo battery box. 6 is the number of LED's powering the branding box.
void logoStrip() {
  for (int i = 0; i < 6; i++) {
    logostrip.setPixelColor(i, 50, 250, 50);
  }
  logostrip.show();
  delay(1000);

}

//display single and double digits, this is for the first digit.
int value1;
void disp_Digit1(int num) {
  if (num >= 2) {
    writeDigit(0, num,0,255,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Green");
    value1 = 2;
  }
  else if (num >= 1) {
    writeDigit(0, num,255,165,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Orange");
    value1 = 1;
  }
  else {
    writeDigit(0, num,0,0,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Off");
    value1 = 0;
    Serial.print("Value of value1 is ");
    Serial.println(value1);
  }
}

//display single and double digits, this is for the second digit, add more for each additional digit.
void disp_Digit2(int num) {
  if (value1 == 2) {
    writeDigit(1, num,0,255,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Green");
  }
  else if (value1 == 1) {
    writeDigit(1, num,255,165,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Orange");
  }
  else if (value1 == 0) {
    writeDigit(1, num,255,0,0);
    strip.show();
    Serial.print("Color is ");
    Serial.println("Red");
  }
}

void getCurrentTime() {
  
  // Gettime
  timeClient.update();
 
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
 
  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);
 
  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month Number: ");
  Serial.println(currentMonth);
 
  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);
 
  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);
 
  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);
 
  Serial.println("");
}

int getDayNumb() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  Serial.print("Current Month Day: ");
  Serial.println(String(monthDay));
  return monthDay;
}

int getMonthNumb() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int currentMonth = ptm->tm_mon+1;
  Serial.print("Current Month: ");
  Serial.println(String(currentMonth));
  return currentMonth;
}

// calculates the remaining to Nov 27th 2021
int calDays() {
  
  int date1;
  clearDisplays();
  if (getMonthNumb() == 10) {        // for the month of Oct
    if (getDayNumb() <= 31) {
      date1 = (31 - getDayNumb())+27;
      Serial.print("The value of date1 is ");
      Serial.println(date1); 
      return date1;
    }
  }
  else if (getMonthNumb() == 11) {       // for the month of Nov
    if (getDayNumb() <= 27) {
      date1 = 27 - getDayNumb();
      return date1;
    }
  }
  else {
      Serial.println("Error in month catch condition");
      int error=99;
      return error;
  }
}

//Clear all the Pixels
void clearDisplays() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    //logostrip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  logostrip.show();
}

// power each digit
void writeDigit(int index, int val, int r, int g, int b) {
  byte digit = digits[val];
  for (int i = 6; i >= 0; i--) {
    int offset = index * (PIXELS_PER_SEGMENT * 7) + i * PIXELS_PER_SEGMENT;

    uint32_t color;
    if (digit & 0x01 != 0) {
      if (val == 1) color = strip.Color(r,g,b);
      if (val == 2) color = strip.Color(r,g,b);
      if (val == 3) color = strip.Color(r,g,b);
      if (val == 4) color = strip.Color(r,g,b);
      if (val == 5) color = strip.Color(r,g,b);
      if (val == 6) color = strip.Color(r,g,b);
      if (val == 7) color = strip.Color(r,g,b);
      if (val == 8) color = strip.Color(r,g,b);
      if (val == 9) color = strip.Color(r,g,b);
      if (val == 0) color = strip.Color(r,g,b);
    }
    else
      color = strip.Color(0, 0, 0);

    for (int j = offset; j < offset + PIXELS_PER_SEGMENT; j++) {
      strip.setPixelColor(j, color);
    }
    digit = digit >> 1;
  }
}
