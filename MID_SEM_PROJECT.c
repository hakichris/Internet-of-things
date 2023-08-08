
#include <MKL25Z4.H>
#include <stdint.h>

volatile unsigned long counter=0;
void AB_ON ();
void BG_ON ();
void GF_ON ();
void FA_ON ();
void AB_OFF ();
void BG_OFF ();
void GF_OFF ();
void FA_OFF ();
void CHECK ();
void FAST_FORWARD_CLOCLWISE_ROTATION(void);
void NORMAL_FORWARD_CLOCLWISE_ROTATION(void);
void FAST_REVERSE_CLOCLWISE_ROTATION(void);
void NORMAL_REVERSE_CLOCLWISE_ROTATION(void);
void FORWARD1 (void);
void FAST_FORWARD();
void STOP (void);
void RED_CONSTANT_RATE(void);
void REVERSE2 ();
void RED_FLASH();
void YELLOW_CONSTANT_RATE();
void YELLOW_FLASH();
void FAST_REVERSE1();
void initialize(void);
unsigned long  millis(void);
void SysTick_Handler(void);


void pintest();



int main(){
#define MASK(x) (1UL << x)
#define LED_BLUE  (1)
#define LED_GREEN  (19)
#define LED_RED  (18)
#define YELLOW_EXT (3)
#define RED_EXT (4)
#define SW1  (9)  // STOP-GO PTC9
#define SW2 (8)    // BACKWARD-FORWARD, PTC8
#define SW3  (1)   // SLOWFAST PTE1


//7 Segment display

#define G (5)
#define A (1)
#define B (0)
#define F (2)

#define FAST_SPEED (25)
#define NORMAL_SPEED (50)

#define RED_YELLOW_INTERVAL (1000)
#define WHITE_INTERVAL (500)
#define BLUE_INTERVAL (500)

#define WHITE_ON_1 (125)
#define WHITE_ON_2 (125)
#define WHITE_OFF_1 (125)
#define WHITE_OFF_2 (500)

#define BLUE_ON_1 (125)
#define BLUE_ON_2 (125)
#define BLUE_OFF_1 (125)
#define BLUE_OFF_2 (500)

#define RED_ON (600)
#define RED_OFF (600)
#define YELLOW_ON (1000)
#define YELLOW_OFF (1000)

#define RED_ON_1 (1000)
#define RED_OFF_1 (1000)
#define YELLOW_ON_1 (400)
#define YELLOW_OFF_1 (400)




SysTick->LOAD = (20971520u/1000u)-1 ;  //configure for every 1 sec restart.
	   SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
	    	SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk; //7 enable+use system clock +interrupts

	initialize();
	while (1)
	{
		CHECK();
		FORWARD1();
		FAST_FORWARD();
		 REVERSE2 ();
		 FAST_REVERSE1();
		 RED_CONSTANT_RATE();
		 RED_FLASH();
		 YELLOW_FLASH();
		 YELLOW_CONSTANT_RATE();
		 FAST_FORWARD_CLOCLWISE_ROTATION();
		 NORMAL_FORWARD_CLOCLWISE_ROTATION();
		 FAST_REVERSE_CLOCLWISE_ROTATION();
		 NORMAL_REVERSE_CLOCLWISE_ROTATION();
		 STOP();

	}
}



