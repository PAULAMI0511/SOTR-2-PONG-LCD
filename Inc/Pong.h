#ifndef _LCD1602_H_
#define _LCD1602_H_

/***********************************************************
 *  Driver de la pantalla LCD 16x2 (controlador HD44780)
 *  en modo de 4 bits.
 *
 *  Conexiones:
 *    1 VSS -> GND            9-10 D2-D3 -> sin conectar
 *    2 VDD -> 5V             11 D4 -> PB12
 *    3 V0  -> pata central   12 D5 -> PB13
 *            de un potenciometro (contraste)
 *    4 RS  -> PB10           13 D6 -> PB14
 *    5 RW  -> GND            14 D7 -> PB15
 *    6 E   -> PB11           15 VLED (A) -> 5V (con resistencia de 220 ohm)
 *    7-8 D0-D1 -> sin conectar   16 VLSS (K) -> GND
 ***********************************************************/

#include "AppTypes.h"

#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_CUSTOM_CHARS 8 /* caracteres que se pueden disenar (CGRAM) */

void Lcd_Init(void);
void Lcd_Clear(void);
void Lcd_SetCursor(u8 Row, u8 Column);
void Lcd_WriteChar(u8 Character);
void Lcd_WriteString(const char *Text);
void Lcd_CreateChar(u8 Slot, const u8 Rows[8]);

#endif // _LCD1602_H_
