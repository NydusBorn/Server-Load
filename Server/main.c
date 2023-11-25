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
static const char ERROR_NO_PORT_GIVEN[] = "You have given no port\n";
static const char ERROR_NO_DATABASE_IP_GIVEN[] = "You have given no database_ip\n";
static const char ERROR_NO_DATABASE_PORT_GIVEN[] = "You have given no database_port\n";
static const char ERROR_NO_DATABASE_USER_GIVEN[] = "You have given no database_user\n";
static const char ERROR_NO_DATABASE_PASSWORD_GIVEN[] = "You have given no database_password\n";
static const char ERRPR_NO_DATABASE_NAME_GIVEN[] = "You have given no database_name\n";
static const char USAGE[] = "Usage: %s port database_ip database_port database_user database_password database_name\nport - which port to listen on\ndatabase_ip - IP of the server the database is located at\ndatabase_port - port used by the database connections on the server\ndatabase_user - username used to connect to the database\ndatabase_password - password used to connect to the database\ndatabase_name - name of the database\n";
static int active = 1;
static char Conn_info[4096];
char UUID_USER[129];

void q_signal_handler(int s) {
    (void) s;
    active = 0;
    fprintf(stdout, "Closing Server\n");
}

struct game_state{
    int focused_research;
    int focused_building;
    double build_priority;
    double boost_priority;
    double research_priority;
    long time;
    //Buildings
    double building_bits;
    double building_bytes;
    double building_kilo_packers;
    double building_mega_packers;
    double building_giga_packers;
    double building_tera_packers;
    double building_peta_packers;
    double building_exa_packers;
    double building_processes;
    double building_overflows;
    //Research
    double research_bits_add;
    double research_bits_mul;
    double research_bytes_add;
    double research_bytes_mul;
    double research_kilo_add;
    double research_kilo_mul;
    double research_mega_add;
    double research_mega_mul;
    double research_giga_add;
    double research_giga_mul;
    double research_tera_add;
    double research_tera_mul;
    double research_peta_add;
    double research_peta_mul;
    double research_exa_add;
    double research_exa_mul;
    double research_process_mul;
    double research_endgame;
}; 

struct game_state get_state_from_db() {
    struct game_state state;
    PGconn *PostGres_conn;
    PGresult *res;
    PostGres_conn = PQconnectdb(Conn_info);
    int success = 0;
    if (PQstatus(PostGres_conn) == CONNECTION_OK) {
        char query[1024];
        memset(query, 0, sizeof(query));
        sprintf(query, "select round(extract(epoch from \"Last seen\") * 1000) from \"Users\" where \"Identificator\" = '%s'", UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            goto escape;
        }
        char *t_value;
        t_value = PQgetvalue(res, 0, 0);
        state.time = atol(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query, "select \"Build_Priority\", \"Boost_Priority\", \"Research_Priority\", \"Focused_Research\", \"Focused_Building\" from \"User_Priority\" join \"Users\" on \"Users\".\"Pk\" = \"User_Priority\".\"Fk_User\" where \"Identificator\" = '%s'", UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            goto escape;
        }
        t_value = PQgetvalue(res, 0, 0);
        state.build_priority = atof(t_value);
        t_value = PQgetvalue(res, 0, 1);
        state.boost_priority = atof(t_value);
        t_value = PQgetvalue(res, 0, 2);
        state.research_priority = atof(t_value);
        t_value = PQgetvalue(res, 0, 3);
        state.focused_research = atoi(t_value);
        t_value = PQgetvalue(res, 0, 4);
        state.focused_building = atoi(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query, "select \"Bits\", \"Bytes\", \"Kilo_Packers\", \"Mega_Packers\", \"Giga_Packers\", \"Tera_Packers\", \"Peta_Packers\", \"Exa_Packers\", \"Processes\", \"Overflows\" from \"User_Buildings\" join \"Users\" on \"Users\".\"Pk\" = \"User_Buildings\".\"Fk_User\" where \"Identificator\" = '%s'", UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            goto escape;
        }
        t_value = PQgetvalue(res, 0, 0);
        state.building_bits = atof(t_value);
        t_value = PQgetvalue(res, 0, 1);
        state.building_bytes = atof(t_value);
        t_value = PQgetvalue(res, 0, 2);
        state.building_kilo_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 3);
        state.building_mega_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 4);
        state.building_giga_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 5);
        state.building_tera_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 6);
        state.building_peta_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 7);
        state.building_exa_packers = atof(t_value);
        t_value = PQgetvalue(res, 0, 8);
        state.building_processes = atof(t_value);
        t_value = PQgetvalue(res, 0, 9);
        state.building_overflows = atof(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query, "select \"Bits_Add\", \"Bits_Mul\", \"Bytes_Add\", \"Bytes_Mul\", \"Kilo_Add\", \"Kilo_Mul\", \"Mega_Add\", \"Mega_Mul\", \"Giga_Add\", \"Giga_Mul\", \"Tera_Add\", \"Tera_Mul\", \"Peta_Add\", \"Peta_Mul\", \"Exa_Add\", \"Exa_Mul\", \"Process_Multiplier\", \"Game_End\" from \"User_Research\" join \"Users\" on \"Users\".\"Pk\" = \"User_Research\".\"Fk_User\" where \"Identificator\" = '%s'", UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            goto escape;
        }
        t_value = PQgetvalue(res, 0, 0);
        state.research_bits_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 1);
        state.research_bits_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 2);
        state.research_bytes_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 3);
        state.research_bytes_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 4);
        state.research_kilo_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 5);
        state.research_kilo_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 6);
        state.research_mega_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 7);
        state.research_mega_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 8);
        state.research_giga_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 9);
        state.research_giga_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 10);
        state.research_tera_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 11);
        state.research_tera_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 12);
        state.research_peta_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 13);
        state.research_peta_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 14);
        state.research_exa_add = atof(t_value);
        t_value = PQgetvalue(res, 0, 15);
        state.research_exa_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 16);
        state.research_process_mul = atof(t_value);
        t_value = PQgetvalue(res, 0, 17);
        state.research_endgame = atof(t_value);
        PQclear(res);
        PQfinish(PostGres_conn);
        success = 1;
    }
    escape:
    if (!success) {
        fprintf(stderr, "Error code on PQconnectdb (if any): %s\n", PQerrorMessage(PostGres_conn));
        fprintf(stderr, "Error code on PQresultStatus (if any): %d\n", PQresultStatus(res));
        PQfinish(PostGres_conn);
        struct game_state fail_state;
        fail_state.time = -1;
        return fail_state;
    }
    else{
        return state;
    }
}



