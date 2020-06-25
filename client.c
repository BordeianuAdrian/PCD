#include "filetransfer_header.h"

//crearea unui socket inet
int createSocket(int *fd){
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd < 0){
        fprintf(stderr, "Eroare: %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}

//adaugarea unui server nou
void serverAdder(struct sockaddr_in *server, char* pathname, int portNumber){
    memset(server, 0, sizeof(struct sockaddr_in));
    server -> sin_family = AF_INET;
    server -> sin_port = htons(portNumber);
    hostEntry = gethostbyname(pathname);  
    if (hostEntry){
        pptr = (struct in_addr **) hostEntry -> h_addr_list;
        memcpy(&(server -> sin_addr), *pptr, sizeof(struct in_addr));
    }else{
        perror("Eroare");
        exit(-1);
    }
}

//conectarea prin socket
void connectSocket(int serverfd){
    int status = connect(serverfd, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if (status < 0){
        perror("Eroare");
        exit(-1);
    }
}

//verifica daca se trimite comanda
bool readAcknowledgement(int fd, char* buffer){
    readNet(fd, buffer);
    if(buffer[0] == 'E'){
        fprintf(stderr, "%s", buffer+1);
        return false;
    }
    return true;
}

//elibereaza bufferul si iese din client
void exitClient(int fd, char *buffer){
    strcpy(buffer, "Q\n");
    writeNet(fd, buffer);
    if(readAcknowledgement(fd, buffer) == true){
	free(buffer);
        exit(-1);
    }
}


void clientListDir(){
    if( !fork() ){
        int fps[2];
        if( pipe( fps ) ){
            fprintf( stderr, "Eroare: %s\n", strerror(errno) );
            exit(1);
        }
        if( !fork() ){
            close( fps[0] );
            close( STDOUT_FILENO );
            dup  ( fps[1] );
            close( fps[1] );
            execlp( "ls", "ls", "-l", NULL );
            fprintf( stdout, "%s\n", strerror(errno) );
        }else{
            close( fps[1] );
            close( STDIN_FILENO );
            dup  ( fps[0] );
            close( fps[0] );
            execlp( "more", "more", "-20", NULL );
            fprintf( stderr, "%s\n", strerror(errno) );
        }
    }else{
        wait( NULL );
    }
}

//schimbarea directorului 
void changeDir(char* pathname){
    if (access(pathname, R_OK) != F_OK){
        fprintf(stderr, "Eroare la schimbarea directorului:  %s\n", strerror(errno));
        return;
    }
    chdir(pathname);
}

//preia numarul portului
int getPortNumber(int fd, char* buffer){
    strcpy(buffer, "D\n");
    writeNet(fd, buffer);
    readNet(fd, buffer);
    int portNumber = atoi(buffer+1);
    return portNumber;
}

//
void serverListDir( int serverfd, int fd, char* buffer, char* commandBuffer){
    strcpy(buffer, "L\n");
    writeNet(serverfd, buffer);
    readAcknowledgement(serverfd, buffer);
    if (fork()){ //primul parinte
        close(fd);
        wait(NULL);
    }
    else{ //primul fiu
        int pipeFD[2];
        pipe (pipeFD);
        int reader, writer;
        reader = pipeFD[0];
        writer = pipeFD[1];

        if(fork()){ //al doilea parinte
            close(writer);
            dup2(reader, 0);
            execlp("more", "more", "-20", NULL);
            perror("more nu s-a realizat ");
        }
        else{ //al doilea fiu
            close(reader);
            dup2(writer, 1);
	    memset(commandBuffer, 0 , 4096);
            readNetVersion2(fd, commandBuffer);
            printf("%s" , commandBuffer);
            exit(-1);
        }
    }
}


//creaza conexiunea cu datele
void createDataConnection(int* fd, int serverfd, char* buffer, char* argv){
    int portNumber = getPortNumber(serverfd, buffer);
    createSocket(fd);
    serverAdder(&servAddr, argv, portNumber);
    connectSocket(*fd);
}

// schimba directorul sv-ului
void changeServerDir(int serverfd, char* pathname, char* buffer){
    strcpy(buffer, "C");
    strcat(buffer, pathname);
    strcat(buffer, "\n");
    writeNet(serverfd, buffer);
    if((readAcknowledgement(serverfd, buffer) == true)){
        printf("Schimbarea directorului server-ului s-a schimbat cu succes!\n");
    }
}

//preia fisierul 
void getFile(int serverfd, int fd, char* buffer, char* pathname){
    strcpy(buffer, "G");
    strcat(buffer, pathname);
    strcat(buffer, "\n");
    writeNet(serverfd, buffer);
    if((readAcknowledgement(serverfd, buffer) == true)){
        strcpy(buffer, pathname);
        parsePathname(buffer, pathname);
        int filePointer = open(pathname, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
        if (filePointer < 0){
            fprintf(stderr, "Eroare la primirea fisierului:  %s\n", strerror(errno));
            close(fd);
            return;
        }
        readNetFile(fd, filePointer, buffer);
        close(fd);
        close(filePointer);
    }
}

//schimba imaginea
void changeImage(int serverfd, int fd, char* buffer, char* pathname){
    strcpy(buffer, "X");
    strcat(buffer, pathname);
    strcat(buffer, "\n");
    writeNet(serverfd, buffer);
    if((readAcknowledgement(serverfd, buffer) == true)){
    }
}

//plaseaza fisierul
void putFile(int serverfd, int fd, char*buffer, char* pathname, char* argv){
    int filePointer = open(pathname, O_RDONLY, 0644);
    if (filePointer < 0){
        fprintf(stderr, "Eroare la plasarea fisierului:  %s\n" ,strerror(errno));
        return;
    }
    createDataConnection(&fd, serverfd, buffer, argv);
    strcpy(buffer, pathname);
    parsePathname(buffer, pathname);
    strcpy(buffer, "P");
    strcat(buffer, pathname);
    strcat(buffer, "\n");
    writeNet(serverfd, buffer);
    if((readAcknowledgement(serverfd, buffer) == true)){
        writeNetFile(fd,filePointer, buffer);
        close(fd);
        close(filePointer);
    }
}

//arata fisierul preluat
void showFile(int serverfd, int fd, char* buffer, char* pathname){
    strcpy(buffer, "G");
    strcat(buffer, pathname);
    strcat(buffer, "\n");
    writeNet(serverfd, buffer);
    if((readAcknowledgement(serverfd, buffer) == true)){
        if (fork()){ //primul parinte
            wait(NULL);
        }else{ //primul fiu
            int pipeFD[2];
            pipe(pipeFD);
            int reader, writer;
            reader = pipeFD[0];
            writer = pipeFD[1];
            if (fork()){ //al doilea parinte
                close(writer);
                dup2(reader, 0);
                execlp("more", "more", "-20", NULL);
                perror("more nu s-a realizat.");
            }else{ //al doilea fiu
                close(reader);
                dup2(writer, 1);
                    printFile(fd, buffer);
                exit(-1);
            }
        }
        close(fd);
    }
}

int main(int argc, char* argv[]){
    if (argc < 2){
        fprintf(stderr,"Nu s-au dat destule argumente!\n");
        exit(-1);
    }
    int socketfd, datafd;
    createSocket(&socketfd);
    serverAdder(&servAddr,argv[1], MY_PORT_NUMBER);
    connectSocket(socketfd);

    char* commandBuffer = malloc(sizeof(char) * 4096);
    char* command = malloc(sizeof(char) * 4096);
    char* pathname = malloc(sizeof(char) * 4096);

    while (true){
        printf("> ");
        fgets(commandBuffer,255,stdin);
        parse(commandBuffer, command, pathname);

        if (strcmp("exit\n",commandBuffer) == 0){
	        free(command);
    	    free(pathname);
            exitClient(socketfd, commandBuffer);
        }
        else if (strcmp("ls\n", commandBuffer) == 0){
            clientListDir();
        }
        else if(strcmp("rls\n", commandBuffer) == 0){
            createDataConnection(&datafd, socketfd, commandBuffer, argv[1]);
            serverListDir(socketfd, datafd, command, commandBuffer);
        }
        else if( strcmp("cd", command ) == 0 ){
            changeDir(pathname);
        }   
        else if( strcmp("rcd", command) == 0){
            changeServerDir(socketfd, pathname, commandBuffer);
        }   
        else if(strcmp("get", command) == 0){
            createDataConnection(&datafd, socketfd, commandBuffer, argv[1]);
            getFile(socketfd, datafd, commandBuffer, pathname);
        }   
        else if(strcmp("put", command) == 0){
            putFile(socketfd, datafd, commandBuffer, pathname, argv[1]);
        }   
        else if(strcmp("cng", command) == 0){
            createDataConnection(&datafd, socketfd, commandBuffer, argv[1]);
            changeImage(socketfd, datafd, commandBuffer, pathname);
        }   
        else if(strcmp("show", command) == 0){
            createDataConnection(&datafd, socketfd, commandBuffer, argv[1]);
            showFile(socketfd, datafd, commandBuffer, pathname);
        }else{
            fprintf(stderr, "Nu ati introdus o comanda valida!\n");
        }
    }
    free(commandBuffer);
    free(command);
    free(pathname);
    return 1;
}
