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

struct game_state {
    int focused_research;
    int focused_building;
    double build_priority;
    double boost_priority;
    double research_priority;
    int dynamic_priority;
    long time;
    //Buildings
    long double building_bits;
    long double building_bytes;
    long double building_kilo_packers;
    long double building_mega_packers;
    long double building_giga_packers;
    long double building_tera_packers;
    long double building_peta_packers;
    long double building_exa_packers;
    long double building_processes;
    int building_overflows;
    //Research
    long double research_bits_add;
    long double research_bits_mul;
    long double research_bytes_add;
    long double research_bytes_mul;
    long double research_kilo_add;
    long double research_kilo_mul;
    long double research_mega_add;
    long double research_mega_mul;
    long double research_giga_add;
    long double research_giga_mul;
    long double research_tera_add;
    long double research_tera_mul;
    long double research_peta_add;
    long double research_peta_mul;
    long double research_exa_add;
    long double research_exa_mul;
    long double research_process_mul;
    long double research_endgame;
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
        sprintf(query,
                "select round(extract(epoch from \"Last seen\")) from \"Users\" where \"Identificator\" = '%s'",
                UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            goto escape;
        }
        char *t_value;
        t_value = PQgetvalue(res, 0, 0);
        state.time = atol(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "select \"Build_Priority\", \"Boost_Priority\", \"Research_Priority\", \"Focused_Research\", \"Focused_Building\", \"Dynamic_Priority\" from \"User_Priority\" join \"Users\" on \"Users\".\"Pk\" = \"User_Priority\".\"Fk_User\" where \"Identificator\" = '%s'",
                UUID_USER);
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
        t_value = PQgetvalue(res, 0, 5);
        state.dynamic_priority = atoi(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "select \"Bits\", \"Bytes\", \"Kilo_Packers\", \"Mega_Packers\", \"Giga_Packers\", \"Tera_Packers\", \"Peta_Packers\", \"Exa_Packers\", \"Processes\", \"Overflows\" from \"User_Buildings\" join \"Users\" on \"Users\".\"Pk\" = \"User_Buildings\".\"Fk_User\" where \"Identificator\" = '%s'",
                UUID_USER);
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
        state.building_overflows = atoi(t_value);
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "select \"Bits_Add\", \"Bits_Mul\", \"Bytes_Add\", \"Bytes_Mul\", \"Kilo_Add\", \"Kilo_Mul\", \"Mega_Add\", \"Mega_Mul\", \"Giga_Add\", \"Giga_Mul\", \"Tera_Add\", \"Tera_Mul\", \"Peta_Add\", \"Peta_Mul\", \"Exa_Add\", \"Exa_Mul\", \"Process_Multiplier\", \"Game_End\" from \"User_Research\" join \"Users\" on \"Users\".\"Pk\" = \"User_Research\".\"Fk_User\" where \"Identificator\" = '%s'",
                UUID_USER);
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
    } else {
        return state;
    }
}

