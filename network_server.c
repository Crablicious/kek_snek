#define _BSD_SOURCE

#include <sys/types.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "defs.h"
#include "network_server.h"

#define ACK_TO 1

static struct sockaddr_in *players;
int g_num_of_players;

int get_num_players(){
  return g_num_of_players;
}

void set_num_players(int num_of_players){
  players = calloc((num_of_players+1), sizeof(struct sockaddr_in));
  g_num_of_players = num_of_players;
}

void init_player(int num, struct sockaddr_in player){
  players[num].sin_family = player.sin_family;
  players[num].sin_port = player.sin_port;
  players[num].sin_addr.s_addr = player.sin_addr.s_addr;
}

void send_player(int sockfd, int num, char *msg, int catID){
  ssize_t count;
  if(catID){
      char appBuf[5];
      sprintf(appBuf, " %d", num);
      strcat(msg, appBuf);
  }
  count = sendto(sockfd, msg, strlen(msg)+1, 0, (struct sockaddr*)&players[num], sizeof(struct sockaddr_in));
  if(count < 0){
    pexit("Error sending: ");
  }
}

void send_all(int sockfd, char *msg, int catID){
  ssize_t count;  
  for(int i = 0; players[i].sin_port != 0; i++){
    if(catID){
      char appBuf[5];
      sprintf(appBuf, " %d", i);
      strcat(msg, appBuf);
    }
    count = sendto(sockfd, msg, strlen(msg)+1, 0, (struct sockaddr*)&players[i], sizeof(struct sockaddr_in));
    if(count < 0){
      pexit("Error sending: ");
    }
  }
}

void ack_player(int sockfd, int num, int ack_num){
  ssize_t count;
  char buffer[20];
  count = read(sockfd, buffer, 20); 
}

void ack_all(int sockfd, char *resend_msg){
  ssize_t count;
  int currPlayer;
  char buffer[20];
  char check_list[g_num_of_players];
  memset(check_list, 0, g_num_of_players);

  int selectVal;  
  struct timeval timeout;
  fd_set fds;
   
  for(int i = 0; i < g_num_of_players; i++){
    timeout.tv_sec = ACK_TO;
    timeout.tv_usec = 0;  

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);  
    
    selectVal = select(sockfd + 1, &fds, NULL, NULL, &timeout);
    if(selectVal == -1){
      pexit("Error select");
    }else if(selectVal == 1){
      count = read(sockfd, buffer, 20);
      if(count < 0){
        pexit("Error reading socket: ");
      }else{
        sscanf(buffer, "1 %d", &currPlayer);
        check_list[currPlayer] = 1;
      }
    }else if(selectVal == 0){ //TO
      for(int j = 0; j < g_num_of_players; j++){
        if(check_list[j] == 0){
          send_player(sockfd, j, resend_msg, 1);
        }
      }
    }
  }
}



void free_all_players(){
  free(players);
}
