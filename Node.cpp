#include "Node.h"
#include "Utils.h"

#include <iostream>
#include <climits>
#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>

Node::~Node()
{
    for(int fd : openRcv)
    {
        close(fd);
    }

    close(mListenFd);
}

void Node::openSocket()
{
    struct sockaddr_in serverSocket = {AF_INET,INADDR_ANY};
    serverSocket.sin_port = mListener.getPort();

    struct sctp_initmsg init;
    init.sinit_num_ostreams = 5;
    init.sinit_max_instreams = 5;
    init.sinit_max_attempts = 4;

    mListenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    if(mListenFd < 0)
    {
        Utils::log( "couldn't make SCTP socket!" ); 
        return;
    }

    int ret = bind(mListenFd, (struct sockaddr*)&serverSocket,sizeof(serverSocket));
    if(ret < 0)
    {
        Utils::log( "coudn't bind socket: " , strerror(errno) );
        return;
    }

    ret = setsockopt(mListenFd, IPPROTO_SCTP, SCTP_INITMSG, &init,sizeof(init));
    if(ret < 0)
    {
        Utils::log( "coudn't set socket: " , strerror(errno) );
        return;
    }


    ret = listen(mListenFd, init.sinit_max_instreams);
    if(ret < 0)
    {
        Utils::log( "coudn't listen!: " , strerror(errno) );
        return;
    }

    Utils::log( "socket open!" );
}

void Node::acceptNeighbors()
{
    if(mListenFd < 0)
    {
        Utils::log( "bad listener!" , mListenFd );
        return;
    }



    while(openRcv.size() < mNeighbors.size())
    {
        int rxFd = accept(mListenFd, (struct sockaddr*)NULL,NULL);

        if(rxFd < 0)
        {
            Utils::log( "couldn't accept connection: " , strerror(errno) );
            return;
        }

        openRcv.push_back(rxFd);
    }

    Utils::log("accepted all neighbors");
}

void Node::sendTo(int uid, std::string msg)
{
    for(auto& c : mNeighbors)
    {
        if(c.getUid() == uid)
        {
            c.sendMsg(msg);
        }
    }
}

void Node::sendExcept(int uid, std::string msg)
{
    for(auto& c : mNeighbors)
    {
        if(c.getUid() != uid)
        {
            c.sendMsg(msg);
        }
    }
}

void Node::listenToNeighbors()
{
    while(!finishedAlg )
    {
        for(int fd : openRcv)
        {
            if(Utils::pollForFd(fd,10,POLLIN) > 0)
            {
                //Utils::log("reading for fd", fd);
                recvMsg(fd); 
            }
        }
    }
}

void Node::recvMsg(int fd)
{
    const int bufSize = 1024;
    char buf[bufSize];

    struct sctp_sndrcvinfo sndrcvinfo;
    int flags=0;
    
    int in = sctp_recvmsg(fd,buf,bufSize,NULL,0,&sndrcvinfo,&flags);

    if(in > 0)
    {
        std::string strMsg(buf);
        //Utils::log("got ", strMsg);
        //std::cout << "Stream ID: " << sndrcvinfo.sinfo_stream << std::endl;
        //std::cout << "PPID: " << sndrcvinfo.sinfo_ppid << std::endl;
        //std::cout << "Flags: " << flags << std::endl;
        msgHandler(strMsg);
    }
}

Connection Node::getOwnConnection()
{
    return mListener;
}

int Node::getUid()
{
    return mUid;
}

void Node::print()
{
    std::cout << mUid << " ";
    mListener.print();
    std::cout << "\n\t";

    for(auto& c : mNeighbors)
    {
        c.print();
    }

    std::cout << std::endl;
}

void Node::connectAll()
{
    for(auto& n : mNeighbors)
    {
        n.makeConnection();
    }
}

void Node::addConnection(Connection c)
{
    mNeighbors.push_back(c);
}

void Node::flood(std::string str)
{
    for(auto& n : mNeighbors)
    {
        n.sendMsg(str);
    }
}

bool Node::isLeader()
{
    return leader == mUid;
}

std::vector<int> Node::getConnectedUids()
{
    std::vector<int> uids;

    for(auto& n : mNeighbors)
    {
        uids.push_back(n.getUid());
    }

    return uids;
}
