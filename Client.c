#include <stdio.h>



#include <stdlib.h>



#include <sys/socket.h>



#include <strings.h>



#include <string.h>



#include <arpa/inet.h>



#include <sys/types.h>



#include <netdb.h>



#include <unistd.h>



#include "../include/global.h"



#include "../include/logger.h"



#include "Commands.h"



int Portno;



void process_client_commands();



int LoggedIn=0;



int ClientFD;











int create_client_socket(int portno) {



	Portno=portno;



	int client_fd = socket(AF_INET, SOCK_STREAM, 0);



	



	if (client_fd < 0) {



		cse4589_print_and_log("\n Socket creation error \n");



		return -1;



	}



	else{



		process_client_commands();



		ClientFD=client_fd;



		return client_fd;



	}



}







void close_connection(int client_fd) {







	close(client_fd);



	



}







void login_to_server(const char* server_ip, int server_port) {







	struct sockaddr_in serv_addr;







	if ((ClientFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {



		cse4589_print_and_log("[LOGIN:ERROR]\n");

		cse4589_print_and_log("[LOGIN:END]\n");



		

		return;



	}







	serv_addr.sin_family = AF_INET;



	serv_addr.sin_port = htons(server_port);



	



	if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {



		cse4589_print_and_log("[LOGIN:ERROR]\n");

		cse4589_print_and_log("[LOGIN:END]\n");



		close(ClientFD);



		return;



	}







	if (connect(ClientFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {



		cse4589_print_and_log("[LOGIN:ERROR]\n");

		cse4589_print_and_log("[LOGIN:END]\n");



		close(ClientFD);



		return;



	}



	else{



		cse4589_print_and_log("[LOGIN:SUCCESS]\n");

		cse4589_print_and_log("[LOGIN:END]\n");

		LoggedIn=1;



		char MESSAGE[10];



		sprintf(MESSAGE, "%d", Portno);



		int j=send(ClientFD,MESSAGE,strlen(MESSAGE),0);	



		char *DataR = (char*) malloc(sizeof(char)*1024);



		int bytes_received = recv(ClientFD, DataR, 1023,0);	







		while(bytes_received<=0){



			int bytes_received = recv(ClientFD, DataR, 1023,0);	



		}



		DataR[bytes_received] = '\0';



		cse4589_print_and_log("%s", DataR);



		process_client_commands();



	}











}







void process_client_commands() {



	// The loop to keep client running and accept commands



	while(1) {







		char *msg = (char*) malloc(sizeof(char)*256);



		char *login = (char*) malloc(sizeof(char)*6);		







		fgets(msg, 256, stdin);



		msg[strlen(msg)-1]= '\0';



		strncpy(login,msg,5);



		login[5] = '\0' ;



		if (strcmp(msg, "AUTHOR") == 0) {



			handle_author_command();



		} 



		else if (strcmp(msg, "IP") == 0) {



			handle_ip_command();



		} 



		else if (strcmp(msg,"PORT")==0){



			handle_port_command(Portno);



		}



		else if (strcmp(msg,"EXIT")==0){



			cse4589_print_and_log("[EXIT:SUCCESS]\n");





			cse4589_print_and_log("[EXIT:END]\n");





			close_connection(ClientFD);



			exit(-1);



		}



		else if ((strcmp(msg,"REFRESH")==0)&& (LoggedIn==1)){



			int j=send(ClientFD,msg,strlen(msg),0);



			char *DataR = (char*) malloc(sizeof(char)*1024);



			int bytes_received = recv(ClientFD, DataR, 1023,0);	







			while(bytes_received<=0){



				int bytes_received = recv(ClientFD, DataR, strlen(DataR),0);	



			}



			DataR[bytes_received] = '\0';



			cse4589_print_and_log("[REFRESH:SUCCESS]\n");



			cse4589_print_and_log("%s", DataR);



			cse4589_print_and_log("[REFRESH:END]\n");



			free(msg);



		}



		else if ((strcmp(msg,"LIST")==0) && (LoggedIn==1)){

			int j=send(ClientFD,msg,strlen(msg),0);



			char *DataR = (char*) malloc(sizeof(char)*1024);



			int bytes_received = recv(ClientFD, DataR, 1023,0);	







			while(bytes_received<=0){



				int bytes_received = recv(ClientFD, DataR, strlen(DataR),0);	



			}



			DataR[bytes_received] = '\0';



			cse4589_print_and_log("[LIST:SUCCESS]\n");



			cse4589_print_and_log("%s", DataR);



			cse4589_print_and_log("[LIST:END]\n");



			free(msg);



		}



		else if ((strcmp(login,"LOGIN")==0)){

			if (LoggedIn==0){

				char *Client_Port = (char*) malloc(30*sizeof(char));



				char *Client_IP = (char*) malloc(30*sizeof(char));





				int count=0;



				int iterator=0;



				int iterator2=0;



				



				for (int i=0; i<strlen(msg); i++){







					char Character[1];



					strncpy(Character,&msg[i],1);



					Character[1]='\0';







					if(count==1){



						Client_IP[iterator2]=*Character;



						iterator2 ++;



					}



					if(count==2){



						Client_Port[iterator]=*Character;



						iterator ++;



					}



					if (strcmp(Character," ")==0){



						count++;



					}

				}

				Client_IP[strlen(Client_IP)-1]= '\0';



				int IPlen= strlen(Client_IP);



				int Portlen= strlen(Client_Port);



				int PORTN= atoi(Client_Port);



				login_to_server(Client_IP,PORTN);



				free(msg);



				}

			else{

				

				cse4589_print_and_log("[LOGIN:ERROR]\n");

				cse4589_print_and_log("[LOGIN:END]\n");

			}



		}

	else{



		cse4589_print_and_log("[%s:ERROR]\n",msg);

		cse4589_print_and_log("[%s:END]\n",msg);



		}

	}

}