void initialize(void){
	//clock gating to port B & D
	SIM->SCGC5|= ((1UL<<10) | (1<<12))  ;
	SIM->SCGC5|= ((1UL<<11) | (1<<13))  ; ////gate ports B & D

//set up pins as GPIO --> configure mux to connect pins as GPIO
PORTB->PCR[LED_GREEN] &=~0X700UL;
PORTB->PCR[LED_GREEN] |= (1UL<<8);

PORTB->PCR[LED_RED] &=~0X700UL;
PORTB->PCR[LED_RED] |= (1UL<<8);

PORTD->PCR[LED_BLUE] &=~0X700UL;
PORTD->PCR[LED_BLUE] |= (1UL<<8);

PORTD->PCR[YELLOW_EXT] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[YELLOW_EXT] |= PORT_PCR_MUX(1);

PORTD->PCR[RED_EXT] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[RED_EXT] |= PORT_PCR_MUX(1);

// 7 segment display pins
PORTB->PCR[A] &=~0X700UL;
PORTB->PCR[A] |= (1UL<<8);

PORTB->PCR[B] &=~0X700UL;
PORTB->PCR[B] |= (1UL<<8);

PORTB->PCR[F] &=~0X700UL;
PORTB->PCR[F] |= (1UL<<8);

PORTD->PCR[G] &=~0X700UL;
PORTD->PCR[G] |= (1UL<<8);

// INPUT PINS
PORTC->PCR[SW1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTC->PCR[SW1] |= PORT_PCR_MUX(1);

PORTC->PCR[SW2] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTC->PCR[SW2] |= PORT_PCR_MUX(1);

PORTE->PCR[SW3] &= ~PORT_PCR_MUX_MASK;	//Clear mux
PORTE->PCR[SW3] |= PORT_PCR_MUX(1);

// PULL UP
//PORTC->PCR[SW1] |=0x2;		//or PORT_PCR_PE_MASK;
//PORTC->PCR[SW2] |=0x2;
//PORTE->PCR[SW3] |=0x2;

//set up led to be output

PTB->PDDR |= MASK(LED_GREEN );
PTB->PDDR |= MASK(LED_RED) ;
PTD->PDDR |= MASK(LED_BLUE );
PTD->PDDR |= MASK(YELLOW_EXT ) ;
PTD->PDDR |= MASK(RED_EXT) ;

PTC->PDDR &= ~MASK(SW1 ) ;		//clear to make input
PTC->PDDR &= ~MASK(SW2 ) ;
PTE->PDDR &= ~MASK(SW3) ;


// 7 Segment Display
PTB->PDDR |= MASK(A);
PTB->PDDR |= MASK(B);
PTB->PDDR |= MASK(F);
PTD->PDDR |= MASK(G) ;



// TURN OFF AL
PTB->PSOR =MASK(LED_RED);
PTB->PSOR =MASK(LED_GREEN);
PTD->PCOR =MASK(YELLOW_EXT);
PTD->PSOR =MASK(LED_BLUE);
PTD->PCOR =MASK(RED_EXT);
//7 Segment Display

PTD->PCOR =MASK(G);
PTB->PCOR =MASK(A);
PTB->PCOR =MASK(B);
PTB->PCOR =MASK(F);

}

int getValue1=0;
int getValue2=0;
int getValue3=0;



void CHECK (){

  if(PTC->PDIR & MASK(SW1))
	getValue1=1;
    else
	   getValue1=0;

  if (PTC->PDIR & MASK(SW2))
	getValue2=1;

   else
  	     getValue2=0;

  if (PTE->PDIR & MASK(SW3))
	getValue3=1;

    else
		getValue3=0;

   }




void White_ON(void){
	PTB->PCOR|=MASK(LED_GREEN);
	PTD->PCOR|=MASK(LED_BLUE);
	PTB->PCOR|=MASK(LED_RED);
}
void White_OFF(){
	PTB->PSOR|=MASK(LED_GREEN);
	PTD->PSOR|=MASK(LED_BLUE);
	PTB->PSOR|=MASK(LED_RED);
}

void FAST_FORWARD_CLOCLWISE_ROTATION(void){
	if(getValue1 && getValue2 && getValue3){
	static enum green_states{S1, S2, S3, S4, S5, S6, S7} next_state = S1;
			unsigned int current_time;
			static unsigned long last_run=0u;
			unsigned long interval=FAST_SPEED;

			current_time= millis();	//current time
			switch (next_state){
			case S1: //first ON
				AB_ON ();
				if((current_time-last_run) >= interval){
					last_run=current_time;
				AB_OFF ();
					next_state= S2;
				}
				break;
			case S2:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					BG_ON ();
					next_state= S3;
				}
				break;
			case S3:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					BG_OFF ();
					next_state= S4;
				}
				break;
			case S4: //off
				if((current_time-last_run) >= interval){
					last_run=current_time;
					GF_ON ();
					next_state= S5;
				}
				break;
			case S5: //off
				if((current_time-last_run) >= interval){
				last_run=current_time;
				GF_OFF ();
				next_state= S6;
				}
				 break;
			case S6: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			FA_ON ();
			next_state= S7;
				}
			 break;
			case S7: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			FA_OFF ();
			next_state= S1;
				}
			break;

			default:
				if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= S1;
				}
				break;

	}
}
}
void FAST_REVERSE_CLOCLWISE_ROTATION(void){
	if (getValue1 && !getValue2 && getValue3){
	static enum green_states{S1, S2, S3, S4, S5, S6, S7} next_state = S1;
			unsigned int current_time;
			static unsigned long last_run=0u;
			unsigned long interval=FAST_SPEED;

			current_time= millis();	//current time
			switch (next_state){
			case S1: //first ON
				AB_ON ();
				if((current_time-last_run) >= interval){
					last_run=current_time;
				AB_OFF ();
					next_state= S2;
				}
				break;
			case S2:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					FA_ON ();
					next_state= S3;
				}
				break;
			case S3:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					FA_OFF ();
					next_state= S4;
				}
				break;
			case S4: //off
				if((current_time-last_run) >= interval){
					last_run=current_time;
					GF_ON ();
					next_state= S5;
				}
				break;
			case S5: //off
				if((current_time-last_run) >= interval){
				last_run=current_time;
				GF_OFF ();
				next_state= S6;
				}
				 break;
			case S6: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			BG_ON ();
			next_state= S7;
				}
			 break;
			case S7: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			BG_OFF ();
			next_state= S1;
				}
			break;

			default:
				if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= S1;
				}
				break;

			}
	}
}

