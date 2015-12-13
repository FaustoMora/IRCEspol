/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/


#ifndef __CONSTANTES__
#define __CONSTANTES__


//Constantes booleanas.
#define TRUE 1
#define FALSE 0

//Constantes del canal.
#define MAX_TAM_CANAL 50
#define MAX_NUM_CANALES 200
#define MAX_USUARIOS_X_CANAL 100

//Constantes del usuario.
#define MAX_TAM_NICKNAME 8
#define MAX_TAM_HOSTNAME 255
#define MAX_TAM_USERNAME 50
#define MAX_TAM_NOMBRE 50
#define MAX_NUM_USUARIOS 200
#define MAX_CANALES_X_USUARIO 20


//Constantes de entrada.
#define MAX_TAM_TEMA 200
#define MAX_PARAMETROS 20
#define MAX_COLA_ESCUCHA 5
#define TAM_BUFFER 512


//Constantes de errores.
#define NOEXISTENICKNAME 401
#define ERR_CANNOTSENDTOCHAN 404
#define NOEXISTERECEPTOR 411
#define INGRESEALGOPARAENVIAR 412
#define COMANDODESCONOCIDO 421
#define FALTANPARAMETROS 461
#define YAESTAREGISTRADO 462 
#define ERR_CHANOPPRIVSNEEDED 482
#define NOEXISTECANAL 403
#define INGRESANICKNAME 431
#define ERR_ERRONEUSNICKNAME 432
#define NICKNAMEENUSO 433
#define NOESTASREGISTRADO 451

#endif