void update_db_with_state(struct game_state state) {
    PGconn *PostGres_conn;
    PGresult *res;
    PostGres_conn = PQconnectdb(Conn_info);
    int success = 0;
    if (PQstatus(PostGres_conn) == CONNECTION_OK) {
        char query[4096];
        memset(query, 0, sizeof(query));
        sprintf(query,
                "update \"Users\" set \"Last seen\" = to_timestamp(%ld) where \"Identificator\" = '%s'",
                state.time, UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            goto escape;
        }
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "update \"User_Priority\" set \"Build_Priority\" = %f, \"Boost_Priority\" = %f, \"Research_Priority\" = %f, \"Focused_Research\" = %d, \"Focused_Building\" = %d, \"Dynamic_Priority\" = %d from \"Users\" where \"Fk_User\" = \"Users\".\"Pk\" and \"Identificator\" = '%s'",
                state.build_priority, state.boost_priority, state.research_priority, state.focused_research,
                state.focused_building, state.dynamic_priority, UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            goto escape;
        }
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "update \"User_Buildings\" set \"Bits\" = %Lf, \"Bytes\" = %Lf, \"Kilo_Packers\" = %Lf, \"Mega_Packers\" = %Lf, \"Giga_Packers\" = %Lf, \"Tera_Packers\" = %Lf, \"Peta_Packers\" = %Lf, \"Exa_Packers\" = %Lf, \"Processes\" = %Lf, \"Overflows\" = %d from \"Users\" where \"Users\".\"Pk\" = \"Fk_User\" and \"Identificator\" = '%s'",
                state.building_bits, state.building_bytes, state.building_kilo_packers, state.building_mega_packers,
                state.building_giga_packers, state.building_tera_packers, state.building_peta_packers,
                state.building_exa_packers, state.building_processes, state.building_overflows, UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            goto escape;
        }
        PQclear(res);
        memset(query, 0, sizeof(query));
        sprintf(query,
                "update \"User_Research\" set \"Bits_Add\" = %Lf, \"Bits_Mul\" = %Lf, \"Bytes_Add\" = %Lf, \"Bytes_Mul\" = %Lf, \"Kilo_Add\" = %Lf, \"Kilo_Mul\" = %Lf, \"Mega_Add\" = %Lf, \"Mega_Mul\" = %Lf, \"Giga_Add\" = %Lf, \"Giga_Mul\" = %Lf, \"Tera_Add\" = %Lf, \"Tera_Mul\" = %Lf, \"Peta_Add\" = %Lf, \"Peta_Mul\" = %Lf, \"Exa_Add\" = %Lf, \"Exa_Mul\" = %Lf, \"Process_Multiplier\" = %Lf, \"Game_End\" = %Lf from \"Users\" where \"Users\".\"Pk\" = \"Fk_User\" and \"Identificator\" = '%s'",
                state.research_bits_add, state.research_bits_mul, state.research_bytes_add, state.research_bytes_mul,
                state.research_kilo_add, state.research_kilo_mul, state.research_mega_add, state.research_mega_mul,
                state.research_giga_add, state.research_giga_mul, state.research_tera_add, state.research_tera_mul,
                state.research_peta_add, state.research_peta_mul, state.research_exa_add, state.research_exa_mul,
                state.research_process_mul, state.research_endgame, UUID_USER);
        res = PQexec(PostGres_conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            goto escape;
        }
        PQclear(res);
        PQfinish(PostGres_conn);
        success = 1;
    }
    escape:
    if (!success) {
        fprintf(stderr, "Error code on PQconnectdb (if any): %s\n", PQerrorMessage(PostGres_conn));
        fprintf(stderr, "Error code on PQresultStatus (if any): %d\n", PQresultStatus(res));
        PQfinish(PostGres_conn);
    }
}

