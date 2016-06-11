#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "math.h"
/* Programme du simulateur de trace cycliste */
/* Pour comprendre le fonctionnement du menu lire les commentaires de ce 
fichier */ 

/******************************************************/

/* initialisation de l'afficheur LCD avec la carte arduino UNO */
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* Boutons correspondants a la croix directionnelle */
const char RightPin = 6;	// fleche droite
const char LeftPin=7;		// fleche gauche
const char UpPin=0;			// fleche haut
const char DownPin=9;		// fleche bas

/* Boutons correspondants a X,Y,A,B */ 
//const char Button1Pin=10; // validation --> non utilise car plus d'entree
/* de libre */
const char Button2Pin=13; // cancel 

/* Entree analog pour la mesure de la vitesse de pedalage */
const int motor_pin = A0; //capteur vitesse

/* Sortie de commande du L293 */
/* --> in1 est inversee par un transistor 2N2222 pour creer le signal in2 */ 
const char ena=10;	// simple enable ou variation de vitesse si PWM
const char in1=1;	/*broche de controle du sens de rotation moteur et commande marche arret 

/* variables liees au delay (debounce, etc,...) */
unsigned int debounce_delay=50;						// temps d'attente pour l'anti rebond
unsigned int next_animation_delay=200;				// temps pour animation qui indique une page suivante dispo dans le menu
unsigned int previous_animation_delay=200;			// temps pour animation qui indique une page precedante dispo dans le menu
// unsigned int selection_screen_transition_delay=500;	// inutilisee 
// unsigned int cursor_blink_delay=100;					// inutilisee

/* variable liees au comportement du menu */
unsigned char page=0;			// numero de page du menu principal
unsigned char reading=1;		// valeur lue sur une entree du µC (boutons) 
unsigned char Buttonchoice=0;	// bouton appuye
unsigned char action=0;			// indique un changement de page du menu principal

/* variables liees aux parametres */
float bike_wght=0; 			// masse velo en Kg //cos(30.0*PI/180.0);
float user_wght=0;			// masse utilisateur en Kg
unsigned int wheel_size=0;	// diametre des roues du velo en pouce(1''=2.54cm)
int chain_wheel_teeth_nb=0;	// nombre de dents du plateau (pedalier)
int cogwheel_teeth_nb=0;    // nombre de dents du pignon (roue)
float gear_ratio=0;			// rapport entre nb dents pignon/nb dents plateau
float alpha_slope=0;		// inclinaison de la pente en degres
float k_motor=0;			// inutilisee

/*variables liees a la simulation */
//float pente_simu = 0;		// inutilisee
float couple_simu = 0;		// valeur du couple a applique au niveau du pedalier
float temps_simu = 0;		// telmps pendant lequel le moteur tourne afin de creer le couple
float longueur_fil = 0;		// longueur de fil a enrouler autour de l'axe moteur
float force_simu = 0;		// 
float eta = 0.90;			//
float beta = 0.33;
float rayon = 0.0;
float viscosite_air = 0.25;
float vitesse_rot = 12.5;	// vitesse de rotation du pedalier
float accel_pesanteur = 9.8;	// valeur de g sur terre
//float masse = 45;				// inutilisee
float frot_sol = 0.002;			// coefficient de frottement entre pneu et sol
float couple_const = 0;			// composante constante du couple a appliquer au niveau du pedalier
float F_ressort = 0;			// force au niveau du ressort (capteur de force)
float average = 0;				// valeur moyenne des mesures du CAN de la tension fournit par le capteur de vitesse

/* variables liees a la sauvegarde des parametres*/
byte st_addr = 0;

/* variables liees au capteur de vitesse*/

float v = 0; 				// vitesse de pedalage en tr/min
double v_lin = 0; 			// vitesse du cycliste
float gear_ratio_sensor=0;	// rapport entre vitesse rotation pedalier et vitesse de rotation du rotor du capteur (58/12 = 4.83 actuellement)
float motor_set=0;			// constante de fem du capteur de vitesse (MCC)
const unsigned char numReadings = 4;	// nb valeur pour calcul de la moyenne glissante de valeur du CAN pour l'entree analog du capteur
unsigned int readings[numReadings];      // valeur de l'entree analog

/* initiallisation du systeme */

