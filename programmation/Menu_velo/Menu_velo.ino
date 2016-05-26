#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const char RightPin = 6;
const char LeftPin=7;
const char UpPin=8;
const char DownPin=9;
const char Button1Pin=10; // validation
const char Button2Pin=13; // cancel 
const int motor_pin = A0; //capteur vitesse

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
float bike_wght=0;
float user_wght=0;
unsigned int wheel_size=0;
int chain_wheel_teeth_nb=0; //plateau
int cogwheel_teeth_nb=0;    //pignon
float gear_ratio=0;
float alpha_slope=0;
float k_motor=0;

/* variables liees a la sauvegarde des parametres*/
byte st_addr = 0;

/* variables liees au capteur de vitesse*/

float v = 0; // vitesse de pedalage en tr/min
float v_lin = 0; // vitesse du cycliste
float gear_ratio_sensor=0;
float motor_set=0;
const unsigned char numReadings = 4;
unsigned int readings[numReadings];      // the readings from the analog input

void setup(){
  lcd.begin(16,2);
  lcd.clear();
  //Serial.begin(9600);
  pinMode(RightPin,INPUT_PULLUP);
  pinMode(LeftPin,INPUT_PULLUP);
  pinMode(UpPin,INPUT_PULLUP);
  pinMode(DownPin,INPUT_PULLUP);
  pinMode(Button1Pin,INPUT_PULLUP);
  pinMode(Button2Pin,INPUT_PULLUP);
  //bike_wght=load_float_variable_EEPROM(st_addr);
  wheel_size=load_int_variable_EEPROM(st_addr);
}

/*void scrollRight(unsigned char column, unsigned char row, char shift ){
 
 }
 
 void scrollRight(unsigned char column, unsigned row){
 
 }*/

byte store_variable_EEPROM(float* value, byte address){
  byte* ptr = (byte*)value;
  byte value_test=0;
  for(byte i=0; i<sizeof(*value);i++){
    EEPROM.write(address,ptr[i]);
    value_test=EEPROM.read(address);
    Serial.print(value_test);
    Serial.print("\t");
    address++;
  }
  return address;
}

byte store_int_variable_EEPROM(unsigned int* value, byte address){
  byte* ptr = (byte*)value;
  byte value_test=0;
  for(byte i=0; i<sizeof(*value);i++){
    EEPROM.write(address,ptr[i]);
    value_test=EEPROM.read(address);
    Serial.print(value_test);
    Serial.print("\t");
    address++;
  }
  return address;
}

float load_float_variable_EEPROM(byte address){
  float* variable=0;
  for(byte i=0;i<sizeof(variable);i++){
   *(variable+i)=EEPROM.read(address);
   address++;
  }
  return *variable;
}  

unsigned int load_int_variable_EEPROM(byte address){
  unsigned int variable;
  for(byte i=0;i<sizeof(variable);i++){
   
   if(i>0) {variable+=(EEPROM.read(address)<<(8*i));}
   else {variable=EEPROM.read(address);}
   Serial.print(variable);
   Serial.print("\t");
   
   address++;
  }
  address-=sizeof(variable);
  return variable;
} 

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
       if(digitalRead(Button1Pin)==0){
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
    }
    return 0;  
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
    //lcd.print("2)EDITEUR");
    next_page(next_anim_delay,14,1);
}

void selection_screen2(unsigned int prev_anim_delay, unsigned int next_anim_delay){
    lcd.setCursor(2,0);
    lcd.print("2)PARAMETRES");
    lcd.setCursor(3,1);
    lcd.print("3)RECORDS");
    next_page(next_anim_delay,14,1);
    previous_page(prev_anim_delay,1,1);
  }
  
void selection_screen3(unsigned int prev_anim_delay, unsigned int next_anim_delay){
    lcd.setCursor(5,0);
    lcd.print("4)AIDE");
    lcd.setCursor(4,1);
    lcd.print("5)CREDITS");
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
        delay(3000);
        lcd.clear();
        lcd.setCursor(3,0);
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
        lcd.setCursor(0,2);
        lcd.print("LE MAGASIN!");        
        delay(3000);
        lcd.setCursor(0,1);
        lcd.print("CLUB ROBOTRONIK!");        
        delay(3000);
        lcd.clear();
        lcd.print("AIDE AUXILIAIRE");
        lcd.setCursor(4,1);
        lcd.print("INProd!");
        delay(3000);
        lcd.clear();
        
}

