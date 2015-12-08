/* 
Written by Michalis Zervos - http://michal.is
 */
 
#include "Canal.h"

/* Canal class methods */

Canal::Canal(char * nombre)
{
	strcpy(this->nombre, nombre);
	this->num_users = 0;
	this->tema[0] = '\0';
}

Canal::~Canal()
{
}


void Canal::getName(char * nombre)
{
	if (nombre != NULL)
		strcpy(nombre, this->nombre);
}

int Canal::countUsers()
{
	return this->num_users;
}
	

int Canal::getUser(int i)
{
	if (i < this->countUsers())
		return this->cusers[i];
	else
		return -1;
}


int validCanal(int cid)
{
	return (canales[cid] != NULL);
}

int Canal::buscarUser(int id)
{
	for (int i = 0; i < this->countUsers(); i++)
		if ( this->canal_users[i] == id )
			return i;

	return -1;
}