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

	int LogInfo=0;

	int ClientFD=-1;

	fd_set watch_list;



	typedef struct ClientsLoggedIn{

		char IP[30];

		char HostName[50];

		int ListeningPort;

	}ClientsLoggedIn;



	ClientsLoggedIn List1[5];

	

ClientsLoggedIn* John;

int ClientExists(char* IP){

	for(int i=0; i<5;i++){

/*		printf("IP DURING CLIENT EXISTS IS *%s*\n",IP);*/

		if (((strcmp(IP,List1[i].IP))==0)&&(strcmp(IP,"")!=0)){

			return 1;

		}

		}

		return 0;

}



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

		int count=0;

		int iterator1=0;

		int iterator2=0;

		int iterator3=0;



		int j=strlen(Actualmsg);

		for (int i=0; i<strlen(Actualmsg); i++){

			char Character[2];

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

	void Parse2(char** HostName,char** IP, char** LPort, char* Actualmsg){

		int count=0;

		int iterator1=0;

		int iterator2=0;

		int iterator3=0;

		int j=strlen(Actualmsg);

/*		printf("Message is *%s*\n",Actualmsg);*/

		for (int i=0; i<strlen(Actualmsg); i++){

			char Character[2];

			strncpy(Character,&Actualmsg[i],1);

			

			Character[1]='\0';



			if(count==2){

/*				printf("Character is *%s*\n",Character);*/

/*				printf("SHOULD BE COPYING INTO IP\n");*/

				(*IP)[iterator2]=*Character;

				iterator2 ++;

			}

			if (count==3){

/*			printf("Character is *%s*\n",Character);*/

/*				printf("SHOULD BE COPYING INTO Lport\n");*/

				(*LPort)[iterator3]=*Character;

				iterator3++;

			}

			if (count==1){

/*			printf("Character is *%s*\n",Character);*/

/*				printf("SHOULD BE COPYING INTO HOSTNAME\n");*/

				(*HostName)[iterator1]=*Character;

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

		John = (ClientsLoggedIn*)malloc(sizeof(ClientsLoggedIn));

		strcpy(John->IP,"");

		strcpy(John->HostName,"");

			John->ListeningPort=0;

		for (int i=0;i<5;i++){

			List1[i]=*John;

		}

		char* Manipulator=malloc(2000);

		strcpy(Manipulator,ServerMessage);

		int Spaces=0;

		int count; // The count of logged-in clients

	  	int l=0;

	  	

	  	for (int i=0; i<ServerMessage[0]-1;i++){

				Spaces=0;

				int chars=0;

				char* listeningPort=malloc(50);

				char* IP=malloc(120);

				char* HostName=malloc(200);

				for(int j=0; j<strlen(Manipulator);j++){

						

						char Character[2];

						strncpy(Character,&Manipulator[j],1);

						Character[1]='\0';

						if (strcmp(Character," ")==0){



							Spaces++;

						}

						if (strcmp(Character,"[")==0){

							LogInfo=1;

							}

						}

					if (Spaces>1){

						Parse2(&HostName,&IP,&listeningPort,Manipulator);

						IP[strlen(IP)-1]='\0';

/*						printf("IP DURING RECEIVE IS *%s*\n",IP);*/

						strcpy(List1[i].IP,IP);

						HostName[strlen(HostName)-1]='\0';

						strcpy(List1[i].HostName,HostName);

						

						int port=atoi(listeningPort);

						List1[i].ListeningPort=port;

						l=l+1;

						int iterator=0;

						for(int j=0; j<strlen(Manipulator);j++){

							char Character[2];

							strncpy(Character,&Manipulator[j],1);

							Character[1]='\0';

							if (strcmp(Character," ")==0){



								iterator++;

							}

							if (iterator==5){

				

								Manipulator=Manipulator+j;

								break;

							}

					}

	  	}

	  }

	    }

	char* ListCommand(){

/*			qsort(List1, 5, sizeof(ClientsLoggedIn), compareClients2);*/

			char *ReturnM = malloc(1024*sizeof(char));



			int id=1;



			for(int i=0; i<5; i++){



				char* IP=malloc(120);

				strcpy(IP,List1[i].IP);

				if (strlen(IP)>0){

/*					printf("List1[i].HostName is *%s*\n",List1[i].HostName);*/

/*					printf("List1[i].IP is *%s*\n",List1[i].IP);*/

/*					printf("List1[i].ListeningPort is *%d*\n",List1[i].ListeningPort);*/

					sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,List1[i].HostName,List1[i].IP, List1[i].ListeningPort);

					id+=1;

				}

	/*			if (LoggedIn==1){*/



	/*				sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,List2[i].HostName,List2[i].IPaddress, List2[i].ListeningPort);*/



	/*				id+=1;*/



	/*		}*/



			}

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



			process_client_commands();

		}

	}



	void process_client_commands() {

/*		printf("CLLIENT COMMANDS CALLED\n");*/

		// The loop to keep client running and accept commands



		while(1) {

/*			printf("TOP OF WHILE LOOP\n");*/

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

		

							char *Input = (char*) malloc(sizeof(char)*256);



							char *login = (char*) malloc(sizeof(char)*6);		

							

							char *Command= (char*) malloc(sizeof(char)*256);

							

							char *Arg1 = (char*) malloc(sizeof(char)*256);

							

							char *Arg2 = (char*) malloc(sizeof(char)*256);

							fgets(Input, 256, stdin);

							Input[strlen(Input)-1]='\0';	

							Parse(&Command,&Arg1,&Arg2,Input);

/*							printf("Arg1 is *%s*\n",Arg1);*/

/*							printf("BEFORE, INPUT IS *%s*\n",Input);*/

/*							printf("AFTER, INPUT IS *%s*\n",Input);*/

							strncpy(login,Input,5);

							



							login[5] = '\0' ;

							if (strcmp(Input, "AUTHOR") == 0) {



								handle_author_command();



							} 



							else if (strcmp(Input, "IP") == 0) {



								char* IP=malloc(2000);

								strcpy(IP,handle_ip_command());

								cse4589_print_and_log("[IP:SUCCESS]\n");

								cse4589_print_and_log("IP:%s\n", IP);

								cse4589_print_and_log("[IP:END]\n");



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

								if ((strcmp(Command,"LOGIN")==0)){

									Arg1[strlen(Arg1)-1]='\0';

									Input[strlen(Input)-1]= '\0';



									int IPlen= strlen(Arg1);



									int PORTN= atoi(Arg2);

									login_to_server(Arg1,PORTN);



									free(Input);

								}

/*							else if (strcmp("SEND",Command)==0){*/

/*								cse4589_print_and_log("[SEND:ERROR]\n[SEND:END]\n");*/

/*							}*/

/*							else if (strcmp("UNBLOCK",Command)==0){*/

/*								cse4589_print_and_log("[UNBLOCK:ERROR]\n[UNBLOCK:END]\n");*/

/*							}*/

/*							else if (strcmp("BLOCK",Command)==0){*/

/*								cse4589_print_and_log("[BLOCK:ERROR]\n[BLOCK:END]\n");*/

/*							}*/

/*							else if (strcmp(Input,"LIST")==0){*/

/*									char* Message=ListCommand();*/

/*									cse4589_print_and_log("[LIST:SUCCESS]\n");*/

/*									cse4589_print_and_log(Message);*/

/*									cse4589_print_and_log("[LIST:END]\n");*/

/*									free(Input);*/



/*								}*/

							}

							

							else if (LoggedIn==1){



								if (strcmp("REFRESH",Input)==0){

/*									printf("REFRESH COMMAND INPUTTED BY CLIENT\n");*/

									cse4589_print_and_log("[REFRESH:SUCCESS]\n[REFRESH:END]\n");

/*									printf("SENDING *%s*\n",Input);*/

									int j=send(ClientFD,Input,strlen(Input),0);



									free(Input);

								}

								else if (strcmp(Input,"LIST")==0){

/*									char* Message=ListCommand();*/

/*									cse4589_print_and_log("[LIST:SUCCESS]\n");*/

/*									cse4589_print_and_log(Message);*/

/*									cse4589_print_and_log("[LIST:END]\n");*/

/*									free(Input);*/

										send(ClientFD,"LIST",strlen("LIST"),0);

								}

								else if (strcmp(Command,"SEND")==0){

									Arg1[strlen(Arg1)-1]='\0';

/*									if (ClientExists(Arg1)==1){*/

										char* IP=malloc(200);

										strcpy(IP,handle_ip_command());

/*										if (strcmp(IP,Arg1)!=0){*/

/*											printf("INPUT IS *%s*\n",Input);*/

											int j=send(ClientFD,Input,strlen(Input),0);

/*										}*/

/*										else{*/

/*											cse4589_print_and_log("[SEND:ERROR]\n[SEND:END]\n");*/

/*											}*/

/*									else{*/

/*										cse4589_print_and_log("[SEND:ERROR]\n[SEND:END]\n");*/

/*									free(Input);*/

/*									}*/

								}

								else if ((strcmp(Command,"BROADCAST")==0)){

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

/*									printf("ARG1 is *%s*\n",Arg1);*/

									if (ClientExists(Arg1)==1){

										char* IP=handle_ip_command();

										if (strcmp(IP,Arg1)!=0){

											int j=send(ClientFD,Input,strlen(Input),0);

									

										}

										}

									else{

										cse4589_print_and_log("[BLOCK:ERROR]\n[BLOCK:END]\n");

									free(Input);

									}

									}

								else if (strcmp(Command,"UNBLOCK")==0){

									if (ClientExists(Arg1)==1){

										char* IP=malloc(120);

										strcpy(IP,handle_ip_command());

										if (strcmp(IP,Arg1)!=0){

											int j=send(ClientFD,Input,strlen(Input),0);



										}

										}

									else{

									

										cse4589_print_and_log("[UNBLOCK:ERROR]\n[UNBLOCK:END]\n");

									free(Input);

									}

								}

								else{



								}

							}

						}

					//END OF READING FROM STDIN

					if ((LoggedIn==1)&&(sock_index!=0)){

/*						printf("END OF STDIN");*/

						char *DataReceived= (char*) malloc(256*sizeof(char));

/*						printf("DONE READING FROM STDIN, DATARECEIVED IS *%s*\n",DataReceived);*/

						char *ServerCommand=(char*) malloc(256*sizeof(char));

						int LengthOfMessageReceived= recv(ClientFD, DataReceived, 1023,0);

						DataReceived[LengthOfMessageReceived]='\0';

						char Mess[8];  // Make sure to allocate enough space for the copied characters and the null-terminator.

						int firstValue;

						// Copy the first 7 characters (0 to 6) from source to destination.

/*						strncpy(Mes[s, DataReceived, 7);*/

						

						printf("RECEIVED form server: *%s*\n",DataReceived);

/*						if (strcmp(Mess,"REFRESH")==0){	*/

/*							printf("RECEIVED MESSAGE IS REFRESH and data received by Server is *%s*\n",DataReceived);*/

/*							char* stripped=malloc(1000);*/

/*							strcpy(stripped,DataReceived+8);*/

/*							ReceiveLoggedinInfo(stripped);*/

/*						}*/

						

/*						else if ((DataReceived[0])>0&&(DataReceived[0]<5)) {*/

/*							*/

/*							ReceiveLoggedinInfo(DataReceived);*/

/*							char* Ret=malloc(2000);*/

/*							strcpy(Ret,ListCommand());*/

/*						*/

/*							cse4589_print_and_log("%s",Ret);*/

/*							if(LogInfo==1){*/

/*								cse4589_print_and_log("[LOGIN:SUCCESS]\n");*/

/*								cse4589_print_and_log("[LOGIN:END]\n");*/

/*								LogInfo=0;*/

/*							}*/

/*						}*/

							cse4589_print_and_log("%s",DataReceived);

							

							free(DataReceived);

/*							process_client_commands();*/

						}

					}

				}

			}

		}

	}
