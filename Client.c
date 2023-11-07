		#include <stdio.h>



	#include <stdlib.h>



	#include <sys/socket.h>



	#include <strings.h>



	#include <string.h>

	#include <ctype.h>

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



	int ClientFD=-1;

	fd_set watch_list;



	typedef struct ClientsLoggedIn{

		char IP[30];

		char HostName[50];

		int ListeningPort;

	}ClientsLoggedIn;



	ClientsLoggedIn List1[5];





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



	void Parse(char** Command,char** FirstArgPointer, char** SecondArgPointer, char* Actualmsg){

		printf("Parse called, Message is *%s*\n",Actualmsg);

		int count=0;

		int iterator1=0;

		int iterator2=0;

		int iterator3=0;



		int j=strlen(Actualmsg);

		for (int i=0; i<strlen(Actualmsg); i++){

			char Character[2];

			strncpy(Character,&Actualmsg[i],1);

			Character[1]='\0';

			printf("CHARACTER as s IS *%s*\n",Character);

			

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

			if (count>2){

				(*Command)[iterator1]=*Character;

				iterator1++;		

			}

			}

			printf("PARSE DONE\n");

	}

	void Parse2(char** Command,char** FirstArgPointer, char** SecondArgPointer, char* Actualmsg){

		printf("Parse 2 called\n");

		int count=0;

		int iterator1=0;

		int iterator2=0;

		int iterator3=0;

		printf("CALLING PARSE2");

		int j=strlen(Actualmsg);

		for (int i=0; i<strlen(Actualmsg); i++){

			char Character[2];

			

			strncpy(Character,&Actualmsg[i],1);

			Character[1]='\0';

			printf("CHARACTER IS *%s*\n",Character);

			if(count==2){

				printf("SHOULD BE COPYING INTO FIRSTARGPOINT\n");

				(*FirstArgPointer)[iterator2]=*Character;

				iterator2 ++;

			}

			if (count>2){

				(*SecondArgPointer)[iterator3]=*Character;

				iterator3++;

			}

			if (count==1){

				printf("SHOULD BE COPYING INTO COMMAND\n");

				printf("Command is *%s*\n",*Command);

				(*Command)[iterator1]=*Character;

				iterator1++;		

			}

			if (strcmp(Character," ")==0){

				count++;



			}

/*			if (count==1){*/

/*				printf("SHOULD BE COPYING INTO COMMAND\n");*/

/*				*/

/*				(*Command)[iterator1]=*Character;*/

/*				iterator1++;		*/

/*			}*/

		}

			printf("PARSE22 DONE. Command is %s\n",*Command);

	}

	void ParseServerMessage(char** ServerCommand,char* ServerMessage){

		int count=0;

		int iterator1=0;

		

		int j=strlen(ServerMessage);

		for (int i=0; i<strlen(ServerMessage); i++){

			char Character[1];

			strncpy(Character,&ServerMessage[i],1);

			Character[1]='\0';

			if (strcmp(Character," ")==0){

				count++;

			}

			if (count==0){

				(*ServerCommand)[iterator1]=*Character;

				iterator1++;		

			}

			}

	}



	void close_connection(int client_fd) {

		close(client_fd);

	}

	int compareClients2(const void *a, const void *b) {

				



			  const ClientsLoggedIn *clientA = (const ClientsLoggedIn *)a;



			  const ClientsLoggedIn *clientB = (const ClientsLoggedIn *)b;



			  return clientA->ListeningPort - clientB->ListeningPort;



		}

	void ReceiveLoggedinInfo(char* ServerMessage){

		char* Manipulator=malloc(2000);

		strcpy(Manipulator,ServerMessage);

		printf("SERVER MESSAGE IS %s\n",ServerMessage);

		int count; // The count of logged-in clients

	  	int l=0;

	  	for (int i=0; i<ServerMessage[0]-1;i++){

				char* listeningPort=malloc(50);

				char* IP=malloc(120);

				char* FUCKYOU=malloc(200);

		  		Parse2(&FUCKYOU,&IP,&listeningPort,ServerMessage);

		  		printf("HOST NAME123123123 IS %s\n",FUCKYOU);

					printf("IP IS %s",IP);

					strcpy(List1[l].IP,IP);

					strcpy(List1[l].HostName,FUCKYOU);

					int port=atoi(listeningPort);

					List1[l].ListeningPort=port;

					l++;

		  		int iterator=0;

		  		printf("FUCK YOU IS %s\n",FUCKYOU);

		  		for(int j=0; j<strlen(Manipulator);j++){

						char Character[1];

						strncpy(Character,&Manipulator[i],1);

						Character[1]='\0';

						if (strcmp(Character," ")==0){

							iterator++;

						}

						if (iterator==4){

							Manipulator=Manipulator+j;

							break;

						}

				}

	  	}

	  

	    }

	char* ListCommand(ClientsLoggedIn List2[]){

			qsort(List2, 5, sizeof(ClientsLoggedIn), compareClients2);

			char *ReturnM = malloc(1024*sizeof(char));



			int id=1;



			for(int i=0; i<5; i++){

				printf("I IS %d and List2[i].IP is %s\n",i,List2[i].IP);

				char* IP=malloc(120);

				strcpy(IP,List2[i].IP);

				printf("IP IS %s\n",IP);

				if (strlen(IP)>0){

					sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,List2[i].HostName,List2[i].IP, List2[i].ListeningPort);

				}

	/*			if (LoggedIn==1){*/



	/*				sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,List2[i].HostName,List2[i].IPaddress, List2[i].ListeningPort);*/



	/*				id+=1;*/



	/*		}*/



			}

			printf("AT THE END OF LIST COMMAND AND RETURN M IS %s",ReturnM);

			return	ReturnM;



		}

	void login_to_server(const char* server_ip, int server_port) {



		struct sockaddr_in serv_addr;



		if ((ClientFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {



			cse4589_print_and_log("[LOGIN:ERROR]\n");

			cse4589_print_and_log("[LOGIN:END]\n");

			ClientFD=-1;

			return;

		}

		serv_addr.sin_family = AF_INET;



		serv_addr.sin_port = htons(server_port);



		if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {



			cse4589_print_and_log("[LOGIN:ERROR]\n");

			cse4589_print_and_log("[LOGIN:END]\n");

			ClientFD=-1;

			close(ClientFD);

			return;

		}

		if (connect(ClientFD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {



			cse4589_print_and_log("[LOGIN:ERROR]\n");

			cse4589_print_and_log("[LOGIN:END]\n");

			ClientFD=-1;

			close(ClientFD);

			return;

		}

		else{

	/*		cse4589_print_and_log("[LOGIN:SUCCESS]\n");*/

	/*		cse4589_print_and_log("[LOGIN:END]\n");*/

			LoggedIn=1;



			char MESSAGE[10];



			sprintf(MESSAGE, "%d", Portno);



			int j=send(ClientFD,MESSAGE,strlen(MESSAGE),0);	



	/*		process_client_commands();*/

		}

	}



	void process_client_commands() {

		// The loop to keep client running and accept commands



		while(1) {

			FD_ZERO(&watch_list);

			FD_SET(0,&watch_list);

		

			if(ClientFD!=-1){

				FD_SET(ClientFD,&watch_list);

			}

			int selret = select(10, &watch_list, NULL, NULL, NULL);

			if(selret < 0){

	       

				perror("select failed.\n");



			}

			if(selret >= 0){

				for(int sock_index=0; sock_index<=5; sock_index+=1){



					if(FD_ISSET(sock_index, &watch_list)){



						if (sock_index == 0){

						printf("SOCK INDEX is 0jifedjiaosjdfioasjdfiojasdfiojsadfiojasiodfjioasdjio");

							char *Input = (char*) malloc(sizeof(char)*256);



							char *login = (char*) malloc(sizeof(char)*6);		

							

							char *Command= (char*) malloc(sizeof(char)*256);

							

							char *Arg1 = (char*) malloc(sizeof(char)*256);

							

							char *Arg2 = (char*) malloc(sizeof(char)*256);

							

							fgets(Input, 256, stdin);

							

							Parse(&Command,&Arg1,&Arg2,Input);

							Input[strlen(Input)-1]= '\0';



							strncpy(login,Input,5);

							



							login[5] = '\0' ;



							if (strcmp(Input, "AUTHOR") == 0) {



								handle_author_command();



							} 



							else if (strcmp(Input, "IP") == 0) {



								handle_ip_command();



							} 



							else if (strcmp(Input,"PORT")==0){



								handle_port_command(Portno);



							}



							else if (strcmp(Input,"EXIT")==0){



								cse4589_print_and_log("[EXIT:SUCCESS]\n");



								cse4589_print_and_log("[EXIT:END]\n");



								close_connection(ClientFD);



								exit(-1);



							}

							else if (LoggedIn==0){

								if ((strcmp(login,"LOGIN")==0)){

									Arg1[strlen(Arg1)-1]= '\0';



									int IPlen= strlen(Arg1);



									int PORTN= atoi(Arg2);



									login_to_server(Arg1,PORTN);



									free(Input);



								}

								else{



								cse4589_print_and_log("[%s:ERROR]\n",Input);

								cse4589_print_and_log("[%s:END]\n",Input);

								fflush(stdout);

								}

							}

							

							else if (LoggedIn==1){

								fflush(stdout);

								if (strcmp("REFRESH",Input)==0){

									printf("REFRESH CALLED\n");

									int j=send(ClientFD,Input,strlen(Input),0);



									free(Input);

								}

								else if (strcmp(Input,"LIST")==0){

									char* Message=ListCommand(List1);

									cse4589_print_and_log("[LIST:SUCCESS]\n");

									cse4589_print_and_log(Message);

									cse4589_print_and_log("[LIST:END]\n");

									free(Input);



								}

								else if (strcmp(Command,"SEND")==0){

									int LengthOfMessageSent=send(ClientFD,Input,strlen(Input),0);

								}

								else if (strcmp(Command,"BROADCAST")==0){

									int LengthOfMessageSent=send(ClientFD,Input,strlen(Input),0);

								}

								else if (strcmp(Input,"LOGOUT")==0){

									cse4589_print_and_log("[%s:SUCCESS]\n",Input);

									cse4589_print_and_log("[%s:END]\n",Input);

									send(ClientFD,"LOGOUT",strlen("LOGOUT"),0);

									close_connection(ClientFD);

									FD_CLR(ClientFD,&watch_list);

									ClientFD=-1;

									LoggedIn=0;

								}

								else if (strcmp(Command,"BLOCK")==0){

									int j=send(ClientFD,Input,strlen(Input),0);



									free(Input);

								}

								else if (strcmp(Command,"UNBLOCK")==0){

									int j=send(ClientFD,Input,strlen(Input),0);



									free(Input);

								}

								else{

								fflush(stdout);

								}

							}

						}

					//END OF READING FROM STDIN

					if ((LoggedIn==1)&&(sock_index!=0)){

						printf("INFINTE LOOP\n");

						char *DataReceived= (char*) malloc(256*sizeof(char));

						char *ServerCommand=(char*) malloc(256*sizeof(char));

						int LengthOfMessageReceived= recv(ClientFD, DataReceived, 1023,0);

						printf("DataReceived is %s\n",DataReceived);

						char Mess[8];  // Make sure to allocate enough space for the copied characters and the null-terminator.

						int firstValue;

						// Copy the first 7 characters (0 to 6) from source to destination.

						strncpy(Mess, DataReceived, 7);

						

						int value = 0;

						printf("Character is *%d*\n",DataReceived[0]);

						

						if (strcmp(Mess,"REFRESH")==0){	

							printf("REFRESH\n");

							char* stripped;

							stripped=DataReceived+7;

							ReceiveLoggedinInfo(stripped);

						}

						

						else if ((DataReceived[0])>0&&(DataReceived[0]<5)) {

							printf("ELSE IF\n");

							ReceiveLoggedinInfo(DataReceived);

							printf("AFTER RECEIVED LOGGEDININGO \n");

							char* Ret=ListCommand(List1);

							printf("AFTER RECEIVED LISTCOMMAND \n");

							cse4589_print_and_log("%s",Ret);

							free(DataReceived);

						}

						fflush(stdout);

						cse4589_print_and_log("%s",DataReceived);

	/*					process_client_commands();*/

						}

					}

				}

			}

		}

	}