void setup(){
  lcd.begin(16,2);		// initialisation lcd	
  lcd.clear();			// on efface son affichage
  
  /* configuration des broches de la carte arduino en entree ou sorties */
  
  pinMode(ena,OUTPUT);	
  pinMode(in1,OUTPUT);
  analogWrite(ena,0);
  //Serial.begin(9600);
  pinMode(RightPin,INPUT_PULLUP);
  pinMode(LeftPin,INPUT_PULLUP);
  pinMode(UpPin,INPUT_PULLUP);
  pinMode(DownPin,INPUT_PULLUP);
  //pinMode(Button1Pin,INPUT_PULLUP);
  pinMode(Button2Pin,INPUT_PULLUP);
  
  /* recuperation des parametres stockes dans l'EEPROM */
  
  //bike_wght=load_float_variable_EEPROM(st_addr);
  //wheel_size=load_int_variable_EEPROM(st_addr);
  
  /* initialisation du tableau utilisee pour la moyenne glissante */
  init_tab(readings,4);
}

/*void scrollRight(unsigned char column, unsigned char row, char shift ){
 
 }
 
 void scrollRight(unsigned char column, unsigned row){
 
 }*/

/* fonction pour stocker un nombre a virgule flottante dans l'EEPROM */
/* bits de poids faible en 1er */
byte store_variable_EEPROM(float* value, byte address){
  byte* ptr = (byte*)value;		// conversion en pointeur sur des entiers 8bits
  byte value_test=0;
  
  /* enregistrement dans l'EEPROM */
  for(byte i=0; i<sizeof(*value);i++){
    EEPROM.write(address,ptr[i]);
    value_test=EEPROM.read(address);
    Serial.print(value_test);
    Serial.print("\t");
    address++;
  }
  return address;	// on retourne l'adresse de stockage de l'EEPROM
}

/* fonction pour stocker un entier dans l'EEPROM */
/* bits de poids faible en 1er */
/* --> pas testee pour des entiers signes mais doit marcher */

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

/* fonction pour recuperer un nombre a virgule flottante dans l'EEPROM */
/* --> NE MARCHE PAS !!!!! */
float load_float_variable_EEPROM(byte address){
  float* variable=0;
  for(byte i=0;i<sizeof(variable);i++){
   *(variable+i)=EEPROM.read(address);
   address++;
  }
  return *variable;
}  

/* fonction pour recuperer un entier dans l'EEPROM */

unsigned int load_int_variable_EEPROM(byte address){
  unsigned int variable;
  
  /* decalages successifs de 8bits pour recuperer la bonne valeur */
  
  for(byte i=0;i<sizeof(variable);i++){
   
   if(i>0) {variable+=(EEPROM.read(address)<<(8*i));}
   else {variable=EEPROM.read(address);}
   Serial.print(variable);
   Serial.print("\t");
   
   address++;
  }
  address-=sizeof(variable);	//on restaure l'ancienne valeur de l'adresse de stockage dans l'EEPROM --> rappel address est une variables globale
  return variable;				// on retourne la valeur de l'entier stocke
} 

/* fonction d'anti rebond */

unsigned char debounce_pullup_button(const char pin_input, unsigned int debounce_delay){
  
    if(digitalRead(pin_input)==0 || digitalRead(pin_input)==1) delay(debounce_delay); // on attend un certain temps
     return digitalRead(pin_input);	// on retourne la valeur de l'entree
}

/* fonction d'attente de relachement du bouton */
/* ATTENTION BLOQUE L'EXECUTION DU PROGRAMME */

void wait_button_release(const char pin_input){
  while(digitalRead(pin_input)==0);
}

/* fonction de detection du bouton appuye */
/* un etat haut --> pas d'appui
   un etat bas --> appui */
unsigned char Button_pressed(unsigned int debounce_delay){
  unsigned char reading=1;
  if(digitalRead(RightPin)==0){
      reading = debounce_pullup_button(RightPin,debounce_delay);
        if(reading==0) { 
         wait_button_release(RightPin);
         return 1;	// si bouton fleche droite appuye
        }
    }
   if(digitalRead(LeftPin)==0){
      reading = debounce_pullup_button(LeftPin,debounce_delay);
        if(reading==0) { 
          wait_button_release(LeftPin);
          return 2;	// si bouton fleche gauche appuye
        }
   }
   if(digitalRead(UpPin)==0){
      reading = debounce_pullup_button(UpPin,debounce_delay);
        if(reading==0) { 
          wait_button_release(UpPin);
          return 3;	// si bouton fleche haut appuye
        }
   }
      if(digitalRead(DownPin)==0){
      reading = debounce_pullup_button(DownPin,debounce_delay);
        if(reading==0){ 
          wait_button_release(DownPin);
          return 4;	// si bouton fleche bas appuye
        }
    }
      /* if(digitalRead(Button1Pin)==0){
      reading = debounce_pullup_button(Button1Pin,debounce_delay);
        if(reading==0) { 
          wait_button_release(Button1Pin);
          return 5;	// si bouton de validation appuye
        }
    }*/
       if(digitalRead(Button2Pin)==0){
      reading = debounce_pullup_button(Button2Pin,debounce_delay);
        if(reading==0) { 
          wait_button_release(Button2Pin);
          return 6;	// si bouton de cancel appuye
        }
    }
    return 0;  // si aucun bouton appuye
}

