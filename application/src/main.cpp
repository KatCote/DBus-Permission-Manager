#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <chrono>

class BusConnection {
    public:

    BusConnection () {

        bus = sdbus::createSessionBusConnection("com.system.application");

        auto proxy = sdbus::createProxy(*bus, "com.system.time", "/com/system/time");
        uint64_t result;
        bool access = true;

        try {
            proxy->callMethod("GetSystemTime")
                .onInterface("com.system.TimeInterface")
                .storeResultsTo(result);

            std::cout << "Полученное решение: " << result << std::endl;

        } catch (const sdbus::Error& e) {
            
            if (e.getMessage() == "Unauthorized Access (Operation not permitted)")
            {
                access = false;
                std::cout << "Результат: ЗАПРЕЩЕНО (Unauthorized Access)" << std::endl;
            }
            else
            {
                std::cerr << "Ошибка вызова метода: " << e.what() << std::endl;
            }

        }

        if (access)
        {
            std::cout << "Результат: РАЗРЕШЕНО.\nSystemTime: " << result << std::endl;
        }

        proxy.release();
        bus.release();
    }

    private:

    std::unique_ptr<sdbus::IConnection> bus;
};

int main() {

    std::cout << "Попытка запуска приложения.." << std::endl;

    try {
        BusConnection busConnection;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}