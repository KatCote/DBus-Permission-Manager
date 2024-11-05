#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <chrono>

class TimeService {
    public:

    TimeService () {

        bus = sdbus::createSessionBusConnection("com.system.time");

        auto registrationProxy = sdbus::createObject(*bus, "/com/system/time");

        registrationProxy->registerMethod("GetSystemTime")
            .onInterface("com.system.TimeInterface")
            .implementedAs([this]() { return GetSystemTime();});
            
        registrationProxy->finishRegistration();

        bus->enterEventLoop();
    }

    public:

    uint64_t GetSystemTime() {
        std::cout << "Получен запрос на получение SystemTime. ";

        auto proxy = sdbus::createProxy(std::move(bus), "com.system.permissions", "/com/system/permissions");
        bool result = false;

        try {
            proxy->callMethod("CheckApplicationHasPermission")
                .onInterface("com.system.PermissionsInterface")
                .withArguments<std::string, int>("/path/to/bin", 0)
                .storeResultsTo(result);

            std::cout << "Полученное значение: " << result << std::endl;
        } catch (const sdbus::Error& e) {
            std::cerr << "Ошибка вызова метода: " << e.what() << std::endl;
        }

        proxy.release();

        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    private:

    std::unique_ptr<sdbus::IConnection> bus;
};

int main() {

    std::cout << "Попытка запуска сервиса.." << std::endl;

    try {
        TimeService service;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}