/* fonction d'animation indiquant une page suivante disponible */

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

/* fonction d'animation indiquant une page precedante disponible */

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

/* page d'accueil du simulateur */

void welcome_screen(unsigned int next_anim_delay){
  lcd.setCursor(0,0);
  lcd.print("*  RANGE ONE  *");
  lcd.setCursor(3,1);
  lcd.print("HOME PAGE");
  next_page(next_anim_delay,14,1);
}

/* fonction affichant la 1ere page du menu principal */

void selection_screen1(unsigned int next_anim_delay){
    lcd.setCursor(4,0);
    lcd.print("1)START");
    lcd.setCursor(4,1);
    //lcd.print("2)EDITEUR");
    next_page(next_anim_delay,14,1);
}

/* fonction affichant la 2eme page du menu principal */

void selection_screen2(unsigned int prev_anim_delay, unsigned int next_anim_delay){
    lcd.setCursor(2,0);
    lcd.print("2)PARAMETRES");
    lcd.setCursor(3,1);
    lcd.print("3)RECORDS");
    next_page(next_anim_delay,14,1);
    previous_page(prev_anim_delay,1,1);
  }
  
/* fonction affichant la 3eme page du menu principal */  
  
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

/* fonction affichant le sous menu des credits */

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
        lcd.setCursor(0,1);
        lcd.print("LE MAGASIN!");        
        delay(3000);
        lcd.setCursor(0,1);
        lcd.print("CLUB ROBOTRONIK!");        
        delay(3000);
        lcd.clear();
        lcd.setCursor(4,1);
        lcd.print("INProd!");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("LUCAS JULLIARD");
        delay(3000);
        lcd.clear();
        
}

/* fonction affichant le sous menu d'aide indiquant a l'utilisateur comment marche le systeme */

