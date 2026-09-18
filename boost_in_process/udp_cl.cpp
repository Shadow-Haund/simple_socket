#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include "../include/arg_parse.h"
#include "../include/timer.h"

using udp = boost::asio::ip::udp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class client{
    public:
        client(io_c &io, int use_hz, int hz, std::string serv_ip, int serv_port, std::string msg)
        : io_context_(io), timer_(std::make_shared<boost::asio::steady_timer>(io)), server_end_(boost::asio::ip::make_address(serv_ip), serv_port),
        client_end_(udp::v4(), serv_port+1), socket_(io, client_end_), msg_(msg), hz_(hz), use_hz_(use_hz) {}

        void start(){
            recv_from_serv();
            hz_timer(timer_, hz_, [this](){send_to_serv();});
            
        }

        void set_msg(std::string msg_new){
            msg_ = msg_new;
        }
        
        std::string get_msg(){
            return msg_;
        }

    private:

        void send_to_serv(){
            socket_.async_send_to(boost::asio::buffer(msg_), server_end_, [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else if (use_hz_){
                    std::cout << "Sending using timer: " << msg_ << std::endl;
                    hz_timer(timer_, hz_, [this](){send_to_serv();});
                }
                else{
                    std::cout << "Sending no timer: " << msg_ << std::endl;
                    send_to_serv();
                }
            });
        }

        void recv_from_serv(){
            socket_.async_receive_from(boost::asio::buffer(buff_), server_end_, [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                    recv_from_serv();
                }
            });
        }

        io_c &io_context_;
        udp::endpoint server_end_;
        udp::endpoint client_end_;
        udp::socket socket_;
        std::string msg_ = "";
        std::array<char, 1024> buff_;
        std::shared_ptr<boost::asio::steady_timer> timer_;
        int hz_;
        int use_hz_;
};


int main(int argc, char* argv[]){
    io_c io_context;
    std::shared_ptr<params> param_val = std::make_shared<params>();
    parse_args(argc, argv, param_val, false);
    client cl(io_context, param_val->use_hz, param_val->hz, param_val->serv_ip, param_val->serv_port, param_val->msg);
    cl.start();
    io_context.run();
    return 0;
}