#ifndef _PONG_H_
#define _PONG_H_

/***********************************************************
 *  Aplicacion PONG sobre el RTOS (segundo entregable).
 *
 *  Tareas:
 *    TaskSound   (prioridad 0): espera el semaforo y hace sonar el buzzer
 *    TaskInput   (prioridad 1): lee potenciometros y boton cada 20 ms
 *    TaskGame    (prioridad 2): mueve la pelota cada 25 ms
 *    TaskDisplay (prioridad 3): dibuja el juego en la pantalla (LCD u OLED)
 *
 *  Sincronizacion / comunicacion entre tareas:
 *    - Mutex:    protege el estado del juego compartido (GameMutex)
 *    - Semaforo: avisa a la tarea de sonido que hay un evento (SoundSem)
 *
 *  Conexiones:
 *    LCD 16x2 (por defecto): RS -> PB10, E -> PB11, D4..D7 -> PB12..PB15,
 *      RW -> GND, VDD -> 5V (conexiones completas en Lcd1602.h)
 *    OLED SSD1306 (opcional): SCL -> PB6, SDA -> PB7, VCC -> 3.3V, GND -> GND
 *    Potenciometro J1 (pata central) -> PA0
 *    Potenciometro J2 (pata central) -> PA1
 *    Boton inicio/fin -> PB0 (el otro lado a GND)
 *    Buzzer activo (+) -> PA8, (-) -> GND
 ***********************************************************/

#include "AppTypes.h"

void Pong_Init(void);

#endif // _PONG_H_