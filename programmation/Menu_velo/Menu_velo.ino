#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const char RightPin = 6;
const char LeftPin=7;
const char UpPin=8;
const char DownPin=9;
const char Button1Pin=10; // validation
const char Button2Pin=13; // cancel 

/* etat des boutons de commande */   
unsigned char RightButtonState = HIGH;
unsigned char LeftButtonState = HIGH;
unsigned char UpButtonState = HIGH;
unsigned char DownButtonState = HIGH;
unsigned char ValidationButtonState = HIGH;
unsigned char CancelButtonState = HIGH;  

/* variables liees au delay (debounce, etc,...) */
unsigned int debounce_delay=50;
unsigned int next_animation_delay=200;
unsigned int previous_animation_delay=200;
unsigned int selection_screen_transition_delay=500; 
unsigned int cursor_blink_delay=100;

/* variable liees au comportement du menu */
unsigned char page=0;
unsigned char reading=1;
unsigned char Buttonchoice=0;
unsigned char action=0;

/* variables liees aux parametres */
//float* bike_wght=0;


void setup(){
  lcd.begin(16,2);
  lcd.clear();
  //Serial.begin(9600);
  pinMode(RightPin,INPUT_PULLUP);
  pinMode(LeftPin,INPUT_PULLUP);
  pinMode(UpPin,INPUT_PULLUP);
  pinMode(DownPin,INPUT_PULLUP);
  /*pinMode(Button1Pin,INPUT_PULLUP);
  pinMode(Button2Pin,INPUT_PULLUP);*/
}

/*void scrollRight(unsigned char column, unsigned char row, char shift ){
 
 }
 
 void scrollRight(unsigned char column, unsigned row){
 
 }*/

unsigned char debounce_pullup_button(const char pin_input, unsigned int debounce_delay){
  
    if(digitalRead(pin_input)==0 || digitalRead(pin_input)==1) delay(debounce_delay);
     return digitalRead(pin_input); 
}

void wait_button_release(const char pin_input){
  while(digitalRead(pin_input)==0);
}

unsigned char Button_pressed(unsigned int debounce_delay){
  unsigned char reading=1;
  if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
         wait_button_release(RightPin);
         return 1;
        }
    }
   if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          wait_button_release(LeftPin);
          return 2;
        }
   }
   if(digitalRead(UpPin)==0){
      reading = debounce_pullup_button(UpPin,debounce_delay);
        if(reading==0) { 
          wait_button_release(UpPin);
          return 3;
        }
   }
      if(digitalRead(DownPin)==0){
      reading = debounce_pullup_button(DownPin,debounce_delay);
        if(reading==0) { 
          wait_button_release(DownPin);
          return 4;
        }
    }
      /* if(digitalRead(Button1Pin)==0){
      reading = debounce_pullup_button(Button1Pin,debounce_delay);
        if(reading==0) { 
          wait_button_release(Button1Pin);
          return 5;
        }
    }
       if(digitalRead(Button2Pin)==0){
      reading = debounce_pullup_button(Button2Pin,debounce_delay);
        if(reading==0) { 
          wait_button_release(Button2Pin);
          return 6;
        }
    }*/
    return 0;  
}

/*unsigned char menu_action(unsigned char Val_button_pressed, unsigned char* page,unsigned char action, unsigned int cursor_blink_delay, unsigned int next_animation_delay,unsigned int previous_animation_delay){

	// mettre le switch et toute les actions associees (page+=1,etc...)
	action=0;
	switch(Val_button_pressed){
		case 1:
			*page=*page+1;
                        action=1;
			break;
		
		case 2:
			*page-=1;
                        action=1;	
			break;
		case 3:
			lcd.setCursor(14,0);
            lcd.cursor();
          	delay(cursor_blink_delay);
          	lcd.noCursor();
          	break;
        case 4:
            lcd.setCursor(14,1);
            lcd.cursor();
        	delay(cursor_blink_delay);
        	lcd.noCursor();
            break;
        case 5:  
        	// a voir
            break;
        case 6:
        	// a voir
            break;
        default:
        	break;
	}
    return action;
}*/

