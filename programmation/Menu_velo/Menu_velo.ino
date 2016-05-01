#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const char RightPin = 6;
const char LeftPin=7;
const char UpPin=8;
const char DownPin=9;
const char Button1Pin=10;
const char Button2Pin=13;

void setup(){
  lcd.begin(16,2);
  pinMode(RightPin,INPUT_PULLUP);
  pinMode(LeftPin,INPUT_PULLUP);
  pinMode(UpPin,INPUT_PULLUP);
  pinMode(DownPin,INPUT_PULLUP);
  pinMode(Button1Pin,INPUT_PULLUP);
  pinMode(Button2Pin,INPUT_PULLUP);
}

/*void scrollRight(unsigned char column, unsigned char row, char shift ){
 
 }
 
 void scrollRight(unsigned char column, unsigned row){
 
 }*/

unsigned char debounce_pullup_button(const char pin_input, unsigned int debounce_delay){
  
    if(digitalRead(pin_input)==0) delay(debounce_delay);
     return digitalRead(pin_input); 
}

unsigned char Button_choice(const char Next, const char Back, const char Down, const char Up, const char Validation, const char Cancel, unsigned int debounce_delay){
  unsigned char reading=1;
  if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
         return 0;
        }
    }
   if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          return 1;
        }
    }
  
}
void next_page(unsigned int time, unsigned char posX, unsigned char posY){
  lcd.setCursor(posX,posY);
  lcd.print(">");
  delay(time);
  lcd.setCursor(posX,posY);
  lcd.print(" >");
  delay(time);
  lcd.setCursor(posX+1,posY);
  lcd.print(" ");
}

void previous_page(unsigned int time, unsigned char posX, unsigned char posY){
  lcd.setCursor(posX,posY);
  lcd.print("<");
  delay(time);
  lcd.setCursor(posX-1,posY);
  lcd.print("< ");
  delay(time);
  lcd.setCursor(posX-1,posY);
  lcd.print(" ");
}

void welcome_screen(){
  lcd.setCursor(0,0);
  lcd.print("* VELO WII 2.0 *");
  lcd.setCursor(3,1);
  lcd.print("HOME PAGE");
  next_page(200,14,1);
}

void selection_screen1(){
    lcd.setCursor(4,0);
    lcd.print("1)START");
    lcd.setCursor(4,1);
    lcd.print("2)EDITEUR");
    next_page(200,14,1);
}

void selection_screen2(){
    lcd.setCursor(2,0);
    lcd.print("3)PARAMETRES");
    lcd.setCursor(3,1);
    lcd.print("4)RECORDS");
    next_page(200,14,1);
    previous_page(200,1,1);
  }
  
void selection_screen3(){
    lcd.setCursor(5,0);
    lcd.print("5)AIDE");
    lcd.setCursor(4,1);
    lcd.print("6)CREDITS");
    lcd.setCursor(0,1);
    /*lcd.print("7)ABOUT ");
    lcd.print("8)CRED");*/
    next_page(200,14,1);
    previous_page(200,1,1);  
}

/*void menu(){

  next_page(2000);
  if(analogRead(A0)>=20){
    lcd.clear();
    while(analogRead(A0)<1024)
    {
      selection_screen();
    }  
  }
  else welcome_screen();
}*/

/* etat des boutons de commande */   
unsigned char RightButtonState = HIGH;
unsigned char LeftButtonState = HIGH;
unsigned char UpButtonState = HIGH;
unsigned char DownButtonState = HIGH;
unsigned char ValidationButtonState = HIGH;
unsigned char CancelButtonState = HIGH;  

/*variables liees au delay (debounce, etc,...) */

unsigned int debounce_delay=50;

unsigned char page=0;
unsigned char reading=1;
unsigned char choice=0;

void loop(){
  /*menu();*/
  while(page==0){

    welcome_screen();
    reading = debounce_pullup_button(RightPin,debounce_delay);
      if(reading==0) {
        page=page+1;

      }
      if(reading==0) {
        delay(500);
        lcd.clear();
      }
  }
  while(page==1){
    selection_screen1();
    if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
          page = page + 1;
        }
    }
   if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          page = page - 1;
        }
    }
   if(reading==0) {
    delay(500);
    lcd.clear();
   }   
  }
  while(page==2){
    selection_screen2();
    if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
          page = page + 1;
        }
    }
    if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          page = page - 1;
        }
    }
    if(digitalRead(DownPin)==0){
      reading = debounce_pullup_button(DownPin,debounce_delay);
        if(reading==0) { 
          lcd.setCursor(14,1);
          lcd.cursor();
          delay(100);
          lcd.noCursor();
        }
    }
    if(digitalRead(UpPin)==0){
      reading = debounce_pullup_button(UpPin,debounce_delay);
        if(reading==0) { 
          lcd.setCursor(14,0);
                    lcd.cursor();
          delay(100);
          lcd.noCursor();
        }
    }
      /*if(reading==0) {
        delay(500);
        lcd.clear();
      }*/
  } 
  while(page==3){
    selection_screen3();
    if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
          page = 1;
        }
    }
    if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          page = page - 1;
        }
    }
      if(reading==0) {
        delay(500);
        lcd.clear();
      }
 }
  reading=1; 
}