void help_submenu(){
	
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Avant de lancer");
		lcd.setCursor(0,1);
		lcd.print("une simulation");
		while(Button_pressed(debounce_delay)!=1);	// attente d'appui sur la fleche droite
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

/* fonction affichant le sous menu de saisie des parametres du systeme */

void settings_submenu(){
  /* parametrage masse velo */
  
  //float bike_wght=0.0;
  float inc_step=1.0;	// valeur de l'increment
  float unit=0.1;		// valeur qui permet de changer la valeur de inc_step pas tres utile en fait
  unsigned char buttonchoice=0;	// bouton appuye
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("MASSE VELO:");
  lcd.setCursor(4,1);
  /* tant que pas d'appui sur bouton de validation ou de cancel */
  while(buttonchoice!=6 && buttonchoice!=5){
  if(unit>100) unit=100.0;
  if(unit<0.1) unit=0.1;
  if(bike_wght<0.0) bike_wght=0.0;
  if(bike_wght>100.0) bike_wght=100.0;
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
  /* division par 10 de inc_step si appui sur fleche droite */ 
  case 1: 
           unit*=10;
           inc_step=1/unit;
           break;         
  /* multiplication par 10 de inc_step si appui sur fleche gauche */     
  case 2:   
          unit/=10;
          inc_step=1/unit;
          break;
  /* incrementation de la masse du velo si appui sur fleche haut */
  case 3:
          bike_wght+=inc_step;
          break;
  /* decrementation de la masse du velo si appui sur fleche bas */        
  case 4: 
          bike_wght-=inc_step;
          break;
  /* enregistrement de la valeur dans l'EEPROM si appui sur bouton validation */
  case 5:
          //st_addr=store_variable_EEPROM(&bike_wght,st_addr);// enregistrement dans flash du poids velo
          break;
  /* si appui sur bouton cancel */
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
         
  /* saisie du parametre suivant selon la meme procedure */
  // parametrage masse utilisateur

  //float user_wght=0.0;
  inc_step=1.0; 
  unit=0.1;
  buttonchoice=0;
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("MASSE CYCLISTE:");
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
  if(wheel_size<0) wheel_size=0;
  if(wheel_size>99)wheel_size=99;
    
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: 
           /*unit*=10;
           if(unit>1) unit=1;
           inc_step=1/unit;*/
           break;         
       
  case 2:   
          /*unit/=10;
          if(unit<0.01) unit=0.01;
          inc_step=1/unit;*/
          break;
   
  case 3:
          wheel_size+=inc_step;
          break;
  case 4: 
          wheel_size-=inc_step;
          break;
  case 5:
        // enregistrement dans flash du poids velo
        //store_int_variable_EEPROM(&wheel_size, st_addr);
          break;
  case 6:
       // rechargement de l'ancienne valeur du poids
       //wheel_size=load_int_variable_EEPROM(st_addr);
         break;
  
  default:
         break;
  }
  
  lcd.print(wheel_size);
  lcd.setCursor(12,1);
  lcd.print("''");
  lcd.setCursor(6,1);
  }
  
  // rapport reduction plateu/roue
  //float gear_ratio=0.0;
  //int chain_wheel_teeth_nb=0;
  //int cogwheel_teeth_nb=0;
  inc_step=1.0; 
  unit = 0.1;
  char pos=0;	//indique la valeur a modifier entre chain_wheel et cogwheel
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

/* fonction affichant le sous menu de lancement de la simulation */

void start_submenu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Indiquer la");
  lcd.setCursor(0,1);
  lcd.print("pente a simuler");
  while(Button_pressed(debounce_delay)!=1);	// attente appui fleche droite
  /* saisie inclinaison pente */
  float inc_step=1.0; 
  float unit=0.1;
  unsigned char buttonchoice=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("INCLINAISON PENTE:");
  lcd.setCursor(4,1);
  
  while(buttonchoice!=6 && buttonchoice!=5){
  if(unit>20.0) unit=20.0;
  if(unit<0.1) unit=0.1;
  if(alpha_slope<0.0) alpha_slope=0.0;
  if(alpha_slope>20.0) alpha_slope=20.0;
  buttonchoice=Button_pressed(25);
  switch(buttonchoice){
    
  case 1: // droite
           unit*=10;
           inc_step=1/unit;
           break;         
       
  case 2: // gauche  
          unit/=10;
          inc_step=1/unit;
          break;
   
  case 3: //haut
          alpha_slope+=inc_step;
          break;
  case 4: //bas
          alpha_slope-=inc_step;
          break;
  case 5:  //validation
          //st_addr=store_variable_EEPROM(&bike_wght,st_addr);// enregistrement dans flash du poids velo
          break;
  case 6:  // retour
       // rechargement de l'ancienne valeur du poids
         break;
  
  default:
         break;
  }
  
  lcd.print(alpha_slope);
  lcd.setCursor(12,1);
  lcd.print("deg");
  lcd.setCursor(4,1);
  }
  /* calcul du couple a appliquer */
  float dist_r=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("debut simu");
  lcd.setCursor(0,1);
  /*average=running_average(motor_pin,4, readings);
  vitesse_rot=drive_speed(average,4.3, 0.00094);
  vitesse_rot=vitesse_rot*PI/30;
  lcd.print(vitesse_rot);*/
  calcul_elements();
  /* mise en route du moteur */
  motor_brake_cmd(ena,in1,/*,in2*/255,temps_simu);
  /* arret et maintien en position du rotor */
  motor_stop_cmd(ena,in1,/*in2*/25);
  
  /* calcul et affichage de la vitesse de pedalage et lineaire du cycliste */ 
  do{
  lcd.clear();
  lcd.setCursor(0,0);
  average=running_average(motor_pin,4, readings);
  vitesse_rot=drive_speed(average,4.83, 0.00094);
  v_lin=linear_speed(vitesse_rot, wheel_size, beta);
  lcd.print(vitesse_rot);
  //vitesse_rot=vitesse_rot*PI/30;
  buttonchoice=Button_pressed(25);
  lcd.setCursor(6,0);
  lcd.print("tr/min");
  lcd.setCursor(0,1);
  lcd.print(v_lin);
  lcd.setCursor(6,1);
  lcd.print("km/h");
  delay(1000);
  dist_r=dist_r+v_lin/3600.0;	// calcul distance parcourue
  }while(buttonchoice!=6);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fin simu :)");
  /* retour en position initiale du rotor */
  motor_loose_cmd(ena,in1,200,temps_simu);
  /* arret et maintien en position du rotor */
  motor_stop_cmd(ena,in1,/*in2*/25);
  /* affichage de la distance parcourue */
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BRAVO VOUS AVEZ");
  lcd.setCursor(0,1);
  lcd.print("PARCOURU:");
  lcd.print(dist_r);
  lcd.print("km");
  while(Button_pressed(debounce_delay)!=6); // attente appui bouton validation pour revenir au menu principal */
  lcd.clear();
}

