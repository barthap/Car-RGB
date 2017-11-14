/*
 * Rgb.h
 *
 * Created: 07.11.2015 12:15:08
 *  Author: Barthap
 */ 


#ifndef RGB_H_
#define RGB_H_

struct Rgb
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	
	Rgb() : r(0), g(0), b(0) {}
	Rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};
struct Hsv
{
	uint8_t h;
	uint8_t s;
	uint8_t v;
	
	Hsv() : h(0), s(0), v(0) {}
	Hsv(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}
};

class RgbController
{
public:
	static Hsv RgbToHsv(Rgb rgb)
	{
		Hsv hsv;
		unsigned char rgbMin, rgbMax;

		rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
		rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

		hsv.v = rgbMax;
		if (hsv.v == 0)
		{
			hsv.h = 0;
			hsv.s = 0;
			return hsv;
		}

		hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
		if (hsv.s == 0)
		{
			hsv.h = 0;
			return hsv;
		}

		if (rgbMax == rgb.r)
		hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
		else if (rgbMax == rgb.g)
		hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
		else
		hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

		return hsv;
	}

	inline void Init(PORT_t& port, TC4_t& timer)
	{
		on=true;

		tc = &timer;
		currentHsv = Hsv(0,255,255);
		savedRgb = Rgb(255,0,0);
		
		port.DIRSET=0x07; //pin 0-2
		
		timer.CTRLB = TC45_WGMODE_SINGLESLOPE_gc;
		timer.CTRLE = TC45_CCAMODE_COMP_gc | TC45_CCBMODE_COMP_gc | TC45_CCCMODE_COMP_gc;
		timer.PER=256;
		timer.CTRLA = TC45_CLKSEL_DIV64_gc;
	}
	inline void _set(const Rgb& rgb)
	{
		if(!tc)return;
		tc->CCABUF=rgb.r;
		tc->CCBBUF=rgb.g;
		tc->CCCBUF=rgb.b;
	}
	inline void _set(uint8_t r, uint8_t g, uint8_t b)
	{
		if(!tc)return;
		tc->CCABUF=r;
		tc->CCBBUF=g;
		tc->CCCBUF=b;
	}
	
	inline void SetRgb(const Rgb& rgb)
	{
		_set(rgb);
		
		savedRgb=rgb;
	}
	inline void SetRgb(uint8_t r, uint8_t g, uint8_t b)
	{
		_set(r,g,b);
		
		savedRgb = Rgb(r,g,b);
		
	}
	inline Rgb GetRgb()
	{
		return Rgb(tc->CCA, tc->CCB, tc->CCC);
	}
	
	inline void SetHsv(const Hsv& hsv)
	{
		currentHsv = hsv;
		
		Rgb rgb;
		unsigned char region, p, q, t;
		unsigned int h, s, v, remainder;

		if (hsv.s == 0)
		{
			rgb.r = hsv.v;
			rgb.g = hsv.v;
			rgb.b = hsv.v;
			SetRgb(rgb);
			return;
		}

		// converting to 16 bit to prevent overflow
		h = hsv.h;
		s = hsv.s;
		v = hsv.v;

		region = h / 43;
		remainder = (h - (region * 43)) * 6;

		p = (v * (255 - s)) >> 8;
		q = (v * (255 - ((s * remainder) >> 8))) >> 8;
		t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

		switch (region)
		{
			case 0:
			rgb.r = v;
			rgb.g = t;
			rgb.b = p;
			break;
			case 1:
			rgb.r = q;
			rgb.g = v;
			rgb.b = p;
			break;
			case 2:
			rgb.r = p;
			rgb.g = v;
			rgb.b = t;
			break;
			case 3:
			rgb.r = p;
			rgb.g = q;
			rgb.b = v;
			break;
			case 4:
			rgb.r = t;
			rgb.g = p;
			rgb.b = v;
			break;
			default:
			rgb.r = v;
			rgb.g = p;
			rgb.b = q;
			break;
		}
		SetRgb(rgb);
	}
	
	inline void On() {on=true;}
	inline void Off() {on=false;}
	
	inline void UpdateFades()
	{
		if(on)
		{
			if(tc->CCA<savedRgb.r)tc->CCABUF=tc->CCA+1;
			if(tc->CCB<savedRgb.g)tc->CCBBUF=tc->CCB+1;
			if(tc->CCC<savedRgb.b)tc->CCCBUF=tc->CCC+1;
		}
		else
		{
			if(tc->CCA>0)tc->CCABUF=tc->CCA-1;
			if(tc->CCB>0)tc->CCBBUF=tc->CCB-1;
			if(tc->CCC>0)tc->CCCBUF=tc->CCC-1;
		}
	}
	

	Hsv currentHsv;
	Rgb savedRgb;
	
	bool on;
	
private:
	TC4_t* tc;

};



#endif /* RGB_H_ */