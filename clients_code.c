#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
int Socket(int domain, int type, int protocol){
        int res = socket(domain, type, protocol);//socket discriptor - ipv_4, TCP
        if(res == -1){
       		perror("cannot create socket!\n");
        	exit(EXIT_FAILURE);
        }

        return res;
}
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	int res = connect(sockfd, addr, addrlen);
    	if (res == -1) {
       		perror("connect failed");
        	exit(EXIT_FAILURE);
    }
}


timer_t vytvorCasovac(int signal){
  struct sigevent kam;
  kam.sigev_notify=SIGEV_SIGNAL;
  kam.sigev_signo=signal;
  
  timer_t casovac;
  timer_create(CLOCK_REALTIME, &kam, &casovac);
  return(casovac); 
}

void spustiCasovac(timer_t casovac, int sekundy)
{
  struct itimerspec casik;
  casik.it_value.tv_sec=sekundy;
  casik.it_value.tv_nsec=0;
  casik.it_interval.tv_sec=0;
  casik.it_interval.tv_nsec=0;
  timer_settime(casovac,CLOCK_REALTIME,&casik,NULL);
}

int main(){
	int fd = Socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in adr = {0};


	
    	adr.sin_family = AF_INET;
	adr.sin_port = htons(7777);

	adr.sin_addr.s_addr = inet_addr("127.0.0.1");
        Connect(fd, (struct sockaddr *) &adr, sizeof adr);

	 
	 write (fd, "HELLO\n", 6);
	 char buf[256];
    	 ssize_t nread;
   	 nread = read(fd, buf, 256);
    	 if (nread == -1) {
        	perror("read failed");
        	exit(EXIT_FAILURE);
   	 }				
   	 if (nread == 0) {
        	printf("EOF occured\n");
   	 }
   	 write(STDOUT_FILENO, buf, nread);

 	timer_t casovac;
 	casovac=vytvorCasovac(SIGKILL);


	
	double gains= 0;
	printf("Client1:");
	printf("Enter your gains:\n");	
	int i=0;
	

	if (scanf("%lf", &gains) != 1) {
	       printf("wrong input, program will be stopped in 5 seconds\n");
	       spustiCasovac(casovac,5);
	       sleep(10);

	}	       
    	send(fd, &gains, sizeof(gains), 0);


							/*
							 * 2 client
							 */
	sleep(3);
   	int fd2 = Socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in adr2 = {0};



        adr2.sin_family = AF_INET;
        adr2.sin_port = htons(7777);

        adr2.sin_addr.s_addr = inet_addr("127.0.0.1");
        Connect(fd2, (struct sockaddr *) &adr2, sizeof adr);
	double inv;
	printf("Client 2:");
	printf("Enter your cost of investment:\n");

	if (scanf("%lf", &inv) != 1) {
               printf("wrong input, program will be stopped in 5 seconds\n");
               spustiCasovac(casovac,5);
               sleep(10);

        }

  	send(fd2, &inv, sizeof(inv), 0);

							 /*
                                                         * 3 client
                                                         */
	sleep(3);
  	 int fd3 = Socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in adr3 = {0};



        adr3.sin_family = AF_INET;
        adr3.sin_port = htons(7777);

        adr3.sin_addr.s_addr = inet_addr("127.0.0.1");
        Connect(fd3, (struct sockaddr *) &adr3, sizeof adr);
        double inv3;
	
	double gains3;
	recv(fd3, &gains3, sizeof(gains3), 0);

        recv(fd3, &inv3, sizeof(inv3), 0);

	printf("Client 3 accepted info:\n");
	printf("Gains: %lf\n", gains3);
	printf("Cost of investment: %lf\n", inv3);
	double client3_calc = gains3 - inv3;

	send(fd3, &client3_calc, sizeof(client3_calc), 0);
							/*
							 * client 4
							 */
	int fd4 = Socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in adr4 = {0};



        adr4.sin_family = AF_INET;
        adr4.sin_port = htons(7777);

        adr4.sin_addr.s_addr = inet_addr("127.0.0.1");
        Connect(fd4, (struct sockaddr *) &adr4, sizeof adr4);

	double g_minus_i;
	recv(fd4, &g_minus_i, sizeof(g_minus_i), 0);
	double client4_calc = g_minus_i/inv;
	printf("client 4 calc: %lf\n ", client4_calc);
	  

	send(fd4, &client4_calc, sizeof(client4_calc), 0);
	

						
                                                       
                                                       


    	sleep(3);
    	close(fd);
    	close(fd2);
    	close(fd3);
	close(fd4);

	return 0;
}
