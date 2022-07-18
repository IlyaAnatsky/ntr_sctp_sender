// Test task from NTR
// This is data sender application

#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include "msg_buffer.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

struct SSenderStatistics
{
    int sent_number_packages;

    SSenderStatistics()
    {
        sent_number_packages = 0;
    }
};
static SSenderStatistics recvStat;

//********************************************************************************
//* main() for data sender application                                           *
//********************************************************************************
int main()
{
    std::cout << "\nData sender application is started\n";

    CConfigIni config("config_data_sender.ini");
    config.Init();

    SConfigV configval(config);
    if (configval.incorrect == true)
    {
        return 1;
    }

    std::stringstream confS;
    confS << "local_ip: " << configval.local_ip << std::endl;
    confS << "local_port: " << configval.local_port << std::endl;
    confS << "remote_ip: " << configval.remote_ip << std::endl;
    confS << "remote_port: " << configval.remote_port << std::endl;
    confS << "total_number_packages: " << configval.total_number_packages << std::endl;
    confS << "do_delay_after_aliquot: " << configval.do_delay_after_aliquot << std::endl;
    confS << "delay_after_aliquot_ms: " << configval.delay_after_aliquot_ms << std::endl;
    confS << "sent_delay: " << configval.sent_delay << std::endl;
    confS << "write_file: " << configval.write_file << std::endl;
    confS << "write_hex: " << configval.write_hex << std::endl;
    std::cout << confS.str();

    std::cout << "\nPlease, check the configuration above and press Enter to continue.\n";
    std::cin.ignore();

    std::string local_ip_s = configval.local_ip;
    std::string remote_ip_s = configval.remote_ip;

    // SCTP
    int client_fd, i, flags;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    struct sctp_sndrcvinfo sndrcvinfo;
    struct sctp_event_subscribe events;
    struct sctp_initmsg initmsg;    

    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
 
    bzero( (void *)&local_addr, sizeof(local_addr) );
    local_addr.sin_family = AF_INET;    
    inet_pton(AF_INET, local_ip_s.c_str(), &local_addr.sin_addr);
    local_addr.sin_port = htons(configval.local_port);
 
    bind(client_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    /* Specify that a maximum of 3 streams will be available per socket */
    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 3;
    initmsg.sinit_max_instreams = 3;
    initmsg.sinit_max_attempts = 2;

    setsockopt(client_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

    bzero((void *)&remote_addr, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, remote_ip_s.c_str(), &remote_addr.sin_addr);
    remote_addr.sin_port = htons(configval.remote_port);

    int res_conn = connect(client_fd, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
    if (res_conn != 0)
    {
        std::cout << "\n connect error: " << res_conn << "\n";
        return 1;
    }

    memset((void *)&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;
    setsockopt(client_fd, SOL_SCTP, SCTP_EVENTS,
               (const void *)&events, sizeof(events));

    /* Sending three messages on different streams */

    DataSender::CMsgBuffer msgBufferObj(configval);

    for (int indexMsg = 1; indexMsg <= configval.total_number_packages; ++indexMsg)
    {
        void *sentBuffer_p = msgBufferObj.getNewSentData();
        short sentDataLength = msgBufferObj.getSentDataLength();
        void *recvBuffer_p = msgBufferObj.getReceiveDataBuffer();

        int sent_n = sctp_sendmsg(client_fd, sentBuffer_p, sentDataLength, NULL, 0, 0, 0, 1, 0, 0);
        int recv_n = 0;

        if( sent_n > 0)
        {            
            do
            {
              std::cout << "recv_n == " << recv_n << "\n";

              int recv_n = sctp_recvmsg(client_fd, recvBuffer_p, msgBufferObj.getMaxDataBuffer(), (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags );

              std::cout << "recv_n == " << recv_n << "\n";

            } while (recv_n <= 0);
        }        

        std::cout << "sent_n: " << sent_n << "recv_n: " << recv_n << " ===> ";

        if ((sent_n == recv_n) &&
            (std::equal((uint8_t*)sentBuffer_p, ((uint8_t*)sentBuffer_p) + sent_n, (uint8_t*)recvBuffer_p) == true))

            std::cout << "All Ok" << std::endl;
        else
            std::cout << "Not Ok" << std::endl;

        recvStat.sent_number_packages++;

        // Waiting for some time, expected: 10ms, according to task requirements
        std::this_thread::sleep_for(std::chrono::milliseconds(configval.sent_delay));

        // Check for dalay, expected for sent(1000): 10ms, according to task requirements
        if ((indexMsg % configval.do_delay_after_aliquot) == 0)
        {
            std::cout << "Dalay after sent #MSG: " << indexMsg << ", (" << configval.delay_after_aliquot_ms << "ms)\n";

            // Waiting for some time, expected: 10ms, according to task requirements
            std::this_thread::sleep_for(std::chrono::milliseconds(configval.delay_after_aliquot_ms));
        }
    }
    // zmq_close (clientSocket);
    // zmq_ctx_destroy (clientContext);

    boost::posix_time::ptime now_stat = boost::posix_time::second_clock::local_time();
    std::string time_stat = to_iso_extended_string(now_stat);

    std::stringstream stat;
    stat << "Data sender statistics (" << time_stat << "):\n";
    stat << "Sent total number of packages: " << recvStat.sent_number_packages << "\n";
    stat << "\nConfiguration:\n"
         << confS.str();
    std::cout << std::endl
              << stat.str();

    std::ofstream fout;
    fout.open("data_sender_stat.log");
    fout << stat.str();
    fout.close();

    std::cout << "\nData sender application will be stopped. Please, press Enter to continue.\n";
    std::cin.ignore();
    return 0;
}
