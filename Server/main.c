#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <time.h>
#include "libpq-fe.h"

static const char HELLO[] = "Welcome to Server Load instance (version 0.1)\nType 'quit' to close the connection\n";
static const char ERROR_NO_PORT_GIVEN[] = "You have given no port, using the default:45682\n";
static int active = 1;

void q_signal_handler(int s) {
    (void) s;
    active = 0;
}


int main(int argc, char **argv) {
    ssize_t HELLO_SIZE = strlen(HELLO);
    int PORT = 45682;

    int sockfd;
    struct sockaddr_in addr;


    struct sigaction signal_action;
    signal_action.sa_handler = q_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    sigaction(SIGINT, &signal_action, NULL);

    if (argc != 2) {
        fprintf(stdout, "Usage: %s port\n", argv[0]);
        printf(ERROR_NO_PORT_GIVEN);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (argc == 2) {
        PORT = atoi(argv[1]);
    }
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Unable to create socket: %s\n",
                strerror(errno));
        return 1;
    }
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Unable to bind socket: %s\n",
                strerror(errno));
        close(sockfd);
        return 1;
    }
    if (listen(sockfd, 5) < 0) {
        fprintf(stderr, "Unable to listen socket: %s\n",
                strerror(errno));
        close(sockfd);
        return 1;
    }

    {
        char hostbuffer[256];

        gethostname(hostbuffer, sizeof(hostbuffer));

        printf("Server running at %s\n", hostbuffer);
        printf("local ips:\n");
    }

    {
        struct ifaddrs *ifaddr;
        int family, s;
        char host[NI_MAXHOST];

        if (getifaddrs(&ifaddr) == -1) {
            perror("getifaddrs");
            exit(EXIT_FAILURE);
        }

        for (struct ifaddrs *ifa = ifaddr; ifa != NULL;
             ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;

            family = ifa->ifa_addr->sa_family;

            if (family == AF_INET) {
                s = getnameinfo(ifa->ifa_addr,
                                sizeof(struct sockaddr_in),
                                host, NI_MAXHOST,
                                NULL, 0, NI_NUMERICHOST);
                if (s != 0) {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }

                printf("\tAt %s:%s address: %s:%d\n", ifa->ifa_name, "ipv4", host, PORT);
            }
        }

        freeifaddrs(ifaddr);
    }
    {
        FILE  *comm = popen("curl https://api.ipify.org", "r");
        char buf_comm[1024];
        fgets(buf_comm, 1024, comm);
        printf("public ip: %s\n", buf_comm);
        pclose(comm);
    }

    int server_closure = 1;

    while (active) {
        struct sockaddr *ca = NULL;
        socklen_t sz = 0;
        int fd;
        pid_t pid;

        if ((fd = accept(sockfd, ca, &sz)) < 0) {
            fprintf(stderr, "Unable to listen socket: %s\n",
                    strerror(errno));
        } else if ((pid = fork()) < 0) {
            fprintf(stderr, "Unable to fork process: %s\n",
                    strerror(errno));
        } else if (pid > 0) {
            close(fd);
        } else if (send(fd, HELLO, HELLO_SIZE, 0) == HELLO_SIZE) {
            int f = 1;
            char buf[255];
            char clientip[20];
            {
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                struct sockaddr_in client_addr;
                socklen_t addr_size = sizeof(struct sockaddr_in);
                getpeername(fd, (struct sockaddr *)&client_addr, &addr_size);
                strcpy(clientip, inet_ntoa(client_addr.sin_addr));
                printf("[%s entered at %d-%02d-%02d %02d:%02d:%02d]\n", clientip, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            }
            while (f) {
                memset(buf, 0, sizeof(buf));
                if ((recv(fd, buf, 254, 0)) > 0) {
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    printf("[from %s at %d-%02d-%02d %02d:%02d:%02d] %s\n", clientip, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);
                    if (strncmp(buf, "quit", 4) == 0) {
                        f = 0;
                    }
                }
            }

            shutdown(fd, SHUT_RDWR);
            close(fd);
            active = 0;
            server_closure = 0;
        } else {
            fprintf(stderr, "Unable to send greeting message: %s\n",
                    strerror(errno));

            close(fd);
        }
    }


    if (server_closure) {
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,  NULL, sizeof(int));
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);

        printf("Closed Server appropriately\n");
    }

    return 0;
}
