#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   22
#define SS_PIN    21

#define red_led   2
#define blue_led  4
#define buzzer    15
#define button_A  12
#define button_B  13
MFRC522 mfrc522(SS_PIN, RST_PIN);
BlynkTimer timer;

char auth[] = "CCL4MZ3pikT2GSQrILWR7CrL08yedXgB";
char ssid[] = "POCOrj";
char pass[] = "123456789";

String content="";
unsigned long currentMillis;
bool state = false;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 2000;
bool ledState = LOW;
bool cas = false;
bool cas2 = false;
bool button = false;
String oneLine;
String sa[10];
int r=0, t=0,count=0;

WidgetLCD lcd(V4);

void myTimerEvent()
{
  Blynk.virtualWrite(V4, millis() / 1000);
}


BLYNK_WRITE(V5){
 oneLine = param.asStr(); 
 count=0;
for (int i=0; i < oneLine.length(); i++){ 
 if(oneLine.charAt(i) == ';'){ 
    sa[t] = oneLine.substring(r, i);//.toInt();
     count++;
    r=(i+1); 
    t++; 
  }
}
}
void setup()
{
  Serial.begin(115200);
  SPI.begin();        
  mfrc522.PCD_Init();
  
  ledcAttachPin(red_led, 1); 
  ledcSetup(1, 1, 8);
  pinMode(buzzer,OUTPUT);
  pinMode(blue_led,OUTPUT);
  pinMode(button_A,INPUT);
  pinMode(button_B,INPUT);
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
  lcd.clear();
}
void loop()
{
  currentMillis = millis();
  Blynk.run();
  timer.run();
     button=(digitalRead(12) || digitalRead(13));
     state=check();
    /* if( state && button){
       Serial.println(state);
        lcd.print(0,0,content);
        lcd.print(4,1,"name");
     }*/
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
  for(int i=0;i<count;i++)
  if (content.substring(1) == sa[i])
  {
    Serial.println("This is the right tag");
    return 1;
  }
    return 0;
}
void flash(){
 
  if(state && !cas2){
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
    lcd.print(0,0,content);
    lcd.print(0,1,"deactivated");
  }
  else {
    ledState = LOW;
     cas = false;
  }
  if(!button && !state && !cas && !cas2){
    ledcWrite(1, 20);
    lcd.clear();
  }
  if (currentMillis - previousMillis2 <= interval && cas2) {
    ledcWrite(1, 255);
    digitalWrite(buzzer,HIGH);
    lcd.print(0,0,"caution");
    lcd.print(0,1,"detonated");
  }
  else {
     cas2 = false;
     digitalWrite(buzzer,LOW);
  }
  
  digitalWrite(blue_led, ledState);
  Serial.println("  ");
  Serial.print(state);
  Serial.print("  ");
  Serial.println(button);
}
