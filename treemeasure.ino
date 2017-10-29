#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <SD.h>

const char* ssid = "";
const char* password = "";

const int ledPin = 0; // blinking led pin
bool ledState = 0;
unsigned long previousMillis = 0;
const int blinkONInterval = 2000; //reversed on esp red led
const int blinkOFFInterval = 100;

const int chipSelect = 4;
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  setupWIFI();
  setupOTA();
  setupSD();

  pinMode(ledPin, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  blink();
}

void setupWIFI(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void setupOTA(){
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupSD(){
  Serial.print("Initializing SD card...");
  
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      return;
    }
    Serial.println("card initialized.");
}

//################################################################
void blink(){
	unsigned long currentMillis = millis();

	if(ledState == 0) {
		if (currentMillis - previousMillis >= blinkOFFInterval) {
			previousMillis = currentMillis;
			ledState = 1;
		}
	} else {
		if(currentMillis - previousMillis >= blinkONInterval) {
			previousMillis = currentMillis;
      ledState = 0;
		}
	}

	// set the LED with the ledState of the variable:
	digitalWrite(ledPin, ledState);

}

void writeFile(String dataString){
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}