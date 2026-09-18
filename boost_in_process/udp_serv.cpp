#include <iostream>
#include <boost/asio.hpp>

using udp = boost::asio::ip::udp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class server{
    public:
        server(io_c &io, int serv_port)
        : io_context_(io), server_end_(udp::v4(), serv_port),
        socket_(io, server_end_) {}

        void start(){
            send_to_cl();
            recv_from_cl(); 
        }

        void set_msg(std::string msg_new){
            msg_ = msg_new;
        }
        
        std::string get_msg(){
            return msg_;
        }

    private:

        void send_to_cl(){
            socket_.async_send_to(boost::asio::buffer(msg_), client_end_, [this](sys_e e, size_t buff_s){
                if (e ){
                    std::cerr << e.message() << std::endl;
                    // if (count_send < count_max){
                    //     count_send++; 
                    //     std::cout << "Trying to send msg again, counter = " << count_send << " / " << count_max << std::endl;
                    //     send_to_cl();
                    // }
                    // else {
                    //     std::cerr << "Socket malfunction" << std::endl;
                    //     return;
                    // }
                }
                else{
                    std::cout << "Sending: " << msg_ << std::endl;
                    count_send = 0;
                    recv_from_cl();
                }
            });
        }


        void recv_from_cl(){
            socket_.async_receive_from(boost::asio::buffer(buff_), client_end_, [this](sys_e e, size_t buff_s){
                if (e ){
                    std::cerr << e.message() << std::endl;
                    // if (count_recv < count_max){
                    //     count_recv++; 
                    //     std::cout << "Trying to send msg again, counter = " << count_recv << " / " << count_max << std::endl;
                    //     recv_from_cl();
                    // }
                    // else {
                    //     std::cerr << "Socket malfunction" << std::endl;
                    //     return;
                    // }
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                    count_recv = 0;
                    send_to_cl();
                }
            });
        }

        io_c &io_context_;
        udp::endpoint server_end_;
        udp::endpoint client_end_;
        udp::socket socket_;
        std::string msg_ = "from_srv_to_cl";
        std::array<char, 1024> buff_;
        int count_send = 0;
        int count_recv = 0;
        int count_max = 5;
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