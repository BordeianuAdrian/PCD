#include "filetransfer_header.h"
#include </usr/include/python2.7/Python.h>

int createSocket()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "Eroare: %s\n", strerror(errno));
        exit(-1);
    }
    return fd;
}

//Legare
void socketBinding(int fd, struct sockaddr_in *server, int portNumber)
{
    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(portNumber);
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    if ((bind(fd, (struct sockaddr *)server, sizeof(servAddr)) < 0))
    {
        perror("Legare esuata");
        exit(-1);
    }
}
//primirea numelui de host al clientului si returnarea acestuia
char *getHostName(struct sockaddr_in *client, char *buffer)
{
    hostEntry = gethostbyaddr(&(client->sin_addr), sizeof(struct in_addr), AF_INET);
    if (hostEntry == NULL)
    {
        buffer = "Host necunoscut";
    }
    else
    {
        buffer = hostEntry->h_name;
    }
    return buffer;
}
//Acceptarea conexiunii cu clientul
int acceptConnection(int fd, struct sockaddr_in *client)
{
    int length = sizeof(struct sockaddr_in);
    fd = accept(fd, (struct sockaddr *)client, (socklen_t *)&length);
    if (fd < 0)
    {
        fprintf(stderr, "Eroare: %s\n", strerror(errno));
        exit(-1);
    }
    return fd;
}

//printarea informatiilor clientului dupa conectare
void printConnectionInfo(char *buffer)
{
    buffer = getHostName(&clientAddr, buffer);
    printf("Child %d: Numele clientului este %s\n", getpid(), buffer);
    printf("Child %d: Adresa IP a client este %s\n", getpid(), inet_ntoa(clientAddr.sin_addr));
    printf("Child %d: Acceptarea conexiunii de la host %s\n", getpid(), getHostName(&clientAddr, buffer));
}
// oprirea copilului si trimiterea confirmatii catre client cand se termina
void exitClient(int fd, char *buffer)
{
    sendAcknowledgement(fd, buffer);
    printf("Copil %d: Iesire ...\n", getpid());
    free(buffer);
    exit(-1);
}
//construirea unui numar de port la intamplare pentru conexiunea de date
int makePortNumber(int fd, struct sockaddr_in *dataAddr)
{
    int length = sizeof(struct sockaddr_in);
    int status = getsockname(fd, (struct sockaddr *)dataAddr, (socklen_t *)&length);
    if (status < 0)
    {
        fprintf(stderr, "getsockname esuat %s\n", strerror(errno));
        return -1;
    }
    return (ntohs(dataAddr->sin_port)); //utilizarea nthos pentru a fi siguri ca numarul de port este in ordinea corecta
}
//trimiterea unui confirmari cu numarul portului
void sendPortNumber(int fd, char *buffer, int portNumber)
{
    buffer[0] = 'A';
    sprintf(buffer + 1, "%d\n", portNumber);
    writeNet(fd, buffer);
}
//schimbarea pozitiei directorului din server
void changeDir(int fd, char *pathname, char *buffer)
{
    int status = chdir(pathname);
    if (status == 0)
    {
        sendAcknowledgement(fd, buffer);
    }
    else
    {
        sendError(fd, buffer);
    }
}
//Aceasta functie este apelata cand clientul cere sa primeasca un fisier de la server.
void sendFile(int client, int fd, char *buffer, char *pathname)
{
    int filePointer = open(pathname, O_RDONLY, 0644);
    if (filePointer < 0)
    {
        sendError(client, buffer);
        close(fd);
        return;
    }
    sendAcknowledgement(client, buffer);
    writeNetFile(fd, filePointer, buffer);
    close(fd);
    close(filePointer);
}
//Aceasta functie este apelata cand clientul cere sa puna un fisier pe server
void getFile(int client, int fd, char *buffer, char *pathname)
{
    int filePointer = open(pathname, O_WRONLY | O_TRUNC | O_EXCL | O_CREAT, 0644);
    if (filePointer < 0)
    {
        sendError(fd, buffer);
        return;
    }
    sendAcknowledgement(client, buffer);
    readNetFile(fd, filePointer, buffer);
    close(fd);
    close(filePointer);
}

