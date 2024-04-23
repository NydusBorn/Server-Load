#include "iostream"
#include "libpq-fe.h"
#include "expected"
#include "format"
#include "cmath"

class game_model {
public:
    struct state_model {
        int8_t focused_research;
        int8_t focused_building;
        float build_priority;
        float boost_priority;
        float research_priority;
        int8_t dynamic_priority;
        int64_t time;
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
        int16_t building_overflows;
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
    state_model state;

    static std::expected<game_model, std::string> from_db(const std::string &db_conn_string, const std::string &UUID_USER) {
        game_model model;
        PGconn *PostGres_conn;
        PGresult *res;
        PostGres_conn = PQconnectdb(db_conn_string.c_str());
        if (PQstatus(PostGres_conn) != CONNECTION_OK) {
            std::string message = std::format("Connection failed\nError code on PQconnectdb (if any): {}\n",
                                              PQerrorMessage(PostGres_conn));
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        std::string query;
        query = std::format("select * from \"Users\" where \"Identificator\" = '{}'", UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string message = std::format("Failed query to get time\nError code on PQconnectdb (if any): {}\n",
                                              PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        std::string t_value;
        t_value = PQgetvalue(res, 0, 0);
        model.state.time = std::stol(t_value);
        PQclear(res);
        query = std::format(
                "select \"Build_Priority\", \"Boost_Priority\", \"Research_Priority\", \"Focused_Research\", \"Focused_Building\", \"Dynamic_Priority\" from \"User_Priority\" join \"Users\" on \"Users\".\"Pk\" = \"User_Priority\".\"Fk_User\" where \"Identificator\" = '{}'",
                UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string message = std::format(
                    "Failed query to get priorities\nError code on PQconnectdb (if any): {}\n",
                    PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        t_value = PQgetvalue(res, 0, 0);
        model.state.build_priority = std::stof(t_value);
        t_value = PQgetvalue(res, 0, 1);
        model.state.boost_priority = std::stof(t_value);
        t_value = PQgetvalue(res, 0, 2);
        model.state.research_priority = std::stof(t_value);
        t_value = PQgetvalue(res, 0, 3);
        model.state.focused_research = std::stoi(t_value);
        t_value = PQgetvalue(res, 0, 4);
        model.state.focused_building = std::stoi(t_value);
        t_value = PQgetvalue(res, 0, 5);
        model.state.dynamic_priority = std::stoi(t_value);
        PQclear(res);
        query = std::format(
                "select \"Bits\", \"Bytes\", \"Kilo_Packers\", \"Mega_Packers\", \"Giga_Packers\", \"Tera_Packers\", \"Peta_Packers\", \"Exa_Packers\", \"Processes\", \"Overflows\" from \"User_Buildings\" join \"Users\" on \"Users\".\"Pk\" = \"User_Buildings\".\"Fk_User\" where \"Identificator\" = '{}'",
                UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string message = std::format("Failed query to get buildings\nError code on PQconnectdb (if any): {}\n",
                                              PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        t_value = PQgetvalue(res, 0, 0);
        model.state.building_bits = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 1);
        model.state.building_bytes = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 2);
        model.state.building_kilo_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 3);
        model.state.building_mega_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 4);
        model.state.building_giga_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 5);
        model.state.building_tera_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 6);
        model.state.building_peta_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 7);
        model.state.building_exa_packers = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 8);
        model.state.building_processes = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 9);
        model.state.building_overflows = std::stoi(t_value);
        PQclear(res);
        query = std::format(
                "select \"Bits_Add\", \"Bits_Mul\", \"Bytes_Add\", \"Bytes_Mul\", \"Kilo_Add\", \"Kilo_Mul\", \"Mega_Add\", \"Mega_Mul\", \"Giga_Add\", \"Giga_Mul\", \"Tera_Add\", \"Tera_Mul\", \"Peta_Add\", \"Peta_Mul\", \"Exa_Add\", \"Exa_Mul\", \"Process_Multiplier\", \"Game_End\" from \"User_Research\" join \"Users\" on \"Users\".\"Pk\" = \"User_Research\".\"Fk_User\" where \"Identificator\" = '{}'",
                UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string message = std::format("Failed query to get research\nError code on PQconnectdb (if any): {}\n",
                                              PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        t_value = PQgetvalue(res, 0, 0);
        model.state.research_bits_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 1);
        model.state.research_bits_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 2);
        model.state.research_bytes_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 3);
        model.state.research_bytes_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 4);
        model.state.research_kilo_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 5);
        model.state.research_kilo_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 6);
        model.state.research_mega_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 7);
        model.state.research_mega_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 8);
        model.state.research_giga_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 9);
        model.state.research_giga_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 10);
        model.state.research_tera_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 11);
        model.state.research_tera_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 12);
        model.state.research_peta_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 13);
        model.state.research_peta_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 14);
        model.state.research_exa_add = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 15);
        model.state.research_exa_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 16);
        model.state.research_process_mul = std::stold(t_value);
        t_value = PQgetvalue(res, 0, 17);
        model.state.research_endgame = std::stold(t_value);
        PQclear(res);
        PQfinish(PostGres_conn);
        return model;
    }

    std::expected<void, std::string> update_db(const std::string &db_conn_string, const std::string &UUID_USER) {
        PGconn *PostGres_conn;
        PGresult *res;
        PostGres_conn = PQconnectdb(db_conn_string.c_str());
        if (PQstatus(PostGres_conn) != CONNECTION_OK) {
            std::string message = std::format("Failed connection to database\nError code on PQconnectdb (if any): {}\n",
                                              PQerrorMessage(PostGres_conn));
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        std::string query;
        query = std::format("update \"Users\" set \"Last seen\" = to_timestamp({}) where \"Identificator\" = '{}'",
                            state.time, UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string message = std::format(
                    "Failed query to update last seen\nError code on PQconnectdb (if any): {}\n",
                    PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        PQclear(res);
        query = std::format(
                "update \"User_Priority\" set \"Build_Priority\" = {}, \"Boost_Priority\" = {}, \"Research_Priority\" = {}, \"Focused_Research\" = {}, \"Focused_Building\" = {}, \"Dynamic_Priority\" = {} from \"Users\" where \"Fk_User\" = \"Users\".\"Pk\" and \"Identificator\" = '{}'",
                state.build_priority, state.boost_priority, state.research_priority, state.focused_research,
                state.focused_building, state.dynamic_priority, UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string message = std::format(
                    "Failed query to update priorities\nError code on PQconnectdb (if any): {}\n",
                    PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        PQclear(res);
        query = std::format(
                "update \"User_Buildings\" set \"Bits\" = {}, \"Bytes\" = {}, \"Kilo_Packers\" = {}, \"Mega_Packers\" = {}, \"Giga_Packers\" = {}, \"Tera_Packers\" = {}, \"Peta_Packers\" = {}, \"Exa_Packers\" = {}, \"Processes\" = {}, \"Overflows\" = {} from \"Users\" where \"Users\".\"Pk\" = \"Fk_User\" and \"Identificator\" = '{}'",
                state.building_bits, state.building_bytes, state.building_kilo_packers, state.building_mega_packers,
                state.building_giga_packers, state.building_tera_packers, state.building_peta_packers,
                state.building_exa_packers, state.building_processes, state.building_overflows, UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string message = std::format(
                    "Failed query to update buildings\nError code on PQconnectdb (if any): {}\n",
                    PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        PQclear(res);
        query = std::format(
                "update \"User_Research\" set \"Bits_Add\" = {}, \"Bits_Mul\" = {}, \"Bytes_Add\" = {}, \"Bytes_Mul\" = {}, \"Kilo_Add\" = {}, \"Kilo_Mul\" = {}, \"Mega_Add\" = {}, \"Mega_Mul\" = {}, \"Giga_Add\" = {}, \"Giga_Mul\" = {}, \"Tera_Add\" = {}, \"Tera_Mul\" = {}, \"Peta_Add\" = {}, \"Peta_Mul\" = {}, \"Exa_Add\" = {}, \"Exa_Mul\" = {}, \"Process_Multiplier\" = {}, \"Game_End\" = {} from \"Users\" where \"Users\".\"Pk\" = \"Fk_User\" and \"Identificator\" = '{}'",
                state.research_bits_add, state.research_bits_mul, state.research_bytes_add, state.research_bytes_mul,
                state.research_kilo_add, state.research_kilo_mul, state.research_mega_add, state.research_mega_mul,
                state.research_giga_add, state.research_giga_mul, state.research_tera_add, state.research_tera_mul,
                state.research_peta_add, state.research_peta_mul, state.research_exa_add, state.research_exa_mul,
                state.research_process_mul, state.research_endgame, UUID_USER);
        res = PQexec(PostGres_conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string message = std::format(
                    "Failed query to update research\nError code on PQconnectdb (if any): {}\n",
                    PQerrorMessage(PostGres_conn));
            PQclear(res);
            PQfinish(PostGres_conn);
            return std::unexpected<std::string>(message);
        }
        PQclear(res);
        PQfinish(PostGres_conn);
        return {};
    }

    static long double peek_procured_value(const state_model state, const int64_t time_diff) {
        long double procured_value = 0.;
        long double time_mul = 0.001;
        procured_value += std::pow((std::floor(state.building_exa_packers) + std::floor(state.research_exa_add)) *
                                   std::pow(std::pow(2., 1.0 / 8.),
                                            std::floor(state.research_exa_mul)),
                                   std::min(1., (state.building_overflows - 8.) /
                                                std::pow(5., 8.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow((std::floor(state.building_peta_packers) + std::floor(state.research_peta_add)) *
                                   std::pow(std::pow(2., 1.0 / 7.),
                                            std::floor(state.research_peta_mul)),
                                   std::min(1., (state.building_overflows - 7.) /
                                                std::pow(5., 7.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow((std::floor(state.building_tera_packers) + std::floor(state.research_tera_add)) *
                                   std::pow(std::pow(2., 1.0 / 6.),
                                            std::floor(state.research_tera_mul)),
                                   std::min(1., (state.building_overflows - 6.) /
                                                std::pow(5., 6.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow((std::floor(state.building_giga_packers) + std::floor(state.research_giga_add)) *
                                   std::pow(std::pow(2., 1.0 / 5.),
                                            std::floor(state.research_giga_mul)),
                                   std::min(1., (state.building_overflows - 4.) /
                                                std::pow(5., 5.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow((std::floor(state.building_mega_packers) + std::floor(state.research_mega_add)) *
                                   std::pow(std::pow(2., 1.0 / 4.),
                                            std::floor(state.research_mega_mul)),
                                   std::min(1., (state.building_overflows - 3.) /
                                                std::pow(5., 4.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow((std::floor(state.building_kilo_packers) + std::floor(state.research_kilo_add)) *
                                   std::pow(std::pow(2., 1.0 / 3.),
                                            std::floor(state.research_kilo_mul)),
                                   std::min(1., (state.building_overflows - 2.) /
                                                std::pow(5., 3.) + 1.));
        procured_value *= 1024.;
        procured_value += std::pow(
                (std::floor(state.building_bytes) + std::floor(state.research_bytes_add)) *
                std::pow(std::pow(2., 1.0 / 2.),
                         std::floor(
                                 state.research_bytes_mul)),
                std::min(1., (state.building_overflows - 1.) /
                             std::pow(5., 2.) + 1.));
        procured_value *= 8.;
        procured_value += std::pow((std::floor(state.building_bits) + std::floor(state.research_bits_add)) *
                                   std::pow(2., std::floor(state.research_bits_mul)),
                                   std::min(1., state.building_overflows / 5. + 1.));
        procured_value *= (std::floor(state.building_processes) *
                           std::pow(3., std::floor(state.research_process_mul)));
        procured_value *= time_mul * 0.1;
        procured_value *= time_diff;
        return procured_value;
    }

    static state_model update_game_state(const state_model initial_state, const int64_t new_time) {
        state_model new_state = initial_state;
        new_state.time = new_time;
        int64_t time_diff = new_time - initial_state.time - 21500;
        if (time_diff < 0) {
            return initial_state;
        }
        long double procured_value = peek_procured_value(initial_state, time_diff);
        procured_value *= std::pow(2., 5. * initial_state.boost_priority);
        long double building_value = procured_value * std::pow(initial_state.build_priority, 2.);
        long double research_value = procured_value * std::pow(initial_state.research_priority, 2.);
        switch (initial_state.focused_building) {
            case 0:
                if (new_state.building_overflows <= 0) {
                    new_state.building_bits = std::min(
                            std::pow(std::pow(initial_state.building_bits, 1.1) + building_value, 1.0 / 1.1),
                            static_cast<long double>(8. * 1.0001));
                } else {
                    new_state.building_bits = std::pow(std::pow(initial_state.building_bits, 1.1) + building_value,
                                                       static_cast<long double>(1.0 / 1.1));
                }
                break;
            case 1:
                if (new_state.building_overflows <= 0) {
                    break;
                } else if (new_state.building_overflows == 1) {
                    new_state.building_bytes = std::min(
                            std::pow(std::pow(24. * initial_state.building_bytes, 1.1) + building_value, 1.0 / 1.1) /
                            24.,
                            static_cast<long double>(1024. * 1.0001));
                } else {
                    new_state.building_bytes =
                            std::pow(std::pow(24. * initial_state.building_bytes, 1.1) + building_value, 1.0 / 1.1) /
                            24.;
                }
                break;
            case 2:
                if (new_state.building_overflows <= 1) {
                    break;
                } else if (new_state.building_overflows == 2) {
                    new_state.building_kilo_packers = std::min(
                            std::pow(std::pow(1024. * 3. * initial_state.building_kilo_packers, 1.1) + building_value,
                                     1.0 / 1.1) /
                            (1024. * 3.),
                            static_cast<long double>(std::pow(1024. * 3., 1.1) * 1.0001)
                    );
                } else {
                    new_state.building_kilo_packers =
                            std::pow(std::pow(1024. * 3. * initial_state.building_kilo_packers, 1.1) + building_value,
                                     1.0 / 1.1) /
                            (1024. * 3.);
                }
                break;
            case 3:
                if (new_state.building_overflows <= 2) {
                    break;
                } else if (new_state.building_overflows == 3) {
                    new_state.building_mega_packers = std::min(
                            std::pow(std::pow((std::pow(1024., 2.) * 3.) * initial_state.building_mega_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 2.) * 3.),
                            static_cast<long double>(std::pow((std::pow(1024., 2.) * 3.), 1.1) * 1.0001)
                    );
                } else {
                    new_state.building_mega_packers =
                            std::pow(std::pow((std::pow(1024., 2.) * 3.) * initial_state.building_mega_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 2.) * 3.);
                }
                break;
            case 4:
                if (new_state.building_overflows <= 3) {
                    break;
                } else if (new_state.building_overflows == 4) {
                    new_state.building_giga_packers = std::min(
                            std::pow(std::pow((std::pow(1024., 3.) * 3.) * initial_state.building_giga_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 3.) * 3.),
                            static_cast<long double>(std::pow((std::pow(1024., 3.) * 3.), 1.1) * 1.0001)
                    );
                } else {
                    new_state.building_giga_packers =
                            std::pow(std::pow((std::pow(1024., 3.) * 3.) * initial_state.building_giga_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 3.) * 3.);
                }
                break;
            case 5:
                if (new_state.building_overflows <= 4) {
                    break;
                } else if (new_state.building_overflows == 5) {
                    new_state.building_tera_packers = std::min(
                            std::pow(std::pow((std::pow(1024., 4.) * 3.) * initial_state.building_tera_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 4.) * 3.),
                            static_cast<long double>(std::pow((std::pow(1024., 4.) * 3.), 1.1) * 1.0001)
                    );
                } else {
                    new_state.building_tera_packers =
                            std::pow(std::pow((std::pow(1024., 4.) * 3.) * initial_state.building_tera_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 4.) * 3.);
                }
                break;
            case 6:
                if (new_state.building_overflows <= 5) {
                    break;
                } else if (new_state.building_overflows == 6) {
                    new_state.building_peta_packers = std::min(
                            std::pow(std::pow((std::pow(1024., 5.) * 3.) * initial_state.building_peta_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 5.) * 3.),
                            static_cast<long double>(std::pow((std::pow(1024., 5.) * 3.), 1.1) * 1.0001)
                    );
                } else {
                    new_state.building_peta_packers =
                            std::pow(std::pow((std::pow(1024., 5.) * 3.) * initial_state.building_peta_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 5.) * 3.);
                }
                break;
            case 7:
                if (new_state.building_overflows <= 6) {
                    break;
                } else {
                    new_state.building_exa_packers =
                            std::pow(std::pow((std::pow(1024., 6.) * 3.) * initial_state.building_exa_packers, 1.1) +
                                     building_value,
                                     1.0 / 1.1) / (std::pow(1024., 6.) * 3.);
                }
                break;
            case 8:
                if (new_state.building_overflows <= 3) {
                    break;
                } else {
                    new_state.building_processes = std::pow(
                            std::pow(initial_state.building_processes, 5.) + building_value,
                            1.0 / 5.);
                }
                break;
            default:
                break;
        }
        switch (initial_state.focused_research) {
            case 0:
                new_state.research_bits_add =
                        std::pow(std::pow(100. * initial_state.research_bits_add, 1.5) + research_value, 1.0 / 1.5) /
                        100.;
                break;
            case 1:
                new_state.research_bits_mul =
                        std::pow(std::pow(500. * initial_state.research_bits_mul, 4000.) + research_value,
                                 1.0 / 4000.) /
                        500.;
                break;
            case 2:
                new_state.research_bytes_add =
                        std::pow(std::pow(24. * 100. * initial_state.research_bytes_add, 1.5) + research_value,
                                 1.0 / 1.5) /
                        (24. * 100.);
                break;
            case 3:
                new_state.research_bytes_mul =
                        std::pow(std::pow(24. * 500. * initial_state.research_bytes_mul, 4000.) + research_value,
                                 1.0 / 4000.) /
                        (24. * 500.);
                break;
            case 4:
                new_state.research_kilo_add =
                        std::pow(std::pow(1024. * 3. * 100. * initial_state.research_kilo_add, 1.5) + research_value,
                                 1.0 / 1.5) /
                        (1024. * 3. * 100.);
                break;
            case 5:
                new_state.research_kilo_mul =
                        std::pow(std::pow(1024. * 3. * 500. * initial_state.research_kilo_mul, 4000.) + research_value,
                                 1.0 / 4000.) /
                        (1024. * 3. * 500.);
                break;
            case 6:
                new_state.research_mega_add =
                        std::pow(std::pow((std::pow(1024., 2.) * 3.) * 100. * initial_state.research_mega_add, 1.5) +
                                 research_value,
                                 1.0 / 1.5) / ((std::pow(1024., 2.) * 3.) * 100.);
                break;
            case 7:
                new_state.research_mega_mul =
                        std::pow(std::pow((std::pow(1024., 2.) * 3.) * 500. * initial_state.research_mega_mul, 4000.) +
                                 research_value,
                                 1.0 / 4000.) / ((std::pow(1024., 2.) * 3.) * 500.);
                break;
            case 8:
                new_state.research_giga_add =
                        std::pow(std::pow((std::pow(1024., 3.) * 3.) * 100. * initial_state.research_giga_add, 1.5) +
                                 research_value,
                                 1.0 / 1.5) / ((std::pow(1024., 3.) * 3.) * 100.);
                break;
            case 9:
                new_state.research_giga_mul =
                        std::pow(std::pow((std::pow(1024., 3.) * 3.) * 500. * initial_state.research_giga_mul, 4000.) +
                                 research_value,
                                 1.0 / 4000.) / ((std::pow(1024., 3.) * 3.) * 500.);
                break;
            case 10:
                new_state.research_tera_add =
                        std::pow(std::pow((std::pow(1024., 4.) * 3.) * 100. * initial_state.research_tera_add, 1.5) +
                                 research_value,
                                 1.0 / 1.5) / ((std::pow(1024., 4.) * 3.) * 100.);
                break;
            case 11:
                new_state.research_tera_mul =
                        std::pow(std::pow((std::pow(1024., 4.) * 3.) * 500. * initial_state.research_tera_mul, 4000.) +
                                 research_value,
                                 1.0 / 4000.) / ((std::pow(1024., 4.) * 3.) * 500.);
                break;
            case 12:
                new_state.research_peta_add =
                        std::pow(std::pow((std::pow(1024., 5.) * 3.) * 100. * initial_state.research_peta_add, 1.5) +
                                 research_value,
                                 1.0 / 1.5) / ((std::pow(1024., 5.) * 3.) * 100.);
                break;
            case 13:
                new_state.research_peta_mul =
                        std::pow(std::pow((std::pow(1024., 5.) * 3.) * 500. * initial_state.research_peta_mul, 4000.) +
                                 research_value,
                                 1.0 / 4000.) / ((std::pow(1024., 5.) * 3.) * 500.);
                break;
            case 14:
                new_state.research_exa_add =
                        std::pow(std::pow((std::pow(1024., 6.) * 3.) * 100. * initial_state.research_exa_add, 1.5) +
                                 research_value,
                                 1.0 / 1.5) / ((std::pow(1024., 6.) * 3.) * 100.);
                break;
            case 15:
                new_state.research_exa_mul =
                        std::pow(std::pow((std::pow(1024., 6.) * 3.) * 500. * initial_state.research_exa_mul, 4000.) +
                                 research_value,
                                 1.0 / 4000.) / ((std::pow(1024., 6.) * 3.) * 500.);
                break;
            case 16:
                new_state.research_process_mul =
                        std::pow(std::pow(1e8 * initial_state.research_process_mul, 8000.) + research_value,
                                 1.0 / 8000.) /
                        (1e8);
                break;
            default:
                break;
        }
        return new_state;
    }

    void dynamic_update_game_state(const long new_time) {
        state_model intermediate = state;
        if (state.dynamic_priority) {
            float best_boost = state.boost_priority;
            float best_build = state.build_priority;
            float best_research = state.research_priority;
            state_model inter_state = update_game_state(intermediate, new_time + 180000);
            long double best_boost_value = peek_procured_value(inter_state, 1000);
            for (int i = 5; i <= 90; i += 5) {
                for (int j = 5; j <= 95 - i; j += 5) {
                    for (int k = 5; k <= 95 - i - j; k += 5) {
                        intermediate.boost_priority = i / 100.;
                        intermediate.build_priority = j / 100.;
                        intermediate.research_priority = k / 100.;
                        inter_state = update_game_state(intermediate, new_time + 10000);
                        long double inter_value = peek_procured_value(inter_state, 1000);
                        if (inter_value > best_boost_value) {
                            best_boost_value = inter_value;
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
        int dynamic_build_focus = state.focused_building == -2 ? 1 : 0;
        if (dynamic_build_focus) {
            int best_build_focus = state.focused_building;
            long double best_projection = 0;
            for (int i = 0; i <= 8; ++i) {
                intermediate.focused_building = i;
                state_model inter_state = update_game_state(intermediate, new_time + 10000);
                long double inter_value = peek_procured_value(inter_state, 1000);
                if (best_projection < inter_value) {
                    best_projection = inter_value;
                    best_build_focus = i;
                }
            }
            intermediate.focused_building = best_build_focus;
        }
        int dynamic_research_focus = state.focused_research == -2 ? 1 : 0;
        if (dynamic_research_focus) {
            int best_research_focus = state.focused_research;
            long double best_projection = 0;
            for (int i = 0; i <= 16; ++i) {
                intermediate.focused_research = i;
                state_model inter_state = update_game_state(intermediate, new_time + 10000);\
            long double inter_value = peek_procured_value(inter_state, 1000);
                if (best_projection < inter_value) {
                    best_projection = inter_value;
                    best_research_focus = i;
                }
            }
            intermediate.focused_research = best_research_focus;
        }
        state_model new_state = update_game_state(intermediate, new_time);
        new_state.focused_building = dynamic_build_focus ? -2 : new_state.focused_building;
        new_state.focused_research = dynamic_research_focus ? -2 : new_state.focused_research;
        state = new_state;
    }

    void reset_non_persistent_params() {
        state.building_bits = 1.0;
        state.building_bytes = 0.;
        state.building_kilo_packers = 0.;
        state.building_mega_packers = 0.;
        state.building_giga_packers = 0.;
        state.building_tera_packers = 0.;
        state.building_peta_packers = 0.;
        state.building_exa_packers = 0.;
        state.research_process_mul = 0.;
        state.research_bits_add = 0.;
        state.research_bits_mul = 0.;
        state.research_bytes_add = 0.;
        state.research_bytes_mul = 0.;
        state.research_kilo_add = 0.;
        state.research_kilo_mul = 0.;
        state.research_mega_add = 0.;
        state.research_mega_mul = 0.;
        state.research_giga_add = 0.;
        state.research_giga_mul = 0.;
        state.research_tera_add = 0.;
        state.research_tera_mul = 0.;
        state.research_peta_add = 0.;
        state.research_peta_mul = 0.;
        state.research_exa_add = 0.;
        state.research_exa_mul = 0.;
        if (state.focused_building != -2) { state.focused_building = -1; }
        if (state.focused_research != -2) { state.focused_research = -1; }
    }
};