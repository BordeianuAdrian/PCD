/*
 * File : client.c
 * Autori: Teodor Branescu, Iulian Cimpan, Adrian Bordeianu, Petri Bogdan, Sebastian Carabasiu
 *
 * Creat : 17 Iunie 2020
 */

#include <stdio.h>
#include <stdlib.h>

// Functii de timp socket-uri etc.
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// Functii de fisiere
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

/* Dimensiunea buffer-ului folosit pentru a trimite fisierul
 */

#define BUFFERT 512


/* Declararea functiilor*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_client_socket (int port, char* ipaddr);

struct sockaddr_in sock_serv;

int main (int argc, char**argv){
	struct timeval start, stop, delta;
    int sfd,fd;
    char buf[BUFFERT];
    off_t count=0, m,sz;//long
	long int n;
    int l=sizeof(struct sockaddr_in);
	struct stat buffer;
    
	if (argc < 2){
		printf("Eroare de utilizare : %s <ip_serv> <port_serv> <filename>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd=create_client_socket(atoi(argv[2]), argv[1]);

    char filename[] = "p.py";
    FILE* file;


    Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);
	file = fopen(filename, "r");
    PyRun_SimpleFile(file, "p.py");
    Py_Finalize();
    
	if ((fd = open(argv[3],O_RDONLY))==-1){
		perror("eroare de deschidere");
		return EXIT_FAILURE;
	}
    
	//dimensiunea fisierelor
	if (stat(argv[3],&buffer)==-1){
		perror("stat esuare");
		return EXIT_FAILURE;
	}
	else
		sz=buffer.st_size;
    
	//pregatirea expedierii
	bzero(&buf,BUFFERT);
    
	gettimeofday(&start,NULL);
    n=read(fd,buf,BUFFERT);
	while(n){
		if(n==-1){
			perror("esuare citire");
			return EXIT_FAILURE;
		}
		m=sendto(sfd,buf,n,0,(struct sockaddr*)&sock_serv,l);
		if(m==-1){
			perror("eroare trimitere");
			return EXIT_FAILURE;
		}
		count+=m;
		//fprintf(stdout,"----\n%s\n----\n",buf);
		bzero(buf,BUFFERT);
        n=read(fd,buf,BUFFERT);
	}
	//cititul tocmai a returnat 0: sfârșitul fișierului
	
	//pentru a debloca serv
	m=sendto(sfd,buf,0,0,(struct sockaddr*)&sock_serv,l);
	gettimeofday(&stop,NULL);
	duration(&start,&stop,&delta);
    
	printf("Numarul de octeti transferati : %lld\n",count);
	printf("O marime totala de : %lld \n",sz);
	printf("Cu o durata de : %ld.%d \n",delta.tv_sec,delta.tv_usec);
    
    close(sfd);
    close(fd);
	return EXIT_SUCCESS;
}

/* Funcție care permite calcularea duratei trimiterii */
int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
    suseconds_t microstart, microstop, microdelta;
    
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}

/* 
	Funcție care permite crearea unui socket
  * Returnează un descriptor de fișier
 */

int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd;
    
	sfd = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd == -1){
        perror("socket esuat");
        return EXIT_FAILURE;
	}
    
    //pregătirea adresei socket-ului  de destinație
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Adresa IP invalida:\n");
		return EXIT_FAILURE;
	}
    
    return sfd;
}
