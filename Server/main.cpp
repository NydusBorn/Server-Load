#include <thread>

#include "cerrno"
#include "chrono"
#include "cmath"
#include "csignal"
#include "cstdlib"
#include "cstring"
#include "game_model.cpp"
#include "ifaddrs.h"
#include "libpq-fe.h"
#include "netdb.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "linux/if_link.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "sys/time.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "random"
#include "pthread.h"
#include "semaphore.h"

static const std::string HELLO = "Welcome to Server Load instance (version 0.2)\nType 'quit' to close the connection\n";
static bool active = true;
static bool verbose = false;
static std::string Conn_info;
static std::mt19937 generator;
static sem_t sem_poster;
static int thread_counter = 0;
static int active_threads = 0;
static pthread_mutex_t mutex_sync = PTHREAD_MUTEX_INITIALIZER;
static int data_post;
static int sockfd;

void q_signal_handler(int s) {
    (void) s;
    active = false;
    std::cout << "Closing Server, please wait" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, nullptr, sizeof(int));
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    sem_destroy(&sem_poster);

    std::cout << "Closed Server appropriately" << std::endl;
}

void *thread_worker(void *arg) {
    int fd;
    PGconn *PostGres_conn;
    PGresult *PostGres_res;
    pthread_mutex_lock(&mutex_sync);
    if (verbose) {
        std::cout << std::format("Thread {} created\n", thread_counter);
    }
    thread_counter++;
    pthread_mutex_unlock(&mutex_sync);

    while (true) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 5;
        auto wait_res = sem_timedwait(&sem_poster, &ts);
        if (!active) {
            break;
        }
        pthread_mutex_lock(&mutex_sync);
        if (wait_res != 0) {
            if (thread_counter >= 11) {
                pthread_mutex_unlock(&mutex_sync);
                break;
            }
            pthread_mutex_unlock(&mutex_sync);
            continue;
        }
        if (verbose) {
            std::cout << std::format("Thread got data\n");
        }
        fd = data_post;
        active_threads += 1;
        pthread_mutex_unlock(&mutex_sync);


        if (send(fd, HELLO.c_str(), HELLO.size(), 0) == HELLO.size()) {
            bool user_connected = true;
            char buf[255];
            char clientip[20];
            if (verbose) {
                time_t t = time(nullptr);
                tm tm = *localtime(&t);
                sockaddr_in client_addr;
                socklen_t addr_size = sizeof(struct sockaddr_in);
                getpeername(fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
                strcpy(clientip, inet_ntoa(client_addr.sin_addr));
                fprintf(stdout, "[%s entered at %d-%02d-%02d %02d:%02d:%02d]\n", clientip, tm.tm_year + 1900,
                        tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            }

            bool user_logged = false;
            std::string UUID_USER;
            std::string UUID_USER_SHORT;
            while (user_connected && active) {
                memset(buf, 0, sizeof(buf));
                if (recv(fd, buf, 254, 0) > 0) {
                    if (verbose) {
                        time_t t = time(nullptr);
                        struct tm tm = *localtime(&t);
                        if (!user_logged) {
                            fprintf(stdout, "[from %s at %d-%02d-%02d %02d:%02d:%02d] %s\n", clientip,
                                    tm.tm_year + 1900,
                                    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, buf);
                        } else {
                            fprintf(stdout, "[from %s at %s at %d-%02d-%02d %02d:%02d:%02d] %s\n",
                                    UUID_USER_SHORT.c_str(),
                                    clientip, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
                                    tm.tm_sec, buf);
                        }
                    }

                    if (static_cast<const std::string &>(buf) == "register" && !user_logged) {
                        if (verbose) {
                            std::cout << "Registering user" << std::endl;
                        } {
                            bool success = false;
                            std::uniform_int_distribution<int> distribution(0, 25);
                            while (!success) {
                                UUID_USER.clear();
                                for (int i = 0; i < 128; ++i) {
                                    UUID_USER.push_back(static_cast<char>('A' + distribution(generator)));
                                }
                                PostGres_conn = PQconnectdb(Conn_info.c_str());
                                if (PQstatus(PostGres_conn) == CONNECTION_OK) {
                                    std::string query;
                                    query = std::format("Select * from \"Users\" where \"Identificator\" = '{}'",
                                                        UUID_USER);
                                    PostGres_res = PQexec(PostGres_conn, query.c_str());
                                    if (PQresultStatus(PostGres_res) == PGRES_TUPLES_OK) {
                                        if (PQntuples(PostGres_res) == 0) {
                                            PQclear(PostGres_res);
                                            query = std::format("INSERT INTO \"Users\" VALUES (DEFAULT, '{}')",
                                                                UUID_USER);
                                            PostGres_res = PQexec(PostGres_conn, query.c_str());
                                            if (PQresultStatus(PostGres_res) == PGRES_COMMAND_OK) {
                                                std::string s_buf = std::format("registered {}", UUID_USER);
                                                if (send(fd, s_buf.c_str(), s_buf.size(), 0)) {
                                                    if (verbose) {
                                                        std::cout << std::format("User {} registered\n", UUID_USER);
                                                    }

                                                    query = std::format(
                                                        "select * from \"Users\" where \"Identificator\" = '{}'",
                                                        UUID_USER);
                                                    PostGres_res = PQexec(PostGres_conn, query.c_str());
                                                    long id = std::stol((PQgetvalue(PostGres_res, 0, 0)));
                                                    query = std::format(
                                                        "insert into \"User_Priority\" values (DEFAULT, '{}')",
                                                        id);
                                                    PQexec(PostGres_conn, query.c_str());
                                                    query = std::format(
                                                        "insert into \"User_Buildings\" values (DEFAULT, '{}')",
                                                        id);
                                                    PQexec(PostGres_conn, query.c_str());
                                                    query = std::format(
                                                        "insert into \"User_Research\" values (DEFAULT, '{}')",
                                                        id);
                                                    PQexec(PostGres_conn, query.c_str());
                                                    user_logged = true;
                                                    UUID_USER_SHORT = UUID_USER.substr(0, 12);
                                                    for (int i = 0; i < 3; ++i) {
                                                        UUID_USER_SHORT.push_back('.');
                                                    }
                                                    success = true;
                                                } else {
                                                    query = std::format(
                                                        "DELETE FROM \"Users\" WHERE \"Identificator\" = '{}'",
                                                        UUID_USER);
                                                    PQexec(PostGres_conn, query.c_str());
                                                    if (verbose) {
                                                        std::cerr << std::format(
                                                            "Couldn't send message: {}\n", strerror(errno));
                                                    }
                                                    break;
                                                }
                                            } else {
                                                if (verbose) {
                                                    std::cerr << std::format("Couldn't insert user: {}\n",
                                                                             PQresultErrorMessage(PostGres_res));
                                                }
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
                                        if (verbose) {
                                            std::cerr << std::format("Failed to select users: {}\n",
                                                                 PQresultErrorMessage(PostGres_res));
                                        }
                                        break;
                                    }
                                } else {
                                    PQfinish(PostGres_conn);
                                    if (verbose) {
                                        std::cerr << std::format("Failed to connect to database: {}\n",
                                                             PQerrorMessage(PostGres_conn));
                                    }
                                    user_connected = false;
                                    break;
                                }
                            }
                            if (!success) {
                                if (verbose) {
                                    std::cerr << "Failed to register user" << std::endl;
                                }
                            }
                        }
                    } else if (static_cast<const std::string &>(buf).starts_with("login ") && !user_logged) {
                        std::string user_attempt;
                        user_attempt = static_cast<const std::string &>(buf).substr(6, 128);
                        if (user_attempt.size() != 128) {
                            if (verbose) {
                                std::cerr << "Incorrect login length" << std::endl;
                            }
                            continue;
                        }
                        if (verbose) {
                            std::cout << "Logging in user" << std::endl;
                        } {
                            PostGres_conn = PQconnectdb(Conn_info.c_str());
                            if (PQstatus(PostGres_conn) == CONNECTION_OK) {
                                std::string query;
                                query = std::format("Select * from \"Users\" where \"Identificator\" = '{}'",
                                                    user_attempt);
                                PostGres_res = PQexec(PostGres_conn, query.c_str());
                                if (PQresultStatus(PostGres_res) == PGRES_TUPLES_OK) {
                                    if (PQntuples(PostGres_res) == 1) {
                                        UUID_USER = user_attempt;
                                        if (send(fd, "logged", 6, 0)) {
                                            if (verbose) {
                                                std::cout << std::format("User {} logged in\n", UUID_USER);
                                            }
                                            UUID_USER_SHORT = UUID_USER.substr(0, 12);
                                            for (int i = 0; i < 3; ++i) {
                                                UUID_USER_SHORT.push_back('.');
                                            }
                                            user_logged = true;
                                        } else {
                                            if (verbose) {
                                                std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                                            }
                                        }
                                    } else {
                                        send(fd, "failed login", 12, 0);
                                        if (verbose) {
                                            std::cerr << std::format("User {} not found\n", user_attempt);
                                        }
                                    }
                                } else {
                                    if (verbose) {
                                        std::cerr << std::format("Failed to select users: {}\n",
                                                             PQresultErrorMessage(PostGres_res));
                                    }
                                }
                                PQclear(PostGres_res);
                            }
                            PQfinish(PostGres_conn);
                        }
                    } else if (static_cast<const std::string &>(buf) == "update" && user_logged) {
                        if (verbose) {
                            std::cout << std::format("Updating user {}\n", UUID_USER_SHORT);
                        } {
                            auto previous_state = game_model::from_db(Conn_info,
                                                                      UUID_USER);
                            if (verbose &&!previous_state.has_value()) {
                                std::cerr << previous_state.error();
                            } else {
                                timeval tv;
                                gettimeofday(&tv, nullptr);
                                previous_state->dynamic_update_game_state((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
                                auto new_state = previous_state.value().state;
                                std::string message;
                                message = std::format(
                                    "updated {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}",
                                    new_state.build_priority, new_state.boost_priority,
                                    new_state.research_priority, new_state.focused_research,
                                    new_state.focused_building, new_state.dynamic_priority,
                                    new_state.building_bits, new_state.building_bytes,
                                    new_state.building_kilo_packers,
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
                                    new_state.research_peta_mul, new_state.research_exa_add,
                                    new_state.research_exa_mul,
                                    new_state.research_process_mul, new_state.research_endgame);
                                if (send(fd, message.c_str(), message.size(), 0)) {
                                    previous_state->update_db(Conn_info,
                                                              UUID_USER);
                                    if (verbose) {
                                        std::cout << std::format("User {} updated\n", UUID_USER_SHORT);
                                    }
                                } else {
                                    if (verbose) {
                                        std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                                    }
                                }
                            }
                        }
                    } else if (strncmp(buf, "set_priority", 12) == 0 && user_logged) {
                        if (verbose) {
                            std::cout << std::format("Setting priority of {}\n", UUID_USER_SHORT);
                        }
                        auto previous_state = game_model::from_db(Conn_info,
                                                                  UUID_USER);
                        double boost_priority = 1;
                        double research_priority = 1;
                        double build_priority = 1;
                        int dynamic_priority = 0;
                        char dispose[128];
                        sscanf(buf, "%s %lf %lf %lf %d", dispose, &boost_priority, &research_priority, &build_priority,
                               &dynamic_priority);
                        if (send(fd, "ok", 2, 0)) {
                            if (verbose) {
                                std::cout << std::format("Priority set to {} {} {} {}\n", boost_priority,
                                                         research_priority, build_priority, dynamic_priority);
                            }
                            previous_state->state.boost_priority = boost_priority;
                            previous_state->state.research_priority = research_priority;
                            previous_state->state.build_priority = build_priority;
                            previous_state->state.dynamic_priority = static_cast<int8_t>(dynamic_priority);
                            auto pos_err = previous_state->update_db(Conn_info,
                                                                     UUID_USER);
                            if (verbose && !pos_err.has_value()) {
                                std::cerr << pos_err.error();
                            }
                        } else {
                            if (verbose) {
                                std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                            }
                        }
                    } else if (strncmp(buf, "set_focus_building", 18) == 0 && user_logged) {
                        if (verbose) {
                            std::cout << std::format("Setting build focus of {}\n", UUID_USER_SHORT);
                        }
                        auto previous_state = game_model::from_db(Conn_info,
                                                                  UUID_USER);
                        char dispose[128];
                        int new_building = -1;
                        sscanf(buf, "%s %d", dispose, &new_building);
                        if (send(fd, "ok", 2, 0)) {
                            if (verbose) {
                                std::cout << std::format("Building focus set to {}\n", new_building);
                            }
                            previous_state->state.focused_building = static_cast<int8_t>(new_building);
                            auto pos_err = previous_state->update_db(Conn_info,
                                                                     UUID_USER);
                            if (verbose && !pos_err.has_value()) {
                                std::cerr << pos_err.error();
                            }
                        } else {
                            if (verbose) {
                                std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                            }
                        }
                    } else if (strncmp(buf, "set_focus_research", 18) == 0 && user_logged) {
                        if (verbose) {
                            std::cout << std::format("Setting research focus of {}\n", UUID_USER_SHORT);
                        }
                        auto previous_state = game_model::from_db(Conn_info,
                                                                  UUID_USER);
                        char dispose[128];
                        int new_research = -1;
                        sscanf(buf, "%s %d", dispose, &new_research);
                        if (send(fd, "ok", 2, 0)) {
                            if (verbose) {
                                std::cout << std::format("Research focus set to {}\n", new_research);
                            }
                            previous_state->state.focused_research = static_cast<int8_t>(new_research);
                            auto pos_err = previous_state->update_db(Conn_info,
                                                                     UUID_USER);
                            if (verbose && !pos_err.has_value()) {
                                std::cerr << pos_err.error();
                            }
                        } else {
                            if (verbose) {
                                std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                            }
                        }
                    } else if (strncmp(buf, "overflow", 8) == 0 && user_logged) {
                        if (verbose) {
                            std::cout << std::format("Overflowing {}\n", UUID_USER_SHORT);
                        }
                        auto previous_state = game_model::from_db(Conn_info,
                                                                  UUID_USER);
                        int success = 0;
                        switch (previous_state->state.building_overflows) {
                            case 0:
                                if (previous_state->state.building_bits >= 8) {
                                    success = 1;
                                }
                                break;
                            case 1:
                                if (previous_state->state.building_bytes >= 1024) {
                                    success = 1;
                                }
                                break;
                            case 2:
                                if (previous_state->state.building_kilo_packers >= 1024) {
                                    success = 1;
                                }
                                break;
                            case 3:
                                if (previous_state->state.building_mega_packers >= 1024) {
                                    success = 1;
                                }
                                break;
                            case 4:
                                if (previous_state->state.building_giga_packers >= 1024) {
                                    success = 1;
                                }
                                break;
                            case 5:
                                if (previous_state->state.building_tera_packers >= 1024) {
                                    success = 1;
                                }
                                break;
                            case 6:
                                if (previous_state->state.building_peta_packers >= 1024) {
                                    success = 1;
                                }
                                break;
                            default:
                                break;
                        }
                        if (success) {
                            if (send(fd, "ok", 2, 0)) {
                                previous_state->reset_non_persistent_params();
                                previous_state->state.building_overflows += 1;
                                auto pos_err = previous_state->update_db(Conn_info,
                                                                         UUID_USER);
                                if (verbose && !pos_err.has_value()) {
                                    std::cerr << pos_err.error();
                                }
                            } else {
                                if (verbose) {
                                    std::cerr << std::format("Couldn't send message: {}\n", strerror(errno));
                                }
                            }
                        } else {
                            send(fd, "failed", 7, 0);
                            if (verbose) {
                                std::cerr << "Failed sanity check" << std::endl;
                            }
                        }
                    }
                } else {
                    user_connected = false;
                }
            }

            if (verbose) {
                if (user_logged) {
                    std::cout << std::format("Closed connection from {} at {}\n", UUID_USER_SHORT, clientip);

                }
                else {
                    std::cout << std::format("Closed connection from {}\n", clientip);
                }
            }

            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, nullptr, sizeof(int));
            shutdown(fd, SHUT_RDWR);
            close(fd);
        } else {
            if (verbose) {
                std::cerr << std::format("Unable to send greeting message: {}\n",
                                         strerror(errno));
            }

            close(fd);
        }
        pthread_mutex_lock(&mutex_sync);
        thread_counter -= 1;
        pthread_mutex_unlock(&mutex_sync);
    }

    pthread_mutex_lock(&mutex_sync);
    thread_counter -= 1;
    pthread_mutex_unlock(&mutex_sync);
    if (verbose) {
        std::cout << "Thread closed" << std::endl;
    }

    return nullptr;
}

void list_ips(const int PORT) {
    char hostbuffer[256];

    gethostname(hostbuffer, sizeof(hostbuffer));

    std::cout << std::format("Server running at {}\n", hostbuffer);
    std::cout << "local ips:" << std::endl;

    struct ifaddrs *ifaddr;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr;
         ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr,
                            sizeof(struct sockaddr_in),
                            host, NI_MAXHOST,
                            nullptr, 0, NI_NUMERICHOST);
            if (s != 0) {
                if (verbose) {
                    std::cerr << std::format("getnameinfo() failed: {}\n", gai_strerror(s));
                }
                return;
            }

            std::cout << std::format("\tAt {}:{} address: {}:{}\n", ifa->ifa_name, "ipv4", host, PORT);
        }
    }

    freeifaddrs(ifaddr);

    FILE *comm = popen("curl https://api.ipify.org", "r");
    char buf_comm[1024];
    fgets(buf_comm, 1024, comm);
    std::cout << std::format("public ip: {}\n", buf_comm);
    pclose(comm);
}

int main(int argc, char **argv) {
    std::random_device device;
    generator = std::mt19937(device());

    struct sigaction signal_action = {};
    signal_action.sa_handler = &q_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    sigaction(SIGINT, &signal_action, nullptr);
    std::cout << "parsing arguments" << std::endl;
    if (argc < 2) {
        std::cout << "You have given no port" << std::endl;
    }
    if (argc < 3) {
        std::cout << "You have given no database_ip" << std::endl;
    }
    if (argc < 4) {
        std::cout << "You have given no database_port" << std::endl;
    }
    if (argc < 5) {
        std::cout << "You have given no database_user" << std::endl;
    }
    if (argc < 6) {
        std::cout << "You have given no database_password" << std::endl;
    }
    if (argc < 7) {
        std::cout << "You have given no database_name" << std::endl;
    }
    if (argc == 7 || argc == 8) {
        std::cout << "Starting Server" << std::endl;
        if (argc == 8 && static_cast<const std::string &>(argv[7]) == "-v") {
            std::cout << "Verbose enabled" << std::endl;
            verbose = true;
        }
    } else {
        std::cout << "Usage: " << argv[0] <<
                " port database_ip database_port database_user database_password database_name [-v]" <<
                std::endl <<
                "port - which port to listen on " << std::endl <<
                "database_ip - IP of the server the database is located at" << std::endl <<
                "database_port - port used by the database connections on the server" << std::endl <<
                "database_user - username used to connect to the database" << std::endl <<
                "database_password - password used to connect to the database" << std::endl <<
                "database_name - name of the database" << std::endl <<
                "-v - verbose mode" << std::endl;
        return EXIT_FAILURE;
    }

    int PORT = std::stoi(argv[1]);
    PGconn *PostGres_conn;
    PGresult *PostGres_res; {
        Conn_info = std::format("hostaddr={} port={} user={} password={} dbname={}", argv[2], argv[3], argv[4], argv[5],
                                argv[6]);
        std::cout << "Attempting contact with database server" << std::endl;
        PostGres_conn = PQconnectdb(Conn_info.c_str());
        if (PQstatus(PostGres_conn) != CONNECTION_OK) {
            std::cerr << std::format("Unable to connect to database: {}\n",
                                     PQerrorMessage(PostGres_conn));
            PQfinish(PostGres_conn);
            return EXIT_FAILURE;
        }
        PQfinish(PostGres_conn);
        std::cout << "Connection succesful" << std::endl;
    }

    std::cout << std::format("Opening port {}\n", PORT);


    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Creating socket" << std::endl;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << std::format("Unable to create socket: {}\n",
                                 strerror(errno));
        return EXIT_FAILURE;
    }
    std::cout << "Socket created" << std::endl;
    std::cout << "Binding socket" << std::endl;
    if (bind(sockfd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        std::cerr << std::format("Unable to bind socket: {}\n",
                                 strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }
    std::cout << "Socket bound" << std::endl;
    std::cout << "Starting to listen for connections" << std::endl;
    if (listen(sockfd, 5) < 0) {
        std::cerr << std::format("Unable to listen socket: {}\n",
                                 strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }
    std::cout << std::format("Port {} opened for connections\n", PORT);
    std::cout << "Listing available IP addresses" << std::endl;
    list_ips(PORT);
    std::cout << "Listed available IP addresses" << std::endl;

    sem_init(&sem_poster, 0, 0);
    for (int i = 0; i < 10; ++i) {
        pthread_t thr;
        pthread_create(&thr, nullptr, &thread_worker, &thr);
    }

    std::cout << "Listening for connections" << std::endl;

    while (active) {
        sockaddr *ca = nullptr;
        socklen_t sz = 0;
        int fd;
        pid_t pid;
        if (verbose) {
            std::cout << "Waiting for connection" << std::endl;
        }
        fd = accept(sockfd, ca, &sz);
        if (fd < 0) {
            if (verbose) {
                std::cerr << std::format("Unable to accept connection: {}\n",
                                         strerror(errno));
            }
            continue;
        }
        if (verbose) {
            std::cout << "Connection accepted" << std::endl;
        }

        pthread_mutex_lock(&mutex_sync);
        data_post = fd;
        if (active_threads >= thread_counter - 2) {
            pthread_t thr;
            pthread_create(&thr, nullptr, &thread_worker, &thr);
        }
        pthread_mutex_unlock(&mutex_sync);

        sem_post(&sem_poster);
    }

    return EXIT_SUCCESS;
}
