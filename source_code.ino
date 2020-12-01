#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   22
#define SS_PIN    21

MFRC522 mfrc522(SS_PIN, RST_PIN);

String tagID= "97 09 A1 7A";
char auth[] = "CCL4MZ3pikT2GSQrILWR7CrL08yedXgB";
char ssid[] = "POCOrj";
char pass[] = "123456789";

String content="";
unsigned long currentMillis;
bool state = false;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 2000;
int ledState = LOW;
bool cas = false;
bool cas2 = false;
bool button = false;

WidgetLCD lcd(V4);

void setup()
{
  Serial.begin(115200);
  SPI.begin();        
  mfrc522.PCD_Init();
  
  ledcAttachPin(2, 1); 
  ledcSetup(1, 1, 8);
  pinMode(15,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  
  Blynk.begin(auth, ssid, pass);
  lcd.clear();
}
void loop()
{
  currentMillis = millis();
  Blynk.run();

     button=(digitalRead(12) || digitalRead(13));
     state=check();
     if( state && button){
       Serial.println(state);
        lcd.print(0,0,content);
        lcd.print(4,1,"name");
     }
    flash();
    content="";
}
bool check(){
        if ( ! mfrc522.PICC_IsNewCardPresent())
        return 0;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return 0;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == tagID)
  {
    Serial.println("This is the right tag");
    return 1;
  }
 
 else   {
    Serial.println("Wrong tag");
    return 0;
  }
}
void flash(){
  if(button && state && !cas2){
      previousMillis = currentMillis;
      cas = true;
  }

  if(button && !state && !cas){
      previousMillis2 = currentMillis;
      cas2 = true;
  }
  
  if (currentMillis - previousMillis <= interval && cas) {
    ledState = HIGH;
    ledcWrite(1, 0);
  }
  else {
    ledState = LOW;
     cas = false;
  }
  if(!button && !state && !cas && !cas2)
    ledcWrite(1, 20);

  if (currentMillis - previousMillis2 <= interval && cas2) {
    ledcWrite(1, 255);
    digitalWrite(15,HIGH);
  }
  else {
     cas2 = false;
     digitalWrite(15,LOW);
  }
  digitalWrite(4, ledState);
  Serial.println("  ");
  Serial.print(state);
  Serial.print("  ");
  Serial.println(button);
}