/*void menu_display(unsigned char* page,unsigned char action, unsigned int next_animation_delay,unsigned int previous_animation_delay){
  
	switch(*page){
//		case 0:
//			// mettre animation transition
//                        if(action)lcd.clear();
//			welcome_screen(next_animation_delay);
//			// mettre animations
//			break;
		case 1:
		        if(action)lcd.clear();
			selection_screen1(next_animation_delay);
			
			break;
		case 2:
                      if(action)lcd.clear();
		      selection_screen2(previous_animation_delay,next_animation_delay);
                      break;
               
                case 3:
                      if(action)lcd.clear();
                      selection_screen3(previous_animation_delay,next_animation_delay);
                      break;
                
	}
			   
}*/
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

void welcome_screen(unsigned int next_anim_delay){
  lcd.setCursor(0,0);
  lcd.print("* VELO WII 2.0 *");
  lcd.setCursor(3,1);
  lcd.print("HOME PAGE");
  next_page(next_anim_delay,14,1);
}

void selection_screen1(unsigned int next_anim_delay){
    lcd.setCursor(4,0);
    lcd.print("1)START");
    lcd.setCursor(4,1);
    lcd.print("2)EDITEUR");
    next_page(next_anim_delay,14,1);
}

void selection_screen2(unsigned int prev_anim_delay, unsigned int next_anim_delay){
    lcd.setCursor(2,0);
    lcd.print("3)PARAMETRES");
    lcd.setCursor(3,1);
    lcd.print("4)RECORDS");
    next_page(next_anim_delay,14,1);
    previous_page(prev_anim_delay,1,1);
  }
  
void selection_screen3(unsigned int prev_anim_delay, unsigned int next_anim_delay){
    lcd.setCursor(5,0);
    lcd.print("5)AIDE");
    lcd.setCursor(4,1);
    lcd.print("6)CREDITS");
    lcd.setCursor(0,1);
    /*lcd.print("7)ABOUT ");
    lcd.print("8)CRED");*/
    next_page(next_anim_delay,14,1);
    previous_page(prev_anim_delay,1,1);  
}
void credits_submenu(){
        lcd.clear();
	lcd.setCursor(3,0);
        lcd.print("TUTEUR");
        lcd.setCursor(4,1);
        lcd.print("M.MONTES");
        delay(3000);
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("REALISATION:");
        delay(3000);
        lcd.clear();  
        lcd.setCursor(5,0);
        lcd.print("JOEL");
        lcd.setCursor(1,1);
        lcd.print("MARTIN-PREVEL");
        delay(3000);
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("MOHAMED SANA");
        lcd.setCursor(1,1);
        lcd.print("MAMADY NABE");
        delay(3000);
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("JORDAN");
        lcd.setCursor(1,1);
        lcd.print("MORRA-TAROUDJIT");
        delay(3000);
        lcd.clear();
        lcd.print("AIDE AUXILIAIRE");
        lcd.setCursor(0,1);
        lcd.print("CLUB ROBOTRONIK!");        
        delay(3000);
        lcd.clear();
}

void help_submenu(){

}

