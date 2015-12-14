/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/
#include "canales.h"
 
/* Class Canal  */
 
//////// Constructores
 
Canal::Canal(char * pun_nombre){
    strcpy(this->nombre, pun_nombre);
    this->num_usuarios = 0;
    this->tema[0] = '\0';
}
 
Canal::~Canal(){
}
 
//////// GETTERS
 
 
// funcion para obtener el numero de usuarios
int Canal::contarUsuarios(){
    return this->num_usuarios;
}
 
 
// funcion para obtener nombre del canal
void Canal::getNombre(char * pun_nombre){
    if (pun_nombre != NULL){
        strcpy(pun_nombre, this->nombre);
    }
}
 
// funcion para obtener tema del canal
int Canal::getTema(char * pun_tema){
    if (pun_tema == NULL){
        return -1;
    }
    strcpy(pun_tema, this->tema);
    return 0;
}
 
 
// funcion para obtener un usuario del canal por posicion
int Canal::getUsuario(int i){
    if (i < this->contarUsuarios()){
        return this->usuarios_canal[i];
    }else{
        return -1;
    }
}
 
// obtener lista de usuarios del canal
// si el longitud es menor enviamos valor de error
// caso contrario recorremos los usuarios en el canal y copiamos
//retornamos el valor de los usuarios del canal como success
int Canal::getUsuarios(int rusuarios[], int tam){
 
    if (tam < this->contarUsuarios()){
        return -1;
    }
    for (int i = 0; i < this->contarUsuarios(); i++){
        rusuarios[i] = this->usuarios_canal[i];
    }
    return this->contarUsuarios();
}
         
 
/////////// SETTERS
 
// funcion para setear un tema
// si tema es mayor al tamaño max se finaliza la cadena
// se guarda en el canal
int Canal::setTema(char * pun_tema){
    int tema_lim = FALSE;
 
    if (pun_tema == NULL){
        return -1;
    }
     
    if (strlen(pun_tema) > MAX_TAM_TEMA){
        tema_lim = TRUE;
        pun_tema[MAX_TAM_TEMA] = '\0';
    }
    strcpy(this->tema, pun_tema);
 
    return tema_lim;
}
 
//////////// FUNCIONES VARIAS de CANAL
 
// buscar usuario en canal por id
int Canal::buscarUsuario(int id){
    for (int i = 0; i < this->contarUsuarios(); i++){
        if ( this->usuarios_canal[i] == id ){
            return i;
        }
    }
    return -1;
}
 
// funcion booleana que devuelve si un usuario se encuentra en el canal
int Canal::perteneceCanal(int id){
    return (this->buscarUsuario(id) >= 0);
}
 
// funcion para añadir un usuario al canal mediante id
// si no hay espacio en canal, id invalido o si
//usario no pertence a canal envia error
//añade el id del usuario al canal y retorna un success
int Canal::anadirUsuario(int id){
    if (this->contarUsuarios() >= MAX_USUARIOS_X_CANAL){
        return -1;
    }
    if (id < 0){
        return -2;
    }
    if (this->perteneceCanal(id)){
        return -3;
    }
    this->usuarios_canal[this->num_usuarios++] = id;
    return 0;
}
 
 
// funcion para anañadir un usuario al canal por nombre
int Canal::anadirUsuario(char *nombre_usu){
    this->anadirUsuario(getUsuarioxNombre(nombre_usu));
}
 
 
// funcion para remover usuario del canal por id
// preguntamos si el la posicion es menor a 0 o no hay usuario en el canal
// buscamos usuario por id, al encontrarlo lo removemos de la cola
int Canal::removerUsuario(int id){
    int userpos;
 
    if (this->contarUsuarios() <= 0){
        return -1;
    }
    if (id < 0){
        return -2;
    }
     
    userpos = this->buscarUsuario(id);
    if (userpos < 0){
        return -3;
    }
    for (int i = userpos; i < this->contarUsuarios()-1; i++){
        this->usuarios_canal[i] = this->usuarios_canal[i+1];
    }
 
    return 0;
}
 
 
// funcion para remvoer usuario del canal por nombre
int Canal::removerUsuario(char *nombre_usu){
    this->removerUsuario(getUsuarioxNombre(nombre_usu));
}
 
// funcion que permite enviar mensaje atravez del canal
void Canal::enviarMensaje(int id, char * msg, int remitente){
    for (int i = 0; i < this->contarUsuarios(); i++){
        if ( this->usuarios_canal[i] != id || remitente ){
            usuarios[this->usuarios_canal[i]]->enviarMensaje(msg);
        }
    }
}
 
 
 
////// funciones extras para Canal //////////
 
// funcion booleana retorna validez de un canal
int canalValido(int id){
    return (canales[id] != NULL);
}
 
 
// funcion que retorna el primer canal disponible
// retorna la posicion del canal si success
int obtenerCanalSinUsar(){
    for (int i = 0; i < MAX_NUM_CANALES; i++){
        if ( !canalValido(i)){
            return i;
        }
    }
    return -1;
}
 
//funcion para obtener el canal por el nombre
//retorna indice del canal en success
int obtenerCanalxNombre(char * pun_nombre){
    char nombre_canal[MAX_TAM_CANAL+1];
    for (int i = 0; i < MAX_NUM_CANALES; i++){
        if (canalValido(i)){
            canales[i]->getNombre(nombre_canal);
            if (strcmp(nombre_canal, pun_nombre) == 0){
                return i;
            }
        }
    }
    return -1;
}
 
// funcion boleana para determinar si un canal existe
int existeCanal(char * nombre_canal){
    return (obtenerCanalxNombre(nombre_canal) >= 0);
}
