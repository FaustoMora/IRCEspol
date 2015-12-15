/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/
 
#ifndef __USUARIOS__
#define __USUARIOS__

#include "constantes.h"
#include "ircs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctime>
#include <unistd.h>
#include <pthread.h>

typedef enum { NONE, NICK, USER, REG } Regstatus;

class Usuario
{
	private:
		char nickname[MAX_TAM_NICKNAME+1];
		char nombre_usuario[MAX_TAM_USERNAME+1];
		char nombre_real[MAX_TAM_NOMBRE+1];
		char nombre_host[MAX_TAM_HOSTNAME+1];
		char *buf;
		char *cmd_parametros[MAX_PARAMETROS];
		int sock;
		pthread_t tid;
		Regstatus estado;
		int canales_usuario[MAX_CANALES_X_USUARIO];
		int num_canales;
		
		
	public:
		pthread_mutex_t lock;

		Usuario(int sock, char * nombre_host, pthread_t tid);
		~Usuario();
		int getNickname(char *nick);
		int setNickname(char *nick);
		int getNombreUsuario(char *nombre_usu);
		int setNombreUsuario(char *nombre_usu);
		int getNombreReal(char *rNombre);
		int setNombreReal(char *rNombre);
		int getNombreHost(char *hNombre);
		int numCanales();
		Regstatus getEstado();
		void setEstado(Regstatus rs);
		int getSock();
		pthread_t getThreadId();
		int unirse(char *chan);
		int unirse(int cid);
		int salirse(char *chan);
		int salirse(int cid);
		int isIn(int cid);
		int buscarCanal(int cid);
		int enviarMensaje(char *msg);	
		int esperarEntrada();
		int parsearEntrada();
		int estaRegistrado();
		void enviarError(int err_num, char * par, char * msg);
		void enviarInfo(char * par, char * msg);
		int act(int num_params);
		int enviarMotd();

};

extern Usuario *usuarios[MAX_NUM_USUARIOS];

int usuarioValido(int uid);
int getUsuarioxNombre(char * rNombre);
int getUsuarioSinUsar();
int nicknameOcupado(char * rname);
int nombreUsuarioValido(char * nombre_usu);
void construirMensaje(char *nick, char *nombre_usu, char *hNombre, char *cmd, char *params[MAX_PARAMETROS], char *buf);

#endif

