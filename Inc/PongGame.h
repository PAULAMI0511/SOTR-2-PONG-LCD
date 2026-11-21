#ifndef _PONG_GAME_H_
#define _PONG_GAME_H_

/***********************************************************
 *  Logica del juego PONG (independiente del RTOS y del
 *  hardware): estado del juego, movimiento de la pelota,
 *  choques, marcador y dibujo en el framebuffer.
 ***********************************************************/

#include "AppTypes.h"

/* --------- Pantalla usada --------- */
/* 1 = LCD 16x2 (HD44780), 0 = OLED SSD1306 128x64 */
#define PONG_DISPLAY_LCD 1

/* --------- Configuracion del juego --------- */
#define PONG_WIN_SCORE 5    /* puntos para ganar               */
#define PONG_SERVE_STEPS 40 /* pausa antes de sacar (pasos)    */

#if PONG_DISPLAY_LCD
/* LCD 16x2: cada caracter es de 5x8 pixeles -> cancha de 80x16 pixeles */
#define PONG_FIELD_WIDTH 80
#define PONG_FIELD_TOP 0
#define PONG_FIELD_BOTTOM 15
#define PONG_PADDLE_HEIGHT 5
#define PONG_PADDLE_WIDTH 1
#define PONG_PADDLE1_X 0
#define PONG_PADDLE2_X 79
#define PONG_BALL_SIZE 2
#define PONG_SPEED_X_START 10 /* 0.6 px por paso   */
#define PONG_SPEED_X_MAX 24   /* 1.5 px por paso   */
#define PONG_SPEED_X_STEP 1   /* aumento por golpe */
#define PONG_SPEED_Y_MAX 14
#define PONG_SPIN_FACTOR 4 /* angulo segun donde golpea la paleta */
#else
/* OLED 128x64 */
#define PONG_FIELD_WIDTH 128
#define PONG_FIELD_TOP 10 /* debajo del marcador */
#define PONG_FIELD_BOTTOM 63
#define PONG_PADDLE_HEIGHT 12
#define PONG_PADDLE_WIDTH 2
#define PONG_PADDLE1_X 2
#define PONG_PADDLE2_X 124
#define PONG_BALL_SIZE 3
#define PONG_SPEED_X_START 24 /* 1.5 px por paso   */
#define PONG_SPEED_X_MAX 56   /* 3.5 px por paso   */
#define PONG_SPEED_X_STEP 3
#define PONG_SPEED_Y_MAX 40
#define PONG_SPIN_FACTOR 6
#endif

/* Las posiciones de la pelota se guardan en "punto fijo": valor * 16.
 * Asi se pueden manejar velocidades como 1.5 pixeles por paso sin usar
 * numeros decimales (float). */
#define PONG_FP 16

typedef enum {
  PONG_STATE_MENU = 0,
  PONG_STATE_PLAYING,
  PONG_STATE_GAME_OVER
} PongState_t;

typedef enum {
  PONG_SOUND_NONE = 0,
  PONG_SOUND_WALL,   /* rebote en la pared   */
  PONG_SOUND_PADDLE, /* rebote en una paleta */
  PONG_SOUND_POINT,  /* alguien anoto        */
  PONG_SOUND_START,  /* inicia el juego      */
  PONG_SOUND_WIN     /* fin del juego        */
} PongSound_t;

typedef struct {
  PongState_t State;
  s16 Paddle1Y; /* borde superior de cada paleta (pixeles) */
  s16 Paddle2Y;
  s32 BallX; /* posicion de la pelota (punto fijo) */
  s32 BallY;
  s16 VelX; /* velocidad (punto fijo por paso) */
  s16 VelY;
  u8 Score1;
  u8 Score2;
  u8 Winner;     /* 1 o 2 cuando termina el juego          */
  u16 ServeWait; /* pasos que faltan para sacar la pelota  */
  u32 Seed;      /* semilla para el numero pseudoaleatorio */
} PongGame_t;

void PongGame_Init(PongGame_t *Game);
void PongGame_SetPaddles(PongGame_t *Game, u16 Adc1, u16 Adc2);
PongSound_t PongGame_StartPressed(PongGame_t *Game);
PongSound_t PongGame_Step(PongGame_t *Game);
void PongGame_Draw(const PongGame_t *Game);

#endif // _PONG_GAME_H_