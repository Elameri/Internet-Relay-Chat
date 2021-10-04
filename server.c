#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>


#include "msg_struct.h"


#define TAILLE 1024
#define MAX_CLIENTS 10

// Linked List functions
struct clients_linkedlist {
    int client_socket_fd;
    char *pseudo;
    char *tiime;
    char *salon;
    struct sockaddr_in *addr_of_client;
    struct clients_linkedlist *next;
};

struct clients_linkedlist init(struct clients_linkedlist clnt, int clnt_fd, struct sockaddr_in *sa){
    clnt.client_socket_fd = clnt_fd;

    char bufff[20];
    time_t t = time(NULL);
    strftime(bufff, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));

    clnt.tiime = malloc(40*sizeof(char));
    strcpy(clnt.tiime, bufff);

    //printf("\nThis program has been writeen at: %s \n", clnt.tiime);
    

    clnt.pseudo = NULL;
    clnt.salon = NULL;
    //clnt.pseudo = malloc(128*sizeof(char));
    clnt.addr_of_client = sa;
    clnt.next = NULL;
    return clnt;
}

void ajout(struct clients_linkedlist *clnt, struct clients_linkedlist *new){

    struct clients_linkedlist *tmp = malloc(sizeof(struct clients_linkedlist));
    tmp = clnt;
    while ((*tmp).next != NULL){
        tmp = (*tmp).next;
    }
    (*tmp).next = malloc(sizeof(struct clients_linkedlist));
    (*tmp).next = new; 

}

void print_list(struct clients_linkedlist clnt){

    struct clients_linkedlist *tmp2 = malloc(sizeof(struct clients_linkedlist));
    tmp2 = &clnt;
    while ((*tmp2).next != NULL){
        printf("sockfd is : %i and Pseudo : %s\n", (*tmp2).client_socket_fd, (*tmp2).pseudo);
        tmp2 = (*tmp2).next;

    }
    printf("sockfd is : %i and Pseudo : %s\n", (*tmp2).client_socket_fd, (*tmp2).pseudo);

    if(tmp2 != NULL){
        free(tmp2);
    }
}

int len(struct clients_linkedlist clnt){
    int count = 0;
    struct clients_linkedlist *tmp0 = malloc(sizeof(struct clients_linkedlist));
    tmp0 = &clnt;
    while ((*tmp0).next != NULL){
        count++;
        tmp0 = (*tmp0).next;

    }
    count++;
    return count;

    if(tmp0 != NULL){
        free(tmp0);
    }

}


char* list_pseudo(struct clients_linkedlist *clnt){

    char *liste = malloc(1024*sizeof(char));
    strcpy(liste, "Online users are : \n");

    struct clients_linkedlist *tmp01 = malloc(sizeof(struct clients_linkedlist));
    tmp01 = clnt;

    while ((*tmp01).next != NULL){
        
        if( ((*tmp01).next)->pseudo != NULL ){
            strcat(liste, "\n\t - ");
            strcat(liste, ((*tmp01).next)->pseudo);
        }
        else {
            strcat(liste, "\n\t - no pseudo\n");
        }
        tmp01 = (*tmp01).next;

    }

    return liste;

    if(tmp01 != NULL){
        free(tmp01);
    }

}

char* infos_pseudo(struct clients_linkedlist *clnt, char *psd){

    char *resultat = malloc(1024*sizeof(char));
    strcpy(resultat, psd);
    strcat(resultat, " connected since ");

    struct clients_linkedlist *tmp02 = malloc(sizeof(struct clients_linkedlist));
    tmp02 = clnt;

    while ((*tmp02).next != NULL){
        
        if( ((*tmp02).next)->pseudo != NULL ){
            if( strcmp( ((*tmp02).next)->pseudo, psd) == 0 ){
                strcat(resultat, ((*tmp02).next)->tiime);
                strcat(resultat, " with IP address ");
                strcat(resultat, inet_ntoa((((*tmp02).next)->addr_of_client)->sin_addr));
                strcat(resultat, " and port number ");

                //int iip = ntohs((((*tmp02).next)->addr_of_client)->sin_port);
                char str[12];
                sprintf(str, "%d", ntohs((((*tmp02).next)->addr_of_client)->sin_port));
                strcat(resultat, str);
                strcat(resultat, "\n");

            }
        }
        tmp02 = (*tmp02).next;
    }

    return resultat;

    if(tmp02 != NULL){
        free(tmp02);
    }

}

