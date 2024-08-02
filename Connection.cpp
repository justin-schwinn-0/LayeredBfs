#include "Connection.h"
#include "Utils.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

Connection::Connection(int uid,std::string h, uint32_t p) : 
    mUid(uid),
    hostname(h),
    port(p)
{
}

Connection::~Connection()
{
    close(mCon);
}

void Connection::sendMsg(std::string msg)
{
    if(mCon < 0)
    {
        Utils::log("we aren't connected to ",hostname);
        return;
    }
    bool sent = false;
    //Utils::log("try send",msg);
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        int ret = sctp_sendmsg(mCon,(void *)msg.c_str(),strlen(msg.c_str())+1,NULL,0,0,0,0,10,0);

        if(ret < 0)
        {
            Utils::error("send failed");
            Utils::log("send faied with ",ret,mCon );
        }
        else
        {
            sent = true;
            //Utils::log("sent:" ,msg);
        }
    }
    while(!sent);

    //load bearing log statment
    //Utils::log("sent ", msg);
}

void Connection::makeConnection()
{

    std::string addr = Utils::getAddressFromHost(hostname);

    struct sockaddr_in serverAddress;
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = port;
    serverAddress.sin_addr.s_addr = inet_addr(addr.c_str());

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if(sd < 0)
    {
        Utils::log( "couldn't make SCTP socket!" ); 
        return;
    }

    int ret;
    do
    {
        sleep(2);
        ret = connect(sd,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    }
    while(ret == ECONNREFUSED);

    if(ret < 0)
    {
        Utils::error("connect failed "+ hostname);
    }
    else
    {
        mCon = sd;
        Utils::log("connection with",hostname,"fd:",sd);
    }
}

void Connection::print()
{
    std::cout << "{ " << hostname << " "<< mUid <<" " << port << " }";
}
