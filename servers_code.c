#include<stdio.h>
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
int Socket(int domain, int type, int protocol){
	  int res = socket(domain, type, protocol);
	  if(res == -1){
    perror("cannot create socket!\n");
	  exit(EXIT_FAILURE);
	}
	
	return res;
	
}	
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	  int res = bind(sockfd, addr, addrlen);
    if (res == -1){
	    perror ("cannot bind socket!\n");
	    exit(EXIT_FAILURE);

	}	
}

void Listen(int sockfd, int backlog){
	int res = listen(sockfd, backlog);
	if(res == -1){
		perror("cannot listen on socket\n");
		exit(EXIT_FAILURE);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
	int res = accept(sockfd, addr, addrlen);
	if(res == -1){
		  perror("cannot accept client\n");
		  exit(EXIT_FAILURE);
	}
	return res;

}
sem_t semaphore;
void *times_100(void*  a){
	sem_wait(&semaphore);
	double value =*((double*)a);
	double ROI = value * 100;
	sem_post(&semaphore);

  printf("ROI = %lf%%\n",ROI);
}
int main(){	
	int sock_desc = Socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in adr = {0};
	adr.sin_family = AF_INET;
	adr.sin_port = htons(7777);
	Bind(sock_desc,(struct sockaddr *) &adr, sizeof adr);
	Listen(sock_desc, 20);
	socklen_t adrlen = sizeof(adr);
	int fd = Accept(sock_desc, (struct sockaddr *) &adr, &adrlen);
	ssize_t nread;
	char buf[256];
	nread = read(fd, buf, 256);
	if (nread == -1){
		perror("cannot read from socket\n");
		exit(EXIT_FAILURE);
	
	}
	if (nread == 0){
		printf("EOF\n");
	}
	write(STDOUT_FILENO, buf, nread);
	write(fd, buf, nread);
	sleep(1);
	double gains;
	double inv;
	char gains_str[20];
	int pipefds[2];
	char buffer[20];
	 if(pipe(pipefds) == -1) {
   	  perror("pipe");
      exit(EXIT_FAILURE);
 	 }
	recv(fd, &gains, sizeof(gains), 0);

	 pid_t pid = fork();
	if(pid == 0) { // in child process

	
	  sprintf(gains_str, "%lf", gains);//str from int
	  close(pipefds[0]);
	  write(pipefds[1], gains_str, (sizeof(gains_str)));
	  sleep(2);//processes synch
	  exit(EXIT_SUCCESS);
	}

	if(pid > 0) { // in main process
	  sleep(1);
	  close(pipefds[1]);
	  int i=0;
	
	  read(pipefds[0], buffer,20);

  	printf("Client 1 sent me this info:\n");
	  printf("Gains:");
	  while(buffer[i] != '\0'){
		printf("%c", buffer[i]);
  	i++;
        }
	printf("\n\n");

	}	

	int fd2 = Accept(sock_desc, (struct sockaddr *) &adr, &adrlen);
	recv(fd2, &inv, sizeof(inv), 0);

	printf("Client 2 sent me this info:\n");
	printf("Cost of investment: %lf\n\n", inv);

	int fd3 = Accept(sock_desc, (struct sockaddr *) &adr, &adrlen);
	send(fd3, &gains, sizeof(gains), 0);
	send(fd3, &inv, sizeof(inv), 0);

	double client3_calc;
	recv(fd3, &client3_calc, sizeof(client3_calc), 0); 
	printf("Client 3 sent me this info:\n");
	printf("Gains - Cost of investment = %lf\n\n", client3_calc);
	
	int fd4 = Accept(sock_desc, (struct sockaddr *) &adr, &adrlen);
	send(fd4, &client3_calc, sizeof(client3_calc), 0);
	send(fd4, &inv, sizeof(inv), 0);
	double g_minus_i_dev_i;
	recv(fd4, &g_minus_i_dev_i, sizeof(g_minus_i_dev_i), 0);

	printf("Client 4 sent me this info:\n");
	

	printf("(Gains - Cost of investment) /Cost of investment =%lf\n\n",g_minus_i_dev_i );

	pthread_t thread1;
	sem_init(&semaphore, 0, 1);
	pthread_create(&thread1, NULL, times_100, (void*) &g_minus_i_dev_i); //NULL - standartne nastavenie vlakna
	pthread_join(thread1, NULL);

	sem_destroy(&semaphore);

  close(fd);
  close(fd2);
  close(fd3);
  close(fd4);	

  close(sock_desc);
 return 0;
}

