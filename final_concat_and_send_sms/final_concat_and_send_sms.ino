/*
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 
 */

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  //rs, en , d4 to d7

//String inputString = "";   
bool inputComplete = false; // for interupt

String contact[2]={"N","N"};
String defaltmsg = "Emergency";
String testmsg = "Testing Ok";

void setup() {
   lcd.begin(16, 2);
   Serial.begin(9600);
   sim.begin(9600);

   lcd.setCursor(2, 0);
   lcd.print("..Welcome..");
   delay(2000);


   
//------------------------- Initialize GSM
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

 //--------------------------Reading contact

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

//----------------------------- check network status 

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

  //----------------------------verify contact number(sending test contact)

 /*
    
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



   */

//Serial.println(InitGsm());  // testing gsm return 1 for sucsess
//Serial.println(StoreContact());  //store contact to contact arry and if sucsess return 1
//Serial.println(CheckNetwork());

// Readcontact(2);
//Sendsms(contact[0],defaltmsg);
//Sendsms(contact[1],defaltmsg);
// GetsingalStrength();

 
}





void loop() {
           String st = GetsingalStrength();
           lcd.clear();
           lcd.print("Signal Strength");           
           lcd.setCursor(6, 1);
           lcd.print(st);
           Serial.println(st);
           delay(100);

  

  

if (inputComplete) {
    Serial.println("Interupted something");

           lcd.clear();
           lcd.print("Data coming"); 
           delay(1000);
    
    inputComplete = false;
  }


}


String GetsingalStrength(){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());  // for ready gsm

  while(sim.available()){
   //Serial.print((char)sim.read());
   int temp = (char)sim.read();    // get garbadge values
  } 
  
  sim.print("AT+CSQ\r\n"); 
  delay(500);
  while(!sim.available());

  String response;
  while(sim.available()){
  response.concat((char)sim.read());
    
  }
//Serial.println(response);                     // strength format  +CSQ: 4,0  
 int a = response.indexOf(' ');
 int b = response.indexOf(',');
 
 String strength = response.substring(a+1, b);  // strength value

 Serial.print("Signal strength:- ");
 Serial.println(strength);
  
  return strength;
  
  
  }



int veryfy(){ 
  
  int n1 = Sendsms(contact[0],testmsg);    // send sms and get return (if sent, return 1)
  int n2 = Sendsms(contact[1],testmsg);
  if(n1==1 and n2==1 ){

    Serial.println("numbers veryfied");
    return 1;
    }else {
        Serial.println("numbers veryfy fail");
      return 0;
      }
  
  }


void serialEvent() {
  String response;
  while (Serial.available()) { 
  response.concat((char)sim.read());
 
  }
  
     Serial.println(response);  
      inputComplete = true;
    }
 







int CheckNetwork(){

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());   //reasy gsm

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

//Serial.println(response);               // responce format +CREG: 1,1
//Serial.println(response.charAt(18));
//Serial.println(response.charAt(20));

if(response.charAt(18)=='0' or response.charAt(18)=='6'){  // char 18 and 20 became 0,1 on home network 
  if(response.charAt(20)=='1'){
    
    Serial.println("Network Ok");
    return 1;
    }else{
      Serial.println("Network Fail");
      return 0;
      }
  }else{
    Serial.println("Network Fail");
    return 0;
    }  
  
  }



int StoreContact(){
  
Serial.println("Reading contact");

contact[0]=Readcontact(1);  // Get contact numbers and store
contact[1]=Readcontact(2);

Serial.println(contact[0]);
Serial.println(contact[1]);

if(contact[0]=="N" or contact[1]=="N"){
  Serial.println("Reading Err");
  return 0;
  }else{

    Serial.println("Contact Stored");
    return 1;
    
    }
  
  
  
  
  }


int InitGsm(){
  
  int gsm=0;

  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());  //wait for responce gsm
  
  while(sim.available()){
 
   // Serial.print((char)sim.read());
   if((char)sim.read()=='O'){
    if((char)sim.read()=='K'){
      gsm=1;  //working Gsm
      
      }
    
    }
   
  } 

if(gsm==1){
  Serial.println("Gsm working");
  }else{
    Serial.println("Gsm Fail..!");
    }

  return gsm;
  
  
  }








int Sendsms(String num, String msg){


  sim.print("AT\r\n");
  delay(500);
  while(!sim.available());   //ready gsm

  while(sim.available()){     // garbadge collection
   //Serial.print((char)sim.read());
   int temp = (char)sim.read();
  }
  
  sim.print("AT+CMGF=1\r\n");    // Text Mode
  delay(1000);
  
  sim.print("AT+CMGS=\"");
  sim.print(num);
  sim.print("\"\r\n");
  delay(1000);
  
  sim.println(msg);
  delay(500);
  sim.println((char)26);//  CTRL+Z  for end msg
  delay(500);


  while(sim.available()){
   //Serial.print((char)sim.read());
   int a = (char)sim.read();
  }

  Serial.print("Msg sent to ");
  Serial.print(num);
  Serial.print(" \r\n");
 
  
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
