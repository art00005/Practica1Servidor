/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: servidor.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Servidor de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez
Alumnos Jesus Bermejo Torrent - Andrés del Rio Tassara

*******************************************************/
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>
#include "protocol.h"




main()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKET sockfd,nuevosockfd;
	struct sockaddr_in  local_addr,remote_addr;
	char buffer_out[1024],buffer_in[1024], cmd[10], usr[10], pas[10];
	int err,tamanio;
	int fin=0, fin_conexion=0;
	int recibidos=0,enviados=0;
	int estado=0;
	int NUM1=0;													//__________________________________Variables NUM__________________________
	int NUM2=0;
	int SUMA=0;
	/** INICIALIZACION DE BIBLIOTECA WINSOCK2 **
	 ** OJO!: SOLO WINDOWS                    **/
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0){
		return(-1);
	}
	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup() ;
		return(-2);
	}
	/** FIN INICIALIZACION DE BIBLIOTECA WINSOCK2 **/


	sockfd=socket(AF_INET,SOCK_STREAM,0);									//-----------------------Primitiva SOCKET= crea un socket y devuelve un descriptor, en caso contrario devuelve -1 ----------------													

	if(sockfd==INVALID_SOCKET)	{
		return(-3);
	}
	else {
		local_addr.sin_family		=AF_INET;			// Familia de protocolos de Internet
		local_addr.sin_port			=htons(TCP_SERVICE_PORT);	// Puerto del servidor
		local_addr.sin_addr.s_addr	=htonl(INADDR_ANY);	// Direccion IP del servidor Any cualquier disponible
													// Cambiar para que conincida con la del host
	}
	
	// Enlace el socket a la direccion local (IP y puerto)
	if(bind(sockfd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0)					//-------------------------Primitiva BIND= asocia una dirección local con un socket-----------------------------
		return(-4);
	
	//Se prepara el socket para recibir conexiones y se establece el tamaño de cola de espera
	if(listen(sockfd,5)!=0)															//-----------------------------Primitiva LISTEN= crea una cola de espera para almacenar solicitudes de conexión--------------
		return (-6);
	
	tamanio=sizeof(remote_addr);

	do
	{

		//-------------------------------------------------EJERCICIO---------------------------------------------------------------------------------

		do{																						//Para el while del siguiente ejercicio

		//-------------------------------------------------FIN------------------------------------------------------------------------------------

		printf ("SERVIDOR> ESPERANDO NUEVA CONEXION DE TRANSPORTE\r\n");
		
		nuevosockfd=accept(sockfd,(struct sockaddr*)&remote_addr,&tamanio);					//--------------------------Primitiva ACCEPT= Espera una solicitud de conexion------------------------

		if(nuevosockfd==INVALID_SOCKET) {
			
			return(-5);
		}

		printf ("SERVIDOR> CLIENTE CONECTADO\r\nSERVIDOR [IP CLIENTE]> %s\r\nSERVIDOR [CLIENTE PUERTO TCP]>%d\r\n",
					inet_ntoa(remote_addr.sin_addr),ntohs(remote_addr.sin_port));

		//Mensaje de Bienvenida
		sprintf_s (buffer_out, sizeof(buffer_out), "%s Bienvenindo al servidor de ECO%s",OK,CRLF);
		
		enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);						//-------------------------Primitiva SEND= envía mensaje -----------------------------
		//TODO Comprobar error de envío	

		//---------------------------------------------------EJERCICIO----------------------------------------------------------------------------
		
			if (enviados == SOCKET_ERROR){
				printf(" Error del socket");
			}
			else if (enviados == 0){
				printf("La conexion ha sido liberada de forma acordada");
			}
			else {
				printf("Datos enviados \n ");
			}

		} while (enviados == SOCKET_ERROR && enviados == 0);                                                        //en caso de error vuelta al inicio no se puede poner fin_conexion=x ya que se reinicializa a 0 en la siguiente linea
		//----------------------------------------------------FIN---------------------------------------------------------------------------------------



		//Se reestablece el estado inicial
		estado = S_USER;
		fin_conexion = 0;

		printf ("SERVIDOR> Esperando conexion de aplicacion\r\n");
		do
		{
			//Se espera un comando del cliente
			recibidos = recv(nuevosockfd,buffer_in,1023,0);										//-------------------------Primitiva RECV= recibir mensaje ---------------------------
			//TODO Comprobar posible error de recepción
			
			//----------------------------------------------------------EJERCICIO-----------------------------------------------------------------

			if (recibidos <= 0){																	//si datos recividos por el sever menores o iguales a 0 error en recepcion
				if (recibidos < 0){
					printf("Se ha liberado la conexion\r\n");
					estado = S_HELO;															//cambio de estado ya que el servidor toma antes el estado S_USER
					fin_conexion=1;
				}
				else{
					printf("Error al recibir\r\n");
					estado = S_HELO;
					fin_conexion=1;
				}
			}
			else{
				buffer_in[recibidos] = 0x00;
				printf("SERVIDOR [bytes recibidos]> %d\r\nSERVIDOR [datos recibidos]>%s", recibidos, buffer_in);
			}

			//------------------------------------------------------------FIN--------------------------------------------------------------------------------
			
			switch (estado)
			{
				case S_USER:    /*****************************************/
					strncpy_s ( cmd, sizeof(cmd),buffer_in, 4);											//_________________________Copia caracteres del buffer a cmd___________________________
					cmd[4]=0x00; // en C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,SC)==0 ) // si recibido es solicitud de conexion de aplicacion
					{
						sscanf_s (buffer_in,"USER %s\r\n",usr,sizeof(usr));								//________________________Lee datos del buffer__________________________________
						
						// envia OK acepta todos los usuarios hasta que tenga la clave
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", OK,CRLF);					//__________________________Envia valores al buffer_______________________________
						
						estado = S_PASS;
						printf ("SERVIDOR> Esperando clave\r\n");
					} else
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_PASS: /******************************************************/

					
					strncpy_s ( cmd, sizeof(cmd), buffer_in, 4);
					cmd[4]=0x00; // en C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,PW)==0 ) // si comando recibido es password
					{
						sscanf_s (buffer_in,"PASS %s\r\n",pas,sizeof(usr));						//?????????????????????????????????sizeof usr

						if ( (strcmp(usr,USER)==0) && (strcmp(pas,PASSWORD)==0) ) // si password recibido es correcto
						{
							// envia aceptacion de la conexion de aplicacion, nombre de usuario y
							// la direccion IP desde donde se ha conectado
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s IP(%s)%s", OK, usr, inet_ntoa(remote_addr.sin_addr),CRLF);
							estado = S_DATA;
							printf ("SERVIDOR> Esperando comando\r\n");
						}
						else
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticación errónea%s",ER,CRLF);
						}
					} else
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_DATA: /***********************************************************/
					
					buffer_in[recibidos] = 0x00;
					
					strncpy_s(cmd,sizeof(cmd), buffer_in, 4);

					printf ("SERVIDOR [Comando]>%s\r\n",cmd);
					
					if ( strcmp(cmd,SD)==0 )
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexión%s", OK,CRLF);
						fin_conexion=1;
					}
					else if (strcmp(cmd,SD2)==0)
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Finalizando servidor%s", OK,CRLF);
						fin_conexion=1;
						fin=1;
					}

					//--------------------------------------------------------EJERCICIO SUM--------------------------------------------------------------
					else if (strcmp(cmd,SUM)==0 ){
						sscanf_s (buffer_in, "sum %i %i\r\n", &NUM1, &NUM2 );
						SUMA = NUM1 + NUM2;
						if (SUMA > 99999 || SUMA < -99999){
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", ERROR, CRLF);
						
						} else{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s%i%s", OK, SP, SUMA, CRLF);
						
						}

					}

					//----------------------------------------------------------------FIN------------------------------------------------------------
					else{

						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
					break;

				default:
					break;
					
			} // switch

			enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0);									//-------------------Primitiva SEND= envío de datos-------------
			//TODO 
			//------------------------------------------------------------EJERCICIO-----------------------------------------------------------------------------
				
			if (enviados == SOCKET_ERROR){
				printf("Error en el envio \r\n ");
				fin_conexion= 1;
			}
			else if (enviados == 0){
				printf("Conexion ha sido liberada de forma acordada");
				fin_conexion= 1;
			}
			else{
				printf("Datos enviados \n ");
			}

			//---------------------------------------------------------------FIN----------------------------------------------------------------------------------


		} while (!fin_conexion);
		printf ("SERVIDOR> CERRANDO CONEXION DE TRANSPORTE\r\n");
		shutdown(nuevosockfd,SD_SEND);													//-------------------------Primitiva SHUTDOWN= Deshabilita la recepción y/o el envío de datos por le socket------------
		closesocket(nuevosockfd);														//------------------------- Primitiva CLOSESOCKET= cierra el socket-----------------------------------

	}while(!fin);

	printf ("SERVIDOR> CERRANDO SERVIDOR\r\n");

	return(0);
} 
