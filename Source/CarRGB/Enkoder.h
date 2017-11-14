/*
 * Enkoder.h
 *
 * Created: 07.11.2015 12:03:40
 *  Author: Barthap
 */ 


#ifndef ENKODER_H_
#define ENKODER_H_

//PORTD: Piny 0 i 1
//Event System Channel: CH0
//Timer: Timer D5 (TCD5)

namespace Enkoder
{
	TC5_t&	EncoderTimer = TCD5;
	PORT_t&	EncoderPort = PORTD;
	
	inline void Init();
	inline uint8_t GetValue();
	inline void SetVal(uint8_t val);
}

inline void Enkoder::Init()
{
	 // wej�cia enkodera
	 PORTCFG.MPCMASK    =    0b00000011;                       // wyb�r pin�w 0 i 1 do konfiguracji
	 EncoderPort.PIN0CTRL     =    PORT_ISC_LEVEL_gc |               // reagowanie na poziom niski
	 PORT_OPC_PULLUP_gc;               // podci�gni�cie do zasilania
	 
	 // konfiguracja systemu zdarze�
	 EVSYS.CH0MUX       =    EVSYS_CHMUX_PORTD_PIN0_gc;        // pin C0 wywo�uje zdarzenie
	 EVSYS.CH0CTRL      =    EVSYS_QDEN_bm|                    // w��czenie dekodera w systemie zdarze�
	 EVSYS_DIGFILT_8SAMPLES_gc;        // filtr cyfrowy
	 
	 // konfiguracja timera
	 EncoderTimer.PER			=	 1024;
	 EncoderTimer.CTRLA         =    TC45_CLKSEL_EVCH0_gc;               // taktowanie systemem zdarze�
	 EncoderTimer.CTRLD         =    TC45_EVACT_QDEC_gc |                // w��czenie dekodera kwadraturowego
	 TC45_EVSEL_CH0_gc;                  // dekoder zlicza impulsy z kana�u 0
}

inline uint8_t Enkoder::GetValue()
{
	return static_cast<uint8_t>(EncoderTimer.CNT/4);
}
inline void Enkoder::SetVal(uint8_t val)
{
	EncoderTimer.CNT = val*4;
}



#endif /* ENKODER_H_ */