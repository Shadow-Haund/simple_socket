#include <iostream>
#include <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class server{
    public:
        server(io_c &io, std::string serv_ip, int serv_port)
        : io_context_(io), server_end_(boost::asio::ip::make_address(serv_ip), serv_port),
        socket_(io) {}

        
        void start(){
            socket_.async_connect(server_end_, [this](sys_e e){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else{
                    std::cout << "Connection established" << std::endl;
                    send_to_serv();
                }
            }); 
        }

    private:

        void send_to_serv(){
            socket_.async_send(boost::asio::buffer(msg_), [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                else{
                    std::cout << "Sending: " << msg_ << std::endl;
                }
                recv_from_serv();
            });
        }


        void recv_from_serv(){
            socket_.async_read_some(boost::asio::buffer(buff_), [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                else if(buff_s == 0){
                    return;
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                }
                send_to_serv();
            });
        }

        io_c &io_context_;
        tcp::endpoint server_end_;
        tcp::endpoint client_end_;
        tcp::socket socket_;
        std::string msg_ = "from_cl_to_srv";
        std::array<char, 1024> buff_;
};

int main(){
    io_c io_context;
    int serv_port = 15000;
    server serv(io_context,"127.0.0.1", serv_port);
    serv.start();
    io_context.run();
    return 0;
}