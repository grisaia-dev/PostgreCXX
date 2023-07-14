#pragma once
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <iomanip>

#define ERROR "\x1b[0;31m[-]\x1b[0m "
#define HIT "\x1b[0;33m[*]\x1b[0m "
#define GOOD "\x1b[0;32m[+]\x1b[0m "
#define ENTER "\x1b[0;35m[>]\x1b[0m "
#define GOODBY "\x1b[1;30m[^_^]\x1b[0m "

class pdb {
private:
    enum class MENU {
        READY_FOR_ENTER,
        EXIT,
        CREATE_STRUCTURE,
        DROP_STRUCTURE,
        PRINT_CLIENTS,
        ADD_NEW_CLIENT,
        ADD_PHONE_CLIENT,
        CHANGE_CLIENT_DATA,
        DROP_CLIENT_PHONE,
        DROP_CLIENT,
        SEARCH_CLIENT
    };
public:
    pdb(pqxx::connection& connect_to);
    ~pdb();

    void run();
private:
    void print_menu();
    void menu();
    void print_clients();

    void create_structure();
    void drop_structure();

    void add_new_client();
    void add_phone_to_client();
    void change_client_data();
    void drop_phone_client();
    void drop_client();
    void search_client();
private:
    pqxx::connection m_connection;
    MENU m_state;
    int enter;
};