void settings_submenu(/*float bike_wght*/){
  //parametrage masse velo
  /*float bike_wght=0.0;
  float inc_step=0.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("POIDS VELO:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6){
  if(unit>100) unit=100.0;
  if(unit<0.1) unit=0.1;
  if(bike_wght<0.0) bike_wght=0.0;
  if(bike_wght>100.0) bike_wght=100.0;
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          inc_step=1/unit;
          break;
   
  case 3:
          bike_wght+=inc_step;
          break;
  case 4: 
          bike_wght-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(bike_wght);
  lcd.setCursor(12,1);
  lcd.print("Kg");
  lcd.setCursor(4,1);
  }   */    
  /*bike_weight_config(bike_wght);
  while(*bike_wght<90); */   

  // parametrage masse utilisateur

  /*float user_wght=0.0;
  float inc_step=0.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("POIDS CYCLISTE:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6){
  if(user_wght<0.0) user_wght=0.0;
  if(user_wght>95.0){
    lcd.setCursor(14,1);
    lcd.print("!!");
    lcd.setCursor(4,1);
  }
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           if(unit>10) unit=100.0;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          if(unit<0.1) unit=0.1;
          inc_step=1/unit;
          break;
   
  case 3:
          user_wght+=inc_step;
          break;
  case 4: 
          user_wght-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(user_wght);
  lcd.setCursor(12,1);
  lcd.print("Kg");
  lcd.setCursor(4,1);
  } */

  //PARAMETRES CALCUL VITESSE
  // constante moteur
  /*double motor_set=0.0;
  double inc_step=0.0; 
  double unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONSTANTE MOTEUR");
  lcd.setCursor(0,1);
  
  while(buttonchoice!=6){
  if(motor_set<0.0) motor_set=0.0;
  if(motor_set>10.0)motor_set=10;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           if(unit>1000000) unit=1000000.0;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          if(unit<0.1) unit=0.1;
          inc_step=1/unit;
          break;
   
  case 3:
          motor_set+=inc_step;
          break;
  case 4: 
          motor_set-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(motor_set);
  lcd.setCursor(7,1);
  lcd.print("V/tr/min");
  lcd.setCursor(0,1);
  }*/
  
  // rapport reduction pedalier/capteur
  /*float gear_ratio_sensor=0.0;
  float inc_step=0.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("R.RED.CAPTEUR v");
  lcd.setCursor(5,1);
  
  while(buttonchoice!=6){
  if(gear_ratio_sensor<0.0) gear_ratio_sensor=0.0;
  if(gear_ratio_sensor>999.9)gear_ratio_sensor=999.9;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           if(unit>100) unit=100.0;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          if(unit<0.01) unit=0.01;
          inc_step=1/unit;
          break;
   
  case 3:
          gear_ratio_sensor+=inc_step;
          break;
  case 4: 
          gear_ratio_sensor-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(gear_ratio_sensor);
  lcd.setCursor(5,1);
  }*/
  // taille roues velo
  /*int wheel_size=0.0;
  float inc_step=0.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TAILLE DES ROUES");
  lcd.setCursor(6,1);
  
  while(buttonchoice!=6){
  if(wheel_size<0.0) wheel_size=0.0;
  if(wheel_size>999.9)wheel_size=999.9;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           if(unit>1) unit=1;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          if(unit<0.01) unit=0.01;
          inc_step=1/unit;
          break;
   
  case 3:
          wheel_size+=inc_step;
          break;
  case 4: 
          wheel_size-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(wheel_size);
  lcd.setCursor(12,1);
  lcd.print("mm");
  lcd.setCursor(6,1);
  }*/
  
  // rapport reduction plateu/roue
  float gear_ratio=0.0;
  int chain_wheel_teeth_nb=0;
  int cogwheel_teeth_nb=0;
  float inc_step=0.0; 
  float unit = 0.1;
  char pos=0;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BRAQUET");
  lcd.setCursor(,1);
  
  while(buttonchoice!=6){
  if(chain_wheel_teeth_nb<0.0) chain_wheel_teeth_nb=0.0;
  if(chain_wheel_teeth_nb>99) chain_wheel_teeth_nb=99;
  if(cogwheel_teeth_nb<0.0) cogwheel_teeth_nb=0.0;
  if(cogwheel_teeth_nb>99) cogwheel_teeth_nb=99;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           pos++;
           if(unit>1) unit=1;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10;
          pos--;
          if(unit<0.01) unit=0.01;
          inc_step=1/unit;
          break;
   
  case 3:
          if(pos<1)chain_wheel_teeth_nb+=inc_step;
          else cogwheel_teeth_nb+=inc_step;
          break;
  case 4: 
          if(pos<1)chain_wheel_teeth_nb-=inc_step;
          else cogwheel_teeth_nb-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  gear_ratio=chain_wheel_teeth_nb/cogwheel_teeth_nb;
  lcd.print(chain_wheel_teeth_nb);
  lcd.setCursor(2,1);
  lcd.print("x");
  lcd.setCursor(3,1);
  lcd.print(cogwheel_teeth_nb);
  lcd.setCursor(6,1);
  lcd.print("RATIO=");
  lcd.setCursor(12,1);
  lcd.print(gear_ratio);
  lcd.setCursor(0,1);
  }
}

