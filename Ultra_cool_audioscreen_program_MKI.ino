#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include <Rotary.h>

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 5 and 6.
Rotary rotary = Rotary(12, 11);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 

 //End of all setup shenanigans, let the program start.

 #define inputA 11
 #define inputB 12
 #define button 13
 #define timeToMute 250 //Time required for the hold on the button to mute.

 //EEPROM Addresses
 #define e_mcValue 0
 #define e_mmValue 1
 #define e_isMuted 2
 #define e_zone1State 3

 //Save to EEPROM variables
 bool savePending = false; //if it needs to save
 int unsigned pendingStart; //time the change was made
 #define timeToSave 3000 //save 30 seconds after a change

 //States of the program
 int zone1State; //The state of zone1, eg. MC/MM/Line123
 int unsigned mcValue = 0; //the current value for the MC.
 int unsigned mmValue = 0; //same but for MM
 int editState = 0; //The zone currently affected by the rotary
 bool isMuted = false; //self explanatory
 
 //Button control variables
 int unsigned pressTime; //Time when the button was pressed.
 int unsigned releaseTime; //Time when the button was released.
 int timeHeld; //releseTime - pressTime
 int lastButtonState; 
 int currentButtonState;
 bool buttonWasPressed = false; //checks if button is currently pressed.

 

void setup() { 
   pinMode (inputA,INPUT);
   pinMode (inputB,INPUT);
   pinMode (button, INPUT);

   for(int i = 2; i <= 10; i++)
     pinMode(i, OUTPUT);

   pinMode(A0, OUTPUT);
   pinMode(A1, OUTPUT);
   
   Serial.begin (9600);
   // Reads the initial state of the outputA

   lastButtonState = digitalRead(button);

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));  
    for(;;); // Don't proceed, loop forever
 }

 loadFromEEPROM();
 setOutputStates();
 writeScreen();
 

 }

 void saveToEEPROM()
 {
   EEPROM.write(e_mcValue, mcValue);
   EEPROM.write(e_mmValue, mmValue);
   EEPROM.write(e_isMuted, isMuted);
   EEPROM.write(e_zone1State, zone1State);  
 }

 void loadFromEEPROM()
 {
    mcValue = EEPROM.read(e_mcValue);
    mmValue = EEPROM.read(e_mmValue);
    isMuted = EEPROM.read(e_isMuted);
    zone1State = EEPROM.read(e_zone1State);
 }

//Prepares the program to write the Phono MM state.
 void writeMM(int x)
 {
    mmValue += x;

    if(mmValue < 1)
      mmValue = 1;

    if(mmValue > 8)
      mmValue = 8;
    
    writeScreen();
 }

//Conversly, this prepares it for the Phono MC state.
 void writeMC(int x)
 {
    mcValue += x;

    if(mcValue < 1)
      mcValue = 1;

    if(mcValue > 6)
      mcValue = 6;
    
    writeScreen();
  
 }

 
//This method is called whenever the rotary is used. Unless isMuted is true.
void rotated(int x)
  {

    switch(editState)
    {
      case 1: 
      
        zone1State += x;
        
        if(zone1State > 4)
        {
           zone1State = 0;
        }
        if(zone1State < 0)
        {
           zone1State = 4;
        }
        
        writeScreen();
      break;
      case 2: 
        switch(zone1State)
        {
          case 0:
           writeMM(x);
          break;
          case 1:
           writeMC(x);
          break;
        }
      break;
    }
  }

/*Because of the fact that unsigned ints overflow at 2^16
 * I use this method to make the time in milliseconds returned by arduino
 * not mess up the variables. I actually don't properly understand why this works, but it does.
 * I don't know how I came up with it. ¯\_(ツ)_/¯
 */
int getTime()
{
  return millis() % 50000;
}


/*This is an absolute mess and without a doubt most poorly method 
 * written in this code, but I don't know how to make it prettier. 
 * Whenever I look at it I am remminded that I am not such a good coder.
 */
void writeScreen()
{  
  setOutputStates();
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(18,4);   
 
  if(isMuted)
  {
    display.setCursor(40,9);
    display.println("MUTE");  
  }
  else
  { 
    String zone1="", zone2="";

    switch(editState)
    {
      case 1:
        zone1 +=">";
      break;
      case 2:
        zone2 +=">";
      break;
    }
    switch(zone1State)
    { 
      case 0:
        
        display.println(String(zone1 + "Phono MM"));
        display.setTextSize(1); 
        display.setCursor(0,24); 
        display.println(String(zone2 + "Load " + String(mmValue * 50) + " pF"));     
      break;

      case 1:
        display.println(String(zone1 + "Phono MC"));
        String x;
        if(mcValue == 6)
        {
           x = "1k";
        }else
        {
          int y = ceil(pow(2, mcValue - 1)) * 25;
          x = y;
        }
        display.setTextSize(1); 
        display.setCursor(0,24); 
        display.println(String(zone2 + "Load " + String(x) + "Ω"));  
        display.println();
      break;
  }

  if(zone1State != 0 && zone1State != 1)
  {
      display.setCursor(30,9); 
      display.println(String(zone1 + "LINE " + String(zone1State - 1)));
  }
}
  
  display.display();

  //Initiate the saving to the EEPROM
  savePending = true;
  pendingStart = getTime();  
}

