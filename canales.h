/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo de cabecera para canales.cpp
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/
 
#ifndef __CANALES__
#define __CANALES__
 
#include <string.h>
#include "constantes.h"
#include "usuarios.h"
 
class Canal
{
    private:
 
    char nombre[MAX_TAM_CANAL+1];
    int num_usuarios;
    int usuarios_canal[MAX_USUARIOS_X_CANAL];
    char tema[MAX_TAM_TEMA+1];
 
    public: 
     
    Canal(char * pun_nombre);
    ~Canal();
    void getNombre(char * pun_nombre);
    int setTema(char * pun_tema);
    int getTema(char * pun_tema);
    int getUsuario(int i);
    int contarUsuarios();
    int perteneceCanal(int id);
    int anadirUsuario(int id);
    int anadirUsuario(char *nombre_usu);
    int removerUsuario(int id);
    int removerUsuario(char *nombre_usu);
    void enviarMensaje(int id, char * msg, int to_remitente);
    int buscarUsuario(int id);
};
 
 
extern Canal *canales[MAX_NUM_CANALES];
 
int canalValido(int id);
int obtenerCanalSinUsar();
int obtenerCanalxNombre(char * pun_nombre);
int existeCanal(char * nombre_canal);
 
#endif
