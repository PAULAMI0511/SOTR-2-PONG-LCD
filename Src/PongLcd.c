/***********************************************************
 *  PongLcd.c - Dibuja el PONG en un LCD de caracteres 16x2.
 *
 *  El LCD no es grafico: solo muestra caracteres de 5x8
 *  pixeles. Pero permite disenar hasta 8 caracteres propios
 *  (CGRAM). Se trata la pantalla como una cancha de 80x16
 *  pixeles (16 x 5 columnas, 2 x 8 filas) y en cada cuadro:
 *    1. Se calcula que pixeles de cada celda estan encendidos
 *       (paletas y pelota).
 *    2. Cada celda que tiene algo se convierte en un caracter
 *       propio (maximo 8: 2 celdas por paleta y hasta 4 para
 *       la pelota).
 *    3. Se escriben las 32 celdas en la pantalla.
 ***********************************************************/

#include "PongLcd.h"
#include "Lcd1602.h"

#define CELL_W 5
#define CELL_H 8
#define EMPTY_CELL 0xFF /* marca de celda sin pixeles */

/* Patron de cada una de las 32 celdas (8 filas x 5 bits) */
static u8 CellRows[LCD_ROWS][LCD_COLUMNS][CELL_H];

static void PongLcd_SetPixel(s16 x, s16 y) {
  if (x < 0 || x >= LCD_COLUMNS * CELL_W || y < 0 || y >= LCD_ROWS * CELL_H) {
    return;
  }
  CellRows[y / CELL_H][x / CELL_W][y % CELL_H] |=
      (u8)(1U << (CELL_W - 1 - (x % CELL_W)));
}

static void PongLcd_FillRect(s16 x, s16 y, s16 w, s16 h) {
  for (s16 i = x; i < x + w; i++) {
    for (s16 j = y; j < y + h; j++) {
      PongLcd_SetPixel(i, j);
    }
  }
}

static u8 PongLcd_CellIsEmpty(u8 r, u8 c) {
  for (u8 k = 0; k < CELL_H; k++) {
    if (CellRows[r][c][k]) {
      return FALSE;
    }
  }
  return TRUE;
}

/* Escribe una fila de 16 caracteres de texto */
static void PongLcd_TextRow(u8 Row, const char *Text) {
  Lcd_SetCursor(Row, 0);
  u8 n = 0;
  while (Text[n] && n < LCD_COLUMNS) {
    Lcd_WriteChar((u8)Text[n++]);
  }
  while (n++ < LCD_COLUMNS) {
    Lcd_WriteChar(' ');
  }
}

static void PongLcd_DrawField(const PongGame_t *Game) {
  /* 1. Calcular los pixeles de cada celda */
  for (u8 r = 0; r < LCD_ROWS; r++) {
    for (u8 c = 0; c < LCD_COLUMNS; c++) {
      for (u8 k = 0; k < CELL_H; k++) {
        CellRows[r][c][k] = 0;
      }
    }
  }
  PongLcd_FillRect(PONG_PADDLE1_X, Game->Paddle1Y, PONG_PADDLE_WIDTH,
                   PONG_PADDLE_HEIGHT);
  PongLcd_FillRect(PONG_PADDLE2_X, Game->Paddle2Y, PONG_PADDLE_WIDTH,
                   PONG_PADDLE_HEIGHT);
  PongLcd_FillRect((s16)(Game->BallX / PONG_FP), (s16)(Game->BallY / PONG_FP),
                   PONG_BALL_SIZE, PONG_BALL_SIZE);

  /* 2. Convertir cada celda con pixeles en un caracter propio */
  u8 Code[LCD_ROWS][LCD_COLUMNS];
  u8 Slot = 0;
  for (u8 r = 0; r < LCD_ROWS; r++) {
    for (u8 c = 0; c < LCD_COLUMNS; c++) {
      Code[r][c] = EMPTY_CELL;
      if (!PongLcd_CellIsEmpty(r, c) && Slot < LCD_CUSTOM_CHARS) {
        Lcd_CreateChar(Slot, CellRows[r][c]);
        Code[r][c] = Slot++;
      }
    }
  }

  /* 3. Escribir las 32 celdas. El marcador va en la fila de arriba, en el
   *    centro, si la pelota no esta pasando por ahi. */
  for (u8 r = 0; r < LCD_ROWS; r++) {
    Lcd_SetCursor(r, 0);
    for (u8 c = 0; c < LCD_COLUMNS; c++) {
      u8 Ch = ' ';
      if (Code[r][c] != EMPTY_CELL) {
        Ch = Code[r][c];
      } else if (r == 0 && c == 6) {
        Ch = (u8)('0' + Game->Score1);
      } else if (r == 0 && c == 9) {
        Ch = (u8)('0' + Game->Score2);
      }
      Lcd_WriteChar(Ch);
    }
  }
}

/*!
 * @brief  Dibuja el menu, la partida o el fin del juego.
 */
void PongLcd_Draw(const PongGame_t *Game) {
  if (Game->State == PONG_STATE_MENU) {
    PongLcd_TextRow(0, "      PONG");
    PongLcd_TextRow(1, "PRESIONE INICIO");
    return;
  }
  if (Game->State == PONG_STATE_GAME_OVER) {
    char Line[17] = "GANA J1   0 - 0";
    Line[6] = (char)('0' + Game->Winner);
    Line[10] = (char)('0' + Game->Score1);
    Line[14] = (char)('0' + Game->Score2);
    PongLcd_TextRow(0, Line);
    PongLcd_TextRow(1, "PRESIONE INICIO");
    return;
  }
  PongLcd_DrawField(Game);
}