int* liste_fd_broad(struct clients_linkedlist *clnt, int le_fd){

    int *result = malloc(11*sizeof(int));
    memset(result, 0, 11);

    struct clients_linkedlist *tmp03 = malloc(sizeof(struct clients_linkedlist));
    tmp03 = clnt;

    int i = 0;
    int count = 0;

    printf("le_fd = %i \n", le_fd);

    while ((*tmp03).next != NULL){
        i++;
        printf("client_socket_fd[%i] = %i \n", i, ((*tmp03).next)->client_socket_fd);
        if( ((*tmp03).next)->client_socket_fd != le_fd ){
            count++;
            result[count] = ((*tmp03).next)->client_socket_fd;
            printf("result[%i] = %i \n", count, result[count]);
        }

        tmp03 = (*tmp03).next;
    }

    result[0] = count;

    return result;

    if(tmp03 != NULL){
        free(tmp03);
    }
}

void ajout_nom(struct clients_linkedlist *clnt, int fd, char *psd){

    struct clients_linkedlist *tmp1 = malloc(sizeof(struct clients_linkedlist));
    tmp1 = clnt;
    while ( (*tmp1).next != NULL ){

        if( (*tmp1).client_socket_fd == fd ){

            (*tmp1).pseudo = malloc(128*sizeof(char));
            strcpy((*tmp1).pseudo, psd);

        }
        
        tmp1 = (*tmp1).next;

    }

    if( (*tmp1).client_socket_fd == fd ){

        (*tmp1).pseudo = malloc(128*sizeof(char));
        strcpy((*tmp1).pseudo, psd);
    }

}

void ajout_channel(struct clients_linkedlist *clnt, int fd, char *channel){

    struct clients_linkedlist *tmp1 = malloc(sizeof(struct clients_linkedlist));
    tmp1 = clnt;
    while ( (*tmp1).next != NULL ){

        if( (*tmp1).client_socket_fd == fd ){

            (*tmp1).salon = malloc(128*sizeof(char));
            strcpy((*tmp1).salon, channel);

        }
        
        tmp1 = (*tmp1).next;

    }

    if( (*tmp1).client_socket_fd == fd ){

        (*tmp1).salon = malloc(128*sizeof(char));
        strcpy((*tmp1).salon, channel);
    }

}

void suppr_channel(struct clients_linkedlist *clnt, int fd){

    struct clients_linkedlist *tmp1 = malloc(sizeof(struct clients_linkedlist));
    tmp1 = clnt;
    while ( (*tmp1).next != NULL ){

        if( (*tmp1).client_socket_fd == fd ){

            (*tmp1).salon = NULL;

        }
        
        tmp1 = (*tmp1).next;

    }

    if( (*tmp1).client_socket_fd == fd ){

        (*tmp1).salon = NULL;
    }

}


int is_name_there(struct clients_linkedlist *clnt, char* psd){
    
    struct clients_linkedlist *tmp3 = malloc(sizeof(struct clients_linkedlist));
    tmp3 = clnt;

    while ((*tmp3).next != NULL){

        if( strcmp( (*tmp3).pseudo, psd) == 0 ){ return 1; }

        tmp3 = (*tmp3).next;
    }


    if((*tmp3).pseudo != NULL){

        if( strcmp( (*tmp3).pseudo, psd) == 0 ){ return 1; }   
    }

    return -1;

    if(tmp3 != NULL){
        free(tmp3);
    }

}

