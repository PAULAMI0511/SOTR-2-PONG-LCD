/***********************************************************
 *  Lcd1602.c - Driver del LCD 16x2 (HD44780) en modo 4 bits.
 *
 *  Cada byte se envia en dos mitades (nibbles) por D4-D7:
 *  primero los 4 bits altos y luego los 4 bajos. RS indica si
 *  el byte es un comando (RS = 0) o un caracter (RS = 1), y un
 *  pulso en E hace que el LCD lea el dato.
 *
 *  Como RW va a GND, el LCD nunca escribe hacia el micro: por
 *  eso no se lee la bandera de ocupado y se usan retardos.
 ***********************************************************/

#include "Lcd1602.h"
#include "Gpio.h"

/* ---------------- Pines ---------------- */
#define LCD_PORT GPIOB_BASE
#define LCD_RS_PIN 10
#define LCD_E_PIN 11
#define LCD_D4_PIN 12 /* D4..D7 = PB12..PB15 */

/* ---------------- Comandos del HD44780 ---------------- */
#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_ENTRY_MODE 0x06   /* el cursor avanza a la derecha  */
#define LCD_CMD_DISPLAY_ON 0x0C   /* pantalla on, cursor off        */
#define LCD_CMD_FUNCTION_SET 0x28 /* 4 bits, 2 lineas, letra 5x8    */
#define LCD_CMD_SET_CGRAM 0x40    /* memoria de caracteres propios  */
#define LCD_CMD_SET_DDRAM 0x80    /* posicion en la pantalla        */
#define LCD_ROW1_OFFSET 0x40      /* la fila 2 empieza en 0x40      */

/* ---------------- Contador de ciclos (DWT) ----------------
 * El Cortex-M3 tiene un contador que suma 1 en cada ciclo de reloj.
 * Se usa para hacer retardos de microsegundos precisos. */
#define DEMCR (*(volatile u32 *)0xE000EDFC)
#define DWT_CTRL (*(volatile u32 *)0xE0001000)
#define DWT_CYCCNT (*(volatile u32 *)0xE0001004)
#define DEMCR_TRCENA (1U << 24)
#define DWT_CTRL_CYCCNTENA (1U << 0)
#define CPU_MHZ 16U

static void Lcd_DelayUs(u32 Us) {
  u32 Start = DWT_CYCCNT;
  u32 Cycles = Us * CPU_MHZ;
  while ((DWT_CYCCNT - Start) < Cycles) {
  }
}

/* Pone 4 bits en D4..D7 y da un pulso en E */
static void Lcd_WriteNibble(u8 Nibble) {
  for (u8 i = 0; i < 4; i++) {
    Gpio_Write(LCD_PORT, LCD_D4_PIN + i, (Nibble >> i) & 1U);
  }
  Gpio_Write(LCD_PORT, LCD_E_PIN, TRUE);
  Lcd_DelayUs(1); /* el pulso debe durar al menos 450 ns */
  Gpio_Write(LCD_PORT, LCD_E_PIN, FALSE);
  Lcd_DelayUs(1);
}

static void Lcd_WriteByte(u8 Value, u8 IsData) {
  Gpio_Write(LCD_PORT, LCD_RS_PIN, IsData);
  Lcd_WriteNibble(Value >> 4);
  Lcd_WriteNibble(Value & 0x0F);
  Lcd_DelayUs(50); /* la mayoria de instrucciones tardan 37 us */
}

static void Lcd_Command(u8 Command) { Lcd_WriteByte(Command, FALSE); }

/***********************************************************
 *                  FUNCIONES PUBLICAS                      *
 ***********************************************************/

/*!
 * @brief  Secuencia de inicializacion en 4 bits indicada por la hoja de
 *         datos del HD44780.
 */
void Lcd_Init(void) {
  /* Habilitar el contador de ciclos */
  DEMCR |= DEMCR_TRCENA;
  DWT_CYCCNT = 0;
  DWT_CTRL |= DWT_CTRL_CYCCNTENA;

  /* Pines como salida */
  Gpio_EnablePort(LCD_PORT);
  Gpio_Config(LCD_PORT, LCD_RS_PIN, GPIO_MODE_OUTPUT_PP);
  Gpio_Config(LCD_PORT, LCD_E_PIN, GPIO_MODE_OUTPUT_PP);
  for (u8 i = 0; i < 4; i++) {
    Gpio_Config(LCD_PORT, LCD_D4_PIN + i, GPIO_MODE_OUTPUT_PP);
  }
  Gpio_Write(LCD_PORT, LCD_RS_PIN, FALSE);
  Gpio_Write(LCD_PORT, LCD_E_PIN, FALSE);

  Lcd_DelayUs(50000); /* esperar > 40 ms despues de encender */

  /* Forzar el modo de 8 bits tres veces y luego pasar a 4 bits */
  Lcd_WriteNibble(0x3);
  Lcd_DelayUs(4500);
  Lcd_WriteNibble(0x3);
  Lcd_DelayUs(150);
  Lcd_WriteNibble(0x3);
  Lcd_DelayUs(150);
  Lcd_WriteNibble(0x2);
  Lcd_DelayUs(150);

  Lcd_Command(LCD_CMD_FUNCTION_SET);
  Lcd_Command(LCD_CMD_DISPLAY_ON);
  Lcd_Clear();
  Lcd_Command(LCD_CMD_ENTRY_MODE);
}

void Lcd_Clear(void) {
  Lcd_Command(LCD_CMD_CLEAR);
  Lcd_DelayUs(2000); /* borrar tarda 1.52 ms */
}

void Lcd_SetCursor(u8 Row, u8 Column) {
  u8 Address = (u8)(Column + ((Row == 0) ? 0 : LCD_ROW1_OFFSET));
  Lcd_Command(LCD_CMD_SET_DDRAM | Address);
}

void Lcd_WriteChar(u8 Character) { Lcd_WriteByte(Character, TRUE); }

void Lcd_WriteString(const char *Text) {
  while (*Text) {
    Lcd_WriteChar((u8)*Text++);
  }
}

/*!
 * @brief  Disena un caracter propio (Slot 0..7). Rows son las 8 filas del
 *         caracter; en cada fila se usan los 5 bits bajos (bit 4 = izquierda).
 *         Despues se muestra escribiendo el codigo Slot con Lcd_WriteChar.
 */
void Lcd_CreateChar(u8 Slot, const u8 Rows[8]) {
  Lcd_Command((u8)(LCD_CMD_SET_CGRAM | ((Slot & 0x07) << 3)));
  for (u8 i = 0; i < 8; i++) {
    Lcd_WriteChar(Rows[i] & 0x1F);
  }
}
