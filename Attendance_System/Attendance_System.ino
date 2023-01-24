//*******************************libraries********************************
//RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 

//************************************************************************
#define SS_PIN  D4  //D2
#define RST_PIN D3  //D1

//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

//************************************************************************
/* Set these to your desired credentials. */
const char *ssid = "vivo 1901";
const char *password = "DEMON12345";
const char* device_token  = "9739dfa7f4ad3e12";


//************************************************************************
String URL = "http://192.168.232.136/rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
unsigned long previousMillis = 0;


//************************************************************************
void setup() {
  delay(1000); 
  SPI.begin();// Init SPI bus
  lcd.init();    // initialize the lcd   
  lcd.backlight(); // backlight ON  
  lcd.print("   Welcome to   ");  
  lcd.setCursor(0,1);
  lcd.print("AttendanceSystem");
  delay(2000);  
  Serial.begin(9600);
  mfrc522.PCD_Init(); // Init MFRC522 card
  //---------------------------------------------
  connectToWiFi();
}


//************************************************************************
void loop() {
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  //look for new card
  lcd.clear();
  lcd.setCursor(0,0);   
  lcd.print(" Tap your Card   ");
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------
//  Serial.println(CardID);
  SendCardID(CardID);
  delay(1000);
}


//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  WiFiClient clientt;
  if(WiFi.isConnected()){
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(clientt ,Link); //initiate HTTP request   //Specify content-type header
    
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload
    
    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
        int serial_number = seprate(user_name);
      Serial.println(user_name);
      Serial.println(serial_number);
      lcd.setCursor(0,0);
      lcd.print(name_seprate(user_name));
      lcd.setCursor(0,1);
      lcd.print(serial_number);
      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
        int serial_number = seprate(user_name);
      Serial.println(user_name);
      Serial.println(serial_number);
      lcd.setCursor(0,0);
      lcd.print(name_seprate(user_name));
      lcd.setCursor(0,1);
      lcd.print(serial_number);
      }
      else if (payload == "succesful") {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("New User Added");
      }
      else if (payload == "available") {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("New user");
        lcd.setCursor(1,0);
        
      }
      delay(2000);
      http.end();  //Close connection
    }
  }
}


//********************connect to the WiFi******************
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    lcd.setCursor(0,0);  
    lcd.print("Connecting to  ");
    lcd.setCursor(0,1); 
    lcd.print("Wifi           ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      //lcd.setCursor(10++,1);
      //lcd.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
    lcd.clear();
    lcd.setCursor(1,0);  
    lcd.print("Connected To    ");
    lcd.setCursor(1,1);  
    lcd.print(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(2000);
}

//*************************To Seprate serial number form resived information*********************************//
int seprate(String names){
  char *ptr;
  int i1;
  int i2;

  int len = names.length();
  char yourstring[len];
  names.toCharArray(yourstring, len);
  
  ptr = strchr(yourstring, ',');
  if (ptr == NULL)
  {
  }
  else
  {
    *ptr = '\0';
    i1 = atoi(&yourstring[1]);
    *ptr = ',';
    ptr++;
    i2 = atoi(ptr);
  }
  return i2;
}
//=======================================================================

//*************************To seprate user name *********************************************************//

String name_seprate(String x){
   int n;
   String y;
   n = x.length();
   
   char c[n];
   strcpy(c, x.c_str());
   for(int i=0; c[i] != '\0'; i++){
       y += c[i];
   }
   return y;
}
