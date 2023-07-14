#include "pdb.hxx"

pdb::pdb(pqxx::connection& connect_to) : m_connection(std::move(connect_to)), m_state(MENU::READY_FOR_ENTER) {
    if (m_connection.is_open())
        std::cout << GOOD << " Подключение выполнено успешно!\n" << "\thost = " << m_connection.hostname() << "\n\tport = " << m_connection.port() << "\n\tdatabase = " << m_connection.dbname() << std::endl; 
}
pdb::~pdb() {
    m_connection.close();
}

void pdb::print_menu() {
    std::cout << HIT << " -------- [Меню] -------- " << std::endl;
    std::cout << "\t[1]  Выход" << std::endl;
    std::cout << "\t[2]  Создать структуру БД" << std::endl;
    std::cout << "\t[3]  Удалить структуру БД" << std::endl;
    std::cout << "\t[4]  Вывести список клиентов" << std::endl;
    std::cout << "\t[5]  Добавить новго клиента в БД" << std::endl;
    std::cout << "\t[6]  Добавить номер телефона клиента в БД" << std::endl;
    std::cout << "\t[7]  Изменить данные клиента в БД" << std::endl;
    std::cout << "\t[8]  Удалить номер телефона клиента в БД" << std::endl;
    std::cout << "\t[9]  Удалить клиента из БД" << std::endl;
    std::cout << "\t[10] Поиск клиента (Имени, Фамилии, Почте, Телефону)" << std::endl;
}

void pdb::menu() {
    print_menu();
    std::cout << ENTER << "Введите пункт меню: ";
    std::cin >> enter;
    m_state = static_cast<MENU>(enter);
    switch (m_state) {
        case MENU::READY_FOR_ENTER:
            std::cout << ERROR << "Нет такого пункта меню!" << std::endl;
            break;
        case MENU::EXIT:
            std::cout << GOODBY << "До свидания!" << std::endl;
            break;
        case MENU::CREATE_STRUCTURE:
            create_structure();
            break;
        case MENU::DROP_STRUCTURE:
            drop_structure();
            break;
        case MENU::PRINT_CLIENTS:
            print_clients();
            break;
        case MENU::ADD_NEW_CLIENT:
            add_new_client();
            break;
        case MENU::ADD_PHONE_CLIENT:
            add_phone_to_client();
            break;
        case MENU::CHANGE_CLIENT_DATA:
            change_client_data();
            break;
        case MENU::DROP_CLIENT_PHONE:
            drop_phone_client();
            break;
        case MENU::DROP_CLIENT:
            drop_client();
            break;
        case MENU::SEARCH_CLIENT:
            search_client();
            break;
        default:
            std::cout << ERROR << "Нет такого пункта меню!" << std::endl;
            break;
    }
}

void pdb::print_clients() {
    pqxx::work tx{m_connection};
    std::cout << HIT << " --- (КЛИЕНТЫ) --- " << std::endl;
    std::cout << "\t" << GOODBY << "ID" << "   -   " << "ИМЯ" << "   -   " << "ФАМИЛИЯ" << "   -   " << "ПОЧТА" << std::endl;
    for (const auto [id, first_name, last_name, email] : tx.query<int, std::string, std::string, std::string>("SELECT клиент.клиент_id, клиент.имя, клиент.фамилия, клиент.почта FROM база.клиент;"))
        std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::endl;  
        // TODO: Жаль что setw с киррилицей работает не правильно
    tx.abort();
}

void pdb::run() {
    do {
        pdb::menu();
    } while (m_state != MENU::EXIT);
}

void pdb::create_structure() {
    pqxx::work tx{m_connection};
    tx.exec(tx.esc("CREATE SCHEMA IF NOT EXISTS база;"));
    tx.exec(tx.esc("CREATE TABLE IF NOT EXISTS база.клиент ("
                       "клиент_id SERIAL NOT NULL PRIMARY KEY, "
                       "имя text NOT NULL, "
                       "фамилия text NOT NULL, "
                       "почта text NOT NULL UNIQUE);"));
    tx.exec(tx.esc("CREATE TABLE IF NOT EXISTS база.телефон ("
                       "телефон_id SERIAL NOT NULL PRIMARY KEY, "
                       "клиент_id integer, "
                       "телефон varchar(11) NOT NULL, "
                       "FOREIGN KEY (клиент_id) REFERENCES база.клиент(клиент_id));"));
    tx.commit();
    std::cout << GOOD << "Структура БД успешно создана!" << std::endl;
}

