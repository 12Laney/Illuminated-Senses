//Illuminated Senses, Spring 2019
//Delaney Winkler
//This project was created for a sensor driven dress, which reacts to a sound sensor, pulse monitor, and color sensor.
//Resources:
//COLOR SENSOR: https://learn.adafruit.com/adafruit-color-sensors/arduino-code
//and color sensor example code
//SOUND SENSOR: https://learn.sparkfun.com/tutorials/sound-detector-hookup-guide?_ga=2.74016722.1921237983.1556662606-1675033028.1556048510
//Pulse Sensor: https://github.com/WorldFamousElectronics/PulseSensorStarterProject 


#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN_ANALOG_IN A3 //SOUND SENSOR

Adafruit_NeoPixel heartVeins = Adafruit_NeoPixel(40, 11, NEO_GRB + NEO_KHZ800);//lights in the bodice
Adafruit_NeoPixel skirtVeins = Adafruit_NeoPixel(72, A4, NEO_GRB + NEO_KHZ800);//lights in the skirt
Adafruit_NeoPixel buttons = Adafruit_NeoPixel(6, 10, NEO_GRB + NEO_KHZ800);//lights in the 6 "buttons"
int RECV_PIN = 9; //for the IR reciever
int sensorVal;
int PulseSensorPurplePin = A2;
int Signal;// holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 500;// Determine which Signal to "count as a beat", and which to ingore.
byte gammatable[256];
IRrecv irrecv(RECV_PIN);
decode_results results; //decodes hex value from the remote
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
//Serial.println("Begining IR");//Serial print for testing
  irrecv.enableIRIn();
//Serial.println("Enabled IR"); //Serial print for testing
  heartVeins.begin();
  skirtVeins.begin();
  buttons.begin();
  heartVeins.show();
  skirtVeins.show();
  buttons.show();

  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255; 
    gammatable[i] = x;      
  }
}

void loop() {
  //Goes into the loop if IR hex values are being recieved
   if (irrecv.decode(&results)) {
//    Serial.println(results.value, HEX); Serial print for testing
      switch(results.value){
         case 0xFF22DD: //value for the color sensor, button A
            ColorSensorCode();
            break;
         case 0xFF02FD: //value for heart rate sensor, button B
            HeartSensorCode();
            break;
         case 0xFFC23D://value for sound sensor, button C
            SoundSensorCode();
            break;
         case 0xFF629D:// Turn all lights off, PWR button on remote
            LightsOut();
            break;
         case 0xFF38C7://Easy Rainbow flow
            RainbowCycle(20);
            break;
         case 0xFF9867://Crazy lights
//            ColorsGoneBonkers();
            break;
      }
      irrecv.resume(); //Continues to listen for remote hex values
  }
}