int is_channel_there(struct clients_linkedlist *clnt, char* channel){
    
    struct clients_linkedlist *tmp3 = malloc(sizeof(struct clients_linkedlist));
    tmp3 = clnt;

    while ((*tmp3).next != NULL){

        if((*tmp3).salon != NULL){
            if( strcmp( (*tmp3).salon, channel) == 0 ){ return 1; }
        }

        tmp3 = (*tmp3).next;
    }


    if((*tmp3).salon != NULL){

        if( strcmp( (*tmp3).salon, channel) == 0 ){ return 1; }   
    }

    return -1;

    if(tmp3 != NULL){
        free(tmp3);
    }

}


char* pseudo_of_fd(struct clients_linkedlist *clnt, int fd){

    struct clients_linkedlist *tmp1 = malloc(sizeof(struct clients_linkedlist));
    tmp1 = clnt;
    char *psd = malloc(128*sizeof(char));

    while ( (*tmp1).next != NULL ){

        if( (*tmp1).client_socket_fd == fd ){

            if( (*tmp1).pseudo != NULL ){
                strcpy(psd, (*tmp1).pseudo);
            }
            else{
                strcpy(psd, "no_pseudo");
            }

            return psd;

        }
        
        tmp1 = (*tmp1).next;

    }

    if( (*tmp1).client_socket_fd == fd ){


            if( (*tmp1).pseudo != NULL ){
                strcpy(psd, (*tmp1).pseudo);
            }
            else{
                strcpy(psd, "no_pseudo");
            }

            return psd;

    }

    strcpy(psd, "no_pseudo_for_this_fd");
    return psd;

}

int fd_of_pseudo(struct clients_linkedlist *clnt, char* psd){
    
    struct clients_linkedlist *tmp3 = malloc(sizeof(struct clients_linkedlist));
    tmp3 = clnt;

    //printf("psd entered is %s\n", psd);

    while ((*tmp3).next != NULL){

        //printf("a psd is %s\n", (*tmp3).pseudo);

        if( strncmp( (*tmp3).pseudo, psd, strlen(psd)) == 0 ){
            printf("pseudo found\n");
            return (*tmp3).client_socket_fd;
        }

        tmp3 = (*tmp3).next;
    }


    if((*tmp3).pseudo != NULL){

        //printf("a psd is %s\n", (*tmp3).pseudo);

        if( strncmp( (*tmp3).pseudo, psd, strlen(psd)) == 0 ){
            printf("pseudo found2\n");
            return (*tmp3).client_socket_fd;
        }   
    }

    return -1;

}

char* list_salon(char **tab_salon, int i_salon){

    char *liste = malloc(1024*sizeof(char));
    strcpy(liste, "Existing channels are : \n");

    int i;
    for(i=0; i<i_salon; i++) {

        if(tab_salon[i] != NULL){
    
            strcat(liste, "\n\t - ");
            printf(" tab sal 0 : %s\n", tab_salon[i]);
            strcat(liste, tab_salon[i]);
        }
    
    }

    return liste;
}

void update_channels(struct clients_linkedlist *clnt, char **tab_salon, int i_salon){
    
    int count = 0;
    int i;
    for(i=0; i<i_salon; i++){
        if(tab_salon[i] != NULL){

            struct clients_linkedlist *tmp3 = malloc(sizeof(struct clients_linkedlist));
            tmp3 = clnt;


            count = 0;

            while ((*tmp3).next != NULL){

                if((*tmp3).salon != NULL){
                    if( strcmp( (*tmp3).salon, tab_salon[i]) == 0 ){
                        count++;
                        break;
                    }
                }

                tmp3 = (*tmp3).next;
            }

            if((*tmp3).salon != NULL){

                if( strcmp( (*tmp3).salon, tab_salon[i]) == 0 ){
                    count++;
                    break;
                } 
            }


            if(count == 0){
                tab_salon[i] = NULL;
            }
        }
    }
}


