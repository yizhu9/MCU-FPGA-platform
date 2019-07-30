/*
 * FILE								: iwdg.h
 * DESCRIPTION				: this file is for iwdg.c
 * Author							: 
 * Rev								: 0.00
 * Date								: 11/21/2015
 */
 
#ifndef __iwdg_h__
#define __iwdg_h__


//-----------------define-------------------------------//

//-----------------Include files-------------------------//

//----------------- Typedef------------------------------//
typedef struct {
	int (* initialize)(void);
	void (* feed_dog)(void);
}IWDG_T;


//----------------- Extern ------------------------------//
extern IWDG_T iwdg;


#endif //__iwdg_h__
