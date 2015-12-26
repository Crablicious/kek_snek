#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>


#include "ascii_lib/ascii_lib.h"
#include "defs.h"
#include "network.h"

/**
1 player starts as server. Forks to server and client automatically connects itself to server. 

Server only update position of objects. Server initiates all objects for clients, so they are uniform. 
 */

/**
Threads Server:
1. Receive keystrokes from clients
2. Server sends out new positions of changed objects. Every X/seconds.
 */



void get_players(int sockfd, int num_of_players){
  struct sockaddr_in inc_player;
  socklen_t inc_player_s;
  int init_msg_len = 20;
  char buffer[init_msg_len];
  ssize_t count;
  set_num_players(num_of_players);
  for(int i = 0; i < num_of_players; i++){
    count = recvfrom(sockfd, buffer, init_msg_len, 0, (struct sockaddr*)&inc_player, &inc_player_s);
    if(count < 0){
      pexit("Recvfrom error: ");
    }else{
      init_player(i, inc_player);
    }
  }
}

void distribute_field(){
  //initiate field size and number of objects.
  //send field;
}

int initiate_game(){
  int sockfd = init_socket(SOCK_DGRAM);
  bind_socket(sockfd, SERVER_PORT);
  get_players(sockfd, 2);
  distribute_field();

  return sockfd;
}

int main(int argc, char *argv[])
{
  int is_server = 0;
  pid_t pid = 0;
  static char address[20];
  if(argc == 2 && strcmp(argv[1], "1") == 0){
    is_server = 1;
    strcpy(address, "127.0.0.1");
  }else if(argc == 2){
    strcpy(address, argv[1]);
  }else{
    puts("Wrong number of arguments");
    exit(EXIT_FAILURE);
  }
  if(is_server){
    if((pid = fork()) < 0){
      pexit("Error creating child: ");
    }
  }
  if(pid){ //server
    int sockfd = initiate_game();

    wait(NULL); //Wait for Client to terminate.
  }else{ //client
    static char *argvc[]={"client", address, NULL};
    execv("./client", argvc);
    pexit("Error execv: ");
  }
  return 0;
}
    