struct game_state update_game_state(struct game_state initial_state, long new_time) {
    struct game_state new_state = initial_state;
    new_state.time = new_time;
    long time_diff = new_time - initial_state.time - 21500;
    if (time_diff < 0) {
        return initial_state;
    }
    long double procured_value = 0;
    long double time_mul = 0.001;
    procured_value += powl((floorl(initial_state.building_exa_packers) + floorl(initial_state.research_exa_add)) *
                           powl(powl(2, 1.0 / 8),
                                floorl(initial_state.research_exa_mul)),
                           fminl(1, (initial_state.building_overflows - 8) /
                                            powl(5, 8) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_peta_packers) + floorl(initial_state.research_peta_add)) *
                           powl(powl(2, 1.0 / 7),
                                floorl(initial_state.research_peta_mul)),
                           fminl(1, (initial_state.building_overflows - 7) /
                                            powl(5, 7) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_tera_packers) + floorl(initial_state.research_tera_add)) *
                           powl(powl(2, 1.0 / 6),
                                floorl(initial_state.research_tera_mul)),
                           fminl(1, (initial_state.building_overflows - 6) /
                                            powl(5, 6) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_giga_packers) + floorl(initial_state.research_giga_add)) *
                           powl(powl(2, 1.0 / 5),
                                floorl(initial_state.research_giga_mul)),
                           fminl(1, (initial_state.building_overflows - 4) /
                                            powl(5, 5) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_mega_packers) + floorl(initial_state.research_mega_add)) *
                           powl(powl(2, 1.0 / 4),
                                floorl(initial_state.research_mega_mul)),
                           fminl(1, (initial_state.building_overflows - 3) /
                                            powl(5, 4) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_kilo_packers) + floorl(initial_state.research_kilo_add)) *
                           powl(powl(2, 1.0 / 3),
                                floorl(initial_state.research_kilo_mul)),
                           fminl(1, (initial_state.building_overflows - 2) /
                                            powl(5, 3) + 1));
    procured_value *= 1024;
    procured_value += powl(
            (floorl(initial_state.building_bytes) + floorl(initial_state.research_bytes_add)) * powl(powl(2, 1.0 / 2),
                                                                                                     floorl(initial_state.research_bytes_mul)),
            fminl(1, (initial_state.building_overflows - 1) /
                             powl(5, 2) + 1));
    procured_value *= 8;
    procured_value += powl((floorl(initial_state.building_bits) + floorl(initial_state.research_bits_add)) *
                           powl(2, floorl(initial_state.research_bits_mul)),
                           fminl(1, initial_state.building_overflows / 5 + 1));
    procured_value *= (floorl(initial_state.building_processes) *
                       powl(3, floorl(initial_state.research_process_mul)));
    procured_value *= powl(2, 5 * initial_state.boost_priority);
    procured_value *= time_mul * 0.1;
    procured_value *= time_diff;
    long double building_value = procured_value * powl(initial_state.build_priority, 2);
    long double research_value = procured_value * powl(initial_state.research_priority, 2);
    switch (initial_state.focused_building) {
        case 0:
            if (new_state.building_overflows <= 0){
                new_state.building_bits = fminl(powl(powl(initial_state.building_bits, 1.1) + building_value, 1.0 / 1.1),8*1.0001);
            } else{
                new_state.building_bits = powl(powl(initial_state.building_bits, 1.1) + building_value, 1.0 / 1.1);
            }
            break;
        case 1:
            if (new_state.building_overflows <= 1){
                new_state.building_bytes = fminl(powl(powl(24 * initial_state.building_bytes, 1.1) + building_value, 1.0 / 1.1) / 24,1024*1.0001);
            }
            else{
                new_state.building_bytes =
                        powl(powl(24 * initial_state.building_bytes, 1.1) + building_value, 1.0 / 1.1) / 24;
            }
            break;
        case 2:
            if (new_state.building_overflows <= 2){
                new_state.building_kilo_packers = fminl(
                        powl(powl(1024 * 3 * initial_state.building_kilo_packers, 1.1) + building_value, 1.0 / 1.1) /
                        (1024 * 3),
                        powl(1024 * 3, 1.1) * 1.0001
                );
            }
            else{
                new_state.building_kilo_packers =
                        powl(powl(1024 * 3 * initial_state.building_kilo_packers, 1.1) + building_value, 1.0 / 1.1) /
                        (1024 * 3);
            }
            break;
        case 3:
            if (new_state.building_overflows <= 3){
                new_state.building_mega_packers = fminl(
                        powl(powl((powl(1024, 2) * 3) * initial_state.building_mega_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 2) * 3),
                        powl((powl(1024, 2) * 3), 1.1) * 1.0001
                );
            }
            else{
                new_state.building_mega_packers =
                        powl(powl((powl(1024, 2) * 3) * initial_state.building_mega_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 2) * 3);
            }
            break;
        case 4:
            if (new_state.building_overflows <= 4){
                new_state.building_giga_packers = fminl(
                        powl(powl((powl(1024, 3) * 3) * initial_state.building_giga_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 3) * 3),
                        powl((powl(1024, 3) * 3), 1.1) * 1.0001
                );
            }
            else{
                new_state.building_giga_packers =
                        powl(powl((powl(1024, 3) * 3) * initial_state.building_giga_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 3) * 3);
            }
            break;
        case 5:
            if (new_state.building_overflows <= 5){
                new_state.building_tera_packers = fminl(
                        powl(powl((powl(1024, 4) * 3) * initial_state.building_tera_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 4) * 3),
                        powl((powl(1024, 4) * 3), 1.1) * 1.0001
                );
            }
            else{
                new_state.building_tera_packers =
                        powl(powl((powl(1024, 4) * 3) * initial_state.building_tera_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 4) * 3);
            }
            break;
        case 6:
            if (new_state.building_overflows <= 6){
                new_state.building_peta_packers = fminl(
                        powl(powl((powl(1024, 5) * 3) * initial_state.building_peta_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 5) * 3),
                        powl((powl(1024, 5) * 3), 1.1) * 1.0001
                );
            }
            else{
                new_state.building_peta_packers =
                        powl(powl((powl(1024, 5) * 3) * initial_state.building_peta_packers, 1.1) + building_value,
                             1.0 / 1.1) / (powl(1024, 5) * 3);
            }
            break;
        case 7:
            new_state.building_exa_packers =
                    powl(powl((powl(1024, 6) * 3) * initial_state.building_exa_packers, 1.1) + building_value,
                         1.0 / 1.1) / (powl(1024, 6) * 3);
            break;
        case 8:
            new_state.building_processes = powl(powl(initial_state.building_processes, 5) + building_value, 1.0 / 5);
            break;
    }
    switch (initial_state.focused_research) {
        case 0:
            new_state.research_bits_add =
                    powl(powl(100 * initial_state.research_bits_add, 1.1) + research_value, 1.0 / 1.1) / 100;
            break;
        case 1:
            new_state.research_bits_mul =
                    powl(powl(500 * initial_state.research_bits_mul, 1.3) + research_value, 1.0 / 1.3) / 500;
            break;
        case 2:
            new_state.research_bytes_add =
                    powl(powl(24 * 100 * initial_state.research_bytes_add, 1.1) + research_value, 1.0 / 1.1) /
                    (24 * 100);
            break;
        case 3:
            new_state.research_bytes_mul =
                    powl(powl(24 * 500 * initial_state.research_bytes_mul, 1.3) + research_value, 1.0 / 1.3) /
                    (24 * 500);
            break;
        case 4:
            new_state.research_kilo_add =
                    powl(powl(1024 * 3 * 100 * initial_state.research_kilo_add, 1.1) + research_value, 1.0 / 1.1) /
                    (1024 * 3 * 100);
            break;
        case 5:
            new_state.research_kilo_mul =
                    powl(powl(1024 * 3 * 500 * initial_state.research_kilo_mul, 1.3) + research_value, 1.0 / 1.3) /
                    (1024 * 3 * 500);
            break;
        case 6:
            new_state.research_mega_add =
                    powl(powl((powl(1024, 2) * 3) * 100 * initial_state.research_mega_add, 1.1) + research_value,
                         1.0 / 1.1) / ((powl(1024, 2) * 3) * 100);
            break;
        case 7:
            new_state.research_mega_mul =
                    powl(powl((powl(1024, 2) * 3) * 500 * initial_state.research_mega_mul, 1.3) + research_value,
                         1.0 / 1.3) / ((powl(1024, 2) * 3) * 500);
            break;
        case 8:
            new_state.research_giga_add =
                    powl(powl((powl(1024, 3) * 3) * 100 * initial_state.research_giga_add, 1.1) + research_value,
                         1.0 / 1.1) / ((powl(1024, 3) * 3) * 100);
            break;
        case 9:
            new_state.research_giga_mul =
                    powl(powl((powl(1024, 3) * 3) * 500 * initial_state.research_giga_mul, 1.3) + research_value,
                         1.0 / 1.3) / ((powl(1024, 3) * 3) * 500);
            break;
        case 10:
            new_state.research_tera_add =
                    powl(powl((powl(1024, 4) * 3) * 100 * initial_state.research_tera_add, 1.1) + research_value,
                         1.0 / 1.1) / ((powl(1024, 4) * 3) * 100);
            break;
        case 11:
            new_state.research_tera_mul =
                    powl(powl((powl(1024, 4) * 3) * 500 * initial_state.research_tera_mul, 1.3) + research_value,
                         1.0 / 1.3) / ((powl(1024, 4) * 3) * 500);
            break;
        case 12:
            new_state.research_peta_add =
                    powl(powl((powl(1024, 5) * 3) * 100 * initial_state.research_peta_add, 1.1) + research_value,
                         1.0 / 1.1) / ((powl(1024, 5) * 3) * 100);
            break;
        case 13:
            new_state.research_peta_mul =
                    powl(powl((powl(1024, 5) * 3) * 500 * initial_state.research_peta_mul, 1.3) + research_value,
                         1.0 / 1.3) / ((powl(1024, 5) * 3) * 500);
            break;
        case 14:
            new_state.research_exa_add =
                    powl(powl((powl(1024, 6) * 3) * 100 * initial_state.research_exa_add, 1.1) + research_value,
                         1.0 / 1.1) / ((powl(1024, 6) * 3) * 100);
            break;
        case 15:
            new_state.research_exa_mul =
                    powl(powl((powl(1024, 6) * 3) * 500 * initial_state.research_exa_mul, 1.3) + research_value,
                         1.0 / 1.3) / ((powl(1024, 6) * 3) * 500);
            break;
        case 16:
            new_state.research_process_mul =
                    powl(powl(1e8 * initial_state.research_process_mul, 5) + research_value, 1.0 / 5) / (1e8);
            break;
    }
    return new_state;
}

