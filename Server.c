#include "Server.h"

#include <arpa/inet.h>

#include <stdio.h>

#include <string.h>

#include <sys/socket.h>

#include <unistd.h>

#include <netinet/in.h>

#include <stdlib.h>

#include "../include/global.h"

#include "../include/logger.h"

#include "Commands.h"

#include <netdb.h>

#define CMD_SIZE 100

#define BUFFER_SIZE 256



int server_socket;



struct sockaddr_in client_addr,server_addr;



fd_set master_list, watch_list;



int head_socket, selret, sock_index, fdaccept=0, caddr_len;



int PORT = 8080;



char buffer[256];





typedef struct Client {



  char IPaddress[30];



  char Name[30];



  int ListeningPort;



  int FD;



} Clients;



Clients List[5];



void remove_connection(int socket) {



	close(socket);



	FD_CLR(socket, &master_list);



	for (int i = 0; i < 5; i++) {



		if (List[i].FD == socket) {



	    	List[i].ListeningPort = 0; 



	    	List[i].FD == 0;



	}	



	}

}



void Parse1(char** Command,char** FirstArgPointer, char** SecondArgPointer, char* Actualmsg){

	

	int count=0;

	int iterator1=0;

	int iterator2=0;

	int iterator3=0;

	

	int j=strlen(Actualmsg);

	printf("Length of Command is %d\n",j);

	for (int i=0; i<strlen(Actualmsg); i++){

		char Character[1];

		strncpy(Character,&Actualmsg[i],1);

		Character[1]='\0';

		if(count==1){

			(*FirstArgPointer)[iterator2]=*Character;

			iterator2 ++;

		}

		if (count>1){

			(*SecondArgPointer)[iterator3]=*Character;

			iterator3++;

		}

		if (strcmp(Character," ")==0){

			count++;



		}

		if (count==0){

			(*Command)[iterator1]=*Character;

			iterator1++;		

		}

		}

}

char* ReturnMessage(const Clients LIST[]){



	char *ReturnM = malloc(1024);



	int id=1;



	for(int i=0; i<5; i++){



		int port_num= List[i].ListeningPort;



		if (port_num!=0){



			sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,LIST[i].Name,LIST[i].IPaddress, port_num);



			id+=1;



	}



}

	return	ReturnM;



}



int compareClients(const void *a, const void *b) {



    const Clients *clientA = (const Clients *)a;



    const Clients *clientB = (const Clients *)b;



    return clientA->ListeningPort - clientB->ListeningPort;



}



int AddClient(char ip[], char Name[], int LP, int FD) {



	Clients ClientToAdd;



	strcpy(ClientToAdd.IPaddress,ip);



	strcpy(ClientToAdd.Name,Name);



	ClientToAdd.ListeningPort=LP;



	ClientToAdd.FD=FD;



	for (int i=0; i<5; i++){



		int N=List[i].FD;



		if (N == 0){



			List[i]=ClientToAdd;



			return (0);

		}



}



return 0;



}





int Create_Server(int PortNO){



    	int port = PortNO; 



	if (port <= 0) {



	    return 1;



	}



    	if (initialize_server(port) < 0) {



		return 1;



	    }



	else{



		PORT=port;



		server_loop();



	}



	return 0;  



	}



// Initialize the server



int initialize_server(int port) {



    PORT = port;



    server_socket = socket(AF_INET, SOCK_STREAM, 0);



    if (server_socket < 0) {



        perror("Socket creation failed");



        return -1;



    }



    server_addr.sin_family = AF_INET;



    server_addr.sin_addr.s_addr = INADDR_ANY;



    server_addr.sin_port = htons(PORT);



    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {



        return -1;



    }



    if (listen(server_socket, 5) < 0) {  // 10 is the max number of waiting connections



        perror("Listen failed\n");



        return -1;



    }



    FD_ZERO(&master_list);



    FD_SET(server_socket, &master_list);



    FD_SET(0,&master_list);



    head_socket = server_socket;



    return 0;  // success



}



// Accept a new connection



int accept_new_connection() {



    int new_socket;



    socklen_t addr_len = sizeof(client_addr);



    new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);



    if (new_socket < 0) {



        perror("Accept failed\n");



        return -1;



    }



    // Add to master_list



    FD_SET(new_socket, &master_list);



    if (new_socket > head_socket){



    	head_socket = new_socket;



    }



    return new_socket;



}