void muteDevice()
{
  isMuted = true;
  Serial.println("Muted");
  writeScreen();
}

void changeState()
{
  if(isMuted)
  {
    isMuted = false;
    editState = 0;
    Serial.println("Unmuted");
    Serial.println(pressTime);
    writeScreen();
  }
  else
  {

    editState++;
    
    //This just makes sure that you can't access zone 2 if you are on a LINE.
    if(zone1State != 0 && zone1State != 1)
    {
      if(editState > 1)
      {
        editState = 0;
      }
    } else if(editState >= 3)
    {
      editState = 0;
    }

    writeScreen();
    Serial.println("State Change: ");
    Serial.print(editState);
    Serial.println();

  }
}

//Gets called when arduino notices the state of the button changed.
void buttonStateChanged()
{
  if(currentButtonState == LOW)
  {
     pressTime = getTime();
     buttonWasPressed = true;
  } else if(buttonWasPressed)
  {
      buttonWasPressed = false;
      timeHeld = getTime() - pressTime;

      if(timeHeld < timeToMute)
      {
        changeState();
      } 
  }
  lastButtonState = currentButtonState;
 
}

void loop() {

  if(savePending && (getTime() - pendingStart > timeToSave))
  {
    Serial.println("Saved to EEPROM. Don't overdo it.");
    savePending = false;
    saveToEEPROM();
  }
  
  if(buttonWasPressed && !isMuted)
   {
      if((getTime() - pressTime) >= timeToMute)
      {
         muteDevice();
      }
   }
   
    currentButtonState = digitalRead(button);
   if(currentButtonState != lastButtonState)
   {
      buttonStateChanged();
   }


  if(!isMuted)
  {
    unsigned char result = rotary.process();
    if (result == DIR_CW) 
    {
       rotated(-1);
    } else if (result == DIR_CCW) 
    {
      rotated(1);
    }

    //If it should flash.
    if(editState != 0)
    {
      
    }
  }
}

/*For some unkown reason the code bellow doesn't work.
 * Not like it matters since I updated the program since 
 * and is now incompatible with the rest of it, do not uncomment it.
 * Though it still bugs me...
 */

/*void loop()
{
  if(buttonWasPressed && !isMuted)
   {
      if((getTime() - pressTime) >= timeToMute)
      {
         muteDevice();
      }
   }
   
    currentButtonState = digitalRead(button);
   if(currentButtonState != lastButtonState)
   {
      buttonStateChanged();
   }

  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    Serial.println("asdasdasd");
    rotated(1);
    
  } else if (result == DIR_CCW) {
    Serial.println("nonon");
    rotated(-1);
  }

   writeScreen();
}*/


/*
 * IF ONLY YOU KNEW HOW BAD THINGS REALLY ARE
 */
void setOutputStates()
{

  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  
  switch(zone1State)
  {
    case 0: //Phono MM
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
      
      digitalWrite(5, HIGH);

      switch(mmValue)
      {
        case 1:
          digitalWrite(6, HIGH);
          digitalWrite(7, HIGH);
          digitalWrite(8, HIGH);
        break;

        case 2:
          digitalWrite(6, HIGH);
          digitalWrite(7, HIGH);
          digitalWrite(8, LOW);
        break;

        case 3:
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
          digitalWrite(8, HIGH);
        break;

        case 4:
          digitalWrite(6, HIGH);
          digitalWrite(7, LOW);
          digitalWrite(8, LOW);
        break;

        case 5:
          digitalWrite(6, LOW);
          digitalWrite(7, HIGH);
          digitalWrite(8, HIGH);
        break;

        case 6:
          digitalWrite(6, LOW);
          digitalWrite(7, HIGH);
          digitalWrite(8, LOW);
        break;

        case 7:
          digitalWrite(6, LOW);
          digitalWrite(7, LOW);
          digitalWrite(8, HIGH);
        break;

        case 8:
          digitalWrite(6, LOW);
          digitalWrite(7, LOW);
          digitalWrite(8, LOW);
        break;
      }
    
    break;
    
    case 1: //Phono MC
      digitalWrite(5, LOW);
      switch(mcValue)
      {
        case 1:
          digitalWrite(2, HIGH);
          digitalWrite(3, LOW);
          digitalWrite(4, HIGH);
        break;
        
        case 2:
          digitalWrite(2, HIGH);
          digitalWrite(3, LOW);
          digitalWrite(4, LOW);
        break;
        
        case 3:
          digitalWrite(2, LOW);
          digitalWrite(3, HIGH);
          digitalWrite(4, HIGH);
        break;
        
        case 4:
          digitalWrite(2, LOW);
          digitalWrite(3, HIGH);
          digitalWrite(4, LOW);
        break;
        
        case 5:
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
          digitalWrite(4, HIGH);
        break;
        
        case 6:
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
          digitalWrite(4, LOW);
        break;
      }

    break;
    
    case 2: //Line 1
      digitalWrite(9, HIGH);
      digitalWrite(10, LOW);
    break;
    
    case 3: //Line 2
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
    break;
    
    case 4: //Line 3
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
    break;
  }
}
