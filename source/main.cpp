#include <exception>
#include <pdb.hxx>


int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    try {
        pqxx::connection connect(
            "host=localhost "
            "port=5432 "
            "dbname=postgres "
            "user=postgres "
            "password=lol123l123"
        );

        pdb db(connect);
        db.run();
    } catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
