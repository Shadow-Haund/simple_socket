#include <iostream>
#include <boost/asio.hpp>

using udp = boost::asio::ip::udp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class client{
    public:
        client(io_c &io, std::string serv_ip, int serv_port)
        : io_context_(io),timer_(io), server_end_(boost::asio::ip::make_address(serv_ip), serv_port),
        client_end_(udp::v4(), serv_port+1), socket_(io, client_end_) {}

        void start(){
            watchdog_timer();
            send_to_serv(); 
        }

    private:

        void send_to_serv(){
            i_o = 0;
            socket_.async_send_to(boost::asio::buffer(msg_), server_end_, [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    if (count_send < count_max){
                        count_send++; 
                        std::cout << "Trying to receive msg again, counter = " << count_send << " / " << count_max << std::endl;
                        send_to_serv();
                    }
                    else {
                        std::cerr << "Socket malfunction" << std::endl;
                        return;
                    }
                }
                else{
                    std::cout << "Sending: " << msg_ << std::endl;
                    watchdog_timer();
                    recv_from_serv();
                }
            });
        }

        void recv_from_serv(){
            i_o = 1;
            socket_.async_receive_from(boost::asio::buffer(buff_), server_end_, [this](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    if (count_recv < count_max){
                        count_recv++; 
                        std::cout << "Trying to receive msg again, counter = " << count_recv << " / " << count_max << std::endl;
                        recv_from_serv();
                    }
                    else {
                        std::cerr << "Socket malfunction" << std::endl;
                        return;
                    }
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                    watchdog_timer();
                    send_to_serv();
                }
            });
        }

        void watchdog_timer(){
            timer_.cancel();
            timer_.expires_after(boost::asio::chrono::milliseconds(timer_exp_mil));
            timer_.async_wait([this](const boost::system::error_code& e){
                if (e){ 
                    count = 0;
                    return; 
                } 
                count++;
                std::cout << "Timer expired" << count << " / " << count_max << std::endl;
                if (i_o == 0){
                    std::cout << "Receive problem, retry" << std::endl;
                    recv_from_serv();
                }
                else{
                    std::cout << "Send problem, retry" << std::endl;
                    send_to_serv();
                }
            });
        }

        io_c &io_context_;
        udp::endpoint server_end_;
        udp::endpoint client_end_;
        udp::socket socket_;
        std::string msg_ = "from_cl_to_srv";
        std::array<char, 1024> buff_;
        boost::asio::steady_timer timer_;
        int count_send = 0;
        int count_recv = 0;
        int count_max = 5;
        int count = 0;
        int i_o = 1;
        int timer_exp_mil = 500;
};

int main(){
    io_c io_context;
    int serv_port = 15000;
    client cl(io_context,"127.0.0.1", serv_port);
    cl.start();
    io_context.run();
    return 0;
}