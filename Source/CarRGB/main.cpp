/*
 * CarRGB.cpp
 *
 * Created: 07.11.2015 11:53:48
 * Author : Barthap
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "Keyboard.h"
#include "Enkoder.h"
#include "Rgb.h"
#include "Eeprom.h"

enum ProgramStates
{
	STATE_OFF,
	STATE_ON,
	STATE_SET_H,
	STATE_SET_S,
	STATE_SET_V,
};

ProgramStates state = STATE_ON;
RgbController led = RgbController();

bool buttonState = true;


volatile bool timerFlag = false;
ISR(TCC5_OVF_vect)
{
	TCC5.INTFLAGS=TC5_OVFIF_bm;
	
	KeybProc();
	
	if(state == STATE_OFF || state == STATE_ON) timerFlag = true;
		
}


template<typename Timer>
inline void InitMainTimer(Timer& timer)
{
	timer.CTRLB = TC45_WGMODE_NORMAL_gc;
	timer.PER=2500;
	timer.INTCTRLA = TC45_OVFINTLVL_LO_gc;
	timer.CTRLA = TC45_CLKSEL_DIV8_gc;
}

void CheckPressedKey(void(*shortPressProc)(),void(*longPressProc)())
{
	static char _key;
	static uint16_t _key_time;
	
	if( IsKeyPressed(KEY))
	{
		_key = 1;
		_key_time = KeysTime();
	}
	else if ( _key )
	{
		_key = 0;
		if( _key_time < 30 )
		{
			shortPressProc();
		}
	}

	if( IsKeyPressed(KEY) && KeysTime() > 100 )
	{
		longPressProc();
		ClrKeyb(KBD_LOCK);
	}
}


void OnShortPress()
{
	switch(state)
	{
		case STATE_OFF:
		case STATE_ON:
			buttonState^=1;
			break;
		case STATE_SET_H:
			state = STATE_SET_S;
			;
			Enkoder::SetVal(led.currentHsv.s);
			break;
		case STATE_SET_S:
			state = STATE_SET_V;			
			Enkoder::SetVal(led.currentHsv.v);
			break;
		case STATE_SET_V:
			state = STATE_SET_H;			
			Enkoder::SetVal(led.currentHsv.h);
			break;
			
	}
}
inline void DelayBlink()
{
	Rgb tmp = led.GetRgb();
	led.SetRgb(0,0,0);
	_delay_ms(50);
	led.SetRgb(255,255,255);
	_delay_ms(50);
	led.SetRgb(tmp);
	_delay_ms(1);
};

void OnLongPress()
{
	switch(state)
	{
		case STATE_OFF:
			buttonState^=1;
			break;
		case STATE_ON:
			state = STATE_SET_H;
			DelayBlink();
			Enkoder::SetVal(led.currentHsv.h);
			break;
		case STATE_SET_H:
		case STATE_SET_S:
		case STATE_SET_V:
			state = STATE_ON;
			DelayBlink();		
			SaveEEProm(led);
			break;
	}
}

inline void UpdateState()
{
	
	//formula = (czujnikState || (stacyjkaState && buttonState));
	//bool stacyjka = !(PORTD.IN & IGNITION);
	bool sensorState = []()->bool{return !(PORTD.IN & SENSOR);}();
	bool ignitionState = []()->bool{return !(PORTD.IN & IGNITION);}();

	bool stateFormula = (sensorState || (buttonState && ignitionState));
	
	if(stateFormula)state = STATE_ON;
	else state = STATE_OFF;
}

inline bool CheckState() {return (state==STATE_ON || state==STATE_OFF);}

int main(void)
{	
	led.Init(PORTC, TCC4);
	LoadEEProm(led);
	Enkoder::Init();  
	InitMainTimer(TCC5);
	
	PORTD.DIRCLR = KEY | SENSOR | IGNITION;
	PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTD.PIN4CTRL = PORT_OPC_PULLUP_gc;
	PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc;
	
	PMIC.CTRL         =    PMIC_LOLVLEN_bm;
	sei(); 
	
			
	uint8_t lastVal = Enkoder::GetValue();		
    while (1) 
    {
		/*if(stacyjkaState)*/CheckPressedKey(OnShortPress, OnLongPress);	//sprawdza stan przycisku
		
		if(Enkoder::GetValue()!=lastVal)	//sprawdza stan enkodera
		{
			lastVal = Enkoder::GetValue();
			switch(state)
			{
				case STATE_SET_H:
					led.SetHsv(Hsv(lastVal,led.currentHsv.s,led.currentHsv.v));
					break;
				case STATE_SET_S:
					led.SetHsv(Hsv(led.currentHsv.h,lastVal,led.currentHsv.v));
					break;
				case STATE_SET_V:
					led.SetHsv(Hsv(led.currentHsv.h,led.currentHsv.s,lastVal));
					break;
				default:
					break;
			}
		}
		switch(state)	//sprawdza stan ledów
		{
			case STATE_OFF:
				if(led.on)led.Off();
				break;
			case STATE_ON:
				if(!led.on)led.On();
				break;			
			default:
				break;
		}
		
		
		if(timerFlag)
		{
			timerFlag = false;
			
			if(CheckState())
			{
				UpdateState();	//sprawdza stan przyciskow i zmienia stan ON/OFF
				
				led.UpdateFades(); //aktualizuje stan fade
			}
		}
		
	
    }	//while
}