long double peek_procured_value(struct game_state initial_state){
    long time_diff = 1000;
    long double procured_value = 0;
    long double time_mul = 0.001;
    procured_value += powl((floorl(initial_state.building_exa_packers) + floorl(initial_state.research_exa_add)) *
                           powl(powl(2, 1.0 / 8),
                                floorl(initial_state.research_exa_mul)),
                           fminl(1, (initial_state.building_overflows - 8) /
                                    powl(5, 8) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_peta_packers) + floorl(initial_state.research_peta_add)) *
                           powl(powl(2, 1.0 / 7),
                                floorl(initial_state.research_peta_mul)),
                           fminl(1, (initial_state.building_overflows - 7) /
                                    powl(5, 7) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_tera_packers) + floorl(initial_state.research_tera_add)) *
                           powl(powl(2, 1.0 / 6),
                                floorl(initial_state.research_tera_mul)),
                           fminl(1, (initial_state.building_overflows - 6) /
                                    powl(5, 6) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_giga_packers) + floorl(initial_state.research_giga_add)) *
                           powl(powl(2, 1.0 / 5),
                                floorl(initial_state.research_giga_mul)),
                           fminl(1, (initial_state.building_overflows - 4) /
                                    powl(5, 5) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_mega_packers) + floorl(initial_state.research_mega_add)) *
                           powl(powl(2, 1.0 / 4),
                                floorl(initial_state.research_mega_mul)),
                           fminl(1, (initial_state.building_overflows - 3) /
                                    powl(5, 4) + 1));
    procured_value *= 1024;
    procured_value += powl((floorl(initial_state.building_kilo_packers) + floorl(initial_state.research_kilo_add)) *
                           powl(powl(2, 1.0 / 3),
                                floorl(initial_state.research_kilo_mul)),
                           fminl(1, (initial_state.building_overflows - 2) /
                                    powl(5, 3) + 1));
    procured_value *= 1024;
    procured_value += powl(
            (floorl(initial_state.building_bytes) + floorl(initial_state.research_bytes_add)) * powl(powl(2, 1.0 / 2),
                                                                                                     floorl(initial_state.research_bytes_mul)),
            fminl(1, (initial_state.building_overflows - 1) /
                     powl(5, 2) + 1));
    procured_value *= 8;
    procured_value += powl((floorl(initial_state.building_bits) + floorl(initial_state.research_bits_add)) *
                           powl(2, floorl(initial_state.research_bits_mul)),
                           fminl(1, initial_state.building_overflows / 5 + 1));
    procured_value *= (floorl(initial_state.building_processes) *
                       powl(3, floorl(initial_state.research_process_mul)));
    procured_value *= time_mul * 0.1;
    procured_value *= time_diff;
    return procured_value;
}

