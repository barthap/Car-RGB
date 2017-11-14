/*
 * Eeprom.h
 *
 * Created: 22.11.2015 14:23:02
 *  Author: Barthap
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

	EEMEM uint8_t eeR;
	EEMEM uint8_t eeG;
	EEMEM uint8_t eeB;
	EEMEM uint8_t eeH;
	EEMEM uint8_t eeS;
	EEMEM uint8_t eeV;

	inline void SaveEEProm(const RgbController &rgb)
	{
		//zapisuje aktualny kolor
		eeprom_write_byte(&eeR, rgb.savedRgb.r);
		eeprom_write_byte(&eeG, rgb.savedRgb.g);
		eeprom_write_byte(&eeB, rgb.savedRgb.b);
		
		eeprom_write_byte(&eeH, rgb.currentHsv.h);
		eeprom_write_byte(&eeS, rgb.currentHsv.s);
		eeprom_write_byte(&eeV, rgb.currentHsv.v);

	}
	inline void LoadEEProm(RgbController &rgb)
	{
		rgb.savedRgb.r = eeprom_read_byte(&eeR);
		rgb.savedRgb.g = eeprom_read_byte(&eeG);
		rgb.savedRgb.b = eeprom_read_byte(&eeB);
		
		rgb.currentHsv.h = eeprom_read_byte(&eeB);
		rgb.currentHsv.s = eeprom_read_byte(&eeS);
		rgb.currentHsv.v = eeprom_read_byte(&eeV);
		//wczytuje kolor z eeprom i od razu wyœwietla
	}
	



#endif /* EEPROM_H_ */