int main()
{
    int dataSocketfd, datafd;
    int listenfd = createSocket();                       //crearea unui socket pentru conexiunea la client
    socketBinding(listenfd, &servAdder, MY_PORT_NUMBER); //bind

    if (listen(listenfd, 4) == -1)
    { //asculta pana la maxim 4 conexiuni
        fprintf(stderr, "Eroare: %s\n", strerror(errno));
        return -1;
    }

    while (true)
    {
        int connectfd = acceptConnection(listenfd, &clientAddr); //accepta conexiunea pe care o ascultam
        int n;
        if (fork())
        {
            close(connectfd);
            waitpid(-1, NULL, WNOHANG);
        }
        else
        {
            if (connectfd < 0)
            {
                fprintf(stderr, "Eroare: %s\n", strerror(errno));
                exit(-1);
            }
            else
            {
                char *buffer = malloc(sizeof(char) * 4096);
                char *pathname = malloc(sizeof(char) * 4096);
                printConnectionInfo(buffer);
                struct timeval tv;
                tv.tv_sec = 3600;
                tv.tv_usec = 0;
                setsockopt(connectfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

                while (true)
                {
                    n = readNet(connectfd, buffer);
                    if (n <= 0)
                    {
                        fprintf(stderr, "Conexiunea cu clientul a fost pierduta\n");
                        free(buffer);
                        free(pathname);
                        exit(-1);
                    }
                    if (strcmp(buffer, "Q\n") == 0)
                    {
                        free(pathname);
                        exitClient(connectfd, buffer);
                    }
                    if (strcmp("D\n", buffer) == 0)
                    {
                        dataSocketfd = createSocket();
                        socketBinding(dataSocketfd, &servAdder, 0);
                        if (listen(dataSocketfd, 4) == -1)
                        {
                            fprintf(stderr, "Eroare: %s\n", strerror(errno));
                            return -1;
                        }
                        int portNumber = makePortNumber(dataSocketfd, &dataAddr);
                        sendPortNumber(connectfd, buffer, portNumber);
                        datafd = acceptConnection(dataSocketfd, &clientAddr);
                    }
                    if (strcmp("L\n", buffer) == 0)
                    {
                        strcpy(buffer, "A\n");
                        writeNet(connectfd, buffer);
                        if (fork())
                        {
                            close(datafd);
                            wait(NULL);
                        }
                        else
                        {
                            dup2(datafd, 1);
                            free(buffer);
                            free(pathname);
                            execlp("ls", "ls", "-l", NULL);
                        }
                    }

                    if (buffer[0] == 'C')
                    {
                        strcpy(pathname, buffer + 1);
                        removeNewLine(pathname);
                        changeDir(connectfd, pathname, buffer);
                    }
                    if (buffer[0] == 'G')
                    {
                        strcpy(pathname, buffer + 1);
                        removeNewLine(pathname);
                        sendFile(connectfd, datafd, buffer, pathname);
                    }
                    if (buffer[0] == 'X')
                    {
                        strcpy(pathname, buffer + 1);
                        printf("Comanda modificare imagine: %s",pathname);

                        enum { kMaxArgs = 64 };
                        int argc = 0;
                        char *argv[kMaxArgs];

                        char *p2 = strtok(pathname, " ");
                        while (p2 && argc < kMaxArgs-1)
                          {
                            argv[argc++] = p2;
                            p2 = strtok(0, " ");
                          }
                        argv[argc] = 0;

                        char filename[] = "p.py";
                        FILE* file;
                        Py_SetProgramName(pathname);
                        Py_Initialize();
                        PySys_SetArgv(argc, argv);
                        file = fopen(filename, "r");
                        PyRun_SimpleFile(file, "p.py");
                        Py_Finalize();

                        sendAcknowledgement(connectfd, buffer);
                    }
                    if (buffer[0] == 'P')
                    {
                        strcpy(pathname, buffer + 1);
                        removeNewLine(pathname);
                        remove(pathname);
                        getFile(connectfd, datafd, buffer, pathname);
                    }
                }
            }
            exit(-1);
        }
    }
    return 1;
}
