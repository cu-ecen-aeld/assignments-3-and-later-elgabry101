#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>   
#include <fcntl.h>     
#include <sys/types.h>
#include <sys/socket.h> 
#include <netdb.h>     
#include <arpa/inet.h>  
#include <signal.h>
#include <errno.h>
#include <syslog.h>
volatile sig_atomic_t exit_requested=0;

void handle_signal(int signo)
{
    exit_requested=1;
}

void handle_client(int client_fd, int write_fd)
{
    char temp[1024];       // temporary read buffer
    char *line = NULL;     // dynamic buffer to store one full message
    size_t total = 0;

    while (1) {
        ssize_t bytes = recv(client_fd, temp, sizeof(temp), 0);
        if (bytes < 0) {
            if (errno == EINTR) continue;
            perror("recv");
            break;
        }
        if (bytes == 0) break; // client closed

        // grow the dynamic buffer
        char *tmp = realloc(line, total + bytes);
        if (!tmp) {
            perror("realloc");
            free(line);
            break;
        }
        line = tmp;

        // copy received chunk into dynamic buffer
        memcpy(line + total, temp, bytes);
        total += bytes;

        // check if message contains newline
        if (memchr(temp, '\n', bytes)) {
            // write full message to file
            size_t written = 0;
            while (written < total) {
                ssize_t w = write(write_fd, line + written, total - written);
                if (w <= 0) {
                    if (errno == EINTR) continue;
                    perror("write");
                    break;
                }
                written += w;
            }

            // rewind file to beginning
            if (lseek(write_fd, 0, SEEK_SET) < 0) {
                perror("lseek");
                break;
            }

            // send entire file back to client
            ssize_t read_bytes;
            while ((read_bytes = read(write_fd, temp, sizeof(temp))) > 0) {
                ssize_t sent = 0;
                while (sent < read_bytes) {
                    ssize_t s = send(client_fd, temp + sent, read_bytes - sent, 0);
                    if (s > 0) {
                        syslog(LOG_INFO, "Sent %zd bytes to client_fd=%d", s, client_fd);
                        sent += s;
                    } else {
                        if (s < 0 && errno == EINTR) continue;
                        break;
                    }
                }
            }

            // rewind file for next message
            if (lseek(write_fd, 0, SEEK_SET) < 0) {
                perror("lseek");
                break;
            }

            // reset dynamic buffer for next message
            free(line);
            line = NULL;
            total = 0;
        }
    }

    free(line);
}
int main(int argc,char* argv[])
{
	openlog("aesdsocket", LOG_PID | LOG_CONS, LOG_USER);
	int daemon_mode = 0;
	if (argc == 2 && strcmp(argv[1], "-d") == 0) {
    	daemon_mode = 1;
	}
	printf("started server");
    struct sigaction sig;

    memset(&sig,0,sizeof(sig));
    sig.sa_handler=handle_signal;
    sigaction(SIGINT,&sig,NULL);
    sigaction(SIGTERM,&sig,NULL);
    int fd;
    fd = socket(PF_INET,SOCK_STREAM,0);
    struct addrinfo hints;
    struct addrinfo * res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;   
    if(getaddrinfo(NULL,"9000",&hints,&res)!=0)
    {
        fprintf(stderr,"error in getaddrinfo");
        exit(-1);
    }
    bind(fd,res->ai_addr,res->ai_addrlen);
    listen(fd,3);
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int write_fd = open("/var/tmp/aesdsocketdata", O_CREAT | O_RDWR | O_APPEND, 0644);
	if (daemon_mode) {
    	pid_t pid = fork();
    	if (pid < 0) {
        	perror("fork failed");
        	exit(EXIT_FAILURE);
    	}
    	if (pid > 0) {
        	// parent exits
        	exit(EXIT_SUCCESS);
    	}

    	// child continues as daemon
    	if (setsid() < 0) {
        	perror("setsid failed");
        	exit(EXIT_FAILURE);
    	}
	}
    while(!exit_requested)
    {
        int client_fd = accept(fd, (struct sockaddr *)&client_addr, &addr_size);
        struct sockaddr_in *addr_in = (struct sockaddr_in *)&client_addr;
		char *ip = inet_ntoa(addr_in->sin_addr);
		if(client_fd<0)
        {
            if(exit_requested){
                syslog(LOG_ERR,"Caught signal, exiting");
                break;
            }
            continue;
        }
        syslog(LOG_INFO,"Accepted connection from %s\n",ip);
        handle_client(client_fd,write_fd);
        syslog(LOG_INFO,"Closed connection from %s\n",ip);
        close(client_fd);
    }   
    close(write_fd);
    unlink("/var/tmp/aesdsocketdata");
    close(fd);
    freeaddrinfo(res);
	closelog(); 
}