void NORMAL_FORWARD_CLOCLWISE_ROTATION(void){
	if (getValue1 && getValue2 && !getValue3){
	static enum green_states{S1, S2, S3, S4, S5, S6, S7} next_state = S1;
			unsigned int current_time;
			static unsigned long last_run=0u;
			unsigned long interval=NORMAL_SPEED;

			current_time= millis();	//current time
			switch (next_state){
			case S1: //first ON
				AB_ON ();
				if((current_time-last_run) >= interval){
					last_run=current_time;
				AB_OFF ();
					next_state= S2;
				}
				break;
			case S2:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					BG_ON ();
					next_state= S3;
				}
				break;
			case S3:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					BG_OFF ();
					next_state= S4;
				}
				break;
			case S4: //off
				if((current_time-last_run) >= interval){
					last_run=current_time;
					GF_ON ();
					next_state= S5;
				}
				break;
			case S5: //off
				if((current_time-last_run) >= interval){
				last_run=current_time;
				GF_OFF ();
				next_state= S6;
				}
				 break;
			case S6: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			FA_ON ();
			next_state= S7;
				}
			 break;
			case S7: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			FA_OFF ();
			next_state= S1;
				}
			break;

			default:
				if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= S1;
				}
				break;

			}
	}
}

void NORMAL_REVERSE_CLOCLWISE_ROTATION(void){
	if(getValue1 && !getValue2 && !getValue3){
	static enum green_states{S1, S2, S3, S4, S5, S6, S7} next_state = S1;
			unsigned int current_time;
			static unsigned long last_run=0u;
			unsigned long interval=NORMAL_SPEED;

			current_time= millis();	//current time
			switch (next_state){
			case S1: //first ON
				AB_ON ();
				if((current_time-last_run) >= interval){
					last_run=current_time;
				AB_OFF ();
					next_state= S2;
				}
				break;
			case S2:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					FA_ON ();
					next_state= S3;
				}
				break;
			case S3:
				if((current_time-last_run) >= interval){
					last_run=current_time;
					FA_OFF ();
					next_state= S4;
				}
				break;
			case S4: //off
				if((current_time-last_run) >= interval){
					last_run=current_time;
					GF_ON ();
					next_state= S5;
				}
				break;
			case S5: //off
				if((current_time-last_run) >= interval){
				last_run=current_time;
				GF_OFF ();
				next_state= S6;
				}
				 break;
			case S6: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			BG_ON ();
			next_state= S7;
				}
			 break;
			case S7: //off
			if((current_time-last_run) >= interval){
			last_run=current_time;
			BG_OFF ();
			next_state= S1;
				}
			break;

			default:
				if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= S1;
				}
				break;

			}
	}

}