void help_submenu(){
	
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Avant de lancer");
		lcd.setCursor(0,1);
		lcd.print("une simulation");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Aller dans le");
		lcd.setCursor(0,1);
		lcd.print("menu PARAMETRES");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("pour configurer");
		lcd.setCursor(0,1);
		lcd.print("le velo.");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Pour entrer une");
		lcd.setCursor(0,1);
		lcd.print("valeur de");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("parametre:");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("fleche droite ->");
		lcd.setCursor(0,1);
		lcd.print("increment/10");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("fleche gauche ->");
		lcd.setCursor(0,1);
		lcd.print("increment*10");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("fleche haut ->");
		lcd.setCursor(0,1);
		lcd.print("incrementer");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("fleche bas ->");
		lcd.setCursor(0,1);
		lcd.print("decrementer");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Attention le");
		lcd.setCursor(0,1);
		lcd.print("poids maximal");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("utilisateur est");
		lcd.setCursor(0,1);
		lcd.print("de 95Kg.");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Pour demarrer");
		lcd.setCursor(0,1);
		lcd.print("une simulation");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("aller dans le");
		lcd.setCursor(0,1);
		lcd.print("menu START");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("puis suivre les");
		lcd.setCursor(0,1);
		lcd.print("instructions.");
		while(Button_pressed(debounce_delay)!=1);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Les meilleurs");
		lcd.setCursor(0,1);
		lcd.print("temps sont");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
                lcd.setCursor(0,0);
		lcd.print("visibles dans le");
		lcd.setCursor(0,1);
		lcd.print("menu RECORDS.");
		while(Button_pressed(debounce_delay)!=1);
                lcd.clear();
}

void settings_submenu(){
  //parametrage masse velo
  //float bike_wght=0.0;
  float inc_step=1.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("POIDS VELO:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
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
          //st_addr=store_variable_EEPROM(&bike_wght,st_addr);// enregistrement dans flash du poids velo
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
  }        

  // parametrage masse utilisateur

  //float user_wght=0.0;
  inc_step=1.0; 
  unit=0.1;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("POIDS CYCLISTE:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
  if(user_wght<0.0) user_wght=0.0;
  /*if(user_wght>95.0){
    lcd.setCursor(14,1);
    lcd.print("!!");
    lcd.setCursor(4,1);
  }*/
    
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
        //st_addr=store_variable_EEPROM(&user_wght,st_addr);
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
  }

  //PARAMETRES CALCUL VITESSE
  // constante moteur
  //double motor_set=0.0;
  inc_step=1.0; 
  unit=0.1;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONSTANTE MOTEUR");
  lcd.setCursor(0,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
  if(motor_set<0.0) motor_set=0.0;
  if(motor_set>10.0)motor_set=10;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           unit*=10;
           if(unit>100) unit=100.0;
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
        motor_set/=1000;
        //st_addr=store_variable_EEPROM(&motor_set,st_addr);
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(motor_set);
  lcd.setCursor(4,1);
  lcd.print("V/1000tr/min");
  lcd.setCursor(0,1);
  }
  
  // rapport reduction pedalier/capteur
  //float gear_ratio_sensor=0.0;
  inc_step=1.0; 
  unit=0.1;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("R.RED.CAPTEUR v");
  lcd.setCursor(5,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
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
       // st_addr=store_variable_EEPROM(&user_wght,st_addr);
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(gear_ratio_sensor);
  lcd.setCursor(5,1);
  }
  // taille roues velo
  //int wheel_size=0.0;
  inc_step=1.0; 
  unit=0.1;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TAILLE DES ROUES");
  lcd.setCursor(6,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
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
        store_int_variable_EEPROM(&wheel_size, st_addr);
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
       wheel_size=load_int_variable_EEPROM(st_addr);
         break;
  
  default:
         break;
  }
  
  lcd.print(wheel_size);
  lcd.setCursor(12,1);
  lcd.print("mm");
  lcd.setCursor(6,1);
  }
  
  // rapport reduction plateu/roue
  //float gear_ratio=0.0;
  //int chain_wheel_teeth_nb=0;
  //int cogwheel_teeth_nb=0;
  inc_step=1.0; 
  unit = 0.1;
  char pos=0;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BRAQUET");
  lcd.setCursor(0,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
  if(chain_wheel_teeth_nb<0.0) chain_wheel_teeth_nb=0.0;
  if(chain_wheel_teeth_nb>99) chain_wheel_teeth_nb=99;
  if(cogwheel_teeth_nb<0.0) cogwheel_teeth_nb=0.0;
  if(cogwheel_teeth_nb>99) cogwheel_teeth_nb=99;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           pos++;
           if(pos==1)break;
           /*unit*=10;
           
           if(unit>1) unit=1;
           inc_step=1/unit;*/
           break;         
       
  case 2:   
          pos--;
          /*if(pos==1)break;
          unit/=10;
          
          if(unit<0.01) unit=0.01;
          inc_step=1/unit;*/
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
  gear_ratio=(float)chain_wheel_teeth_nb/(float)cogwheel_teeth_nb;
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

void start_submenu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Indiquer la");
  lcd.setCursor(0,1);
  lcd.print("pente a simuler");
  while(Button_pressed(debounce_delay)!=1);
  
}

