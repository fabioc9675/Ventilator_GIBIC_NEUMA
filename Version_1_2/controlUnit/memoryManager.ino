/*
 * File:   memoryManager.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "memoryManager.h"
#include <EEPROM.h>

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// Calibracion sensores fabrica
// Calibracion de los sensores de presion - coeficientes regresion lineal
extern volatile float AMP_CAM_1;
extern volatile float OFFS_CAM_1;
extern volatile float AMP_BAG_2;
extern volatile float OFFS_BAG_2;
extern volatile float AMP_PAC_3;
extern volatile float OFFS_PAC_3;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
extern volatile float AMP_FI_1;
extern volatile float OFFS_FI_1;
extern volatile float LIM_FI_1;
extern volatile float AMP_FI_2;
extern volatile float OFFS_FI_2;
extern volatile float LIM_FI_2;
extern volatile float AMP_FI_3;
extern volatile float OFFS_FI_3;

// Sensor de flujo Espiratorio
extern volatile float AMP_FE_1;
extern volatile float OFFS_FE_1;
extern volatile float LIM_FE_1;
extern volatile float AMP_FE_2;
extern volatile float OFFS_FE_2;
extern volatile float LIM_FE_2;
extern volatile float AMP_FE_3;
extern volatile float OFFS_FE_3;

// variable para ajustar el nivel cero de flujo y calcular el volumen
extern volatile float VOL_SCALE;      // Factor de escala para ajustar el volumen
extern volatile float VOL_SCALE_SITE; // Factor de escala para ajustar el volumen

// **********************************************************
// Calibracion sensores Sitio
// Calibracion de los sensores de presion - coeficientes regresion lineal
extern volatile float AMP_CAM_1_SITE;
extern volatile float OFFS_CAM_1_SITE;
extern volatile float AMP_BAG_2_SITE;
extern volatile float OFFS_BAG_2_SITE;
extern volatile float AMP_PAC_3_SITE;
extern volatile float OFFS_PAC_3_SITE;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
extern volatile float AMP_FI_1_SITE;
extern volatile float OFFS_FI_1_SITE;
extern volatile float LIM_FI_1_SITE;
extern volatile float AMP_FI_2_SITE;
extern volatile float OFFS_FI_2_SITE;
extern volatile float LIM_FI_2_SITE;
extern volatile float AMP_FI_3_SITE;
extern volatile float OFFS_FI_3_SITE;

// Sensor de flujo Espiratorio
extern volatile float AMP_FE_1_SITE;
extern volatile float OFFS_FE_1_SITE;
extern volatile float LIM_FE_1_SITE;
extern volatile float AMP_FE_2_SITE;
extern volatile float OFFS_FE_2_SITE;
extern volatile float LIM_FE_2_SITE;
extern volatile float AMP_FE_3_SITE;
extern volatile float OFFS_FE_3_SITE;

extern String SerialID;
extern String menuString;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
unsigned int contCiclos = 0; // variable que controla el numero de ciclos que ha realizado un ventilador

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// funcion para configuracion de memoria
void init_Memory(void)
{
    EEPROM.begin(1024);

    contCiclos = eeprom_wr_int(0, 'r'); // carga la cantidad de ciclos en la bolsa

    SerialID.reserve(12);

    // Carga del Serial del equipo
    SerialID = readString(eeprom_values::SERIAL_ADDR);
    // Calibracion de los sensores de presion - coeficientes regresion lineal
    AMP_CAM_1 = readFloat(eeprom_values::AMP_CAM_1_ADDR);
    AMP_BAG_2 = readFloat(eeprom_values::AMP_BAG_2_ADDR);
    AMP_PAC_3 = readFloat(eeprom_values::AMP_PAC_3_ADDR);
    OFFS_CAM_1 = readFloat(eeprom_values::OFFS_CAM_1_ADDR);
    OFFS_BAG_2 = readFloat(eeprom_values::OFFS_BAG_2_ADDR);
    OFFS_PAC_3 = readFloat(eeprom_values::OFFS_PAC_3_ADDR);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1 = readFloat(eeprom_values::AMP_FI_1_ADDR);
    AMP_FI_2 = readFloat(eeprom_values::AMP_FI_2_ADDR);
    AMP_FI_3 = readFloat(eeprom_values::AMP_FI_3_ADDR);
    OFFS_FI_1 = readFloat(eeprom_values::OFFS_FI_1_ADDR);
    OFFS_FI_2 = readFloat(eeprom_values::OFFS_FI_2_ADDR);
    OFFS_FI_3 = readFloat(eeprom_values::OFFS_FI_3_ADDR);
    LIM_FI_1 = readFloat(eeprom_values::LIM_FI_1_ADDR);
    LIM_FI_2 = readFloat(eeprom_values::LIM_FI_2_ADDR);
    // Sensor de flujo Espiratorio
    AMP_FE_1 = readFloat(eeprom_values::AMP_FE_1_ADDR);
    AMP_FE_2 = readFloat(eeprom_values::AMP_FE_2_ADDR);
    AMP_FE_3 = readFloat(eeprom_values::AMP_FE_3_ADDR);
    OFFS_FE_1 = readFloat(eeprom_values::OFFS_FE_1_ADDR);
    OFFS_FE_2 = readFloat(eeprom_values::OFFS_FE_2_ADDR);
    OFFS_FE_3 = readFloat(eeprom_values::OFFS_FE_3_ADDR);
    LIM_FE_1 = readFloat(eeprom_values::LIM_FE_1_ADDR);
    LIM_FE_2 = readFloat(eeprom_values::LIM_FE_2_ADDR);
    // variable para ajustar el nivel cero de flujo y calcular el volumen
    VOL_SCALE = readFloat(eeprom_values::VOL_SCALE_ADDR);           // Factor de escala para ajustar el volumen
    VOL_SCALE_SITE = readFloat(eeprom_values::VOL_SCALE_SITE_ADDR); // Factor de escala para ajustar el volumen

    // Calibracion sensores Sitio
    AMP_CAM_1_SITE = readFloat(eeprom_values::AMP_CAM_1_SITE_ADDR);
    AMP_BAG_2_SITE = readFloat(eeprom_values::AMP_BAG_2_SITE_ADDR);
    AMP_PAC_3_SITE = readFloat(eeprom_values::AMP_PAC_3_SITE_ADDR);
    OFFS_CAM_1_SITE = readFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR);
    OFFS_BAG_2_SITE = readFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR);
    OFFS_PAC_3_SITE = readFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1_SITE = readFloat(eeprom_values::AMP_FI_1_SITE_ADDR);
    AMP_FI_2_SITE = readFloat(eeprom_values::AMP_FI_2_SITE_ADDR);
    AMP_FI_3_SITE = readFloat(eeprom_values::AMP_FI_3_SITE_ADDR);
    OFFS_FI_1_SITE = readFloat(eeprom_values::OFFS_FI_1_SITE_ADDR);
    OFFS_FI_2_SITE = readFloat(eeprom_values::OFFS_FI_2_SITE_ADDR);
    OFFS_FI_3_SITE = readFloat(eeprom_values::OFFS_FI_3_SITE_ADDR);
    LIM_FI_1_SITE = readFloat(eeprom_values::LIM_FI_1_SITE_ADDR);
    LIM_FI_2_SITE = readFloat(eeprom_values::LIM_FI_2_SITE_ADDR);
    // Sensor de flujo Espiratorio
    AMP_FE_1_SITE = readFloat(eeprom_values::AMP_FE_1_SITE_ADDR);
    AMP_FE_2_SITE = readFloat(eeprom_values::AMP_FE_2_SITE_ADDR);
    AMP_FE_3_SITE = readFloat(eeprom_values::AMP_FE_3_SITE_ADDR);
    OFFS_FE_1_SITE = readFloat(eeprom_values::OFFS_FE_1_SITE_ADDR);
    OFFS_FE_2_SITE = readFloat(eeprom_values::OFFS_FE_2_SITE_ADDR);
    OFFS_FE_3_SITE = readFloat(eeprom_values::OFFS_FE_3_SITE_ADDR);
    LIM_FE_1_SITE = readFloat(eeprom_values::LIM_FE_1_SITE_ADDR);
    LIM_FE_2_SITE = readFloat(eeprom_values::LIM_FE_2_SITE_ADDR);

    // inicializacion de variables String
    menuString.reserve(512);
    menuString = "";
}

// funcion para escribir Strings en la memoria EEPROM
void writeString(eeprom_values eeAddress, String dataStr)
{
    int _size = SERIAL_LENGTH;
    for (int i = 0; i < _size; i++)
    {
        EEPROM.write(eeAddress + i, dataStr[i]);
    }
    EEPROM.write(eeAddress + _size, '\0'); // add termination NULL
    EEPROM.commit();
}

// funcion para leer Strings en la memoria EEPROM
String readString(eeprom_values eeAddress)
{
    int i = 0;
    char dataStr[12];
    int len = 0;
    unsigned char k;
    k = EEPROM.read(eeAddress);
    while (k != '\0' && len < 11)
    {
        k = EEPROM.read(eeAddress + len);
        dataStr[len] = k;
        len++;
    }
    dataStr[len] = '\0';
    return String(dataStr);
}

// funcion para obtener valores flotantes de la EEPROM
float readFloat(eeprom_values eeAddress)
{
    float varValue = 0;
    EEPROM.get(eeAddress, varValue);
    return varValue;
}

// funcion para escribir valores flotantes de la EEPROM
float writeFloat(eeprom_values eeAddress, float varValue)
{
    float recoverValue = 0;
    EEPROM.put(eeAddress, varValue);
    EEPROM.commit();
    EEPROM.get(eeAddress, recoverValue);
    return recoverValue;
}

// funcion para almacenar los cambios en las constantes de Calibracion
void changeMenu(uint8_t coeftype, uint8_t modeCalibration, float newCoeficient, int place)
{
    float writeData = 0;
    String confirm;
    if (coeftype == AMPL_1)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_CAMARA) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_CAM_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_CAMARA) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_CAM_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_BOLSA) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_BAG_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_BOLSA) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_BAG_2_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_PACIENTE) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_PAC_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_PACIENTE) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_PAC_3_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == VOLUMEN_PACIENTE) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::VOL_SCALE_ADDR, newCoeficient);
        }
        else if ((modeCalibration == VOLUMEN_PACIENTE) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::VOL_SCALE_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == OFFS_1)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_CAMARA) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_CAM_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_CAMARA) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_BOLSA) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_BAG_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_BOLSA) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == PRESION_PACIENTE) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_PAC_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == PRESION_PACIENTE) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == LIMS_1)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::LIM_FI_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::LIM_FI_1_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::LIM_FE_1_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::LIM_FE_1_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == AMPL_2)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_2_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_2_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == OFFS_2)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_2_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_2_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == LIMS_2)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::LIM_FI_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::LIM_FI_2_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::LIM_FE_2_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::LIM_FE_2_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == AMPL_3)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FI_3_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::AMP_FE_3_SITE_ADDR, newCoeficient);
        }
    }
    else if (coeftype == OFFS_3)
    {
        if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_INSPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FI_3_SITE_ADDR, newCoeficient);
        }
        if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == FACTORY))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_3_ADDR, newCoeficient);
        }
        else if ((modeCalibration == FLUJO_ESPIRATORIO) && (place == SITE))
        {
            writeData = writeFloat(eeprom_values::OFFS_FE_3_SITE_ADDR, newCoeficient);
        }
    }

    confirm = "Dato guardado: " + String(writeData, 5) + "\n";
    Serial.print(confirm);
}

// funcion para el almacenamiento de datos de ciclado en la memoria
int eeprom_wr_int(int dataIn, char process)
{
    int dataRead = 0;
    if (process == 'w')
    {
        byte r1 = (dataIn & 0xff);
        EEPROM.write(0, r1);
        r1 = (dataIn & 0xff00) >> 8;
        EEPROM.write(1, r1);
        r1 = (dataIn & 0xff0000) >> 16;
        EEPROM.write(2, r1);
        r1 = (dataIn & 0xff000000) >> 24;
        EEPROM.write(3, r1);
        EEPROM.commit();
        return dataRead;
    }
    else if (process == 'r')
    {
        dataRead = EEPROM.read(0);
        dataRead = (EEPROM.read(1) << 8) + dataRead;
        dataRead = (EEPROM.read(2) << 16) + dataRead;
        dataRead = (EEPROM.read(3) << 24) + dataRead;
        return dataRead;
    }
    return dataRead;
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
