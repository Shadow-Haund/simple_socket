#include <iostream>
#include <string>

struct params {
    int use_hz = 0;
    int hz = 1;
    int serv_port = 15000;
    std::string serv_ip = "127.0.0.1";
    std::string msg = "client_to_server";
};

void print_usage_cl(const char* prog) {
    std::cout 
        << "Usage: " << prog << " [options]\n"
        << "Options:\n"
        << "  --use_hz<int>      Использовать возможность отправлять сообщения с заданной частотой.\n"
        << "                     0 - не использовать, 1 - использовать (по-умолчанию: 0)\n"
        << "  --hz <int>         Частота (по-умолчанию: 1)\n"
        << "  --port <int>       Порт (по-умолчанию: 5000)\n"
        << "  --ip <string>      IP-адрес (по-умолчанию: 127.0.0.1)\n"
        << "  --msg <string>     Сообщение(по-умолчанию: \"client_to_server\")\n"
        << "  -h, --help         Справка\n"
        << "\nExamples:\n"
        << "  " << prog << " --msg hello\n"
        << "  " << prog << " --port 8080 --msg hi\n"
        << "  " << prog << " --use_hz 1 --hz 10 --ip 0.0.0.0 --port 9000 --msg test\n";
}


void print_usage_serv(const char* prog) {
    std::cout 
        << "Usage: " << prog << " [options]\n"
        << "Options:\n"
        << "  --port <int>       Порт (по-умолчанию: 5000)\n"
        << "  --msg <string>     Сообщение(по-умолчанию: \"client_to_server\")\n"
        << "  -h, --help         Справка\n"
        << "\nExamples:\n"
        << "  " << prog << " --msg hello\n"
        << "  " << prog << " --port 8080 --msg hi\n"
        << "  " << prog << " --port 9000\n";
}


bool parse_args(int argc, char* argv[], std::shared_ptr<params> p, bool is_serv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            if (is_serv) {print_usage_serv(argv[0]); }
            else {print_usage_cl(argv[0]); }
            std::exit(0);
        }
        else if (arg == "--use_hz") {
            if (++i >= argc) { std::cerr << "Ожидается значение после --use_hz\n"; return false; }
            p->use_hz = std::stoi(argv[i]);
        }
        else if (arg == "--hz") {
            if (++i >= argc) { std::cerr << "Ожидается значение после --hz\n"; return false; }
            p->serv_port = std::stoi(argv[i]);
        }
        else if (arg == "--port") {
            if (++i >= argc) { std::cerr << "Ожидается значение после --port\n"; return false; }
            p->serv_port = std::stoi(argv[i]);
        }
        else if (arg == "--ip") {
            if (++i >= argc) { std::cerr << "Ожидается значение после --ip\n"; return false; }
            p->serv_ip = argv[i];
        }
        else if (arg == "--msg") {
            if (++i >= argc) { std::cerr << "Ожидается значение после --msg\n"; return false; }
            p->msg = argv[i];
        }
        else {
            std::cerr << "Неизвестный аргумент: " << arg << "\n";
            print_usage_cl(argv[0]);
            return false;
        }
    }
    return true;
}
