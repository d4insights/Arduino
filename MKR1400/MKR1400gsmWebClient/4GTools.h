/*
 *  Biblioteca de conexión 4G 
 * 
 * 
 * 
*/

#include <MKRGSM.h>

const char PINNUMBER[]     = "1111";                  // Este es el PIN de la SIM, anda igua si esta vacío 
const char GPRS_APN[]      = "igprs.claro.com.ar";    // GPRS APN (provisto por CLARO ARGENTINA)
const char GPRS_LOGIN[]    = "clarogprs";             // GPRS login (puede ir vacío en CLARO)
const char GPRS_PASSWORD[] = "clarogprs";             // GPRS password (puede ir vacío en CLARO)


GSMClient client;                                     // Objeto Cliente GSM
GPRS gprs;
GSM gsmAccess;
