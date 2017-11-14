/* 
* Keyboard.h
*
* Created: 07.11.2015 13:27:31
* Author: Barthap
*/


#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_PORT   PIND

#define KEY PIN2_bm
#define SENSOR PIN4_bm
#define IGNITION PIN5_bm

#define KBD_LOCK     1
#define KBD_NOLOCK   0

#define KBD_DEFAULT_ART   ((void *)0)

void
ClrKeyb( int lock );

unsigned int
GetKeys( void );

unsigned int
KeysTime( void );

unsigned int
IsKeyPressed( unsigned int mask );

unsigned int
IsKey( unsigned int mask );

void
KeybLock( void );

void
KeybSetAutoRepeatTimes( unsigned short * AutoRepeatTab );

void
KeybProc( void );

#ifdef __cplusplus
}
#endif

#endif //__KEYBOARD_H__