int* fds_with_channel(struct clients_linkedlist *clnt, char* channel){

    int *result = malloc(11*sizeof(int));
    memset(result, 0, 11);
    
    struct clients_linkedlist *tmp03 = malloc(sizeof(struct clients_linkedlist));
    tmp03 = clnt;

    int count = 0;

    char *strr_support;
    strr_support = malloc(128*sizeof(char));

    while ((*tmp03).next != NULL){

        if( ((*tmp03).next)->salon != NULL ){

            strr_support = malloc(128*sizeof(char));
            strcpy(strr_support, ((*tmp03).next)->salon);
            strr_support[strcspn(strr_support,"\n")] = 0;

            //if( strncmp( ((*tmp03).next)->salon, channel, sizeof(channel) - 1) == 0 ){
            if( strcmp( strr_support, channel) == 0 ){
                count++;
                result[count] = ((*tmp03).next)->client_socket_fd;
            }
        }
        tmp03 = (*tmp03).next;
    }

    result[0] = count;

    return result;

}


// Server functions

int create_server_socket(char *addr_ip, short port) {
    printf("Creating Server socket...\n");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(addr_ip, &(addr.sin_addr));

    printf("Binding...\n");
    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1) { perror("ERROR while binding"); exit(1); }

    printf("Server Listening...\n\n");
    int listenResult = listen(server_socket, 10);
    if (listenResult == -1) { perror("ERROR while Listening"); exit(1); }

    return server_socket;
}

