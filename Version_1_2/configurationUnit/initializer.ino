/*
 * File:   initializer.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "initializer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// inicializacion de los pines del microcontrolador
void init_GPIO(void)
{
    //Encoder setup
    pinMode(A, INPUT_PULLUP);           // A como entrada
    pinMode(B, INPUT_PULLUP);           // B como entrada
    pinMode(SW, INPUT_PULLUP);          // SW como entrada
    pinMode(SILENCE_BTN, INPUT_PULLUP); // switch para el manejo de silencio
    pinMode(STANDBY, INPUT_PULLUP);
    pinMode(STABILITY_BTN, INPUT_PULLUP);
    pinMode(BUZZER_BTN, OUTPUT);
    pinMode(LUMINR, OUTPUT);
    pinMode(LUMING, OUTPUT);
    pinMode(LUMINB, OUTPUT);
    pinMode(SILENCE_LED, OUTPUT);
    pinMode(STANDBY_LED, OUTPUT);
    pinMode(STABILITY_LED, OUTPUT);

    pinMode(BATTALARM, INPUT);

    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
    attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(SILENCE_BTN), silenceButtonInterrupt, FALLING);
    // attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);

    digitalWrite(STANDBY_LED, HIGH);
    digitalWrite(STABILITY_LED, HIGH);
    digitalWrite(LUMING, LOW);
    // pinMode(LED, OUTPUT);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/