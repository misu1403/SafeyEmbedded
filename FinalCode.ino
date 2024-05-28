#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most accurate BPM math
#include <PulseSensorPlayground.h> 
#include <SoftwareSerial.h>    
#include "DHT.h"

#define DHTPIN 2  
#define DHTTYPE DHT22      
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial B(0, 1);

const int PulseWire = 0;       // 'S' Signal pin connected to A0
const int LED13 = 13;          // The on-board Arduino LED
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore
                               
PulseSensorPlayground pulseSensor;  // Creates an object

unsigned long lastDHTTransmissionTime = 0;   // Last transmission time for DHT data
unsigned long lastPulseTransmissionTime = 0; // Last transmission time for pulse data
unsigned long lastECGTransmissionTime = 0;   // Last transmission time for ECG data

void setup() {
  B.begin(115200);
  dht.begin();

  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -

  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       
  pulseSensor.setThreshold(Threshold);   

  if (pulseSensor.begin()) {
    Serial.println("PulseSensor object created!");
  }
}

void loop() {
  readAndTransmitDHTData();   // Read and transmit DHT data every 10 seconds
  readAndTransmitPulseData(); // Read and transmit pulse data every second
  //readAndTransmitECGData();   // Read and transmit ECG data every 10 ms
}

void readAndTransmitDHTData() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDHTTransmissionTime >= 10000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    B.print("Humidity: ");
    B.print(h);
    B.print("% Temperature: ");
    B.print(t);
    B.println(" C");
    lastDHTTransmissionTime = currentMillis;
  }
}

void readAndTransmitPulseData() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPulseTransmissionTime >= 1000) {
    int myBPM = pulseSensor.getBeatsPerMinute();
    if (pulseSensor.sawStartOfBeat()) {
      B.print("Heartbeat detected! BPM: ");
      B.println(myBPM);
    }
    lastPulseTransmissionTime = currentMillis;
  }
}

void readAndTransmitECGData() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastECGTransmissionTime >= 10) { // 10 ms interval
    int ecgValue = analogRead(PulseWire);
    B.print("ECG value: ");
    B.println(ecgValue);
    lastECGTransmissionTime = currentMillis;
  }
}