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
#include <sys/time.h>
#include "libpq-fe.h"
#include "math.h"
#include "game_model.cpp"

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
            char UUID_USER_SHORT[16];
            while (f && active) {
                memset(buf, 0, sizeof(buf));
                if (recv(fd, buf, 254, 0) > 0) {
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    if (!USER_LOGGED){
                        fprintf(stdout, "[from %s at %d-%02d-%02d %02d:%02d:%02d] %s\n", clientip, tm.tm_year + 1900,
                                tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);
                    }
                    else{
                        fprintf(stdout, "[from %s at %s at %d-%02d-%02d %02d:%02d:%02d] %s\n", UUID_USER_SHORT, clientip, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);
                    }
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
                                                if (send(fd, s_buf, strlen(s_buf), 0)) {
                                                    fprintf(stdout, "User %s registered\n", UUID_USER);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query,
                                                            "select * from \"Users\" where \"Identificator\" = '%s'",
                                                            UUID_USER);
                                                    PostGres_res = PQexec(PostGres_conn, query);
                                                    long id = atol(PQgetvalue(PostGres_res, 0, 0));
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query,
                                                            "insert into \"User_Priority\" values (DEFAULT, '%ld')",
                                                            id);
                                                    PQexec(PostGres_conn, query);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query,
                                                            "insert into \"User_Buildings\" values (DEFAULT, '%ld')",
                                                            id);
                                                    PQexec(PostGres_conn, query);
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query,
                                                            "insert into \"User_Research\" values (DEFAULT, '%ld')",
                                                            id);
                                                    PQexec(PostGres_conn, query);
                                                    USER_LOGGED = 1;
                                                    strncpy(UUID_USER_SHORT, UUID_USER, 12);
                                                    UUID_USER_SHORT[12] = '.';
                                                    UUID_USER_SHORT[13] = '.';
                                                    UUID_USER_SHORT[14] = '.';
                                                    UUID_USER_SHORT[15] = '\0';
                                                    success = 1;
                                                } else {
                                                    memset(query, 0, sizeof(query));
                                                    sprintf(query,
                                                            "DELETE FROM \"Users\" WHERE \"Identificator\" = '%s'",
                                                            UUID_USER);
                                                    PQexec(PostGres_conn, query);
                                                    fprintf(stderr, "couldn't send message: %s\n", strerror(errno));
                                                    break;
                                                }
                                            } else {
                                                fprintf(stderr, "Couldn't insert user: %s\n",
                                                        PQresultErrorMessage(PostGres_res));
                                                break;
                                            }
                                            PQclear(PostGres_res);
                                            PQfinish(PostGres_conn);
                                        } else {
                                            PQclear(PostGres_res);
                                            PQfinish(PostGres_conn);
                                        }
                                    } else {
                                        PQclear(PostGres_res);
                                        PQfinish(PostGres_conn);
                                        fprintf(stderr, "Failed to select users: %s\n",
                                                PQresultErrorMessage(PostGres_res));
                                        break;
                                    }
                                } else {
                                    PQfinish(PostGres_conn);
                                    fprintf(stderr, "Failed to connect to database: %s\n",
                                            PQerrorMessage(PostGres_conn));
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
                        if (strlen(user_attempt) != 128) {
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
                                        if (send(fd, "logged", 6, 0)) {
                                            fprintf(stdout, "User %s logged in\n", UUID_USER);
                                            strncpy(UUID_USER_SHORT, UUID_USER, 12);
                                            UUID_USER_SHORT[12] = '.';
                                            UUID_USER_SHORT[13] = '.';
                                            UUID_USER_SHORT[14] = '.';
                                            UUID_USER_SHORT[15] = '\0';
                                            USER_LOGGED = 1;
                                        } else {
                                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                                        }
                                    } else {
                                        send(fd, "failed login", 12, 0);
                                        fprintf(stderr, "User %s not found\n", user_attempt);
                                    }
                                } else {
                                    fprintf(stderr, "Failed to select users: %s\n", PQresultErrorMessage(PostGres_res));
                                }
                                PQclear(PostGres_res);
                            }
                            PQfinish(PostGres_conn);
                        }


                    } 
                    else if (strncmp(buf, "update", 6) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Updating user %s\n", UUID_USER_SHORT);
                        {
                            std::string info = std::string(Conn_info);
                            std::string uuid = std::string(UUID_USER);
                            auto previous_state = game_model::from_db(info,
                                                                            uuid);
                            if (!previous_state.has_value()){
                                fprintf(stderr, previous_state.error().c_str());
                            }
                            else{
                                struct timeval tv;
                                gettimeofday(&tv, NULL);
                                previous_state->dynamic_update_game_state((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
                                auto new_state = previous_state.value().state;
                                char message[16384];
                                memset(message, 0, sizeof(message));
                                sprintf(message,
                                        "updated %f %f %f %d %d %d %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %d %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf",
                                        new_state.build_priority, new_state.boost_priority,
                                        new_state.research_priority, new_state.focused_research, new_state.focused_building, new_state.dynamic_priority, 
                                        new_state.building_bits, new_state.building_bytes, new_state.building_kilo_packers,
                                        new_state.building_mega_packers, new_state.building_giga_packers,
                                        new_state.building_tera_packers, new_state.building_peta_packers,
                                        new_state.building_exa_packers, new_state.building_processes,
                                        new_state.building_overflows, new_state.research_bits_add,
                                        new_state.research_bits_mul, new_state.research_bytes_add,
                                        new_state.research_bytes_mul, new_state.research_kilo_add,
                                        new_state.research_kilo_mul, new_state.research_mega_add,
                                        new_state.research_mega_mul, new_state.research_giga_add,
                                        new_state.research_giga_mul, new_state.research_tera_add,
                                        new_state.research_tera_mul, new_state.research_peta_add,
                                        new_state.research_peta_mul, new_state.research_exa_add, new_state.research_exa_mul,
                                        new_state.research_process_mul, new_state.research_endgame);
                                if (send(fd, message, strlen(message), 0)) {
                                    previous_state->update_db(info,
                                                            uuid);
                                    fprintf(stdout, "User %s updated\n", UUID_USER_SHORT);
                                }
                                else{
                                    fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                                }
                            }
                        }
                    }
                    else if (strncmp(buf, "set_priority", 12) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Setting priority of %s\n", UUID_USER_SHORT);
                        auto previous_state = game_model::from_db(reinterpret_cast<std::string &>(Conn_info),
                                                                  reinterpret_cast<std::string &>(UUID_USER));
                        double boost_priority = 1;
                        double research_priority = 1;
                        double build_priority = 1;
                        int dynamic_priority = 0;
                        char dispose[128];
                        sscanf(buf, "%s %lf %lf %lf %d", dispose, &boost_priority, &research_priority, &build_priority, &dynamic_priority);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Priority set to %lf %lf %lf %d\n", boost_priority, research_priority, build_priority, dynamic_priority);
                            previous_state->state.boost_priority = boost_priority;
                            previous_state->state.research_priority = research_priority;
                            previous_state->state.build_priority = build_priority;
                            previous_state->state.dynamic_priority = dynamic_priority;
                            previous_state->update_db(reinterpret_cast<std::string &>(Conn_info),
                                                      reinterpret_cast<std::string &>(UUID_USER));
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "set_focus_building", 18) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Setting build focus of %s\n", UUID_USER_SHORT);
                        auto previous_state = game_model::from_db(reinterpret_cast<std::string &>(Conn_info),
                                                                  reinterpret_cast<std::string &>(UUID_USER));
                        char dispose[128];
                        int new_building = -1;
                        sscanf(buf, "%s %d", dispose, &new_building);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Building focus set to %d\n", new_building);
                            previous_state->state.focused_building = new_building;
                            previous_state->update_db(reinterpret_cast<std::string &>(Conn_info),
                                                      reinterpret_cast<std::string &>(UUID_USER));
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "set_focus_research", 18) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Setting research focus of %s\n", UUID_USER_SHORT);
                        auto previous_state = game_model::from_db(reinterpret_cast<std::string &>(Conn_info),
                                                                  reinterpret_cast<std::string &>(UUID_USER));
                        char dispose[128];
                        int new_research = -1;
                        sscanf(buf, "%s %d", dispose, &new_research);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Research focus set to %d\n", new_research);
                            previous_state->state.focused_research = new_research;
                            previous_state->update_db(reinterpret_cast<std::string &>(Conn_info),
                                                      reinterpret_cast<std::string &>(UUID_USER));
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "overflow", 8) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Overflowing %s\n", UUID_USER_SHORT);
                        auto previous_state = game_model::from_db(reinterpret_cast<std::string &>(Conn_info),
                                                                  reinterpret_cast<std::string &>(UUID_USER));
                        int success = 0;
                        switch (previous_state->state.building_overflows) {
                            case 0:
                                if (previous_state->state.building_bits >= 8){
                                    success = 1;
                                }
                                break;
                            case 1:
                                if (previous_state->state.building_bytes >= 1024){
                                    success = 1;
                                }
                                break;
                            case 2:
                                if (previous_state->state.building_kilo_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 3:
                                if (previous_state->state.building_mega_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 4:
                                if (previous_state->state.building_giga_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 5:
                                if (previous_state->state.building_tera_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 6:
                                if (previous_state->state.building_peta_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            default:
                                break;
                        }
                        if (success){
                            if (send(fd, "ok", 2, 0)) {
                                previous_state->reset_non_persistent_params();
                                previous_state->state.building_overflows += 1;
                                previous_state->update_db(reinterpret_cast<std::string &>(Conn_info),
                                                          reinterpret_cast<std::string &>(UUID_USER));
                            }
                            else{
                                fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                            }
                        }
                        else{
                            send(fd, "failed", 7, 0);
                            fprintf(stderr, "Failed sanity check\n");
                        }
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
