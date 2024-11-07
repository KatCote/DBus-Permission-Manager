#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>
#include <iostream>

enum Permissions
{ 
    SystemTime = 0
};

class PermissionsService
{
    public:

    sqlite3* db;
    char* errMessage = 0;

    public:

    PermissionsService()
    {
        bus = sdbus::createSessionBusConnection("com.system.permissions");

        auto proxy = sdbus::createObject(*bus, "/com/system/permissions");

        proxy->registerMethod("RequestPermission")
            .onInterface("com.system.PermissionsInterface")
            .implementedAs([this](int32_t permissionEnumCode) { RequestPermission(permissionEnumCode);});

        proxy->registerMethod("CheckApplicationHasPermission")
            .onInterface("com.system.PermissionsInterface")
            .implementedAs([this](const std::string& applicationExecPath, int32_t permissionEnumCode) {
                return CheckApplicationHasPermission(applicationExecPath, permissionEnumCode);});
            
        proxy->finishRegistration();

        int sqlRetVal = sqlite3_open("PermissionsDB.db", &db);

        if (sqlRetVal) {
            std::cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << std::endl;
            std::exit(0);
        } else {
            std::cout << "База данных: УСПЕШНО" << std::endl;
        }

        const char * sql = "CREATE TABLE IF NOT EXISTS SystemTimePerm (path TEXT PRIMARY KEY);";
        sqlRetVal = sqlite3_exec(db, sql, 0, 0, &errMessage);
        
        if (sqlRetVal != SQLITE_OK) {
            std::cerr << "Ошибка выполнения SQL запроса: " << errMessage << std::endl;
            sqlite3_free(errMessage);
        } else {
            std::cout << "Таблица: УСПЕШНО" << std::endl;
        }

        bus->enterEventLoop();

        sqlite3_close(db);
    }

    public:

    void RequestPermission(int permissionEnumCode)
    {
        std::string path = "/path/to/bin";

        std::cout << "Запрос разрешения с кодом '" << permissionEnumCode << "'. ";

        const char* sql = "INSERT OR IGNORE INTO SystemTimePerm (path) VALUES ('/path/to/bin');";
        int sqlRetVal = sqlite3_exec(db, sql, 0, 0, &errMessage);

        if (sqlRetVal != SQLITE_OK) {
            std::cerr << "Ошибка выполнения SQL запроса: " << errMessage << std::endl;
            sqlite3_free(errMessage);
        } else {
            std::cout << "Добавлено разрешение для пути '" << path << "'." << std::endl;
        }
    }

    bool CheckApplicationHasPermission(const std::string& applicationExecPath, int permissionEnumCode)
    {
        std::cout <<
            "Проверка разрешения " << "c кодом '" << permissionEnumCode <<
            "' для приложения по пути '" << applicationExecPath << "'.\nРезультат: ";

        auto callback = [](void * data, int argc, char ** argv, char ** azColName)
        {
            auto * ctx = static_cast<std::tuple<std::string, bool*>*>(data);
            std::string compareValue = std::get<0>(*ctx);
            bool * foundMatch = std::get<1>(*ctx);

            for (int i = 0; i < argc; i++)
            {
                std::string dbValue = argv[i] ? argv[i] : "NULL";
                if (dbValue == compareValue)
                {
                    *foundMatch = true;
                    break;
                }
            }

            return 0;
        };

        const char * sql = "SELECT path FROM SystemTimePerm;";
        bool foundMatch = false;
        std::tuple<std::string, bool *> callbackData(applicationExecPath, &foundMatch);

        auto retVal = sqlite3_exec(db, sql, callback, &callbackData, &errMessage);
        if (retVal != SQLITE_OK) {
            std::cerr << "Ошибка выполнения SQL запроса: " << errMessage << std::endl;
            sqlite3_free(errMessage);
        }
        else
        {
            std::cout << (foundMatch ? "РЕЗРЕШЕНО" : "ЗАПРЕЩЕНО") << std::endl;
        }
        
        return (bool) foundMatch;
    }

    private:

    std::unique_ptr<sdbus::IConnection> bus;
};

int main()
{
    std::cout << "Попытка запуска сервиса.." << std::endl;

    try {
        PermissionsService service;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}