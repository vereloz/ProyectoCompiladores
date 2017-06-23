#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <pigpio.h>

#define led 	04
#define ventilador    03


/* Cada que se crea una conexión entra a esta función
 * para recbir multiples mensaje */
void *conexion(void *socket_desc)
{

    int sock = *(int*)socket_desc;
    int tam_bytes;
    char *mensaje , mensaje_recibido[2000];
    //Al establecer la conexión se envian mensajes
    mensaje = "Se estableció conexion TCP\n";
    write(sock , mensaje , strlen(mensaje));
    mensaje = "Escribe el comando que deseas realizar \n";
    write(sock , mensaje , strlen(mensaje));

    //Cada que recibe un mensaje del cliente
    while( (tam_bytes = recv(sock , mensaje_recibido , 2000 , 0)) > 0 )
    {





        printf(mensaje_recibido);
        if (strcmp(mensaje_recibido, "encender led\r\n") == 0){
				printf("Encendiendo led \n");
				write(sock ,"Encendiendo led \n", strlen("Encendiendo led \n"));
				gpioWrite(led,1);
        }
        else if (strcmp(mensaje_recibido, "apagar led\r\n") == 0){
        		printf("Apagando led\n");
        		write(sock ,"Apagando led\n", strlen("Apagando led\n"));
				gpioWrite(led,0);
        }
        else if (strcmp(mensaje_recibido, "encender ventilador\r\n") == 0){
        		printf("Encendiendo ventilador\n");
        		write(sock ,"Encendiendo ventilador\n", strlen("Encendiendo ventilador\n"));
				gpioWrite(ventilador,1);
         }
        else if (strcmp(mensaje_recibido, "apagar ventilador\r\n") == 0){
				printf("Apagando ventilador\n");
				write(sock ,"Apagando ventilador\n", strlen("Apagando ventilador\n"));
				gpioWrite(ventilador,0);
		 }
        else{
        	write(sock ,mensaje_recibido, strlen(mensaje_recibido));
        }
		//clear the mensaje buffer
		memset(mensaje_recibido, 0, 2000);
    }

    if(tam_bytes == 0)
    {
        puts("cliente disconnected");
        fflush(stdout);
    }
    else if(tam_bytes == -1)
    {
        perror("recv failed");
    }

    return 0;
}



int main(int argc , char *argv[])
{

    int socket_desc , cliente_sock , c;
    struct sockaddr_in servidor , cliente;


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    if (gpioInitialise()<0) {
    			return -1; //Inicializando pigpio
    		}

    gpioSetMode(ventilador,PI_OUTPUT);
    gpioSetMode(led,PI_OUTPUT);
    gpioWrite(led,0);
    gpioWrite(ventilador,0);
    puts("El socket ha sido creado");


    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons( 7777 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&servidor , sizeof(servidor)) < 0)
    {

        perror("bind falló. Error");
        return 1;
    }
    puts("bind hecho");

    //Escuchar el socket
    listen(socket_desc , 3);


    //Aceptar la conexión
    puts("Esperando conexiones...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

    while( (cliente_sock = accept(socket_desc, (struct sockaddr *)&cliente, (socklen_t*)&c)) )
    {
        puts("Conexión aceptada");

        if( pthread_create( &thread_id , NULL ,  conexion , (void*) &cliente_sock) < 0)
        {
            perror("Error al intentar crear el hilo");
            return 1;
        }


        puts("Comenzando comunicación");
    }

    if (cliente_sock < 0)
    {
        perror("Falló al intentar establecer comunicación");
        return 1;
    }

    return 0;
}