struct game_state dynamic_update_game_state(struct game_state initial_state, long new_time){
    struct game_state intermediate = initial_state;
    if (initial_state.dynamic_priority){
        double best_boost = initial_state.boost_priority, best_build = initial_state.build_priority, best_research = initial_state.research_priority;
        long double best_boost_value = peek_procured_value(intermediate);
        for (int i = 5; i <= 90; i += 5) {
            for (int j = 5; j <= 95 - i; j += 5) {
                for (int k = 5; k <= 95 - i - j; k += 5) {
                    intermediate.boost_priority = (double)i / 100;
                    intermediate.build_priority = (double)j / 100;
                    intermediate.research_priority = (double)k / 100;
                    if (peek_procured_value(intermediate) > best_boost_value){
                        best_boost_value = peek_procured_value(intermediate);
                        best_boost = intermediate.boost_priority;
                        best_build = intermediate.build_priority;
                        best_research = intermediate.research_priority;
                    }
                }
            }
        }
        intermediate.boost_priority = best_boost;
        intermediate.build_priority = best_build;
        intermediate.research_priority = best_research;
    }
    int dynamic_build_focus = initial_state.focused_building == -2 ? 1 : 0;
    if (dynamic_build_focus){
        int best_build_focus = initial_state.focused_building;
        long double best_projection = 0;
        for (int i = 0; i <= 8; ++i) {
            intermediate.focused_building = i;
            struct game_state inter_state = update_game_state(intermediate, new_time);
            if (best_projection < peek_procured_value(inter_state)){
                best_projection = peek_procured_value(inter_state);
                best_build_focus = i;
            }
        }
        intermediate.focused_building = best_build_focus;
    }
    int dynamic_research_focus = initial_state.focused_research == -2 ? 1 : 0;
    if (dynamic_research_focus){
        int best_research_focus = initial_state.focused_research;
        long double best_projection = 0;
        for (int i = 0; i <= 16; ++i) {
            intermediate.focused_research = i;
            struct game_state inter_state = update_game_state(intermediate, new_time);
            if (best_projection < peek_procured_value(inter_state)){
                best_projection = peek_procured_value(inter_state);
                best_research_focus = i;
            }
        }
        intermediate.focused_research = best_research_focus;
    }
    struct game_state new_state = update_game_state(intermediate, new_time);
    new_state.focused_building = dynamic_build_focus ? -2 : new_state.focused_building;
    new_state.focused_research = dynamic_research_focus ? -2 : new_state.focused_research;
    return new_state;
}

