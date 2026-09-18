#include <iostream>
#include <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class server{
    public:
        server(io_c &io, int serv_port)
        : io_context_(io), server_end_(tcp::v4(), serv_port),
        socket_(io), acceptor_(io, server_end_) {}

        
        void start(){
            acceptor_.async_accept(socket_, [this](sys_e e){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else{
                    std::cout << "Connection established" << std::endl;
                    recv_from_ip();
                }
            });
        }

    private:

        void send_to_ip(){
            socket_.async_send(boost::asio::buffer(msg_), [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    return;
                }
                else{
                    std::cout << "Sending: " << msg_ << std::endl;
                    recv_from_ip();
                }
            });
        }


        void recv_from_ip(){
            socket_.async_read_some(boost::asio::buffer(buff_), [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    return;
                }
                else if (buff_s == 0){
                    std::cout << "Connection is clossed" << std::endl;
                    return;
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                    send_to_ip();
                }
            });
        }

        io_c &io_context_;
        tcp::endpoint server_end_;
        tcp::acceptor acceptor_;
        tcp::socket socket_;
        std::string msg_ = "from_cl_to_srv";
        std::array<char, 1024> buff_;
};

int main(int argc, char* argv[]){
    io_c io_context;
    int serv_port = 15000;
    if (argc == 2) serv_port = std::stoi(argv[1]);
    server serv(io_context, serv_port);
    serv.start();
    io_context.run();
    return 0;
}