int main(int argc, char **argv) {
    ssize_t HELLO_SIZE = strlen(HELLO);

    int sockfd;
    struct sockaddr_in addr;

    setbuf(stdout, NULL);

    struct sigaction signal_action;
    signal_action.sa_handler = q_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    sigaction(SIGINT, &signal_action, NULL);
    printf("parsing arguments\n");
    switch (argc) {
        case 1:
            fprintf(stdout, ERROR_NO_PORT_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_IP_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PORT_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_USER_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PASSWORD_GIVEN);
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 2:
            fprintf(stdout, ERROR_NO_DATABASE_IP_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PORT_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_USER_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PASSWORD_GIVEN);
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 3:
            fprintf(stdout, ERROR_NO_DATABASE_PORT_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_USER_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PASSWORD_GIVEN);
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 4:
            fprintf(stdout, ERROR_NO_DATABASE_USER_GIVEN);
            fprintf(stdout, ERROR_NO_DATABASE_PASSWORD_GIVEN);
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 5:
            fprintf(stdout, ERROR_NO_DATABASE_PASSWORD_GIVEN);
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 6:
            fprintf(stdout, ERRPR_NO_DATABASE_NAME_GIVEN);
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        case 7: {
            int args_len = 100;
            args_len += strlen(argv[2]);
            args_len += strlen(argv[3]);
            args_len += strlen(argv[4]);
            args_len += strlen(argv[5]);
            args_len += strlen(argv[6]);
            if (args_len > 4096) {
                fprintf(stderr, "Too long arguments\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "Starting Server\n");
            break;
        }
        default:
            fprintf(stdout, USAGE, argv[0]);
            exit(EXIT_FAILURE);
    }

    int PORT = atoi(argv[1]);
    PGconn *PostGres_conn;
    PGresult *PostGres_res;
    {
        sprintf(Conn_info, "hostaddr=%s port=%s user=%s password=%s dbname=%s", argv[2], argv[3], argv[4], argv[5],
                argv[6]);
        fprintf(stdout, "Attempting contact with database server\n");
        PostGres_conn = PQconnectdb(Conn_info);
        if (PQstatus(PostGres_conn) != CONNECTION_OK) {
            fprintf(stderr, "Unable to connect to database: %s\n",
                    PQerrorMessage(PostGres_conn));
            PQfinish(PostGres_conn);
            exit(EXIT_FAILURE);
        }
        PQfinish(PostGres_conn);
        fprintf(stdout, "Connection succesful\n");
    }

    fprintf(stdout, "Opening port %d\n", PORT);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    fprintf(stdout, "Creating socket\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Unable to create socket: %s\n",
                strerror(errno));
        return 1;
    }
    fprintf(stdout, "Socket created\n");
    fprintf(stdout, "Binding socket\n");
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Unable to bind socket: %s\n",
                strerror(errno));
        close(sockfd);
        return 1;
    }
    fprintf(stdout, "Socket bound\n");
    fprintf(stdout, "Starting to listen for connections\n");
    if (listen(sockfd, 5) < 0) {
        fprintf(stderr, "Unable to listen socket: %s\n",
                strerror(errno));
        close(sockfd);
        return 1;
    }
    fprintf(stdout, "Port %d opened for connections\n", PORT);
    fprintf(stdout, "Listing available IP addresses\n");
    {
        char hostbuffer[256];

        gethostname(hostbuffer, sizeof(hostbuffer));

        fprintf(stdout, "Server running at %s\n", hostbuffer);
        fprintf(stdout, "local ips:\n");
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
                    fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }

                fprintf(stdout, "\tAt %s:%s address: %s:%d\n", ifa->ifa_name, "ipv4", host, PORT);
            }
        }

        freeifaddrs(ifaddr);
    }
    {
        FILE *comm = popen("curl https://api.ipify.org", "r");
        char buf_comm[1024];
        fgets(buf_comm, 1024, comm);
        fprintf(stdout, "public ip: %s\n", buf_comm);
        pclose(comm);
    }
    fprintf(stdout, "Listed available IP addresses\n");

    int server_closure = 1;

    while (active) {
        struct sockaddr *ca = NULL;
        socklen_t sz = 0;
        int fd;
        pid_t pid;
        fprintf(stdout, "Waiting for connection\n");
        fd = accept(sockfd, ca, &sz);
        if (fd < 0) {
            fprintf(stderr, "Unable to accept connection: %s\n",
                    strerror(errno));
            continue;
        }
        fprintf(stdout, "Connection accepted\n");
        fprintf(stdout, "Forking process\n");
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Unable to fork process: %s\n",
                    strerror(errno));
            continue;
        }
        fprintf(stdout, "Process with pid %d created\n", pid);
        if (pid > 0) {
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
                getpeername(fd, (struct sockaddr *) &client_addr, &addr_size);
                strcpy(clientip, inet_ntoa(client_addr.sin_addr));
                fprintf(stdout, "[%s entered at %d-%02d-%02d %02d:%02d:%02d]\n", clientip, tm.tm_year + 1900,
                        tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            }
            
            int USER_LOGGED = 0;
            while (f) {
                memset(buf, 0, sizeof(buf));
                if (recv(fd, buf, 254, 0) > 0) {
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    fprintf(stdout, "[from %s at %d-%02d-%02d %02d:%02d:%02d] %s\n", clientip, tm.tm_year + 1900,
                            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);
                    if (strncmp(buf, "register", 8) == 0 && !USER_LOGGED) {
                        fprintf(stdout, "Registering user\n");
                        {
                            int success = 0;
                            memset(UUID_USER, 0, sizeof(UUID_USER));
                            while (!success) {
                                for (int i = 0; i < 128; ++i) {
                                    UUID_USER[i] = (char) ('A' + (rand() % 20));
                                }
                                PostGres_conn = PQconnectdb(Conn_info);
                                if (PQstatus(PostGres_conn) == CONNECTION_OK) {
                                    char query[1024];
                                    memset(query, 0, sizeof(query));
                                    sprintf(query, "Select * from \"Users\" where \"Identificator\" = '%s'", UUID_USER);
                                    PostGres_res = PQexec(PostGres_conn, query);
                                    if (PQresultStatus(PostGres_res) == PGRES_TUPLES_OK) {
                                        if (PQntuples(PostGres_res) == 0) {
                                            PQclear(PostGres_res);
                                            memset(query, 0, sizeof(query));
                                            sprintf(query, "INSERT INTO \"Users\" VALUES (DEFAULT, '%s')", UUID_USER);
                                            PostGres_res = PQexec(PostGres_conn, query);
                                            if (PQresultStatus(PostGres_res) == PGRES_COMMAND_OK) {
                                                char s_buf[1024];
                                                memset(s_buf, 0, sizeof(s_buf));
                                                sprintf(s_buf, "registered %s", UUID_USER);
                                                if (send(fd, s_buf, strlen(s_buf), 0)){
                                                    fprintf(stdout, "User %s registered\n", UUID_USER);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query, "insert into \"User_Priority\" values (DEFAULT, '%s')", UUID_USER);
                                                    PQexec(PostGres_conn, query);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query, "insert into \"User_Buildings\" values (DEFAULT, '%s')", UUID_USER);
                                                    PQexec(PostGres_conn, query);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query, "insert into \"User_Research\" values (DEFAULT, '%s')", UUID_USER);
                                                    PQexec(PostGres_conn, query);
                                                    USER_LOGGED = 1;
                                                    success = 1;
                                                }
                                                else{
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query, "DELETE FROM \"Users\" WHERE \"Identificator\" = '%s'", UUID_USER);
                                                    PQexec(PostGres_conn, query);
                                                    fprintf(stderr, "couldn't send message: %s\n", strerror(errno));
                                                    break;
                                                }
                                            }else{
                                                fprintf(stderr, "Couldn't insert user: %s\n", PQresultErrorMessage(PostGres_res));
                                                break;
                                            }
                                            PQclear(PostGres_res);
                                            PQfinish(PostGres_conn);
                                        }
                                        else{
                                            PQclear(PostGres_res);
                                            PQfinish(PostGres_conn);
                                        }
                                    }
                                    else{
                                        PQclear(PostGres_res);
                                        PQfinish(PostGres_conn);
                                        fprintf(stderr, "Failed to select users: %s\n", PQresultErrorMessage(PostGres_res));
                                        break;
                                    }
                                }
                                else{
                                    PQfinish(PostGres_conn);
                                    fprintf(stderr, "Failed to connect to database: %s\n", PQerrorMessage(PostGres_conn));
                                    f = 0;
                                    break;
                                }
                            }
                            if (!success) {
                                fprintf(stderr, "Failed to register user\n");
                            }
                        }
                    }
                    else if (strncmp(buf, "login", 5) == 0 && !USER_LOGGED) {
                        char *user_attempt;
                        user_attempt = strdup(buf);
                        strsep(&user_attempt, " ");
                        user_attempt = strsep(&user_attempt, " ");
                        //TODO: Check for memory leaks with valgrind or something
                        if (strlen(user_attempt) != 128){
                            fprintf(stderr, "Incorrect login length\n");
                            continue;
                        } 
                        fprintf(stdout, "Logging in user\n");
                        {
                            PostGres_conn = PQconnectdb(Conn_info);
                            if (PQstatus(PostGres_conn) == CONNECTION_OK) {
                                char query[1024];
                                memset(query, 0, sizeof(query));
                                sprintf(query, "Select * from \"Users\" where \"Identificator\" = '%s'", user_attempt);
                                PostGres_res = PQexec(PostGres_conn, query);
                                if (PQresultStatus(PostGres_res) == PGRES_TUPLES_OK) {
                                    if (PQntuples(PostGres_res) == 1) {
                                        memcpy(UUID_USER, user_attempt, strlen(user_attempt));
                                        UUID_USER[128] = '\0';
                                        if (send(fd, "logged", 6, 0)){
                                            fprintf(stdout, "User %s logged in\n", UUID_USER);
                                            USER_LOGGED = 1;
                                        }
                                        else{
                                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                                        }
                                    }
                                    else{
                                        send(fd, "failed login", 12, 0);
                                        fprintf(stderr, "User %s not found\n", user_attempt);
                                    }
                                }
                                else{
                                    fprintf(stderr, "Failed to select users: %s\n", PQresultErrorMessage(PostGres_res));
                                }
                                PQclear(PostGres_res);
                            }
                            PQfinish(PostGres_conn);
                        }
                        
                        
                    }
                    else if (strncmp(buf, "update", 6) == 0 && USER_LOGGED) {
                        //TODO
                    }
                } else {
                    f = 0;
                }
            }
            
            fprintf(stdout, "Closing connection from %s\n", clientip);
            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(int));
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
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(int));
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);

        fprintf(stdout, "Closed Server appropriately\n");
    }

    return EXIT_SUCCESS;
}
