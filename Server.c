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

		

		int MessagesReceived;

		

		int MessagesSent;

		

		int LoggedIn;

		

		char BlockList[5][30];

		

		int NumberOfBlocked;

	} Client;



	typedef struct Message{

			char SourceIP[30];

			char Message[256];

		}Message;

	typedef struct Backlog {

		char DestIP[30];

		

		Message MessageList[100];

		

		int NumOfMessages;

	}Backlog;

	char* EmptyString[30];

	Client List[5];

	

	Backlog ListOfBacklogs[5];



	Backlog* EmptyLog;



	Message* Empty;



	Client* Dummy;



	char* BlockList[5];

	void AddToBacklog(char* SourceIP,char* DestIP,char* Message){

		struct Message NewMessage;

		strcpy(NewMessage.SourceIP,SourceIP);

		strcpy(NewMessage.Message, Message);

		int exists=0;

		for(int i=0; i<5; i++){

			struct Backlog ClientInList=ListOfBacklogs[i];

			if (strcmp(ClientInList.DestIP,DestIP)==0){

				exists=1;

				ListOfBacklogs[i].MessageList[ClientInList.NumOfMessages]=NewMessage;

				ListOfBacklogs[i].NumOfMessages+=1;		

			}

		}

		if (exists==0){

			struct Backlog NewBacklog;

			strcpy(NewBacklog.DestIP,DestIP);

			NewBacklog.MessageList[0]=NewMessage;

			NewBacklog.NumOfMessages=1;

			for(int i=0; i<5; i++){

				if (strcmp(ListOfBacklogs[i].DestIP,"69")==0){

					ListOfBacklogs[i]=NewBacklog;

					break;

					}

			}	

		}

	}



	void ClearBacklog(char* DestIP,char* Message){

		for(int i=0; i<5; i++){

			struct Backlog CurrentClient=ListOfBacklogs[i];

			if (strcmp(CurrentClient.DestIP,DestIP)==0){

				CurrentClient.NumOfMessages=0;

			}

			

		}

	}



	void LogClientOut(int socket){

		close(socket);

		FD_CLR(socket, &master_list);

		for (int i = 0; i < 5; i++) {

			if (List[i].FD == socket) {

				List[i].LoggedIn=0;

				List[i].FD=-1;

		}

	}

}





	char* GetIPAddress(int client_fd) {

		char* Ip=malloc(100*sizeof(char));

		  for (int i = 0; i < 5; i++) {

		      if (List[i].FD == client_fd) {

		      		strcpy(Ip,List[i].IPaddress);

		          return Ip;

		      }

		  }

		  return NULL; // Client with the given FD not found

	}



	void Parse1(char** Command,char** FirstArgPointer, char** SecondArgPointer, char* Actualmsg){

		

		int count=0;

		int iterator1=0;

		int iterator2=0;

		int iterator3=0;

		

		int j=strlen(Actualmsg);

		for (int i=0; i<strlen(Actualmsg); i++){

			char Character[1];

			strncpy(Character,&Actualmsg[i],1);

			Character[1]='\0';

			if(count==1){

				(*FirstArgPointer)[iterator2]=*Character;

				iterator2 ++;

			}

			//POSSIBLE ERRORS DUE TO MESSAGE SIZE 

			if ((count>1)&&(iterator3<256)){

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

	char* MessageCreator(char* Message,char* Command,char* SourceIP,char* DestIP, int success){

		char *ReturnM = malloc(300*sizeof(char));

		if (success==1){

			sprintf(ReturnM,"[%s:SUCCESS]\n",Command);

			if (((((strcmp(Command,"LOGIN")==0)||(strcmp(Command,"SEND")==0))||(strcmp(Command,"BLOCK")==0))||(strcmp(Command,"UNBLOCK")==0))||				(strcmp(Command,"BROADCAST")==0)){

				sprintf(ReturnM+strlen(ReturnM),"[%s:END]\n",Command);

			}

			else if (strcmp(Command,"RECEIVED")==0){

				sprintf(ReturnM+strlen(ReturnM),"msg from:%s\n[msg]:%s\n[RECEIVED:END]\n", SourceIP,Message);

			}

				

			}

		else{

			sprintf(ReturnM,"[%s:ERROR]\n[%s:END]\n",Command,Command);

		}	

		return ReturnM;

	}

	

	char* ReturnMessage(const Client LIST[],int a){

		int numLoggedIn=1;

		char *ReturnM = malloc(1024*sizeof(char));

		if (a==1){

			sprintf(ReturnM+strlen(ReturnM),"REFRESH ");

		}

		sprintf(ReturnM+strlen(ReturnM),"%d",numLoggedIn);

		int id=1;

		

		for(int i=0; i<5; i++){

			

			int LoggedIn=List[i].LoggedIn;

			if (LoggedIn==1){



				sprintf(ReturnM+strlen(ReturnM), "%d %s %s %d ",id,LIST[i].Name,LIST[i].IPaddress, LIST[i].ListeningPort);

				numLoggedIn;

				id+=1;

				numLoggedIn+=1;

		}



		}

		if(a==0){

			ReturnM[0]=numLoggedIn+0;

			}

		else{

			ReturnM[8]=numLoggedIn+0;

		}



		return	ReturnM;	



	}



	int compareClients(const void *a, const void *b) {



		  const Client *clientA = (const Client *)a;



		  const Client *clientB = (const Client *)b;



		  return clientA->ListeningPort - clientB->ListeningPort;



	}



char* RMessage(){

		int id=0;

		char *M=malloc(4000);

		qsort(List, 5, sizeof(Client), compareClients);

		for (int i=0;i<5;i++){

/*			printf("List[i].LoggedIn is %d\n",List[i].LoggedIn);*/

/*			printf("List[i].Name is *%s*\n",List[i].Name);*/

			if (List[i].LoggedIn==1){

				id+=1;

				sprintf(M+strlen(M), "%-5d%-35s%-20s%-8d\n",id,List[i].Name,List[i].IPaddress, List[i].ListeningPort);

			}	

	}

	return M;

}

int GetClientByIP(char* IP){

	char* ListIP=malloc(30*sizeof(char));

	for (int i=0;i<5; i++){

		strcpy(ListIP,List[i].IPaddress);

		if (strcmp(ListIP,IP)==0){



			return i;

	}

}

	return -1;



	}

int AddClient(char ip[], char Name[], int LP, int FD) {



		Client ClientToAdd;



		strcpy(ClientToAdd.IPaddress,ip);



		strcpy(ClientToAdd.Name,Name);



		ClientToAdd.ListeningPort=LP;



		ClientToAdd.FD=FD;

		

		ClientToAdd.MessagesReceived=0;

		

		ClientToAdd.MessagesSent=0;

		

		ClientToAdd.LoggedIn=1;

		int ClientID=GetClientByIP(ip);

		if (ClientID>-1){

			List[ClientID].LoggedIn=1;

			List[ClientID].FD=FD;

		}

		else{

		

			for (int i=0; i<5; i++){



				int N=List[i].FD;



				if (N == -2){



					List[i]=ClientToAdd;



					return (0);

				}

		}

	}

}

int BlockClient(char* SourceIP,char *DestIP,int SourceSock,int DestSock){

	for(int i=0;i<5;i++){

		Client CurrentClient=List[i];

		char* ClientToBlock=GetIPAddress(DestSock);

		if (((strcmp(CurrentClient.IPaddress,ClientToBlock)==0)&&(strcmp(CurrentClient.IPaddress,SourceIP)!=0))){

			for (int j=0; j<5;j++){

				char* BlockedClientIP=malloc(30*sizeof(char));

				strcpy(BlockedClientIP,List[GetClientByIP(SourceIP)].BlockList[j]);

				if (strcmp(BlockedClientIP,DestIP)==0){

					return 0;

				}

				else{



					int BlockerID=GetClientByIP(SourceIP);

					if (BlockerID>-1){



						Client Blocker=List[BlockerID];

						int numBlocked=Blocker.NumberOfBlocked;

						fflush(stdout);

						strcpy(List[BlockerID].BlockList[numBlocked],ClientToBlock);

						List[BlockerID].NumberOfBlocked+=1;

					return 1;

				}

	}

	

}

}

}

return 0;

}

int UnBlockClient(char* SourceIP,char *DestIP,int SourceSock,int DestSock){

	int success=0;

	int BlockerID=GetClientByIP(SourceIP);

	if(BlockerID>-1){

		int BlockedID=GetClientByIP(DestIP);

		if((BlockedID>-1)){

			for(int i=0;i<5;i++){

				char* BlockListIP=malloc(30*sizeof(char));

				strcpy(BlockListIP,List[BlockerID].BlockList[i]);

				if (strcmp(BlockListIP,DestIP)==0){

					strcpy(List[BlockerID].BlockList[i],"");

					return 1;

				}

			}

		}

	}

return 0;

}

int BlockedMessage(char* IPaddress){

		int exists=0;

		Client LIST[5];

		for (int i=0;i<5;i++){

			LIST[i]=*Dummy;

		}

		char *ReturnM = malloc(1024);

		int ClientFD=GetClientByIP(IPaddress);

		if (ClientFD==-2){

			cse4589_print_and_log("[BLOCKED:ERROR]\n[BLOCKED:END]\n");

			return 1;

		}

		else{

			for(int i=0; i<5; i++){

				if(GetClientByIP(List[ClientFD].BlockList[i])!=-1){

					exists=1;

					LIST[i]=List[GetClientByIP(List[ClientFD].BlockList[i])];

				}

			}	

		}

		if (exists==0){

			cse4589_print_and_log("[BLOCKED:ERROR]\n[BLOCKED:END]\n");

			return 1;

		}

		qsort(LIST, 5, sizeof(Client), compareClients);

		

		sprintf(ReturnM+strlen(ReturnM),"[BLOCKED:SUCCESS]\n");

		int id=1;

		for(int i=0; i<5; i++){



			if (LIST[i].FD!=-2){



				sprintf(ReturnM+strlen(ReturnM), "%-5d%-35s%-20s%-8d\n",id,LIST[i].Name,LIST[i].IPaddress, LIST[i].ListeningPort);



				id+=1;



		}



		}

		sprintf(ReturnM+strlen(ReturnM),"[BLOCKED:END]\n");

		cse4589_print_and_log(ReturnM);

	return	1;

}

















void remove_connection(int socket) {



		char* Ip=malloc(120);

		strcpy(Ip,GetIPAddress(socket));

		int ID=GetClientByIP(Ip);

		if (ID>-1){

			List[ID]=*Dummy;

		}

		close(socket);

		FD_CLR(socket, &master_list);

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







void SendMessage(char *Command,char *Arg1,char *Arg2,char *SenderIP,char *DataReceived, int sock_index){

	int Exists=0;

	Parse1(&Command,&Arg1,&Arg2,DataReceived);

	Arg1[strlen(Arg1)-1]='\0';

/*	printf("COMMAND IS *%s*\n",Command);*/

/*		printf("Arg1 IS *%s*\n",Arg1);*/

/*			printf("Arg2 IS *%s*\n",Arg2);*/

/*				printf("SenderIP IS *%s*\n",SenderIP);*/

	if (strcmp(Command,"SEND")==0){

		for (int i = 0; i < 5; i++) {

			Client currentClient = List[i];

				char *ClientIP=malloc(256*sizeof(char));

				strcpy(ClientIP,currentClient.IPaddress);

				if (strcmp(Arg1,ClientIP)==0){

					 	if(List[i].FD!=sock_index){



							Exists=1;

							char* MessageToSender=(char*) malloc(1024*sizeof(char));

							strcpy(MessageToSender,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,1));

							int MSLen=strlen(MessageToSender);

							send(sock_index,MessageToSender,MSLen,0);

/*							printf("SENT TO CLIENT: *%s*\n",MessageToSender);*/

							int NumberDestHasBlocked=List[i].NumberOfBlocked;

							int blocked=0;

							int DestID=GetClientByIP(Arg1);



							for (int j=0;j<NumberDestHasBlocked;j++){



								if(strcmp(List[i].BlockList[j],SenderIP)==0){



									

									blocked=1;

								}

							}

							if (blocked==0){

								List[GetClientByIP(SenderIP)].MessagesSent+=1;

								List[GetClientByIP(ClientIP)].MessagesReceived+=1;

								if (currentClient.LoggedIn==1){

									char* MessageToDest=(char*) malloc(1024*sizeof(char));

									strcpy(MessageToDest,MessageCreator(Arg2,"RECEIVED",GetIPAddress(sock_index),Arg1,1));

									int MDLen=strlen(MessageToDest);

/*									printf("SENDER IP IS: *%s*\nCLIENT IP IS: *%s*\n ARG2 is *%s*\n",SenderIP,ClientIP,Arg2);*/

									send(currentClient.FD,MessageToDest,MDLen,0);

									cse4589_print_and_log("[RELAYED:SUCCESS]\nmsg from:%s, to:%s\n[msg]:%s\n[RELAYED:END]\n",SenderIP,ClientIP, Arg2);

/*																printf("SENT TO CLIENT: *%s*\n",MessageToDest);*/



								}

								else{



									AddToBacklog(GetIPAddress(sock_index),ClientIP,Arg2);

								}

								break;

							}



						}

						

				}

			}

		if (Exists==0){



					char* MessageToSender=(char*)malloc(1024*sizeof(char));

					strcpy(MessageToSender,MessageCreator(Command,Command,Command,Command,0));

					int MSLen=strlen(MessageToSender);

					send(sock_index,MessageToSender,MSLen,0);

/*												printf("SENT TO CLIENT: *%s*\n",MessageToSender);*/

		}

}

}



void BroadcastMessage(char *Command,char *Arg1,char *Arg2,char *SenderIP,char *DataReceived, int sock_index){

	int success=0;

	int Exists=0;

		Parse1(&Command,&Arg1,&Arg2,DataReceived);

		for (int i = 0; i < 5; i++) {

			Client currentClient = List[i];

			char *ClientIP=malloc(256*sizeof(char));

			strcpy(ClientIP,currentClient.IPaddress);

		 	if((List[i].FD!=sock_index)&& (List[i].FD!=-2)){



				Exists=1;

				int blocked=0;

				int DestID=GetClientByIP(Arg1);

				int NumberDestHasBlocked=List[i].NumberOfBlocked;

				for (int j=0;j<NumberDestHasBlocked;j++){

								if(strcmp(List[i].BlockList[j],SenderIP)==0){



									

									blocked=1;

								}

							}

				if (blocked==0){

					List[GetClientByIP(SenderIP)].MessagesSent+=1;

					List[GetClientByIP(ClientIP)].MessagesReceived+=1;

					if (currentClient.LoggedIn==1){

					

						success==1;

						char* MessageToDest=(char*) malloc(1024*sizeof(char));

						strcpy(MessageToDest,MessageCreator(Arg1,"RECEIVED",GetIPAddress(sock_index),ClientIP,1));

						int MDLen=strlen(MessageToDest);

						send(currentClient.FD,MessageToDest,MDLen,0);

/*													printf("SENT TO CLIENT: *%s*\n",MessageToDest);*/

					}

					else{



						AddToBacklog(GetIPAddress(sock_index),ClientIP,Arg1);

					}

			}

			}

			}

			if (Exists==0){



						char* MessageToSender=(char*)malloc(1024*sizeof(char));

						strcpy(MessageToSender,MessageCreator(Command,Command,Command,Command,0));

						int MSLen=strlen(MessageToSender);

						send(sock_index,MessageToSender,MSLen,0);

/*													printf("SENT TO CLIENT: *%s*\n",MessageToSender);*/

			}

			else{

					char* MessageToSender=(char*) malloc(1024*sizeof(char));

					strcpy(MessageToSender,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,1));

					int MSLen=strlen(MessageToSender);

					send(sock_index,MessageToSender,MSLen,0);

					cse4589_print_and_log("[RELAYED:SUCCESS]\nmsg from:%s, to:%s\n[msg]:%s\n[RELAYED:END]\n",SenderIP,"255.255.255.255",Arg1);

/*												printf("SENT TO CLIENT: *%s*\n",MessageToSender);*/

			}

}

	// Initialize the server

char* statistics(const Client LIST[]) {

	char *ReturnM = malloc(1024*sizeof(char));

	int id=1;

	for(int i=0; i<5; i++){

		if (List[i].FD!=-2){

			char* Logged=malloc(30*sizeof(char));

			if (List[i].LoggedIn==1){

				Logged=malloc(9*sizeof(char));

				strcpy(Logged,"logged-in");

			}

			else{

				Logged=malloc(10*sizeof(char));

				strcpy(Logged,"logged-out");

			}

			



				sprintf(ReturnM+strlen(ReturnM),"%-5d%-35s%-8d%-8d%-8s\n", id,List[i].Name,List[i].MessagesSent,List[i].MessagesReceived,Logged);



				id+=1;



		}

	}

	return ReturnM;

	}

	



	int initialize_server(int port) {



	/*Backlog EmptyLog;*/

	EmptyLog = (Backlog*)malloc(sizeof(Backlog));



	/*Message Empty;*/

	Empty = (Message*)malloc(sizeof(Message));



	/*Client Dummy;*/

		Dummy = (Client*)malloc(sizeof(Client));

		Dummy->FD=-2;

		strcpy(Dummy->Name,"EMPTY");

		Dummy->ListeningPort=0;

		Dummy->MessagesReceived=0;

		Dummy->MessagesSent=0;

		Dummy->LoggedIn=0;

		for(int i=0;i<5;i++){

			strcpy(Dummy->BlockList[i],"");

		}

		strcpy(Dummy->IPaddress,"69");

		strcpy(Empty->SourceIP,"69");

		strcpy(EmptyLog->DestIP,"69");

		for(int i=0;i<5;i++){

			ListOfBacklogs[i]=*EmptyLog;

		}

		for (int i=0; i<5;i++){

			List[i]=*Dummy;

		}





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



								cse4589_print_and_log("[EXIT:SUCCESS]\n");



								cse4589_print_and_log("[EXIT:END]\n");



								exit(-1);



							}

							else if (strcmp(cmd,"AUTHOR")==0){



								handle_author_command();



							}



							else if (strcmp(cmd,"IP")==0){

								char* IP=malloc(120);

								strcpy(IP,handle_ip_command());

								cse4589_print_and_log("[IP:SUCCESS]\n");

								cse4589_print_and_log("IP:%s\n", IP);

								cse4589_print_and_log("[IP:END]\n");

							}



							else if (strcmp(cmd,"PORT")==0){



								handle_port_command(PORT);



							}

							else if (strcmp(cmd,"STATISTICS")==0){

								qsort(List, 5, sizeof(Client), compareClients);

								char *Data= malloc(2000);

								

								strcpy(Data,statistics(List));

								cse4589_print_and_log("[STATISTICS:SUCCESS]\n");

								cse4589_print_and_log("%s", Data);

								cse4589_print_and_log("[STATISTICS:END]\n");

								statistics(List);

							}

							else if (strcmp(cmd,"LIST")==0){



								char *DataToSend=RMessage();



								cse4589_print_and_log("[LIST:SUCCESS]\n");

								cse4589_print_and_log("%s", DataToSend);

								cse4589_print_and_log("[LIST:END]\n");



							}

							

							else{

								char *Command1= (char*) malloc(256*sizeof(char));

								char *AArg1= (char*) malloc(256*sizeof(char));

								char *AArg2 = (char*) malloc(256*sizeof(char));

								AArg1[strlen(AArg1)-1]='\0';

								char *SenderIP=(char*) malloc(256*sizeof(char));

								Parse1(&Command1,&AArg1,&AArg2,cmd);

								if (strcmp(Command1,"BLOCKED")==0){

									BlockedMessage(AArg1);

								}



								else{

									fflush(stdout);

								}



								}









						}



						

						//new client trying to login

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

							

							//trying to send backclogged messages

							for (int i=0;i<5;i++){

								char* BackLogIP=malloc(30*sizeof(char));

								strcpy(BackLogIP,ListOfBacklogs[i].DestIP);



								if(strcmp(BackLogIP,GetIPAddress(fdaccept))==0){

									int NumOfMessages=ListOfBacklogs[i].NumOfMessages;

									if (NumOfMessages>0){

										for(int j=0;j<NumOfMessages;j++){

											char* SourceIP=malloc(30*sizeof(char));

											char* Msg=malloc(256*sizeof(char));

											strcpy(SourceIP,ListOfBacklogs[i].MessageList[j].SourceIP);



											strcpy(Msg,ListOfBacklogs[i].MessageList[j].Message);



											strcpy(ListOfBacklogs[i].MessageList[j].Message,"");

											strcpy(ListOfBacklogs[i].MessageList[j].SourceIP,"");

											char* MessageToDest=(char*) malloc(1024*sizeof(char));

											strcpy(MessageToDest,MessageCreator(Msg,"RECEIVED",SourceIP,BackLogIP,1));

											int MDLen=strlen(MessageToDest);

											send(fdaccept,MessageToDest,MDLen,0);

											cse4589_print_and_log("[RELAYED:SUCCESS]\nmsg from:%s, to:%s\n[msg]:%s\n[RELAYED:END]\n",SourceIP,BackLogIP, Msg);

/*																													printf("SENT TO CLIENT: *%s*\n",MessageToDest);*/

										}								

									}

									ListOfBacklogs[i].NumOfMessages=0;

								}

							}

							qsort(List, 5, sizeof(Client), compareClients);

							

							char *DataToSend= RMessage();

					



							send(fdaccept,DataToSend,strlen(DataToSend),0);

/*														printf("SENT TO CLIENT: *%s*\n",DataToSend);*/

							char *LoginData=MessageCreator("LOGIN","LOGIN","LOGIN","LOGIN",1);

/*							printf("LOGIN DATA IS *%s*\n",LoginData);*/

							int f=send(fdaccept,LoginData,strlen(LoginData),0);

						}



					/* Read from existing clients */



						else{

							

							int *SOCKET = &sock_index;

							int Sock=*SOCKET;

							char *DataReceived= (char*) malloc(sizeof(char)*1023);

							

							if(recv(sock_index, DataReceived, 256, 0) <= 0){



								remove_connection(sock_index);

/*								printf("RECEIVED FROM CLIENT:*%s*\n",DataReceived);*/

							}

							else {

								

								if (strcmp(DataReceived,"LOGOUT")==0){

									LogClientOut(sock_index);

								}

								else if(strcmp(DataReceived,"LIST")==0){

									char *DataToSend=malloc(4000);

									sprintf(DataToSend,"[LIST:SUCCESS]\n");

									char *M=malloc(2000);

									strcpy(M,RMessage());

									sprintf(DataToSend+strlen(DataToSend),M);

									sprintf(DataToSend+strlen(DataToSend),"[LIST:END]\n");

									send(sock_index,DataToSend,strlen(DataToSend),0);

								

								}

								else if (strcmp(DataReceived,"REFRESH")==0){



									char *DataToSend=malloc(2000);

									strcpy(DataToSend,ReturnMessage(List,1));



									send(sock_index,DataToSend,strlen(DataToSend),0);





								}

								else{

									char *Command= (char*) malloc(256*sizeof(char));

									char *Arg1= (char*) malloc(256*sizeof(char));

									char *Arg2 = (char*) malloc(256*sizeof(char));

									char *SenderIP=(char*) malloc(256*sizeof(char));

									int Exists=0;

									Parse1(&Command,&Arg1,&Arg2,DataReceived);

									strcpy(SenderIP,GetIPAddress(sock_index));

									if (strcmp(Command,"SEND")==0){

										printf("SEND TRIGGERED\n");

										SendMessage(Command,Arg1,Arg2,SenderIP,DataReceived,sock_index);

									}

									else if (strcmp(Command,"BROADCAST")==0){

										BroadcastMessage(Command,Arg1,Arg2,SenderIP,DataReceived,sock_index);

									}

									else if (strcmp(Command,"BLOCK")==0){

										char* MessageToSender1=(char*) malloc(1024*sizeof(char));

										int BlockedID=GetClientByIP(Arg1);

										if(BlockedID>=0){

											int BlockSock=List[BlockedID].FD;

											int status=BlockClient(SenderIP,Arg1,sock_index,BlockSock);

											if(status==1){

												strcpy(MessageToSender1,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,1));

												int MSLen=strlen(MessageToSender1);

												send(sock_index,MessageToSender1,MSLen,0);

											}

											else{

												strcpy(MessageToSender1,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,0));

												int MSLen=strlen(MessageToSender1);

												send(sock_index,MessageToSender1,MSLen,0);

											}

											}

										else{

											strcpy(MessageToSender1,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,0));

											int MSLen=strlen(MessageToSender1);

											send(sock_index,MessageToSender1,MSLen,0);

										}

									}

							else if(strcmp(Command,"UNBLOCK")==0){

								char* MessageToSender2=(char*) malloc(1024*sizeof(char));

										int UnBlockedID=GetClientByIP(Arg1);

										if(UnBlockedID>=0){

											int UnBlockSock=List[UnBlockedID].FD;

											int status=UnBlockClient(SenderIP,Arg1,sock_index,UnBlockSock);

											if(status==1){

												strcpy(MessageToSender2,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,1));

												int MSLen=strlen(MessageToSender2);

												send(sock_index,MessageToSender2,MSLen,0);

											}

											else{

												strcpy(MessageToSender2,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,0));

												int MSLen=strlen(MessageToSender2);

												send(sock_index,MessageToSender2,MSLen,0);

											}

											}

										else{

											strcpy(MessageToSender2,MessageCreator(DataReceived,Command,GetIPAddress(sock_index),SenderIP,0));

											int MSLen=strlen(MessageToSender2);

											send(sock_index,MessageToSender2,MSLen,0);

										}

							}

				



						}



					}



				}



			}

		}

	}

	}

	}
