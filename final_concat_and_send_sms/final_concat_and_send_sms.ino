/* LCD display pin configuration

 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 */
#define PrintSerialMonitor  //if define it, print on serial monitor 
#define SkipVerify    // if define it, skip phone number verify step 

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11);  //communicate for gsm
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  //pin connect(rs, en , d4 to d7)

  // for interupt serial data
String inputSerialData = "";
bool inputComplete = false; 

String contact[2]={"N","N"};  // for store contact numbers
String emergencytext = "Emergency";  //emergency msg
String testmsg = "Testing Ok";  // test msg

void setup() {
  
  Serial.begin(9600);
  sim.begin(9600);
  
  lcd.begin(16, 2);  // start lcd
  lcd.setCursor(2, 0);
  lcd.print("..Welcome..");
  delay(2000);
   

    //Initialize GSM--------------------
  lcd.clear();
  lcd.print("Initialize GSM");
  delay(1000);
  
  if(InitGsm()==1){
    lcd.clear();
    lcd.print("GSM OK");
    delay(1000);
  }else{
    while(1){
      lcd.clear();
      lcd.print("GSM Fail");
      delay(500);
      }
    }
    
    //Reading contact-------------------
  lcd.clear();
  lcd.print("Reading Contact");
  delay(500);
  
  if(StoreContact()==1){
    lcd.clear();
    lcd.print("Contacts OK");
    delay(500);
  }else{
    while(1){
      lcd.clear();
      lcd.print("Contacts Fail");
      delay(500);
      }
    }

    //check network status---------------
  lcd.clear();
  lcd.print("Connectig..");
  delay(500);
  
  if(CheckNetwork()==1){
    lcd.clear();
    lcd.print("Network conected");
    delay(500);
  }else{
    while(1){
      lcd.clear();
      lcd.print("Network Fail");
      delay(500);
      }
    }

#if !defined(SkipVerify)
    //verify contact number(sending test contact)    
  lcd.clear();
  lcd.print("veryfing Numbers");
  delay(500);
  
  if(veryfy()==1){
    lcd.clear();
    lcd.print("Verified");
    delay(500);
  }else{
    while(1){
      lcd.clear();
      lcd.print("Verified Fail");
      delay(500);
      }
    }
#endif


//Serial.println(InitGsm());  // testing gsm return 1 for sucsess
//Serial.println(StoreContact());  //store contact to contact arry and if sucsess return 1
//Serial.println(CheckNetwork());

// Readcontact(2);
//Sendsms(contact[0],defaltmsg);
//Sendsms(contact[1],defaltmsg);
// GetsingalStrength(); 

  lcd.clear();
}

void loop() {
  
  ShowSignalBar(GetsingalStrength()); // show siganl bar with current strength
  
  lcd.setCursor(0, 1);
  lcd.print("Operating.");
  delay(100);
  lcd.print(".");
  delay(100);
  lcd.print(".");
  delay(100);
  lcd.print(".");
  delay(100);
  
  if(inputComplete){
    lcd.clear();
    lcd.print("Data coming");
    delay(500);

    //inputSerialData  -> this variable have serial data 
    //Serial.println("Interupted something");   
    //Serial.println(inputSerialData.charAt(20));
    String Firedata = inputSerialData.substring(22, 32);
    
    if(!(Firedata.indexOf("fire")==-1)){  // if serial data have "fire" string, should send sms
    
      Sendsms(contact[0],emergencytext);
      Sendsms(contact[1],emergencytext);
      
      lcd.clear();
      lcd.print("Sent SMS");
      delay(1000);
      
    }else{
      //if haven't identify fire data
      lcd.clear();
      lcd.print("You are safe");
      delay(1000);
      }
      
    inputComplete = false;
    inputSerialData ="";
  }
  
}


void ShowSignalBar(int strength){

  // Custom charactors data
  byte Signalbar[] = {
  B11111,
  B10101,
  B01110,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
  };


  byte sig4[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
  };

  byte sig3[] = {
  B00000,
  B00000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
  };

  byte sig2[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11000,
  B11000,
  B11000,
  B11000
  };

  byte sig1[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11000,
  B11000
  };

  //Create custom charactors
  lcd.createChar(0, Signalbar);
  lcd.createChar(1, sig1);
  lcd.createChar(2, sig2);
  lcd.createChar(3, sig3);
  lcd.createChar(4, sig4);
  
  lcd.clear();
  lcd.setCursor(11, 0); // battery bar cordinate
  
  lcd.write(byte(0));   //show signal tower
  
  if(strength<10){    //show signl bars
    lcd.write(byte(1));
  
  }else if(strength<15){    
      lcd.write(byte(1));
      lcd.write(byte(2));
  
  }else if(strength<20){
  
      lcd.write(byte(1));
      lcd.write(byte(2));
      lcd.write(byte(3)); 
  
  }else if(strength<30){
  
      lcd.write(byte(1));
      lcd.write(byte(2));
      lcd.write(byte(3));
      lcd.write(byte(4));

      }
}


