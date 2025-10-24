#define BLYNK_TEMPLATE_ID "TMPL65mLBbL1u"
#define BLYNK_TEMPLATE_NAME "Project GP7"
#define BLYNK_AUTH_TOKEN "aShZubT87tIPdFHIB6JJhtPqsIEqxDKD"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "UTeM-IoT";
char pass[] = "123456789";

int led = D2;
int pir = D5;
int ldr = A0;
int relay = D6;

int ldrValue = 0;
int Day = 950; 
float voltage;
int motion;
bool systemEnabled = false;  

BLYNK_WRITE(V6) {
  systemEnabled = param.asInt(); 
  digitalWrite(relay,systemEnabled); 
}

void setup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  digitalWrite(relay,LOW);
}

void loop() {
  Blynk.run();

  if (!systemEnabled) {
    analogWrite(led, 0);  
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V4, "SYSTEM OFF");
    Blynk.virtualWrite(V5, "SYSTEM OFF");
    Blynk.virtualWrite(V7, "SYSTEM OFF"); 
    return;  // Skip loop
  }

  ldrValue = analogRead(ldr);  
  motion = digitalRead(pir);  
  voltage = ldrValue * (3.3 / 1024);

  Serial.print("LDR Value: ");
  Serial.print(ldrValue);
  Serial.print(" voltage: ");
  Serial.println(voltage);

  if (ldrValue > Day) {
    Serial.println(" - MORNING");
    analogWrite(led, 0); 
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V4, "MORNING");
    Blynk.virtualWrite(V5, "...");
  } 
  else {
    Serial.println(" - NIGHT");
    if (motion == HIGH) {
      Serial.println("Movement Detected");
      analogWrite(led, 255);  
      Blynk.virtualWrite(V1, 255);
      Blynk.virtualWrite(V5, "Movement Detected");

      for (int i = 10; i >= 0; i--) {
      Blynk.virtualWrite(V7, i);
      delay(1000);                
  }
    } 
    else {
      analogWrite(led, 50);  
      Blynk.virtualWrite(V1, 50);
      Blynk.virtualWrite(V5, "");
      Blynk.virtualWrite(V4, "NIGHT");
    }  
  }

  Blynk.virtualWrite(V2, ldrValue);
  Blynk.virtualWrite(V3, voltage);
  Blynk.virtualWrite(V7, "...");
  delay(1000);
}