void AB_ON (){
	PTB->PSOR =MASK(A);
	PTB->PSOR =MASK(B);
}
void AB_OFF (){
	PTB->PCOR =MASK(A);
	PTB->PCOR =MASK(B);
}
void BG_ON (){
	PTB->PSOR =MASK(B);
	PTD->PSOR =MASK(G);
}
void BG_OFF (){
	PTB->PCOR =MASK(B);
	PTD->PCOR =MASK(G);
}
void GF_ON (){
	PTD->PSOR =MASK(G);
	PTB->PSOR =MASK(F);
}
void GF_OFF(){
	PTD->PCOR =MASK(G);
	PTB->PCOR =MASK(F);
}
void FA_ON (){
	PTB->PSOR =MASK(F);
	PTB->PSOR =MASK(A);
}
void FA_OFF (){
	PTB->PCOR =MASK(F);
	PTB->PCOR =MASK(A);
}


void STOP (void){
if(!getValue1){
	enum red_yellow_states{ST_RED_ON,ST_RED_ON_DELAY, ST_RED_OFF, ST_RED_OFF_DELLAY, ST_YELLOW_ON,ST_YELLOW_ON_DELAY,
		ST_YELLOW_OFF,ST_YELLOW_OFF_DELLAY};
		static enum red_yellow_states next_state = ST_RED_ON;
		unsigned long current_time=0u;
		static unsigned long last_run=0u;
		unsigned long interval=RED_YELLOW_INTERVAL; //1ms
		switch (next_state){
		case ST_RED_ON:
			PTD->PSOR |=MASK(RED_EXT);	//turn on RED
			next_state= ST_RED_ON_DELAY;
			break;

		case ST_RED_ON_DELAY :
			current_time= millis();	//current time
			if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= ST_RED_OFF;
			}
			break;

	    case ST_RED_OFF:
			PTD->PCOR |=MASK(RED_EXT);
			next_state= ST_RED_OFF_DELLAY;
			break;

	    case ST_RED_OFF_DELLAY:
	    	current_time= millis();	//current time
	    		if((current_time-last_run) >= interval){
	    		last_run=current_time;
	    		next_state=ST_YELLOW_ON ;
	    	}
	    	break;

	    case ST_YELLOW_ON:
			PTD->PSOR |=MASK(YELLOW_EXT);	//turn on BLUE
			next_state= ST_YELLOW_ON_DELAY;
			 break;

		case ST_YELLOW_ON_DELAY:
			current_time= millis();	//current time
			if((current_time-last_run) >= interval){
				last_run=current_time;
				next_state= ST_YELLOW_OFF;
			}
			break;

		case ST_YELLOW_OFF:
		    PTD->PCOR |=MASK(YELLOW_EXT);	//turn off BLUE
			next_state= ST_YELLOW_OFF_DELLAY;
			break;
		case ST_YELLOW_OFF_DELLAY:
			current_time= millis();	//current time
			if((current_time-last_run) >= interval){
			 last_run=current_time;
			 next_state= ST_RED_ON;
			}
			break;

		default:
			next_state= ST_RED_ON;
			break;
		}
}
}



void FORWARD1 (void){ // FLASHED 2X PATERN
	if (getValue1 && getValue2 && !getValue3){
		static enum green_states{S1, S2, S3, S4} next_state = S1;
		unsigned int current_time;
		static unsigned long last_run=0u;

		current_time= millis();	//current time
		switch (next_state){
		case S1: //first ON
			White_ON();
			//PTD->PSOR |= MASK(Buzzer);  //turn on, move to default to run only once
			if((current_time-last_run) >= WHITE_ON_1){
				last_run=current_time;
				White_OFF();
				next_state= S2;
			}
			break;
		case S2:
			if((current_time-last_run) >= WHITE_OFF_1){
				last_run=current_time;
				White_ON();
				next_state= S3;
			}
			break;
		case S3:
			if((current_time-last_run) >= WHITE_ON_2){
				last_run=current_time;
				White_OFF();
				next_state= S4;
			}
			break;
		case S4: //off
			if((current_time-last_run) >= WHITE_OFF_2){
				last_run=current_time;
				next_state= S1;
			}
			break;
		default:
			White_OFF();
			break;

		}

 }

}

