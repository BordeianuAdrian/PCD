
#include <stdio.h>
#include <stdlib.h>

// librarii pentru time function, sockets, htons, file stat
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// librarii pentru file function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
// dimensiunea bufferului folosit pentru a trimite fis (in mai multe blocuri)
#define BUFFERT 512


// declaratiile functiilor
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_server_socket (int port);

struct sockaddr_in sock_serv,clt;

int main (int argc, char**argv){
    //file descriptor si socket file descriptor
	int fd, sfd;
    
	char buf[BUFFERT];
	off_t count=0, n; // long 
	char filename[256];
    unsigned int l=sizeof(struct sockaddr_in);
	
    // variabila pt data
	time_t intps;
	struct tm* tmi;
    
	if (argc != 2){
		printf("eroare la utilizare : %s <port_serv>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd = create_server_socket(atoi(argv[1]));
    
	intps = time(NULL);
	tmi = localtime(&intps);
	bzero(filename,256);
	sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
	printf("Crearea fisierului output: %s\n",filename);
    
	//deschiderea fisierului
	if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
		perror("Eroare la deschidere");
		return EXIT_FAILURE;
	}
    
	//pregatirea trimiterii
	bzero(&buf,BUFFERT);
    n=recvfrom(sfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	while(n){
		printf("%lld de date primite \n",n);
		if(n==-1){
			perror("Eroare la citire");
			return EXIT_FAILURE;
		}
		count+=n;
		write(fd,buf,n);
		bzero(buf,BUFFERT);
        n=recvfrom(sfd,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	}
    
	printf("Numarul de octeti transferati: %lld \n",count);
    
    close(sfd);
    close(fd);
	return EXIT_SUCCESS;
}

// functie pentru calcularea duratei trimiterii
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

/* Functia care permite crearea unui socket si atasarea acestuia la sistem
 * Returneaza un descriptor de fisier in tabelul descriptorului de proces
 * bind permite definirea sa cu sistemul
 */
int create_server_socket (int port){
    int l;
	int sfd;
    
	sfd = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd == -1){
        perror("Eroare socket");
        return EXIT_FAILURE;
	}
    
    //pregatirea adresei socket-ului de destinatie
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
	sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
	//alocarea unei identitati socket-ului
	if(bind(sfd,(struct sockaddr*)&sock_serv,l)==-1){
		perror("Eroare bind");
		return EXIT_FAILURE;
	}
    
    
    return sfd;
}
