/* 
* Keyboard.cpp
*
* Created: 07.11.2015 13:27:31
* Author: Barthap
*/

#include <avr/io.h>
#include "Keyboard.h"

#define PERIOD_1S      100
#define PERIOD_750MS   75
#define PERIOD_500MS   50
#define PERIOD_100MS   10
#define PERIOD_30MS    3

// ***********************************************************************
static volatile unsigned int keys = 0;
// Wewn�trzna zmienna w kt�rej przechowywany jest stan klawiszy
// Zawarto�� tej zmiennej pobierana jest przez program za pomoca funkcji
//  GetKeys(); je�li zmienna keycnt jest wi�ksza od zera.
// ***********************************************************************

// ***********************************************************************
static volatile unsigned int keycnt = 0;
// Wewn�trzna zmienna w kt�rej przechowywana jest licznik autorepetycji
// Zwi�kszany okresowo w momencie gdy s� naci�ni�te klawisze i ich stan
// nie zmienia si� przez d�u�szy czas. Zmniejszana podczas ka�dego
// wywo�ania funkcji GetKeys();
// ***********************************************************************

// ***********************************************************************
static volatile unsigned int keytime = 0;
// Wewn�trzna zmienna reprezentuj�ca czas od ostatniego naci�ni�cia
// klawiszy. Zwracana do programu za pomoc� funkcji KeysTime();
// Je�li nie ma wci�ni�tego �adnego klawisza, zmienna utrzymuje warto�� 0;
// ***********************************************************************

// ***********************************************************************
static volatile unsigned int prevKeybSt = 0;
// Wewn�trzna zmienna pami�taj�ca stan klawiszy z poprzedniego wywo�ania
// funkcji KeybProc(). S�u�y do wykrycia zmiany stanu klawiatury
// ***********************************************************************

// ***********************************************************************
static volatile unsigned int arTime = 0;
// Wewn�trzna zmienna reprezentuj�ca czas w kt�rym ma nast�pi� zwiekszenie
// licznika klawiatury. Zwi�kszana o odpowiedni czas zale�ny o czas�w
// autorepetycji po ka�dym zwiekszeniu licznika klawiatury
// ***********************************************************************

// ***********************************************************************
static volatile unsigned char arIndex = 0;
// Wewn�trzna zmienna indeksuj�ca tablic� z kolejnymi czasami autorepetycji
// ***********************************************************************

// ***********************************************************************
static volatile unsigned char keyblock = 0;
// Wewn�trzna zmienna ustawiana na 1 funkcj� KeybLock() lub ClrKeyb()
// z parametrem KBD_LOCK. Je�li zmienna ma warto�� 1 obs�uga klawiatury
// zostaje zablokowana do momentu zwolnienia wszystkich klawiszy.
// ***********************************************************************

// ***********************************************************************
// Domy�lna tablica z kolejnymi czasami autorepetycji. Ostatnia pozycja r�na
// od zera jest czasem autorepetycji niesko�czonej. Ilo�� pozycji dowolna,
// ostatnia pozycja musi by� r�wna 0;
static const unsigned short DefaultAutoRepeatTab[] =
{
	500,
	0
};

static unsigned short
* volatile _art = (unsigned short *)DefaultAutoRepeatTab,
* volatile art = (unsigned short *)DefaultAutoRepeatTab;

// ***********************************************************************
// Funkcja dostarczaj�ca surowy stan klawiatury.
// Stan wysoki dla wcisni�tych klawiszy. Je�li funkcja KeybProc()
// b�dzie wywo�ywana z przerwania to funkcja GetKeybSample() musi
// trwa� jak najkr�cej
// ***********************************************************************
unsigned int
GetKeybSample( void )
{
	return (( ~PORTD.IN ) & ( KEY ));
}