/*void bike_weight_config(float* bike_wght){
   
  float inc_step=0.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("POIDS VELO:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6){
  if(unit>100) unit=100.0;
  if(unit<0.1) unit=0.1;
  if(*bike_wght<0.0) *bike_wght=0.0;
  if(*bike_wght>100.0) *bike_wght=100.0;
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10.0;
           inc_step=1/unit;
           break;         
       
  case 2:   
          unit/=10.0;
          inc_step=1/unit;
          break;
   
  case 3:
          *bike_wght+=inc_step;
          break;
  case 4: 
          *bike_wght-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
          //return *bike_wght;
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(*bike_wght);
  lcd.setCursor(12,1);
  lcd.print("Kg");
  lcd.setCursor(4,1);
  }
}*/

void submenu_display(unsigned char page, unsigned char Buttonchoice/*, float bike_wght*/){ 
  
  switch(page){
    
    case 1:
            //if(Buttonchoice==3) start_submenu();
            //if(Buttonchoice==4) editor_submenu();
            break;
   
    case 2:
            if(Buttonchoice==3) settings_submenu(/*bike_wght*/);
            //if(Buttonchoice==4) records_submenu();    
            break;
    
    case 3:
            //if(Buttonchoice==3) help_submenu();
            if(Buttonchoice==4) credits_submenu();
            break;
    default:
            break;
  }
}
           


void loop(){
  /*menu();*/
  /*while(page==0){

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
    selection_screen1(next_animation_delay);
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
    selection_screen2(previous_animation_delay,next_animation_delay);
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
      }
      
  } 
  while(page==3){
    selection_screen3(previous_animation_delay,next_animation_delay);
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
  reading=1;*/
 
/*start: do{
     welcome_screen(next_animation_delay);
     Buttonchoice = Button_pressed(25);
    action=menu_action(Buttonchoice,page,action,200,200,200);
 }while(Buttonchoice==0);
 
 do {
   menu_display(page,action,200,200);
   Buttonchoice = Button_pressed(25);
   action=menu_action(Buttonchoice,page,action,200,200,200);
   
 }while(*page!=0 || Buttonchoice==0);
  goto start;*/
  do{
      welcome_screen(next_animation_delay);
      Buttonchoice = Button_pressed(25);
  }while(Buttonchoice==0);
  
while(1){ 
  switch(Buttonchoice){
		case 1:
			page++;
                        action=1;
			break;
		
		case 2:
			page--;
                        action=1;	
			break;
		case 3:
                        submenu_display(page,Buttonchoice/*,bike_wght*/);
          	        break;
                case 4:
                        submenu_display(page,Buttonchoice/*,bike_wght*/);
                        break;
                case 5:  
        	// a voir
                        break;
                case 6:
        	// a voir
                        break;
        default:
        	break;
	}

switch(page){
		case 0:
			// mettre animation transition
                        if(action)lcd.clear();
			welcome_screen(next_animation_delay);
			// mettre animations
			break;
		case 1:
		        if(action)lcd.clear();
			selection_screen1(next_animation_delay);
			
			break;
		case 2:
                      if(action)lcd.clear();
		      selection_screen2(previous_animation_delay,next_animation_delay);
                      break;
               
                case 3:
                      if(action)lcd.clear();
                      selection_screen3(previous_animation_delay,next_animation_delay);
                      break;
               default:  
                      page=0; 
	}
action=0;
Buttonchoice = Button_pressed(25);

}
}

