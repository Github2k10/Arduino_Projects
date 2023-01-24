//*******************************libraries********************************
#include <SPI.h>          //Serial Peripheral Interface used to communicate with pheripheral devices.
#include <MFRC522.h>      //MFRC522 used for read/write different types of RFID cards or tags.
#include<LiquidCrystal.h> //It is used for read/write different operation of LCD.
//************************************************************************

//*******************************Pin Define*******************************
int const RedLed=A0;
int const GreenLed=A1;
int const Buzzer=8;
#define SS_PIN 10
#define RST_PIN 9
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
//************************************************************************

//*******************************Card Define******************************
byte card_ID[4];                                //card UID size 4byte
byte Name1[4]={0xE5,0x1F,0xCD,0x10};            //first UID card
byte Name2[4]={0x6B,0xA1,0x4D,0x1F};            //second UID card
byte Name3[4]={0xA4,0x84,0xB0,0x13};            // Third UID card
byte Name4[4]={0x3B,0x3C,0xD2,0x0D};            // Fourth UID card
                                                //if you want the arduino to detect the cards only once
int NumbCard[4];                                //this array content the number of cards. in my case i have just two cards.
int j=0;        
String Name;                                    //user name
long Number;                                    //user number
//************************************************************************ 

void setup() {
  Serial.begin(9600);                           // Initialize serial communications with the PC
  SPI.begin();                                  // Initialization SPI bus
  mfrc522.PCD_Init();                           // Initialization MFRC522 card
  lcd.begin(16, 2);                             // Initialization LCD Display
   //---------------------------------------------------------------------------------------------
  Serial.println("CLEARSHEET");                 // clears starting at row 1
  Serial.println("LABEL,Date,Time,Name,Number");// make four columns (Date,Time,[Name:"user name"]line 48 & 52,[Number:"user number"]line 49 & 53)
  lcd.print("Tap your Card....");               // print on lcd display
   //---------------------------------------------------------------------------------------------
  pinMode(RedLed,OUTPUT);                       //Initialize Red Led
  pinMode(GreenLed,OUTPUT);                     //Initialize Green Led
  pinMode(Buzzer,OUTPUT);                       //Initialize Buzzer
   //---------------------------------------------------------------------------------------------
   }
    
void loop() {
                                                //look for new card
   if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;                                       //got to start of loop if there is no card present
 }
                                                // Select one of the cards
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;                                       //if read card serial(0) returns 1, the uid struct contians the ID of the read card.
 }
   //--------------------------------------------------------------------------------------------
      // Check for card user
 for (byte i = 0; i < mfrc522.uid.size; i++) {
     card_ID[i]=mfrc522.uid.uidByte[i];

       if(card_ID[i]==Name1[i]){
       Name="ALPHA";                             //user name
       Number=01;                               //user number
       j=0;                                     //first number in the NumbCard array 
       lcd.setCursor(0, 0);                     //Set the cursor on first column first row on lcd display
       lcd.print("WELCOME,ALPHA_01 ");          //print the user name on lcd display
       lcd.setCursor(0, 1);                     //set the cursor on first column second row on lcd display
       lcd.print("Authorized Access");          //print the message on the lcd display
      }
      
      else if(card_ID[i]==Name2[i]){
       Name="BETA";                            //user name
       Number=02;                               //user number
       j=1;                                     //Third number in the NumbCard array 
       lcd.setCursor(0, 0);                     //Set the cursor on first column first row on lcd display
       lcd.print("WELCOME, BETA_02 ");           //print the user name on lcd display
       lcd.setCursor(0, 1);                     //set the cursor on first column second row on lcd display
       lcd.print("Authorized Access");          //print the message on the lcd display  
      }
      
      else if(card_ID[i]==Name3[i]){
       Name="GAMMA";                              //user name
       Number=03;                                //user number
       j=2;                                      //Third number in the NumbCard array
       lcd.setCursor(0, 0);                      //Set the cursor on first column first row on lcd display
       lcd.print("WELCOME,GAMMA_03 ");            //print the user name on lcd display
       lcd.setCursor(0, 1);                      //set the cursor on first column second row on lcd display
       lcd.print("Authorized Access");           //print the message on the lcd display
      }
      
      else if(card_ID[i]==Name4[i]){
       Name="DELTA";                             //user name
       Number=04;                                //user number
       j=3;                                      //Third number in the NumbCard array : NumbCard[j]
       lcd.setCursor(0, 0);                      //Set the cursor on first column first row on lcd display
       lcd.print("WELCOME,DELTA_04 ");           //print the user name on lcd display
       lcd.setCursor(0, 1);                      //set the cursor on first column second row on lcd display
       lcd.print("Authorized Access");           //print the message on the lcd display
      }
      
      //If the card is not valid. So red led will glow and access denied.
      else{
       digitalWrite(GreenLed,LOW);                
       digitalWrite(RedLed,HIGH);                 
       lcd.setCursor(0, 0);
       lcd.print("OOPS! STARANGER!  ");
       lcd.setCursor(0, 1);
       lcd.print("CARD NOT VAILED!  ");
       digitalWrite(Buzzer, HIGH);
       delay(2000);
       digitalWrite(Buzzer, LOW);
       goto cont;                                 //go directly to "cont" line
     }
}

    //-------------------------------------------------------------------------------------------  
       // Sending data to Excel Sheet and command for glow green led for authorized access and beep sound 
      NumbCard[j] = 1;                            //put 1 in the NumbCard array : NumbCard[j]={1,1} to let the arduino know if the card was detecting 
      Serial.print("DATA,DATE,TIME," + Name);     //send the Name to excel
      Serial.print(",");
      Serial.println(Number);                     //send the Number to excel
      digitalWrite(GreenLed,HIGH);                
      digitalWrite(RedLed,LOW);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer,LOW);
      delay(2000);      
      digitalWrite(GreenLed,LOW);
      lcd.setCursor(0, 0);
      lcd.print("Tap your Card....");
      lcd.setCursor(0, 1);
      lcd.print("                 ");
      Serial.println("SAVEWORKBOOKAS,Names/WorkNames");
      
  //----------------------------------------------------------------------------------------------  
    //defalt command for unauthorised access
cont:
lcd.setCursor(0, 0);
lcd.print("Tap your Card....");
lcd.setCursor(0, 1);
lcd.print("                 ");
digitalWrite(RedLed,LOW);

}