void serialEvent(){

  String response;
  
  while (Serial.available()){
    response.concat((char)sim.read());
  }
  
  inputSerialData = response;
  inputComplete = true;
  
  
  /*
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputSerialData += inChar;
  
    if(inChar == '\n'){
    stringComplete = true;
    }
    
  }
  */
}


int GetsingalStrength(){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());  // for ready gsm
  
  while(sim.available()){
    //Serial.print((char)sim.read());
    int temp = (char)sim.read();  // get garbadge values
    
    }
    
  sim.print("AT+CSQ\r\n");
  delay(500);
  
  while(!sim.available());
  
  String response;
  while(sim.available()){
  response.concat((char)sim.read());
  
  }
  
  //Serial.println(response);   // strength format  +CSQ: 4,0
  int a = response.indexOf(' ');
  int b = response.indexOf(',');
  
  String strength = response.substring(a+1, b);   // strength value
 #if defined(PrintSerialMonitor)
  Serial.print("Signal strength:- ");
  Serial.println(strength);
 #endif
  
  return strength.toInt();
}


int veryfy(){

  int n1 = Sendsms(contact[0],testmsg);   // send sms and get return (if sent, return 1)
  int n2 = Sendsms(contact[1],testmsg);
  
  if(n1==1 and n2==1 ){
    #if defined(PrintSerialMonitor)
    Serial.println("numbers veryfied");
    #endif
    return 1;
    
  }else {
     #if defined(PrintSerialMonitor)
    Serial.println("numbers veryfy fail");
    #endif
    return 0;
    
    }
}


int CheckNetwork(){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());    //ready gsm
  
  while(sim.available()){
    //Serial.print((char)sim.read());
    int temp = (char)sim.read();    // get garbadge values
    }
    
  sim.print("AT+CREG?\r\n");
  delay(500);
  while(!sim.available());

  String response;
  while(sim.available()){
    response.concat((char)sim.read());
    
    }
    
  //Serial.println(response);     // responce format +CREG: 1,1
  //Serial.println(response.charAt(18));
  //Serial.println(response.charAt(20));
  
  if(response.charAt(18)=='0' or response.charAt(18)=='6'){ //char 18 and 20 became 0,1 on home network
    if(response.charAt(20)=='1'){
       #if defined(PrintSerialMonitor)
      Serial.println("Network Ok");
      #endif
      return 1;
        
    }else{
       #if defined(PrintSerialMonitor)
      Serial.println("Network Fail");
      #endif
      return 0;
      }
      
  }else{
     #if defined(PrintSerialMonitor)
    Serial.println("Network Fail");
    #endif
    return 0;
    }
    
}



int StoreContact(){
  
   #if defined(PrintSerialMonitor)
  Serial.println("Reading contact");
  #endif
  
  contact[0]=Readcontact(1);  // Get contact numbers and store
  contact[1]=Readcontact(2);

   #if defined(PrintSerialMonitor)
  Serial.println(contact[0]);
  Serial.println(contact[1]);
  #endif
  
  if(contact[0]=="N" or contact[1]=="N"){
     #if defined(PrintSerialMonitor)
    Serial.println("Reading Err");
    #endif
    return 0;
    
  }else{
     #if defined(PrintSerialMonitor)
    Serial.println("Contact Stored");
    #endif
    return 1;
    }
}


int InitGsm(){

  int gsm=0;
  
  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());  //wait for responce gsm
  
  String response;
  while(sim.available()){
    response.concat((char)sim.read());  //responce string have AT +OK 
    }
    
  String cmd = response.substring(6,8); //substring OK part
  if(!(cmd.indexOf("OK")==-1)){   //if true Gsm return ok
    gsm=1;
    }
    
  return gsm;
  
}


int Sendsms(String num, String msg){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());  //ready gsm
  
  while(sim.available()){   //garbadge collection
    //Serial.print((char)sim.read());
    int temp = (char)sim.read();
    }
    
  sim.print("AT+CMGF=1\r\n");   // Text Mode
  delay(1000);
  
  sim.print("AT+CMGS=\"");
  sim.print(num);
  sim.print("\"\r\n");
  delay(1000);
  
  sim.println(msg);
  delay(500);
  sim.println((char)26);    //CTRL+Z  for end msg
  delay(500);
    
  #if defined(PrintSerialMonitor)
  Serial.print("Msg sent to ");
  Serial.print(num);
  Serial.print(" \r\n");
  #endif
  
  return 1;
  
}


String Readcontact(int index){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());   //ready gsm
  
  while(sim.available()){       //  collect garbage
    // Serial.print((char)sim.read());
    int a = (char)sim.read();
    
    }

  sim.print("AT+CPBR=");
  sim.print(index);
  sim.print("\r\n");
  delay(500);
  
  while(!sim.available());
  
  String response;
  while(sim.available()){
    response.concat((char)sim.read());
    //Serial.print(ss);
    
    }
    
  String phoneno = response.substring(22, 32);  // get contact no from string
  return phoneno;
  
}