void pdb::drop_structure() {
    pqxx::work tx{m_connection};
    tx.exec(tx.esc("DROP SCHEMA IF EXISTS база CASCADE;"));
    tx.commit();
    std::cout << GOOD << "Структура БД успешно удалена!" << std::endl;
}

void pdb::add_new_client() {
    std::string first_name, last_name, email;
    std::cout << ENTER << "Введите Имя: "; 
    std::cin >> first_name;
    std::cout << ENTER << "Введите Фамилию: "; 
    std::cin >> last_name;
    std::cout << ENTER << "Введите Почту: "; 
    std::cin >> email;
    pqxx::work tx{m_connection};
    m_connection.prepare("add_client", "INSERT INTO база.клиент(имя, фамилия, почта) VALUES($1, $2, $3)");
    tx.exec_prepared("add_client", first_name, last_name, email);
    tx.commit();
    std::cout << GOOD << "Клиент добавлен!" << std::endl;
}

void pdb::add_phone_to_client() {
    std::string phone = "", client_id = "";
    print_clients();
    pqxx::work tx{m_connection};
    std::cout << GOODBY << "Если хотите вернутся обратно в меню, введите 0!" << std::endl;
    std::cout << ENTER << "Введите какому клиенту (из списка выше) вы хотите добавить номер телефона (id): ";
    std::cin >> client_id;
    if (client_id != "0") {
        std::cout << ENTER << "Введите номер телефона: ";
        std::cin >> std::setw(11) >> phone;
        m_connection.prepare("add_phone", "INSERT INTO база.телефон(клиент_id, телефон) VALUES($1, $2)");
        tx.exec_prepared("add_phone", client_id, phone);
        tx.commit();
        std::cout << GOOD << "Телефон клиента добавлен!" << std::endl;
    } else {
        std::cout << GOODBY << "Возвращаемся в меню!" << std::endl;
    }
}

void pdb::change_client_data() {
    std::string enter_data, change_data, client_id;
    print_clients();
    pqxx::work tx{m_connection};
    std::cout << GOODBY << "Если хотите вернутся в меню введите 0!" << std::endl;
    std::cout << ENTER << "Введите id клиента у которого хотите изменить данные: ";
    std::cin >> client_id;
    if (client_id == "0") {
        std::cout << GOODBY << "Возвращение в меню!" << std::endl;
    } else {
        std::cout << ENTER << "Что именно выхотите изменить у клиента? (имя, фамилия, почта, телефон): ";
        std::cin >> enter_data;
        if (enter_data == "имя") {
            std::cout << ENTER << "Введите на что изменить: ";
            std::cin >> change_data;
            m_connection.prepare("change_first_name", "UPDATE база.клиент SET имя = $1 WHERE клиент_id = $2");
            tx.exec_prepared("change_first_name", change_data, client_id);
            tx.commit();
            std::cout << GOOD << "Имя клиента измененно!" << std::endl;
        } else if (enter_data == "фамилия") {
            std::cout << ENTER << "Введите на что изменить: ";
            std::cin >> change_data;
            m_connection.prepare("change_last_name", "UPDATE база.клиент SET фамилия = $1 WHERE клиент_id = $2");
            tx.exec_prepared("change_last_name", change_data, client_id);
            tx.commit();
            std::cout << GOOD << "Фамилия клиента измененна!" << std::endl;
        } else if (enter_data == "почта") {
            std::cout << ENTER << "Введите на что изменить: ";
            std::cin >> change_data;
            m_connection.prepare("change_email_name", "UPDATE база.клиент SET почта = $1 WHERE клиент_id = $2");
            tx.exec_prepared("change_email_name", change_data, client_id);
            tx.commit();
            std::cout << GOOD << "Почта клиента измененна!" << std::endl;
        } else if (enter_data == "телефон") {
            std::cout << ENTER << "Введите номер телефона: ";
            std::cin >> change_data;
            m_connection.prepare("change_phone", "UPDATE база.телефон SET телефон = $1 WHERE клиент_id = $2");
            tx.exec_prepared("change_phone", change_data, client_id);
            tx.commit();
            std::cout << GOOD << "Телефон клиента изменен!" << std::endl;
        } else {
            std::cout << ERROR << "Нет такого значения! используйте -> имя, фамилия, почта" << std::endl;
        }
    }
}