void server_loop() { 



	while (1) {



		memcpy(&watch_list, &master_list, sizeof(master_list));



		int STDIN= fileno(stdin);



		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);



		if(selret < 0){



			perror("select failed.\n");



			}



		if(selret > 0){



			/* Loop through socket descriptors to check which ones are ready */



			for(sock_index=0; sock_index<=head_socket; sock_index+=1){



				if(FD_ISSET(sock_index, &watch_list)){



					if (sock_index == STDIN){

					

						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);



						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL){ //Mind the newline character that will be written to cmd



							exit(-1);



						}

						cmd[strlen(cmd)-1]='\0';



						if (strcmp(cmd,"EXIT")==0){



							remove_connection(head_socket);



							cse4589_print_and_log("[EXIT:SUCCESS]\n");



							cse4589_print_and_log("[EXIT:END]\n");



							exit(-1);



						}



						else if (strcmp(cmd,"AUTHOR")==0){



							handle_author_command();



						}



						else if (strcmp(cmd,"IP")==0){

						

							handle_ip_command();

						}



						else if (strcmp(cmd,"PORT")==0){



							handle_port_command(PORT);



						}

						else if (strcmp(cmd,"LIST")==0){



							char *DataToSend= ReturnMessage(List);

							cse4589_print_and_log("[LIST:SUCCESS]\n");

							cse4589_print_and_log("%s", DataToSend);

							cse4589_print_and_log("[LIST:END]\n");



						}

						else{





							cse4589_print_and_log("[%s:ERROR]\n",cmd);

							cse4589_print_and_log("[%s:END]\n",cmd);

							



							}



						free(cmd);



						fflush(stdout);



					}



					



					else if(sock_index == server_socket){



						caddr_len = sizeof(client_addr);



						fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);



						if(fdaccept < 0){



							perror("Accept failed.");



						}



						char client_ip[INET_ADDRSTRLEN];



						char client_hostname[256];



						getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), client_hostname, sizeof(client_hostname), NULL, 0, 0);



						inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));



						/* Add to watched socket list */



						FD_SET(fdaccept, &master_list);



						if(fdaccept > head_socket) {



							head_socket = fdaccept;



						}



						



						char *DataR = (char*) malloc(sizeof(char)*256);



						int bytes_received = recv(fdaccept, DataR, 255,0);



						if (bytes_received > 0) {



					



							DataR[bytes_received] = '\0';



					



						}







						



						AddClient(client_ip,client_hostname,atoi(DataR),fdaccept);



						qsort(List, 5, sizeof(Clients), compareClients);



						char *DataToSend= ReturnMessage(List);



						fflush(stdout);



						send(fdaccept,DataToSend,strlen(DataToSend),0);



					}



				/* Read from existing clients */



					else{

	

						

						/* Initialize buffer to receieve response */

	

						char *NewData = (char*) malloc(sizeof(char)*256);



						memset(NewData, '\0', 256);

						

						if(recv(sock_index, NewData, 256, 0) <= 0){



							remove_connection(sock_index);



						}



						else {

							char *DataR = (char*) malloc(sizeof(char)*256);

							

							//Process incoming data from existing clients here ...



							if (strlen(NewData) > 0) {



								DataR[strlen(NewData)-1] = '\0';



							}

							

							if ((strcmp(NewData,"REFRESH")==0) || (strcmp(NewData,"LIST")==0)){



								char *DataToSend= ReturnMessage(List);



								send(sock_index,DataToSend,strlen(DataToSend),0);



								fflush(stdout);



							}

							else{

								char *Command= (char*) malloc(256*sizeof(char));

								char *Arg1= (char*) malloc(256*sizeof(char));

								char *Arg2 = (char*) malloc(256*sizeof(char));

								Parse1(&Command,&Arg1,&Arg2,NewData);

								if (strcmp(Command,"SEND")){

									for (int i = 0; i < 5; i++) {

										struct Client currentClient = List[i];

								  		char *ClientIP= currentClient.IPaddress;

								  		if (strcmp(Arg1,ClientIP)==0){

								  			printf("Client exists!");

								  			send(sock_index,"1",1,0);

								  			break;

										}

										else{

											printf("Client does not exist");

											send(sock_index,"12",2,0);

									}



								}



							}

							printf("COMMAND IS %s",Command);

							printf("RECEIVED FROM CLIENT:%s",NewData);



						}



			



					}



				}



			}



		}

	}

}

}