void FAST_FORWARD(){
	 if(getValue1 && getValue2 && getValue3){

	 enum Forward_states{S1, S2, S3,S4,S5,S6};
	 		static enum Forward_states next_state = S1;
	 		unsigned long current_time=0u;
	 		static unsigned long last_run=0u;
	 		unsigned long interval=WHITE_INTERVAL;
	 		current_time= millis();

	 		switch (next_state){
	 		case S1:
	 			White_ON();
	 			if((current_time-last_run) >= WHITE_ON_1){
	 			last_run=current_time;
	 			White_OFF();
	 			next_state= S2;
	 			}
	 			break;

	 		case S2 :
	 			if((current_time-last_run) >= interval){
	 				last_run=current_time;
	 				White_ON();
	 				next_state= S3;
	 			}
	 			break;

	 	    case S3:
	 	    	if((current_time-last_run) >= WHITE_ON_1*5){
	 	    		last_run=current_time;
	 	    		White_OFF();
	 	    		 next_state= S4;
	 			}
	 			break;
	 	    case S4:
	 	    	if((current_time-last_run) >= interval*3){
	 	    		last_run=current_time;
	 	    		White_ON();
	 	    		next_state= S5;
	 	    	}
	 	    	break;

	 	    case S5:
	 	    	if((current_time-last_run) >= WHITE_ON_1 ){
	 	    		 last_run=current_time;
	 	    		  White_OFF();
	 	    		 next_state= S6;
	 	    		 }
	 	    	break;

	 	   case S6:
	 	   	if((current_time-last_run) >= interval*6){
	 	   	 	  last_run=current_time;
	 	   	 	    White_ON();
	 	   	 	 next_state= S1;
	 	   	 	  	 }
	 	   	 	break;
	 		default:
	 	     	next_state= S1;
	 			break;
	 		}
	 }
 }

 void REVERSE2 (){ // 2x patern
if ((getValue1 && !getValue2 && !getValue3)){
	 static enum green_states{S1, S2, S3, S4} next_state = S1;
	 		unsigned int current_time;
	 		static unsigned long last_run=0u;

	 		current_time= millis();	//current time
	 		switch (next_state){
	 		case S1: //first ON
	 			PTD->PCOR|=MASK(LED_BLUE);
	 			if((current_time-last_run) >= BLUE_ON_1){
	 				last_run=current_time;
	 				PTD->PSOR|=MASK(LED_BLUE);
	 				next_state= S2;
	 			}
	 			break;
	 		case S2:
	 			if((current_time-last_run) >= BLUE_OFF_1){
	 				last_run=current_time;
	 				PTD->PCOR|=MASK(LED_BLUE);
	 				next_state= S3;
	 			}
	 			break;
	 		case S3:
	 			if((current_time-last_run) >= BLUE_ON_2){
	 				last_run=current_time;
	 				PTD->PSOR|=MASK(LED_BLUE);
	 				next_state= S4;
	 			}
	 			break;
	 		case S4: //off
	 			if((current_time-last_run) >= BLUE_OFF_2){
	 				last_run=current_time;
	 				next_state= S1;
	 			}
	 			break;
	 		default:
	 			next_state= S1;
	 			break;

	 		}
 }
 }


void FAST_REVERSE1(){
	if (getValue1 && !getValue2 && getValue3){
	 enum Forward_states{S1, S2, S3,S4,S5,S6};
		 		static enum Forward_states next_state = S1;
		 		unsigned long current_time=0u;
		 		static unsigned long last_run=0u;
		 		unsigned long interval=BLUE_INTERVAL;
		 		current_time= millis();

		 		switch (next_state){
		 		case S1:
		 			PTD->PCOR|=MASK(LED_BLUE);
		 			if((current_time-last_run) >= BLUE_ON_1){
		 			last_run=current_time;
		 			PTD->PSOR|=MASK(LED_BLUE);
		 			next_state= S2;
		 			}
		 			break;

		 		case S2 :
		 			if((current_time-last_run) >= interval){
		 				last_run=current_time;
		 				PTD->PCOR|=MASK(LED_BLUE);
		 				next_state= S3;
		 			}
		 			break;

		 	    case S3:
		 	    	if((current_time-last_run) >= BLUE_ON_1*5 ){
		 	    		last_run=current_time;
		 	    		PTD->PSOR|=MASK(LED_BLUE);
		 	    		 next_state= S4;
		 			}
		 			break;
		 	    case S4:
		 	    	if((current_time-last_run) >= interval*3){
		 	    		last_run=current_time;
		 	    		PTD->PCOR|=MASK(LED_BLUE);
		 	    		next_state= S5;
		 	    	}
		 	    	break;

		 	    case S5:
		 	    	if((current_time-last_run) >= BLUE_ON_1){
		 	    		 last_run=current_time;
		 	    		PTD->PSOR|=MASK(LED_BLUE);
		 	    		 next_state= S6;
		 	    		 }
		 	    	break;
		 	   case S6:
		 	     	if((current_time-last_run) >= interval*6){
		 	   		 last_run=current_time;
		 	   		 	 PTD->PCOR|=MASK(LED_BLUE);
		 	   		  next_state= S1;
		 	   		 		 }
		 	   			break;
		 		default:
		 	     	next_state= S1;
		 			break;
		 		}
 }
}