int wait_client(int server_socket) {
    struct message msgstruct;

    // Cleaning memory
    memset(&msgstruct, 0, sizeof(struct message));

    // Initialising the LinkedList, and putting 1st node to 0
    struct sockaddr_in empty_addr;
    memset(&empty_addr, '\0', sizeof(empty_addr));

    struct clients_linkedlist *clients = malloc(sizeof(struct clients_linkedlist));
    *clients = init(*clients, 0, &empty_addr);

    struct clients_linkedlist *a_client;

    ajout_nom(clients, 0, "None");


    // Poll
    struct pollfd pollfds[MAX_CLIENTS + 1];
    memset(pollfds, 0, (MAX_CLIENTS + 1)*sizeof(struct pollfd));
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN;
    int numberClients = 0;


    int bufSize;

    char *nicklist;
    // FREE IS OBLIGATORY at the end
    char *str_support ;
    char *unicast_support ;
    int *int_support ;
    int i_sup = 0;

    char **tab_salon = malloc(20*sizeof(char*));
    int i_salon = 0;
    //tab_salon[i_salon]

    // file sending support
    char *file_receiver = NULL;
    char *file_name = NULL;
    char *file_sender = NULL;


    while (1){

        int pollResult = poll(pollfds, numberClients + 1, -1);
        int i=0;

        if (pollResult > 0){

            if (pollfds[0].revents & POLLIN){

                //Accept
                struct sockaddr_in client_addr;
                socklen_t size_addr = sizeof(struct sockaddr_in);
                int client_fd = accept(server_socket, (struct sockaddr *)&client_addr, &size_addr);
                if(client_fd == 1) { perror("ERROR while accepting"); exit(1); }
                printf("Connect Accepted %s\n", inet_ntoa(client_addr.sin_addr));

                //Adding Client Information to the Linked List
                a_client = malloc(sizeof(struct clients_linkedlist));
                *a_client = init(*a_client, client_fd, &client_addr);
                ajout(clients, a_client);
                //free(a_client);
                // Adding the client fd to pollfd

                for (i = 1; i < MAX_CLIENTS; i++){

                    memset(&msgstruct, 0, sizeof(struct message));
                    msgstruct.type = NOTHING;

                    if (pollfds[i].fd == 0){
                        pollfds[i].fd = client_fd;
                        pollfds[i].events = POLLIN;
                        numberClients++;

                        strncpy(msgstruct.infos, "please login with: /nick <your_pseudo>", sizeof(msgstruct.infos) - 1);
                        if (send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                            printf("Problem while login demand\n");
                            break;
                        } 
                        break;
                    }
                }
                printf("=====> Number of Clients : %d\n\n", numberClients);
            }




            for (i = 1; i < MAX_CLIENTS; i++) {

                memset(&msgstruct, 0, sizeof(struct message));
                msgstruct.type = NOTHING;

                if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN) {

                    memset(&msgstruct, 0, sizeof(struct message));
                    msgstruct.type = NOTHING;


                    bufSize = recv(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                    //printf("msg type in struct : %s\n", msg_type_str[msgstruct.type]);


                    if (bufSize == -1) { // si la lecture sur pollfds[i] echoue
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        numberClients--;
                    }

                    else if (bufSize == 0) { // si y a rien a lire
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        numberClients--;
                    }


                    else {  // si on a bien recu un msg du client

                        printf("\nMSG RECEIVED : %s, contains : %s from fd %i\n", msg_type_str[msgstruct.type], msgstruct.infos, pollfds[i].fd);

                        if(strcmp( msg_type_str[msgstruct.type], "NICKNAME_NEW") == 0){

                            //int is_name = is_name_there(clients, infos_ptr);

                            if(is_name_there(clients, msgstruct.infos) == -1){

                                //printf("Name not in the list\n\n");

                                printf("Welcome : %s\n", msgstruct.infos);
   
                                ajout_nom(clients, pollfds[i].fd, msgstruct.infos);


                                str_support = malloc(138*sizeof(char));
                                strcpy(str_support, "Welcome ");
                                strcat(str_support, msgstruct.infos);

                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = NICKNAME_NEW;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                                //free(str_support);
                                
                            }
                            else{

                                //printf("bla name in the list, name is %s and list :\n", msgstruct.infos);

                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = NICKNAME_NEW;

                                strncpy(msgstruct.infos, "pseudo already used please choose another one", sizeof(msgstruct.infos) - 1);
                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            }
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "NICKNAME_LIST") == 0){

                            memset(&msgstruct, 0, sizeof(struct message));

                            msgstruct.type = NICKNAME_LIST;

                            nicklist = malloc(1024*sizeof(char));
                            nicklist = list_pseudo(clients);

                            // sending first msg with pld_len
                            //msgstruct.pld_len = sizeof(nicklist);
                            //send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            // sending second msg with the mssg
                            msgstruct.pld_len = -1;
                            strncpy(msgstruct.infos, nicklist, sizeof(msgstruct.infos) - 1);
                            send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "NICKNAME_INFOS") == 0){
                            
                            if(is_name_there(clients, msgstruct.infos) != -1){

                                str_support = malloc(138*sizeof(char));
                                str_support = infos_pseudo(clients, msgstruct.infos);

                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = NICKNAME_INFOS;

                                // sending first msg with pld_len
                                //msgstruct.pld_len = sizeof(str_support);
                                //send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                                // sending second msg with the mssg
                                msgstruct.pld_len = -1;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);
                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            }
                            else{
                                //strncpy(msgstruct.infos, "Pseudo not found", sizeof(msgstruct.infos) - 1);
                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "BROADCAST_SEND") == 0) && (msgstruct.pld_len == -1) ){
                            
                            str_support = malloc((sizeof(msgstruct.infos)+1)*sizeof(char));
                            strcpy(str_support, msgstruct.infos);

                            int_support = malloc(11*sizeof(int));
                            int_support = liste_fd_broad(clients, pollfds[i].fd);

                            for (i_sup = 1; i_sup <= int_support[0]; i_sup++){

                                //sending the size of the msg
                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = BROADCAST_SEND;
                                msgstruct.pld_len = sizeof(str_support);
                                strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));
                                if ( send(int_support[i_sup], (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct broad1: %s\n", strerror(errno));
                                    break;
                                }

                                //then sending the msg
                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = BROADCAST_SEND;
                                msgstruct.pld_len = -1;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                                strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));

                                if ( send(int_support[i_sup], (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct broad2: %s\n", strerror(errno));
                                    break;
                                }

                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "UNICAST_SEND") == 0) && (msgstruct.pld_len != -1) ){
                            
                            unicast_support = malloc(138*sizeof(char));
                            strcpy(unicast_support, msgstruct.infos);

                            
                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = UNICAST_SEND;
                            strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));


                            // sending first msg with pld_len
                            msgstruct.pld_len = sizeof(str_support);

                            if ( send(fd_of_pseudo(clients, unicast_support), (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct uni1: %s\n", strerror(errno));
                                    break;
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "UNICAST_SEND") == 0) && (msgstruct.pld_len == -1) ){
                            
                            str_support = malloc(138*sizeof(char));
                            strcpy(str_support, msgstruct.infos);
                            

                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = UNICAST_SEND;
                            msgstruct.pld_len = -1;
                            strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));
                            strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                            if ( send(fd_of_pseudo(clients, unicast_support), (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct uni2: %s\n", strerror(errno));
                                    break;
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "ECHO_SEND") == 0) && (msgstruct.pld_len == -1) ){

                            // sending first msg with pld_len
                            msgstruct.pld_len = sizeof(msgstruct.infos);
                            send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            // sending second msg with the mssg
                            msgstruct.pld_len = -1;
                            if (send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                printf("Problem while sending msgstruct for echo\n");
                                break;
                            }
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "MULTICAST_CREATE") == 0){
                            update_channels(clients, tab_salon, i_salon);
                            if(is_channel_there(clients, msgstruct.infos) == -1){

                                ajout_channel(clients, pollfds[i].fd, msgstruct.infos);
                                printf("channel : %s created\n", msgstruct.infos);

                                tab_salon[i_salon] = malloc(128*sizeof(char));
                                strcpy(tab_salon[i_salon], msgstruct.infos);
                                i_salon++;

                                str_support = malloc(148*sizeof(char));
                                strcpy(str_support, "You have created ");
                                strcat(str_support, msgstruct.infos);


                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_CREATE;
                                msgstruct.pld_len = -1;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            }
                            else{
                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_CREATE;
                                msgstruct.pld_len = -2;

                                strncpy(msgstruct.infos, "channel_name already used please choose another one", sizeof(msgstruct.infos) - 1);
                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            }
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "MULTICAST_LIST") == 0){

                            update_channels(clients, tab_salon, i_salon);
                            memset(&msgstruct, 0, sizeof(struct message));

                            msgstruct.type = MULTICAST_LIST;

                            nicklist = malloc(1024*sizeof(char));
                            nicklist = list_salon(tab_salon, i_salon);

                            // sending first msg with pld_len
                            //msgstruct.pld_len = sizeof(nicklist);
                            //send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);

                            // sending second msg with the mssg
                            msgstruct.pld_len = -1;
                            strcpy(msgstruct.infos, nicklist);
                            send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "MULTICAST_JOIN") == 0){
                            update_channels(clients, tab_salon, i_salon);

                            if(is_channel_there(clients, msgstruct.infos) == 1){

                                ajout_channel(clients, pollfds[i].fd, msgstruct.infos);
                                str_support = malloc(148*sizeof(char));
                                strcpy(str_support, msgstruct.infos);

                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_JOIN;
                                msgstruct.pld_len = -1;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            }

                            else{

                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_JOIN;
                                msgstruct.pld_len = -2;

                                strncpy(msgstruct.infos, "channel_name doesn't exist", sizeof(msgstruct.infos) - 1);
                                send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0);
                            } 
                        }

                        else if(strcmp( msg_type_str[msgstruct.type], "MULTICAST_QUIT") == 0){

                            suppr_channel(clients, pollfds[i].fd);
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "MULTICAST_SEND") == 0) && (msgstruct.pld_len != -1) ){

                            update_channels(clients, tab_salon, i_salon);

                            int_support = malloc(11*sizeof(int));
                            int_support = fds_with_channel(clients, msgstruct.infos);
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "MULTICAST_SEND") == 0) && (msgstruct.pld_len == -1) ){

                            update_channels(clients, tab_salon, i_salon);

                            str_support = malloc((sizeof(msgstruct.infos)+1)*sizeof(char));
                            strcpy(str_support, msgstruct.infos);

                            for (i_sup = 1; i_sup <= int_support[0]; i_sup++){

                                printf("sending %s to %i \n", str_support, int_support[i_sup]);

                                //sending the size of the msg
                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_SEND;
                                msgstruct.pld_len = sizeof(str_support);
                                strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));
                                if ( send(int_support[i_sup], (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct multi1: %s\n", strerror(errno));
                                    break;
                                }

                                //then sending the msg
                                memset(&msgstruct, 0, sizeof(struct message));
                                msgstruct.type = MULTICAST_SEND;

                                msgstruct.pld_len = -1;
                                strncpy(msgstruct.infos, str_support, sizeof(msgstruct.infos) - 1);

                                strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));

                                if(int_support[i_sup] != pollfds[i].fd){
                                    if ( send(int_support[i_sup], (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                        printf("Problem while sending msgstruct multi2: %s\n", strerror(errno));
                                        break;
                                    }
                                }
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0) && (msgstruct.pld_len != -1) ){  

                            file_receiver = malloc(138*sizeof(char));
                            strcpy(file_receiver, msgstruct.infos);

                            file_sender = malloc(138*sizeof(char));
                            strcpy(file_sender, pseudo_of_fd(clients, pollfds[i].fd));

                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "FILE_REQUEST") == 0) && (msgstruct.pld_len == -1) ){

                            file_name = malloc(138*sizeof(char));
                            strcpy(file_name, msgstruct.infos);
                            
                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = FILE_REQUEST;
                            strcpy(msgstruct.nick_sender, pseudo_of_fd(clients, pollfds[i].fd));
                            strcpy(msgstruct.infos, file_name);

                            // sending first msg with pld_len
                            msgstruct.pld_len = sizeof(str_support);
                            send(fd_of_pseudo(clients, file_receiver), (void*)&msgstruct, sizeof(msgstruct), 0);

                            msgstruct.pld_len = -1;

                            if ( send(fd_of_pseudo(clients, file_receiver), (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct filerequest: %s\n", strerror(errno));
                                    break;
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "FILE_ACCEPT") == 0) ){
                            
                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = FILE_ACCEPT;

                            strcpy(msgstruct.nick_sender, file_sender);
                            strcpy(msgstruct.infos, " accepted file transfert. But unfortunately this functionality isn't implemented yet");
                            msgstruct.pld_len = -1;

                            if ( send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct filerequest: %s\n", strerror(errno));
                                    break;
                            }
                        }

                        else if( (strcmp( msg_type_str[msgstruct.type], "FILE_REJECT") == 0) ){

                            
                            memset(&msgstruct, 0, sizeof(struct message));
                            msgstruct.type = FILE_REJECT;

                            strcpy(msgstruct.nick_sender, file_sender);
                            strcpy(msgstruct.infos, " cancelled file transfer.");
                            msgstruct.pld_len = -1;

                            if ( send(pollfds[i].fd, (void*)&msgstruct, sizeof(msgstruct), 0) <= 0) {
                                    printf("Problem while sending msgstruct filerequest: %s\n", strerror(errno));
                                    break;
                            }
                        }


                    }

                    //On affiche le nombre de clients actifs
                    printf("=====> Number of Clients : %d\n\n", numberClients);
                }


            }
        }
        else if (pollResult == -1){
            perror("ERROR whith poll");
            exit(1);
        }
    }


    if(nicklist != NULL){
        free(nicklist);
    }
    if(str_support != NULL){
        free(str_support);
    }
    if(unicast_support != NULL){
        free(unicast_support);
    }
    if(int_support != NULL){
        free(int_support);
    }
    if(file_receiver != NULL){
        free(file_receiver);
    }
    if(file_name != NULL){
        free(file_name);
    }
    if(file_sender != NULL){
        free(file_sender);
    }


}

int main(int argc, char *argv[]) {	

	if ( argc != 2 ){
		printf("Usage : pgm port_number\n");
		return 0;
	}

    char *str_port = argv[1];
    int port = atoi(str_port);

    char  *addr_ip = "127.0.0.1";

    int server_socket = create_server_socket(addr_ip, port);

    int client_socket = wait_client(server_socket);

    printf("server end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}