void pdb::drop_phone_client() {
    std::string client_id = "";
    print_clients();
    std::cout << GOODBY << "Введите 0 что бы выйти в меню!" << std::endl;
    std::cout << ENTER << "Введите id клиента, у которого хотите удалить номер: ";
    std::cin >> client_id;
    if (client_id == "0") {
        std::cout << GOODBY << "Возврат в меню!" << std::endl;   
    } else {
        pqxx::work tx{m_connection};
        m_connection.prepare("drop_phone","DELETE FROM база.телефон WHERE клиент_id = $1");
        tx.exec_prepared("drop_phone", client_id);
        tx.commit();
        std::cout << GOOD << "Телефон клиента удален из БД!" << std::endl;
    }
}

void pdb::drop_client() {
    std::string client_id = "";
    print_clients();
    std::cout << GOODBY << "Введите 0 что бы выйти в меню!" << std::endl;
    std::cout << ENTER << "Введите id клиента которого хотите удалить из БД: ";
    std::cin >> client_id;
    if (client_id == "0") {
        std::cout << GOODBY << "Возвращение в меню!" << std::endl;
    } else {
        pqxx::work tx{m_connection};
        m_connection.prepare("drop_client", "DELETE FROM база.клиент WHERE клиент_id = $1");
        tx.exec_prepared("drop_client", client_id);
        tx.commit();
        std::cout << GOOD << "Клиент удален из БД!" << std::endl;
    }
}

void pdb::search_client() {
    std::string search_enter, name, last_name;
    int state;
    std::cout << HIT << "Как вы хотите найти клиента?\n(0 - Выход, 1 - Имя, 2 - Фамилия, 3 - Почта, 4 - Телефон, 5 - Имени и Фамилии)\n" << ENTER;
    std::cin >> state;
    pqxx::work tx{m_connection};
    switch (state) {
        case 0:
            std::cout << GOODBY << "Возвращение в меню!" << std::endl;
            break;
        case 1:
            std::cout << ENTER << "Введите имя: ";
            std::cin >> search_enter;
            for (const auto [id, first_name, last_name, email] : tx.query<int, std::string, std::string, std::string>("SELECT клиент_id, имя, фамилия, почта FROM база.клиент WHERE имя = '" + tx.esc(search_enter) + "';"))
                std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::endl;
            break;
        case 2:
            std::cout << ENTER << "Введите фамилию: ";
            std::cin >> search_enter;
            for (const auto [id, first_name, last_name, email] : tx.query<int, std::string, std::string, std::string>("SELECT клиент_id, имя, фамилия, почта FROM база.клиент WHERE фамилия = '" + tx.esc(search_enter) + "';"))
                std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::endl;
            break;
        case 3:
            std::cout << ENTER << "Введите почту: ";
            std::cin >> search_enter;
            for (const auto [id, first_name, last_name, email] : tx.query<int, std::string, std::string, std::string>("SELECT клиент_id, имя, фамилия, почта FROM база.клиент WHERE почта = '" + tx.esc(search_enter) + "';"))
                std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::endl;
            break;
        case 4:
            std::cout << ENTER << "Введите номер телефона: ";
            std::cin >> search_enter;
            for (const auto [id, first_name, last_name, email, phone] : tx.query<int, std::string, std::string, std::string, std::string>("SELECT клиент.клиент_id, имя, фамилия, почта, телефон.телефон FROM база.клиент INNER JOIN база.телефон ON клиент.клиент_id = телефон.клиент_id WHERE телефон.телефон = '"+ tx.esc(search_enter) +"';"))
                std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::setw(19) << phone << std::endl;
            break;
        case 5:
            std::cout << ENTER << "Введите имя: ";
            std::cin >> name;
            std::cout << ENTER << "Введите фамилию: ";
            std::cin >> last_name;
            for (const auto [id, first_name, last_name, email] : tx.query<int, std::string, std::string, std::string>("SELECT клиент_id, имя, фамилия, почта FROM база.клиент WHERE имя = '" + tx.esc(name) + "' AND фамилия = '" + tx.esc(last_name) + "';"))
                std::cout << "\t" << HIT << std::setw(4) << id << std::setw(10) << first_name << std::setw(14) << last_name << std::setw(18) << email << std::endl;
            break;
        default:
            std::cout << ERROR << "Нет такого пункта!" << std::endl;
            break;
    }
}