void RED_CONSTANT_RATE(void){
 if((getValue1 && getValue2 && !getValue3)){
	static enum green_states{S1, S2} next_state = S1;
			 		unsigned int current_time;
			 		static unsigned long last_run=0u;

			 		current_time= millis();	//current time
			 		switch (next_state){
			 		case S1: //first ON
			 			PTD->PSOR =MASK(RED_EXT);
			 			if((current_time-last_run) >= RED_ON){
			 				last_run=current_time;
			 				PTD->PCOR =MASK(RED_EXT);
			 				next_state= S2;
			 			}
			 			break;
			 		case S2:
			 			if((current_time-last_run) >= RED_OFF){
			 				last_run=current_time;
			 			next_state= S1;
			 				}
			 			break;

			 		default:
			 			next_state= S1;
			 				break;

}
}
}

void RED_FLASH(void){
if ((getValue1 && !getValue2 && getValue3)){
	static enum green_states{S1, S2} next_state = S1;
			 		unsigned int current_time;
			 		static unsigned long last_run=0u;

			 		current_time= millis();	//current time
			 		switch (next_state){
			 		case S1: //first ON
			 			PTD->PSOR =MASK(RED_EXT);
			 			if((current_time-last_run) >= RED_ON_1){
			 				last_run=current_time;
			 				PTD->PCOR =MASK(RED_EXT);
			 				next_state= S2;
			 			}
			 			break;
			 		case S2:
			 			if((current_time-last_run) >= RED_OFF_1){
			 				last_run=current_time;
			 			next_state= S1;
			 				}
			 			break;

			 		default:
			 			next_state= S1;

			 		break;

			 		}

}
}

void YELLOW_FLASH(){

	if(getValue1 && getValue2 && !getValue3){
	 static enum green_states{S1, S2} next_state = S1;
			 		unsigned int current_time;
			 		static unsigned long last_run=0u;
			 		current_time= millis();	//current time
			 		switch (next_state){
			 		case S1:
			 			PTD->PSOR |=MASK(YELLOW_EXT);
			 			if((current_time-last_run) >= YELLOW_ON){
			 				last_run=current_time;
			 				PTD->PCOR |=MASK(YELLOW_EXT);
			 				next_state= S2;
			 			}
			 			break;
			 		case S2: //off
			 			if((current_time-last_run) >= YELLOW_OFF){
			 				last_run=current_time;
			 				next_state= S1;
			 			}
			 			break;
			 		default:
			 			next_state= S1;
			 			break;
			 		}
	}
}
void YELLOW_CONSTANT_RATE(){

if(getValue1 && !getValue2 && getValue3){
	 static enum green_states{S1, S2} next_state = S1;
			 		unsigned int current_time;
			 		static unsigned long last_run=0u;
			 		current_time= millis();	//current time
			 		switch (next_state){
			 		case S1:
			 			PTD->PSOR |=MASK(YELLOW_EXT);
			 			if((current_time-last_run) >= YELLOW_ON_1){
			 				last_run=current_time;
			 				PTD->PCOR |=MASK(YELLOW_EXT);
			 				next_state= S2;
			 			}
			 			break;
			 		case S2: //off
			 			if((current_time-last_run) >= YELLOW_OFF_1){
			 				last_run=current_time;
			 				next_state= S1;
			 			}
			 			break;
			 		default:
			 			next_state= S1;
			 			break;

			 		}
}

}



void SysTick_Handler(void){
	counter++;
}
unsigned long  millis(void){
	return (unsigned long)counter;
}

