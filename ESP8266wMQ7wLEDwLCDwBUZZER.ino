#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); //

// WiFi Connected
#define SSID        "delphi"      // WIFI name
#define PASSWORD    "delphi1234"        // WIFI password

// Line Notify TOKEN 
#define LINE_TOKEN  "Xmq3tXLUEYjt9ka3KNzGrkh6fFxIfkliyqecjgTM85H"   // LINE TOKEN (Bright's) (SmokeDetector)

// Set IFTTT Webhooks event name and key
#define IFTTT_Key "l4h1eLmi34ErJ5rHby-oRHDv0iPsIGWvhXGuUAEa9gr" // IFTTT KEY(Del's)
#define IFTTT_Event "SMOKE" 
WiFiClient client;

int IFTTT_Value1 = 0;

// Define LED LIGHT
#define Rrow1 14 //
#define Rrow2 12 //
#define Rrow3 13 //

// Define BUZZER
#define buzzer 15

// Define Analog A0
int smokeA0 = A0;

void setup() {

  
  // I/O PIN
  pinMode(smokeA0, INPUT);
  
  pinMode(Rrow1, OUTPUT);
  pinMode(Rrow2, OUTPUT);
  pinMode(Rrow3, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(115200);

  // WiFi Connect
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  // Define Line Token
  LINE.setToken(LINE_TOKEN);
}

// function IFTTT
void send_webhook(){// function Send Webook to IFTTT
  // construct the JSON payload
  String jsonString = "";
  jsonString += "{\"value1\":\"";
  jsonString += IFTTT_Value1;
  jsonString += "\"}";
  int jsonLength = jsonString.length();  
  String lenString = String(jsonLength);
  // connect to the Maker event server
  client.connect("maker.ifttt.com", 80);
  // construct the POST request
  String postString = "";
  postString += "POST /trigger/";
  postString += IFTTT_Event;
  postString += "/with/key/";
  postString += IFTTT_Key;
  postString += " HTTP/1.1\r\n";
  postString += "Host: maker.ifttt.com\r\n";
  postString += "Content-Type: application/json\r\n";
  postString += "Content-Length: ";
  postString += lenString + "\r\n";
  postString += "\r\n";
  postString += jsonString; // combine post request and JSON
  client.print(postString);
  delay(500);
  client.stop();
}

void loop() {
  // put your main code here, to run repeatedly:
  int analogSensor = analogRead(A0); // อ่านค่าแก๊ส
  Serial.print("Pin A0 ="); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
  Serial.println(analogSensor); // พิมพ์ค่าของตัวแปร val

  // IFTTT
  IFTTT_Value1 = analogSensor;
  // Send Webook to IFTTT
  send_webhook();

  delay(1000);

  // lcd Carbon VALUE
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Carbon: ");
  lcd.setCursor(9,0);
  lcd.print(analogSensor);
  lcd.setCursor(13,0);
  lcd.print("ppm");

  

  if(analogSensor > 495) { // Condition 3 (6 LIGHT)

    // Red LED
    digitalWrite(Rrow1, HIGH);
    digitalWrite(Rrow2, HIGH);
    digitalWrite(Rrow3, HIGH);

    // Buzzer
    digitalWrite(buzzer, HIGH);
    
    // Display LCD
    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.setCursor(9, 1);
    lcd.print("DANGER!");

    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับอันตรายมาก!!"); // ส่งข้อความผ่านไลน์
    }
  else if (analogSensor > 400){ // condition2 (4 LIGHT)

    // Red LED
    digitalWrite(Rrow1, HIGH);
    digitalWrite(Rrow2, HIGH);
    digitalWrite(Rrow3, LOW);

    // Buzzer
    digitalWrite(buzzer, HIGH);
    
    // Display LCD
    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.setCursor(9, 1);
    lcd.print("BAD");
    
    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับที่เริ่มก่อให้เกิดอันตราย"); // ส่งข้อความผ่านไลน์
    }
  else if (analogSensor > 360){ // condition1 (2 LIGHT)

    // Red LED
    digitalWrite(Rrow1, HIGH);
    digitalWrite(Rrow2, LOW);
    digitalWrite(Rrow3, LOW);

    // Buzzer
    digitalWrite(buzzer, LOW);
    
    // Display LCD
    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.setCursor(9, 1);
    lcd.print("GOOD");

    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับปกติ"); // ส่งข้อความผ่านไลน์
    }
  else{ // nothing happen (0 LIGHT)

    // Red LED
    digitalWrite(Rrow1, LOW);
    digitalWrite(Rrow2, LOW);
    digitalWrite(Rrow3, LOW);

    // Buzzer
    digitalWrite(buzzer, LOW);
    
    // Display LCD
    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.setCursor(9, 1);
    lcd.print("WELL");
    
    }
}