struct game_state reset_non_persistent_params(struct game_state old_state){
    struct game_state new_state = old_state;
    new_state.building_bits = 1;
    new_state.building_bytes = 0;
    new_state.building_kilo_packers = 0;
    new_state.building_mega_packers = 0;
    new_state.building_giga_packers = 0;
    new_state.building_tera_packers = 0;
    new_state.building_peta_packers = 0;
    new_state.building_exa_packers = 0;
    new_state.research_process_mul = 0;
    new_state.research_bits_add = 0;
    new_state.research_bits_mul = 0;
    new_state.research_bytes_add = 0;
    new_state.research_bytes_mul = 0;
    new_state.research_kilo_add = 0;
    new_state.research_kilo_mul = 0;
    new_state.research_mega_add = 0;
    new_state.research_mega_mul = 0;
    new_state.research_giga_add = 0;
    new_state.research_giga_mul = 0;
    new_state.research_tera_add = 0;
    new_state.research_tera_mul = 0;
    new_state.research_peta_add = 0;
    new_state.research_peta_mul = 0;
    new_state.research_exa_add = 0;
    new_state.research_exa_mul = 0;
    if (new_state.focused_building != -2) new_state.focused_building = -1;
    if (new_state.focused_research != -2) new_state.focused_research = -1;
    return new_state;
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
                            struct game_state previous_state = get_state_from_db();
                            if (previous_state.time == -1){
                                fprintf(stderr, "Couldn't get state\n");
                            }
                            else{
                                struct timeval tv;
                                gettimeofday(&tv, NULL);
                                struct game_state new_state = dynamic_update_game_state(previous_state,
                                                                                (tv.tv_sec * 1000) + (tv.tv_usec / 1000));
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
                                    update_db_with_state(new_state);
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
                        struct game_state previous_state = get_state_from_db();
                        double boost_priority = 1;
                        double research_priority = 1;
                        double build_priority = 1;
                        int dynamic_priority = 0;
                        char dispose[128];
                        sscanf(buf, "%s %lf %lf %lf %d", dispose, &boost_priority, &research_priority, &build_priority, &dynamic_priority);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Priority set to %lf %lf %lf %d\n", boost_priority, research_priority, build_priority, dynamic_priority);
                            struct game_state new_state = previous_state;
                            new_state.boost_priority = boost_priority;
                            new_state.research_priority = research_priority;
                            new_state.build_priority = build_priority;
                            new_state.dynamic_priority = dynamic_priority;
                            update_db_with_state(new_state);
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "set_focus_building", 18) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Setting build focus of %s\n", UUID_USER_SHORT);
                        struct game_state previous_state = get_state_from_db();
                        char dispose[128];
                        int new_building = -1;
                        sscanf(buf, "%s %d", dispose, &new_building);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Building focus set to %d\n", new_building);
                            struct game_state new_state = previous_state;
                            new_state.focused_building = new_building;
                            update_db_with_state(new_state);
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "set_focus_research", 18) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Setting research focus of %s\n", UUID_USER_SHORT);
                        struct game_state previous_state = get_state_from_db();
                        char dispose[128];
                        int new_research = -1;
                        sscanf(buf, "%s %d", dispose, &new_research);
                        if (send(fd, "ok", 2, 0)) {
                            fprintf(stdout, "Research focus set to %d\n", new_research);
                            struct game_state new_state = previous_state;
                            new_state.focused_research = new_research;
                            update_db_with_state(new_state);
                        }
                        else{
                            fprintf(stderr, "Couldn't send message: %s\n", strerror(errno));
                        }
                    }
                    else if (strncmp(buf, "overflow", 8) == 0 && USER_LOGGED) {
                        fprintf(stdout, "Overflowing %s\n", UUID_USER_SHORT);
                        struct game_state previous_state = get_state_from_db();
                        int success = 0;
                        switch (previous_state.building_overflows) {
                            case 0:
                                if (previous_state.building_bits >= 8){
                                    success = 1;
                                }
                                break;
                            case 1:
                                if (previous_state.building_bytes >= 1024){
                                    success = 1;
                                }
                                break;
                            case 2:
                                if (previous_state.building_kilo_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 3:
                                if (previous_state.building_mega_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 4:
                                if (previous_state.building_giga_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 5:
                                if (previous_state.building_tera_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            case 6:
                                if (previous_state.building_peta_packers >= 1024){
                                    success = 1;
                                }
                                break;
                            default:
                                break;
                        }
                        if (success){
                            if (send(fd, "ok", 2, 0)) {
                                struct game_state new_state = reset_non_persistent_params(previous_state);
                                new_state.building_overflows += 1;
                                update_db_with_state(new_state);
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
