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

// dimensiunea cozii de clienti
#define BACKLOG 1

// declaratiile functiilor
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_server_socket (int port);

struct sockaddr_in sock_serv,sock_clt;


int main(int argc,char** argv){
    int sfd,fd;
    unsigned int length=sizeof(struct sockaddr_in);
    long int n, m,count=0;
    unsigned int nsid;
    ushort clt_port;
    char buffer[BUFFERT],filename[256];
    char dst[INET_ADDRSTRLEN];
    
    // variabila pt data
	time_t intps;
	struct tm* tmi;
    
    if(argc!=2) {
        perror("utilizare: ./a.out <num_port>\n");
        exit(3);
    }
    
    sfd = create_server_socket(atoi(argv[1]));
    
    bzero(buffer,BUFFERT);
    listen(sfd,BACKLOG);
    
    nsid=accept(sfd,(struct sockaddr*)&sock_clt,&length);
    if(nsid==-1){
        perror("Accept esuat.");
        return EXIT_FAILURE;
    }
    else {
        if(inet_ntop(AF_INET,&sock_clt.sin_addr,dst,INET_ADDRSTRLEN)==NULL){
            perror("Eroare la socket.");
            exit (4);
        }
        clt_port=ntohs(sock_clt.sin_port);
        printf("Pornirea conexiunii pentru: %s:%d\n",dst,clt_port);
        
        //prelucrarea numelui fisierului output
        bzero(filename,256);
        intps = time(NULL);
        tmi = localtime(&intps);
        bzero(filename,256);
        sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
        printf("Crearea fisierului output copiat: %s\n",filename);
        
        if ((fd=open(filename,O_CREAT|O_WRONLY,0600))==-1)
        {
            perror("Deschiderea a esuat.");
            exit (3);
        }
        bzero(buffer,BUFFERT);
        
        n=recv(nsid,buffer,BUFFERT,0);
        while(n) {
            if(n==-1){
                perror("recv a esuat.");
                exit(5);
            }
            if((m=write(fd,buffer,n))==-1){
                perror("scrierea a esuat.");
                exit (6);
            }
            count=count+m;
            bzero(buffer,BUFFERT);
            n=recv(nsid,buffer,BUFFERT,0);
        }
        close(sfd);
        close(fd);
    }
    close(nsid);
    
    printf("Sfarsitul transmisiei: %s.%d\n",dst,clt_port);
    printf("Nr de octeti primiti: %ld \n",count);
    
    return EXIT_SUCCESS;
}

/* Functia care permite crearea unui socket si atasarea acestuia la sistem
 * Returneaza un descriptor de fisier in tabelul descriptorului de proces
 * bind permite definirea sa cu sistemul
 */
int create_server_socket (int port){
    int l;
	int sfd;
    int yes=1;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("eroare socket.");
        return EXIT_SUCCESS;
	}
    	/*SOL_SOCKET : manevreaza optiunile de la nivelul API-ului socket-ului
     *SO_REUSEADDR : pentru repornirea unui server in cazul unei opriri bruste
     * pentru a nu avea erori la crearea socket-ului
     * cazul in care sunt mai multe servere pe ac port
     */
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(int)) == -1 ) {
        perror("Eroare setsocket.");
        exit(5);
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