void ColorSensorCode(){
  uint16_t clear, red, green, blue;
  tcs.setInterrupt(false);      // turn on LED in color sensor to start the color sensing process
  delay(60);  // takes 50ms to read 
  tcs.getRawData(&red, &green, &blue, &clear);//getting the color values
  tcs.setInterrupt(true);  // turn off LED in color sensor
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  //sum += clear; // clear contains RGB already so no need to re-add it
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();
 
  Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
  colorWipe(buttons.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
  colorWipe(skirtVeins.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
  colorWipe(heartVeins.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]), 0);
  delay(3000);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<buttons.numPixels(); i++) {
      buttons.setPixelColor(i, c);
      buttons.show();
      delay(wait);
  }
  for(uint16_t i=0; i<skirtVeins.numPixels(); i++) {
      skirtVeins.setPixelColor(i, c);
      skirtVeins.show();
      delay(wait);
  }
  for(uint16_t i=0; i<heartVeins.numPixels(); i++) {
      heartVeins.setPixelColor(i, c);
      heartVeins.show();
      delay(wait);
  }
}
void HeartSensorCode(){
  tcs.setInterrupt(true);  // turn off LED in color sensor
  Signal = analogRead(PulseSensorPurplePin);
  Serial.println(Signal);
  for(int k=0;k<6;k++){
    buttons.setPixelColor(k, buttons.Color(150,0,0)); 
    buttons.show();
  }
  if(Signal>Threshold){ 
  for(int i=0;i<16;i++){
    heartVeins.setPixelColor(i, heartVeins.Color(0,0,0)); 
    heartVeins.show();
  //    delay(20);
    }
    heartVeins.setPixelColor(16,0,0,105);
    heartVeins.setPixelColor(18,0,0,105);
    heartVeins.setPixelColor(20,0,0,105);
    heartVeins.setPixelColor(22,0,0,105);
    heartVeins.setPixelColor(24,0,0,105);

    heartVeins.setPixelColor(17,150,0,0);
    heartVeins.setPixelColor(19,150,0,0);
    heartVeins.setPixelColor(21,150,0,0);
    heartVeins.setPixelColor(23,150,0,0);
    heartVeins.setPixelColor(25,150,0,0);
    heartVeins.show();

    for(int j=0;j<48;j++){
      skirtVeins.setPixelColor(j,150,0,0);
      skirtVeins.show();
    }
    for(int l=48;l<72;l++){
      skirtVeins.setPixelColor(l,50,0,150);
      skirtVeins.show();
    }
    
  }else{
    heartVeins.setPixelColor(16,75,0,0);
    heartVeins.setPixelColor(18,75,0,0);
    heartVeins.setPixelColor(20,75,0,0);
    heartVeins.setPixelColor(22,75,0,0);
    heartVeins.setPixelColor(24,75,0,0);

    heartVeins.setPixelColor(17,0,0,0);
    heartVeins.setPixelColor(19,0,0,0);
    heartVeins.setPixelColor(21,0,0,0);
    heartVeins.setPixelColor(23,0,0,0);
    heartVeins.setPixelColor(25,0,0,0);
    heartVeins.show();
    for(int m=49;m<72;m++){
      skirtVeins.setPixelColor(m,75,0,0);
      skirtVeins.show();
    }
    for(int k=0;k<72;k++){
      skirtVeins.setPixelColor(k,0,0,0);
      skirtVeins.show();
    }
  }
}
void SoundSensorCode(){
  tcs.setInterrupt(true);  // turn off LED in color sensor
  sensorVal= analogRead(PIN_ANALOG_IN);
  Serial.print("Sensor Value: ");
  Serial.println(sensorVal);

  if (sensorVal < 30){
    for(int i=0;i<40;i++){
      heartVeins.setPixelColor(i, heartVeins.Color(90,90,0)); 
      heartVeins.show();
    }
   for(int j=0;j<72;j++){
      skirtVeins.setPixelColor(j, skirtVeins.Color(90,90,0)); 
      skirtVeins.show();
    }
   for(int k=0;k<6;k++){
      buttons.setPixelColor(k, buttons.Color(90,90,0)); 
      buttons.show();
    }
  }else if ((sensorVal >= 30) && (sensorVal <45)){//Set bottom row to green
    greenLevel();
    for(int k=0;k<6;k++){
    buttons.setPixelColor(k, buttons.Color(0,150,0)); 
    buttons.show();
    }
    for(int i=0;i<40;i++){
      heartVeins.setPixelColor(i, heartVeins.Color(150,150,0)); 
      heartVeins.show();
    }
    
  }else if ((sensorVal >=45) &&(sensorVal<60)){ //sets level of skirt to a blue color
    greenLevel();
    blueLevel(); 
    for(int k=0;k<6;k++){
      buttons.setPixelColor(k, buttons.Color(0,0,150)); 
      buttons.show();
    }
    for(int i=0;i<40;i++){
      heartVeins.setPixelColor(i, heartVeins.Color(0,0,150)); 
      heartVeins.show();
    }
    
  }else if ((sensorVal >=60) &&(sensorVal<75)){
    greenLevel();
    blueLevel();
    orangeLevel();
    for(int k=0;k<6;k++){
      buttons.setPixelColor(k, buttons.Color(90,70,0)); 
      buttons.show();
    }
    for(int i=0;i<40;i++){
      heartVeins.setPixelColor(i, heartVeins.Color(90,70,0)); 
      heartVeins.show();
    }
  }else if(sensorVal>=75){
    skirtVeins.setPixelColor(0,150,0,0);
    skirtVeins.setPixelColor(6,150,0,0);
    skirtVeins.setPixelColor(12,150,0,0);
    skirtVeins.setPixelColor(18,150,0,0);
    skirtVeins.setPixelColor(24,150,0,0);
    skirtVeins.setPixelColor(30,150,0,0);
    skirtVeins.setPixelColor(36,150,0,0);
    skirtVeins.setPixelColor(42,150,0,0);
    skirtVeins.setPixelColor(48,150,0,0);
    skirtVeins.show();

    for(int k=0;k<6;k++){
    buttons.setPixelColor(k, buttons.Color(150,0,0)); 
    buttons.show();
    }
    for(int i=0;i<40;i++){
      heartVeins.setPixelColor(i, heartVeins.Color(150,0,0)); 
      heartVeins.show();
    }
//    delay(20);
  }
}

