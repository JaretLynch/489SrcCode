

#include <netinet/in.h>

#include <arpa/inet.h>

#include <string.h> 

#include <stdio.h>

#include <unistd.h>

#include "Client.h"

#include "Commands.h"

#include "Server.h"

#include "../include/global.h"

#include "../include/logger.h"



void handle_author_command();

char* handle_ip_command();

void handle_port_command();



char local_ip[INET_ADDRSTRLEN];

int listening_port;



extern char* IPaddr;

void handle_author_command() {

    const char *command_str = "AUTHOR";

    const char *ubit_name = "Jaretlyn";  // TODO: Replace with your UBIT name

/*    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);*/

/*    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubit_name);*/

/*    cse4589_print_and_log("[%s:END]\n", command_str);*/

	cse4589_print_and_log("[%s:SUCCESS]\n", command_str);

	cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubit_name);

	cse4589_print_and_log("[%s:END]\n", command_str);

}







char* handle_ip_command(const char *Address) {

	struct sockaddr_in serv_addr, local_addr;

	socklen_t addr_len = sizeof(local_addr);

	int socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_port = htons(53);

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_addr.s_addr = inet_addr("8.8.8.8");



	if (connect(socket_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {

		perror("connect failed");

	return "ERROR";

	}



	if (getsockname(socket_desc, (struct sockaddr *)&local_addr, &addr_len) < 0) {

		perror("getsockname failed");

		return "ERROR";

	}





	char ip[INET_ADDRSTRLEN];

	

	inet_ntop(AF_INET, &local_addr.sin_addr, ip, INET_ADDRSTRLEN);

	strcpy(IPaddr,ip);

	close(socket_desc);

	return(IPaddr);

	}





void handle_port_command(int listening_port) {

    const char *command_str = "PORT";

/*    cse4589_print_and_log("[%s:SUCCESS]\n", command_str);*/

/*    cse4589_print_and_log("PORT:%d\n", listening_port);*/

/*    cse4589_print_and_log("[%s:END]\n", command_str);*/

	cse4589_print_and_log("[%s:SUCCESS]\n", command_str);

	cse4589_print_and_log("PORT:%d\n", listening_port);

	cse4589_print_and_log("[%s:END]\n", command_str);

}



