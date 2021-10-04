#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "msg_struct.h"

#define TAILLE 1024

int create_client_socket(char *addr_ip, short port) {
    printf("Creating Client socket...\n");

    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(addr_ip, &(addr.sin_addr));

    printf("Client connecting to Server...\n\n");

    int connect_result = connect(client_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (connect_result == -1) { perror("ERROR while Connecting"); exit(1); }

    return client_socket;
}


void send_msg(int sockfd) {

    
    char buf[TAILLE] = "";
    char *buf_support = NULL;
    struct message msgstruct;
    char *element_decoupe = NULL;
    char *le_pseudoo = NULL;
    char *le_salon = NULL;
    char *str_support;

    // select variables
    fd_set readfds;
    int fd_stdin = fileno(stdin);
    int select_result;
    struct timeval tv;

    //sending file variables
    char *file_name = NULL;
    int passed = 0;

    

    memset(&msgstruct, 0, sizeof(struct message));
    if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
        printf("\n[Server] : %s\n", msgstruct.infos);
    }


    while (1) {

        // Cleaning memory
        memset(&msgstruct, 0, sizeof(struct message));
        memset(buf, 0, TAILLE);
        element_decoupe = malloc(256*sizeof(char));

        // putting stdin fd in readfds after cleaning it
        FD_ZERO(&readfds);
        FD_SET(fd_stdin, &readfds);

        // time to wait in scanf
        tv.tv_sec = 9;
        tv.tv_usec = 0;

        // Envoyer au serveur une chaine ecrite au clavier
        if (le_salon != NULL){
            le_salon[strcspn(le_salon,"\n")] = 0;
            printf("\n>>>>[%s]: ", le_salon);
        }
        else{
            printf("\n>>>>: ");
        }
        //printf("\n>>>>: ");
        fflush(stdout);
        select_result = select(fd_stdin + 1, &readfds, NULL, NULL, &tv);


        if (select_result == -1) { // if there is an error with select
            printf("Error while using select\n");
            exit(1);
        }
                
        else if (select_result == 0) { // if nothing was entered or Enter wasn't pressed afr 8s

            // reading what was sent to us
            memset(&msgstruct, 0, sizeof(struct message));

            if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), MSG_DONTWAIT) > 0){

                if( (msgstruct.pld_len == -1) && (strcmp( msgstruct.infos, "") != 0) ){
                    if( (strcmp( msg_type_str[msgstruct.type], "BROADCAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "UNICAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "MULTICAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0) ){
                        
                        if(strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0){
                        	passed++;
	                    	msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
	                    	msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
	                    	printf("\n[%s] want to send you the file: '%s'. Do you accept? [Y/N]\n", msgstruct.nick_sender, msgstruct.infos);
                    	}
                    	else if( (strcmp( msg_type_str[msgstruct.type], "FILE_ACCEPT") == 0) || (strcmp( msg_type_str[msgstruct.type], "FILE_REJECT") == 0) ){
                			msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                			msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
                			printf("\n[%s] %s \n", msgstruct.nick_sender, msgstruct.infos);
                		}
                        else{
	                        msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
	                        printf("\n[%s] : %s\n", msgstruct.nick_sender, msgstruct.infos);
                        }
                    }
                    else{
                        printf("\n[Server] : %s\n", msgstruct.infos);
                    }
                }
            }

        }
                
        else {  // if something was entered and Enter was pressed
                
            if(fgets(buf, 256, stdin) == NULL){
                printf("# Attention # : What you entered is not valid !\n");
            }
            else{ // if fgets worked successfully

            	buf_support = malloc(128*sizeof(char));
            	strcpy(buf_support, buf);


                // if the client sent /quit or /who
                if(strcmp( buf, "/quit\n") == 0){

                    if(le_salon != NULL){
                        le_salon = NULL;
                        memset(&msgstruct, 0, sizeof(struct message));
                        msgstruct.type = MULTICAST_QUIT;
                        msgstruct.pld_len = -1;
                        send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0);
                    }
                    else{
                        printf("Ending the connection...\n");
                        break;
                    }
                }

                else if(strcmp( buf, "/who\n" ) == 0){

                    msgstruct.type = NICKNAME_LIST;

                    if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                        printf("Problem while sending msgstruct\n");
                        break;
                    }
                    //waiting server's reponse
                    memset(&msgstruct, 0, sizeof(struct message));
                    if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                        printf("[Server] : %s\n", msgstruct.infos);
                    }
                }

                else if(strcmp( buf, "/channel_list\n" ) == 0){

                    msgstruct.type = MULTICAST_LIST;

                    if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                        printf("Problem while sending msgstruct\n");
                        break;
                    }
                    //waiting server's reponse
                    memset(&msgstruct, 0, sizeof(struct message));
                    if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                        printf("[Server] : %s\n", msgstruct.infos);
                    }
                }

                else if( (strcmp( buf, "Y\n" ) == 0) && (passed > 0) ){

                    msgstruct.type = FILE_ACCEPT;

                    if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                        printf("Problem while sending msgstruct\n");
                        break;
                    }

                    //waiting server's reponse
                    memset(&msgstruct, 0, sizeof(struct message));
                    if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                        //printf("[Server] : %s\n", msgstruct.infos);
                        msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                		msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
                		printf("\n[%s] %s \n", msgstruct.nick_sender, msgstruct.infos);
                    }
                }

                else if( (strcmp( buf, "N\n" ) == 0) && (passed > 0) ){

                    msgstruct.type = FILE_REJECT;
                    //strncpy(msgstruct.infos, " cancelled file transfer.", sizeof(msgstruct.infos) - 1);

                    if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                        printf("Problem while sending msgstruct\n");
                        break;
                    }

                    //waiting server's reponse
                    memset(&msgstruct, 0, sizeof(struct message));
                    if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                        //printf("[Server] : %s\n", msgstruct.infos);
                        msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                		msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
                		printf("\n[%s] %s \n", msgstruct.nick_sender, msgstruct.infos);
                    }
                }

                else { // if the client wrote /msgtype+arg or something else

                    element_decoupe = strtok(buf," ");

                    // Filling msgstruct
                    if(strncmp( element_decoupe, "/nick", sizeof("/nick") - 1) == 0){

                        msgstruct.type = NICKNAME_NEW;
                        // cut /nick and only take the pseudo
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            element_decoupe = strtok(NULL," "); // NULL if nothing after pseudo
                            if(element_decoupe != NULL){// if pseudo contains space
                                printf("# Attention # : Your pseudo is not valid !\n");
                            }
                            else{ // if valid pseudo

                                if(le_pseudoo != NULL){
                                    strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                                }

                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct\n");
                                    break;
                                }

                                le_pseudoo = malloc(128*sizeof(char));
                                strncpy(le_pseudoo, msgstruct.infos, sizeof(le_pseudoo) - 1);

                                //waiting server's reponse
                                memset(&msgstruct, 0, sizeof(struct message));
                                if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                                    printf("[Server] : %s\n", msgstruct.infos);
                                }
                            }
                        }

                        else{
                            printf("# Attention # : Please specify a pseudo!\n");
                        }
                    }

                    else if(strncmp( element_decoupe, "/whois", sizeof("/whois") - 1) == 0){

                        msgstruct.type = NICKNAME_INFOS;
                        // cut /whois and only take the pseudo
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);


                            element_decoupe = strtok(NULL," "); // NULL if nothing after pseudo
                            if(element_decoupe != NULL){// if pseudo contains space
                                printf("# Attention # : The pseudo is not valid !\n");
                            }
                            else{ // if valid pseudo

                                if(le_pseudoo != NULL){
                                    strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                                }

                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for whois\n");
                                    break;
                                }


                                //waiting server's reponse
                                memset(&msgstruct, 0, sizeof(struct message));
                                if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                                    printf("[Server] : %s\n", msgstruct.infos);
                                }
                            }
                        }

                        else{
                            printf("# Attention # : Please specify a pseudo!\n");
                        }
                    }

                    else if(strncmp( element_decoupe, "/msgall", sizeof("/msgall") - 1) == 0){

                        msgstruct.type = BROADCAST_SEND;
                        // cut /msgall and only take the msg
                        element_decoupe = strtok(NULL,"");
                        if(element_decoupe != NULL){

                            // sending first msg with pld_len
                            msgstruct.pld_len = sizeof(element_decoupe);
                            if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("Problem while sending msgstruct for msg1\n");
                                break;
                            }

                            // sending second msg with the mssg
                            msgstruct.pld_len = -1;

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("Problem while sending msgstruct for msgall\n");
                                break;
                            }

                        }

                        else{
                            printf("# Attention # : Please enter a msg!\n");
                        }
                    }

                    else if(strncmp( element_decoupe, "/msg", sizeof("/msg") - 1) == 0){

                        msgstruct.type = UNICAST_SEND;
                        if(le_pseudoo != NULL){
                            strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                        }

                        // cut /msg and only take the pseudo
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            // cut /msg and pseudo and only take the msg
                            element_decoupe = strtok(NULL,"");
                            if(element_decoupe != NULL){

                                // sending first msg with pld_len
                                msgstruct.pld_len = sizeof(element_decoupe);
                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for msg1\n");
                                    break;
                                }

                                // sending second msg with the mssg
                                strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);
                                msgstruct.pld_len = -1;
                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for msg1\n");
                                    break;
                                }
                            }
                            else{
                                printf("# Attention # : Please enter a msg!\n");
                            }
                        }

                        else{
                            printf("# Attention # : Please enter a pseudo!\n");
                        }
                    }

                    else if(strncmp( element_decoupe, "/create", sizeof("/create") - 1) == 0){

                        msgstruct.type = MULTICAST_CREATE;
                        // cut /create and only take the channel_name
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            str_support = malloc(128*sizeof(char));
                            strncpy(str_support, msgstruct.infos, sizeof(le_salon) - 1);

                            element_decoupe = strtok(NULL," "); // NULL if nothing after channel_name
                            if(element_decoupe != NULL){// if channel_name contains space
                                printf("# Attention # : Your channel_name is not valid !\n");
                            }
                            else{ // if valid channel_name

                                if(le_pseudoo != NULL){
                                    strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                                }

                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for multicast create\n");
                                    break;
                                }

                                //waiting server's reponse
                                memset(&msgstruct, 0, sizeof(struct message));
                                if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                                    if(msgstruct.pld_len == -1){
                                        le_salon = malloc(128*sizeof(char));
                                        strncpy(le_salon, str_support, sizeof(le_salon) - 1);
                                    }
                                    printf("[Server] : %s\n", msgstruct.infos);
                                }
                            }
                        }
                    }

                    else if(strncmp( element_decoupe, "/join", sizeof("/join") - 1) == 0){

                        msgstruct.type = MULTICAST_JOIN;
                        // cut /join and only take the channel_name
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            str_support = malloc(128*sizeof(char));
                            strncpy(str_support, msgstruct.infos, sizeof(le_salon) - 1);

                            element_decoupe = strtok(NULL," "); // NULL if nothing after channel_name
                            if(element_decoupe != NULL){// if channel_name contains space
                                printf("# Attention # : channel_name is not valid !\n");
                            }
                            else{ // if valid channel_name

                                if(le_pseudoo != NULL){
                                    strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                                }

                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for multicast create\n");
                                    break;
                                }

                                //waiting server's reponse
                                memset(&msgstruct, 0, sizeof(struct message));
                                if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) > 0){
                                    if(msgstruct.pld_len == -1){
                                        le_salon = malloc(128*sizeof(char));
                                        strncpy(le_salon, msgstruct.infos, sizeof(le_salon) - 1);
                                    }
                                    printf("[Server] : %s\n", msgstruct.infos);
                                }
                            }
                        }
                    }

                    else if(strncmp( element_decoupe, "/send", sizeof("/send") - 1) == 0){

                        msgstruct.type = FILE_REQUEST;
                        if(le_pseudoo != NULL){
                            strncpy(msgstruct.nick_sender, le_pseudoo, sizeof(msgstruct.nick_sender) - 1);
                        }

                        // cut /send and only take the pseudo
                        element_decoupe = strtok(NULL," ");
                        if(element_decoupe != NULL){

                            strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);

                            // cut /send and pseudo and only take the filename
                            element_decoupe = strtok(NULL,"");
                            if(element_decoupe != NULL){

                                // sending first msg with pld_len
                                msgstruct.pld_len = sizeof(element_decoupe);
                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for send1\n");
                                    break;
                                }

                                // sending second msg with the mssg
                                file_name = malloc(128*sizeof(char));
                                strcpy(file_name, element_decoupe);

                                strncpy(msgstruct.infos, element_decoupe, sizeof(msgstruct.infos) - 1);
                                msgstruct.pld_len = -1;
                                if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct for send2\n");
                                    break;
                                }

                            }
                            else{
                                printf("# Attention # : Please enter a filename!\n");
                            }
                        }

                        else{
                            printf("# Attention # : Please enter a pseudo!\n");
                        }
                    }

                    else {  // utilisation d'ECHO SEND

                        memset(&msgstruct, 0, sizeof(struct message));

                        if(le_salon != NULL){

                            // sending first msg with pld_len
                            msgstruct.type = MULTICAST_SEND;
                            msgstruct.pld_len = sizeof(buf);
                            strcpy(msgstruct.infos, le_salon);
                            send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            // sending second msg with the mssg
                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = MULTICAST_SEND;
                            msgstruct.pld_len = -1;
                            strcpy(msgstruct.infos, buf_support);

                            if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("Problem while sending msgstruct for echo\n");
                                break;
                            }
                        }
                        else{
                        
                            
                            msgstruct.type = ECHO_SEND;

                            // sending first msg with pld_len
                            msgstruct.pld_len = sizeof(buf_support);
                            send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            // sending second msg with the mssg
                            msgstruct.pld_len = -1;
                            strcpy(msgstruct.infos, buf_support);

                            if (send(sockfd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("Problem while sending msgstruct for echo\n");
                                break;
                            }

                        }

                    }
                }
            }
        }


        // reading what was sent to us
        memset(&msgstruct, 0, sizeof(struct message));
        if(recv(sockfd, (void*)&msgstruct, sizeof(msgstruct), MSG_DONTWAIT) > 0){

            if( (msgstruct.pld_len == -1) && (strcmp( msgstruct.infos, "") != 0) ){
                if( (strcmp( msg_type_str[msgstruct.type], "BROADCAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "UNICAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "MULTICAST_SEND") == 0) || (strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0) ){
                    
                    if(strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0){
                    	passed++;
                    	msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                    	msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
                    	printf("\n[%s] want to send you the file: '%s'. Do you accept? [Y/N]\n", msgstruct.nick_sender, msgstruct.infos);

                    }
                    else{
                    	msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                    	printf("\n[%s] : %s\n", msgstruct.nick_sender, msgstruct.infos);
                    }
                }
                else if( (strcmp( msg_type_str[msgstruct.type], "FILE_ACCEPT") == 0) || (strcmp( msg_type_str[msgstruct.type], "FILE_REJECT") == 0) ){
                	msgstruct.nick_sender[strcspn(msgstruct.nick_sender,"\n")] = 0;
                	msgstruct.infos[strcspn(msgstruct.infos,"\n")] = 0;
                	printf("\n[%s] %s \n", msgstruct.nick_sender, msgstruct.infos);
                }
                else{
                    printf("\n[Server] : %s\n", msgstruct.infos);
                }
            }
        }

        //printf("\n        pld_len: %i\n      nick_sender: %s\n       type: %s\n      infos: %s\n\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

    }




    if(element_decoupe != NULL){
    	free(element_decoupe);
    }
    if(le_pseudoo != NULL){
    	free(le_pseudoo);
    }
    if(le_salon != NULL){
    	free(le_salon);
    }
    if(str_support != NULL){
    	free(str_support);
    }
    if(buf_support != NULL){
    	free(buf_support);
    }
    if(file_name != NULL){
    	free(file_name);
    }
}

int main(int argc, char *argv[]) {

	if ( argc != 3 ){
		printf("Usage : pgm ip_addr port_number\n");
		return 0;
	}

    char *addr_ip = argv[1];
    char *str_port = argv[2];
    int port = atoi(str_port);

    //char  *addr_ip = "127.0.0.1";
    //short port = 9090;

    int client_socket = create_client_socket(addr_ip, port);

    send_msg(client_socket);

    close(client_socket);

    return 0;
}