void greenLevel(){
  skirtVeins.setPixelColor(5,0,150,0);
    skirtVeins.setPixelColor(11,0,150,0);
    skirtVeins.setPixelColor(17,0,150,0);
    skirtVeins.setPixelColor(23,0,150,0);
    skirtVeins.setPixelColor(29,0,150,0);
    skirtVeins.setPixelColor(35,0,150,0);
    skirtVeins.setPixelColor(41,0,150,0);
    skirtVeins.setPixelColor(72,0,150,0);

    skirtVeins.setPixelColor(4,0,150,0);
    skirtVeins.setPixelColor(10,0,150,0);
    skirtVeins.setPixelColor(16,0,150,0);
    skirtVeins.setPixelColor(22,0,150,0);
    skirtVeins.setPixelColor(28,0,150,0);
    skirtVeins.setPixelColor(34,0,150,0);
    skirtVeins.setPixelColor(40,0,150,0);
    skirtVeins.setPixelColor(46,0,150,0);

    skirtVeins.setPixelColor(51,0,150,0); // botom layer green
    skirtVeins.setPixelColor(55,0,150,0);
    skirtVeins.setPixelColor(59,0,150,0);
    skirtVeins.setPixelColor(63,0,150,0);
    skirtVeins.setPixelColor(67,0,150,0);
    skirtVeins.setPixelColor(71,0,150,0);
    skirtVeins.show();
//    delay(20);
}
void blueLevel(){
    skirtVeins.setPixelColor(2,0,0,150);
    skirtVeins.setPixelColor(3,0,0,150);
    skirtVeins.setPixelColor(8,0,0,150);
    skirtVeins.setPixelColor(10,0,0,150);
    skirtVeins.setPixelColor(14,0,0,150);
    skirtVeins.setPixelColor(15,0,0,150);
    skirtVeins.setPixelColor(20,0,0,150);
    skirtVeins.setPixelColor(21,0,0,150);

    skirtVeins.setPixelColor(26,0,0,150);
    skirtVeins.setPixelColor(27,0,0,150);
    skirtVeins.setPixelColor(32,0,0,150);
    skirtVeins.setPixelColor(33,0,0,150);
    skirtVeins.setPixelColor(38,0,0,150);
    skirtVeins.setPixelColor(39,0,0,150);
    skirtVeins.setPixelColor(44,0,0,150);
    skirtVeins.setPixelColor(45,0,0,150);

    skirtVeins.setPixelColor(50,0,0,150);
    skirtVeins.setPixelColor(54,0,0,150);
    skirtVeins.setPixelColor(58,0,0,150);
    skirtVeins.setPixelColor(62,0,0,150);
    skirtVeins.setPixelColor(66,0,0,150);
    skirtVeins.setPixelColor(70,0,0,150);
    skirtVeins.show();
//    delay(20);
}
void orangeLevel(){
    skirtVeins.setPixelColor(1,90,70,0);
    skirtVeins.setPixelColor(7,90,70,0);
    skirtVeins.setPixelColor(13,90,70,0);
    skirtVeins.setPixelColor(19,90,70,0);
    skirtVeins.setPixelColor(25,90,70,0);
    skirtVeins.setPixelColor(31,90,70,0);
    skirtVeins.setPixelColor(37,90,70,0);
    skirtVeins.setPixelColor(43,90,70,0);

    skirtVeins.setPixelColor(49,90,70,0);
    skirtVeins.setPixelColor(53,90,70,0);
    skirtVeins.setPixelColor(57,90,70,0);
    skirtVeins.setPixelColor(61,90,70,0);
    skirtVeins.setPixelColor(65,90,70,0);
    skirtVeins.setPixelColor(69,90,70,0);

    skirtVeins.setPixelColor(48,90,70,0);
    skirtVeins.setPixelColor(52,90,70,0);
    skirtVeins.setPixelColor(56,90,70,0);
    skirtVeins.setPixelColor(60,90,70,0);
    skirtVeins.setPixelColor(64,90,70,0);
    skirtVeins.setPixelColor(68,90,70,0);
    skirtVeins.show();
//    delay(20);
}
void LightsOut(){
  tcs.setInterrupt(true);  // turn off LED in color sensor
  for(int i=0;i<40;i++){
    heartVeins.setPixelColor(i, heartVeins.Color(0,0,0)); //The 90 degree lights in the bodice
    heartVeins.show();
  }
   for(int j=0;j<72;j++){
    skirtVeins.setPixelColor(j, skirtVeins.Color(0,0,0)); //The lights in the skirt
    skirtVeins.show();
  }
   for(int k=0;k<6;k++){
    buttons.setPixelColor(k, buttons.Color(0,0,0)); //The "button" lights
    buttons.show();
  }
}
void RainbowCycle(uint8_t wait){
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< skirtVeins.numPixels(); i++) {
      skirtVeins.setPixelColor(i, Wheel(((i * 256 / skirtVeins.numPixels()) + j) & 255));
    }
    for(i=0; i< heartVeins.numPixels(); i++) {
      heartVeins.setPixelColor(i, Wheel(((i * 256 / heartVeins.numPixels()) + j) & 255));
    }
    for(i=0; i< buttons.numPixels(); i++) {
      buttons.setPixelColor(i, Wheel(((i * 256 / buttons.numPixels()) + j) & 255));
    }
    skirtVeins.show();
    heartVeins.show();
    buttons.show();
    delay(wait);
  }
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return skirtVeins.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    return heartVeins.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    return buttons.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return skirtVeins.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    return heartVeins.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    return buttons.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  WheelPos -= 170;
  return skirtVeins.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  return heartVeins.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  return buttons.Color(255 - WheelPos * 3, 0, WheelPos * 3);
}
