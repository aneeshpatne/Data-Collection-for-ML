#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SFE_BMP180.h>
#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
SFE_BMP180 pressure;
BH1750 GY30;
#define ALTITUDE 943.7
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#include "DHT.h"

#define DHTTYPE DHT11 
const int DHTPin = D3; 
DHT dht(DHTPin, DHTTYPE); 

#define ON_Board_LED 2  

const char* ssid = "iPhone"; 
const char* password = "12345678"; 



const char* host = "script.google.com";
const int httpsPort = 443;


WiFiClientSecure client; 

String GAS_ID = "AKfycbwFpVxwCvKm8WvxWelWZoOJlOgJMo3Guqumyvhe3IyukDOCJBOyYNUCV-7GriK0a2DY"; //--> spreadsheet script ID


void setup() {
 Wire.begin();
pressure.begin();
  GY30.begin();

  Serial.begin(115200);
  delay(500);

  dht.begin();  
  delay(500);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);         
  oled.clearDisplay(); 
  
  WiFi.begin(ssid, password); 
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH); 

  
  Serial.print("Connecting");
 oled_text("Connecting");               
  while (WiFi.status() != WL_CONNECTED) {
    
    Serial.print(".");
    
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    
  }
  
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
   oled_text("CONNECTED");
  
  if (pressure.begin())
Serial.println("BMP180 init success");
else
{
Serial.println("BMP180 init fail\n\n");
Serial.println("Check connection");
}
}


void loop() {

  Wire.beginTransmission(0x77);
  char status;
  double T, P, p0, a;
  int h = dht.readHumidity();
  float t = dht.readTemperature();
  status = pressure.startTemperature();
  status = pressure.getTemperature(T);
  status = pressure.startPressure(3);
  
 
status = pressure.getPressure(P, T);
if (status = 0)
{
Serial.println("error starting pressure measurement\n");
}
Wire.endTransmission();

Wire.beginTransmission(0x77);
float lux = GY30.readLightLevel();
Wire.endTransmission();

  if (isnan(h) || isnan(t) ||isnan(P) || isnan(lux)) {
    Serial.println("Sensors not connected properly");
    delay(500);
    oled_text("CHECK WIRE!");               
    return;
  }
  if (lux == -2){
     Serial.println("I2C ERROR");
    delay(500);
    oled_text("I2C ERROR!");               
    return;
    }
  String Temp = "Temperature : " + String(t) + " °C";
  String Humi = "Humidity : " + String(h) + " %";
  String Pressure = "Pressure : " + String(P);
  String Light = "Light Intensity: " + String(lux) ;
  Serial.println(Temp);
  Serial.println(Humi);
  Serial.println(Pressure);
  Serial.println(Light);
  sendData(t, h, P,lux); 
    delay(2000);         
  oled.clearDisplay(); 

  oled.setTextSize(2);          
  oled.setTextColor(WHITE);     
  oled.setCursor(0, 1);        
  oled.println(t); 
  oled.setCursor(0, 20);        
  oled.println(h);
  oled.setCursor(0, 40);        
  oled.println(lux);
  oled.startscrollright(0x00, 0x0F); 
  oled.display();               
}



void sendData(float tem, int hum,float Pressure, float lux) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    oled_text("connection failed");
    return;
  }
  

  
  String string_temperature =  String(tem);
  String string_pressure = String(Pressure);
  // String string_temperature =  String(tem, DEC); 
  String string_humidity =  String(hum, DEC); 
  String light_intensity = String(lux);
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity+"&pressure="+string_pressure+"&light=" + light_intensity;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  

  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has conditionally accepted");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();


    
    digitalWrite(ON_Board_LED, HIGH);

  delay(1000);
   oled_text("updating..");
     digitalWrite(ON_Board_LED, LOW);
  //----------------------------------------
} 

int oled_text(String data){
  oled.clearDisplay(); 
  oled.setTextSize(2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(0, 1);        // position to display
  oled.println(data); 
  oled.startscrollright(0x00, 0x0F); 
  oled.display();               // show on OLED

  }