void submenu_display(unsigned char page, unsigned char Buttonchoice/*, float bike_wght*/){ 
  
  switch(page){
    
    case 1:
            if(Buttonchoice==3) start_submenu();
            //if(Buttonchoice==4) editor_submenu();
            lcd.clear();
            break;
   
    case 2:
            if(Buttonchoice==3) settings_submenu();
            //if(Buttonchoice==4) records_submenu(); 
            lcd.clear();   
            break;
    
    case 3:
            if(Buttonchoice==3) help_submenu();
            if(Buttonchoice==4) credits_submenu();
            break;
    default:
            break;
  }
}
           
/* fonctions liees au capteur de vitesse */

double drive_speed(int analogValue, float gear_ratio_sensor, float motor_param){
  float v_d= analogValue/gear_ratio_sensor*5.0/1023.0/motor_param;
  return v_d;
}
  
double linear_speed(float v, float wheel_diam, float gear_ratio){ 
  float wheel_size = 2.54*wheel_diam/100.0; //conversion centimetre --> metre
   float v_rd = v*gear_ratio*3.14/30.0;
   float s = v_rd*wheel_size*3.6; // vitesse du cycliste en km/h
   return s;
}

void init_tab(unsigned int* tab, unsigned char Size){
  for (int thisReading = 0; thisReading < Size; thisReading++)
    *(readings+thisReading) = 0;
}
//average function

float running_average(const int analogInputPin, const unsigned char numReading, unsigned int* tab){
  static unsigned char i = 0;
  static unsigned int index;                  // the index of the current reading
  static float total;                  // the running total
  static float average;
 if(i==0){ 
   index = 0;                 
   total = 0;                  
   average = 0; 
   i++;
 }
 total= total - *(tab+index);         
  // read from the sensor:  
  *(tab+index) = analogRead(analogInputPin);
  // add the reading to the total:
  total= total + *(tab+index);       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings; 
  return average;
}

/* fonctions liees a la commande du moteur */

void motor_brake_cmd( const char ena, const char in1, const char in2, int pwm_value){
  
 analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  // pour la phase de test
  //delay(10000);
}

void motor_loose_cmd( const char ena, const char in1, const char in2, int pwm_value){
  
  analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  // pour la phase de test
  //delay(10000);
}

void motor_stop_cmd(const char ena, const char in1, const char in2, int pwm_value){
  
  analogWrite(ena,pwm_value);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  // pour phase de test. Peut etre a garder ??
  delay(1000);
}

void loop(){
  /*menu();*/

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
                        submenu_display(page,Buttonchoice);
          	        break;
                case 4:
                        submenu_display(page,Buttonchoice);
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

