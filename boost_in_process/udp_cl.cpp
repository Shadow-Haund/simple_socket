#include <iostream>
#include <boost/asio.hpp>

using udp = boost::asio::ip::udp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class client{
    public:
        client(io_c &io, std::string serv_ip, int serv_port)
        : io_context_(io), server_end_(boost::asio::ip::make_address(serv_ip), serv_port),
        client_end_(udp::v4(), serv_port+1), socket_(io, client_end_) {}

        void start(){send_to_serv(); }

    private:

        void send_to_serv(){
            socket_.async_send_to(boost::asio::buffer(msg_), server_end_, [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                else{
                    std::cout << "Sending: " << msg_ << std::endl;
                }
                recv_from_serv();
            });
        }

        void recv_from_serv(){
            socket_.async_receive_from(boost::asio::buffer(buff_), server_end_, [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                else if (buff_s < 1)
                    recv_from_serv();
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                }
                send_to_serv();
            });
        }

        io_c &io_context_;
        udp::endpoint server_end_;
        udp::endpoint client_end_;
        udp::socket socket_;
        std::string msg_ = "from_cl_to_srv";
        std::array<char, 1024> buff_;
};

int main(){
    io_c io_context;
    int serv_port = 15000;
    client cl(io_context,"127.0.0.1", serv_port);
    cl.start();
    io_context.run();
    return 0;
}