/* fonction affichant le sous menu des records */

void records_submenu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("JOEL a realise");
  lcd.setCursor(0,1);
  lcd.print("un exploit.");
  while(Button_pressed(debounce_delay)!=1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Il a pedale");
  lcd.setCursor(0,1);
  lcd.print("pendant 1 heure."); 
  while(Button_pressed(debounce_delay)!=1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pourrez-vous le");
  lcd.setCursor(0,1);
  lcd.print("battre ?? :P");
  while(Button_pressed(debounce_delay)!=1);
}

/* fonction de selection du sous menu a afficher */
void submenu_display(unsigned char page, unsigned char Buttonchoice/*, float bike_wght*/){ 
  
  switch(page){
    
    case 1:
            if(Buttonchoice==3) start_submenu();
            //if(Buttonchoice==4) editor_submenu();
            lcd.clear();
            break;
   
    case 2:
            if(Buttonchoice==3) settings_submenu();
            if(Buttonchoice==4) records_submenu(); 
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

/* fonction calculant le couple a appliquer (force appliquee sur les plaquette) au niveau du pedalier */
void calcul_elements(){
  rayon = (float)wheel_size*2.54/100.0;	// calcul rayon de la roue
  couple_const = eta*beta*rayon;		// calcul composante constante du couple a appliquer
  couple_simu = couple_const*(viscosite_air*(beta*vitesse_rot*rayon)*(beta*vitesse_rot*rayon) + (user_wght+bike_wght)*accel_pesanteur*sin(alpha_slope*PI/180) + frot_sol*(user_wght+bike_wght)*accel_pesanteur*cos(alpha_slope*PI/180));	// calcul du couple a appliquer
  F_ressort = 5.5*couple_simu;
  longueur_fil =2*F_ressort/2821.0 ;	// 2821 --> raideur du ressort en 
  temps_simu = 0.8*longueur_fil;
  
}
           
/* fonctions liees au capteur de vitesse */


/* calcul de la vitesse de pedalage */
double drive_speed(float analogValue, float gear_ratio_sensor, float motor_param){
  float v_d= analogValue/gear_ratio_sensor*5.0/1023.0/motor_param;
  return v_d;
}

/* calcul de la vitesse de deplacement du cycliste */  
double linear_speed(float v, float wheel_diam, float gear_ratio){ 
   float wheel_d = 2.54*wheel_diam/100.0; //conversion centimetre --> metre
   float v_rd = v/gear_ratio*3.14/30.0;
   float s = v_rd*wheel_d/2.0*3.6; // vitesse du cycliste en km/h
   return s;
}

/* initialisation d'un tableau pour la moyenne glissante */
void init_tab(unsigned int* tab, unsigned char Size){
  for (int thisReading = 0; thisReading < Size; thisReading++)
    *(readings+thisReading) = 0;
}

/* calcul de la moyenne glissante */
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

/* serre les plaquettes sur le disque */
void motor_brake_cmd( const char ena, const char in1, /*const char in2,*/ int pwm_value, float on_time){
  on_time*=60000;
 analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,HIGH);
  //digitalWrite(in2,LOW);
  // pour la phase de test
  delay(on_time);
}

/* deserre les plaquettes sur le disque */
void motor_loose_cmd( const char ena, const char in1,/* const char in2,*/ int pwm_value, float on_time){
  on_time*=60000;
  analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,LOW);
  //digitalWrite(in2,HIGH);
  // pour la phase de test
  delay(on_time);
}

/* arrete la rotation du moteur et maintien le rotor en position */
void motor_stop_cmd(const char ena, const char in1/*, const char in2*/, int pwm_value){
  
  analogWrite(ena,pwm_value);
  digitalWrite(in1,HIGH);
  //digitalWrite(in2,HIGH);
  // pour phase de test. Peut etre a garder ??
  delay(1500);	// attente avant nouvelle commande du moteur pour ne pas tout casser ^^
}


/* boucle du programme principal */
void loop(){
  /*menu();*/
  /* gestion du menu */
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