// ***********************************************************************
// Funkcja wywo�ywana z jakiegos przerwania w tym przypadku co 10ms
// ***********************************************************************
void
KeybProc( void )
{
	unsigned int realKeybSt;
	
	// Pobranie stanu klawiszy
	realKeybSt = GetKeybSample();
	
	// Sprawdzenie czy stan sie zmieni�
	if( prevKeybSt != realKeybSt )
	{
		// Stan sie zmieni� wi�c resetowanie klawiatury i wyj�cie z funkcji
		ClrKeyb( KBD_NOLOCK );
		return;
	}
	
	// Sprawdzenie czy brak naci�ni�tych klawiszy lub klawiatura zablokowana
	if( !realKeybSt || keyblock )
	{
		// Ponowne sprawdzenie czy brak nacisni�tych klawiszy
		// Je�li tak to odblokowanie klawiatury
		if( !realKeybSt ) keyblock = 0;
		return;
	}
	// Zwiekszenie licznika czasu klawiatury
	keytime++;
	// Zachowanie stanu klawiszy
	keys = realKeybSt;
	
	// Obs�uga autorepetycji
	// Sprawdzenie czy licznik czsu klawiatury osi�gn�� czas nast�pnej
	// autorepetycji
	if( keytime >= arTime )
	{
		// Zwi�kszenie licznika autorepetycji
		keycnt++;
		
		// Obliczenie kolejnego czasu autorepetycji
		_art = art;
		if( _art[ arIndex + 1 ]) arIndex++;
		arTime += _art[ arIndex ];
	}
}



// ***********************************************************************
// Funkcja zwraca stan klawiszy do programu je�li licznik autorepetycji
// r�zny od zera
// ***********************************************************************
unsigned int
GetKeys( void )
{
	if( keycnt )
	{
		keycnt--;     // Je�li funkcja KeybProc() b�dzie wywo�ywana z przerwania
		// to ta instrukcja musi by� wykonana atomowo.
		return keys ;
	}
	return 0;
}


// ***********************************************************************
// Funkcja zwraca czas wciskania aktualnej kombinacji klawiszy
// ***********************************************************************
unsigned int
KeysTime( void )
{
	return keytime;
}

// ***********************************************************************
// Funkcja zwraca stan klawiszy zgodnie z podana mask� jako argument funkcji
// ***********************************************************************
unsigned int
IsKeyPressed( unsigned int mask )
{
	return keys & mask;
}

// ***********************************************************************
// Funkcja zwraca stan klawiszy zgodnie z ustawiona mask� podana jako
// argument funkcji, jest brany pod uwage licznik autorepetycji
// Ale pobranie stanu klawiwszy nie zminiejsza licznika autorepetycji
// ***********************************************************************
unsigned int
IsKey( unsigned int mask )
{
	if(keycnt)
	{
		return keys & mask ;
	}
	return 0;
}


// ***********************************************************************
// Funkcja resetuje stan klawiatury. Jako parametr nale�y poda� sta��
// KBD_LOCK lub KBD_NOLOCK, kt�re odpowiednio blokuj� lub nie klawiatur�
// Je�li funkcja KeybProc() b�dzie wywo�ywana z przerwania to funkcja
// ClrKeyb() musi by� wykonana atomowo.
// ***********************************************************************

void
ClrKeyb( int lock )
{
	prevKeybSt = GetKeybSample();
	keys = 0;
	keytime = 0;
	keycnt = 0;
	arIndex = 0;
	arTime = _art[0];
	if( lock ) keyblock = 1;
}


// ***********************************************************************
// Funkcja blokuje klawiatur�. Odblokowanie nast�puje po zwolnieniu
// wszystkich klawiszy.
// ***********************************************************************
void
KeybLock( void )
{
	keyblock = 1;
}


// ***********************************************************************
// Funkcja podmienia tablic� z mi�dzyczasami autorepetycji. Nowa tablica
// powinna by� zgodna z wcze�niej opisanym formatem
// ***********************************************************************
void
KeybSetAutoRepeatTimes( unsigned short * AutoRepeatTab )
{
	if( AutoRepeatTab == KBD_DEFAULT_ART )
	art = (unsigned short *)DefaultAutoRepeatTab;
	else
	art = AutoRepeatTab;
}