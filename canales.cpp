/*
SERVIDOR IRC EN C++
Sistemas Operativos
ESPOL 2015-2016
Archivo canales.cpp
Grupo: Andres Sornoza, Fausto Mora, Wilson Enriquez
Adaptacion Simplificada de Michalis Zervos - http://michal.is
*/

#include "canales.h"
 
/* ----------------- Constructores ----------------------*/
 
//Constructor
Canal::Canal(char * pun_nombre){
    strcpy(this->nombre, pun_nombre);
    this->num_usuarios = 0;
    this->tema[0] = '\0';
}
 
//Destructor.
Canal::~Canal(){
}
 

/* ----------------- Getters ----------------------*/

// Funcion para obtener nombre del canal.
void Canal::getNombre(char * pun_nombre){
    if (pun_nombre != NULL){
        strcpy(pun_nombre, this->nombre);
    }
}
 
// Funcion para obtener tema del canal.
int Canal::getTema(char * pun_tema){
    if (pun_tema == NULL){
        return -1;
    }
    strcpy(pun_tema, this->tema);
    return 0;
}
 
 
// Funcion para obtener un usuario del canal por posicion en la lista del canal.
int Canal::getUsuario(int i){
    if (i < this->contarUsuarios()){
        return this->usuarios_canal[i];
    }else{
        return -1;
    }
}

/* ----------------- Setters ----------------------*/

// Funcion para setear un tema, si tema es mayor al tamano maximo establecidoe finaliza la cadena, se guarda en el canal.

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


/* ----------------- Funciones Auxiliares ----------------------*/

// Funcion para obtener el numero de usuarios
int Canal::contarUsuarios(){
    return this->num_usuarios;
} 

// Buscar usuario en canal por id
int Canal::buscarUsuario(int id){
    for (int i = 0; i < this->contarUsuarios(); i++){
        if ( this->usuarios_canal[i] == id ){
            return i;
        }
    }
    return -1;
}
 
// Funcion que devuelve 1(int) si un usuario se encuentra en el canal.
int Canal::perteneceCanal(int id){
    return (this->buscarUsuario(id) >= 0);
}
 
// Funcion para añadir un usuario al canal mediante id.
// Si no hay espacio en canal retorna -1, Si id es invalido retorna -2, Si ya pertenece al canal -3
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
 
 
// Funcion para añadir un usuario al canal por nombre.
// Utiliza la funcion getUsuarioxNombre de usuarios.cpp para obtener el id y volver a llamar a anadirUsuario(int)
int Canal::anadirUsuario(char *nombre_usu){
    this->anadirUsuario(getUsuarioxNombre(nombre_usu));
}

//Funcion para remvoer usuario del canal por id.
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
 
 
// Funcion para remvoer usuario del canal por nombre.
int Canal::removerUsuario(char *nombre_usu){
    this->removerUsuario(getUsuarioxNombre(nombre_usu));
}
 
 
// Funcion que permite enviar mensaje por el canal.
void Canal::enviarMensaje(int id, char * msg, int remitente){
    for (int i = 0; i < this->contarUsuarios(); i++){
        if ( this->usuarios_canal[i] != id || remitente ){
            usuarios[this->usuarios_canal[i]]->enviarMensaje(msg);
        }
    }
}
 
 
 
/* ----------------- Funciones Externas de la Clase ----------------------*/
 
// Funcion que retorna 1(int) si un canal existe o es valido.
int canalValido(int id){
    return (canales[id] != NULL);
}
 
 
// Funcion que retorna la posicion del primer canal disponible.
int obtenerCanalSinUsar(){
    for (int i = 0; i < MAX_NUM_CANALES; i++){
        if ( !canalValido(i)){
            return i;
        }
    }
    return -1;
}
 
// Funcion para obtener el canal por el nombre, retorna el indice en succes.
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
 
// Funcion que retorna 1(int) para determinar si un canal existe con determinado nombre.
int existeCanal(char * nombre_canal){
    return (obtenerCanalxNombre(nombre_canal) >= 0);
}
