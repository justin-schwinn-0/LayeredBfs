#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <mutex>

#include <sys/socket.h>
#include <sys/types.h>

class Connection
{    
public:
    Connection(int uid ,std::string h, uint32_t p); 

    Connection(); 
    ~Connection();
/*

    void sendMsg(std::string msg);


    void acceptCon();


    void resetRemoteConnection();

*/
    int getUid()
    { return mUid; }

    void sendMsg(std::string msg);

    void makeConnection();

    uint32_t getPort()
    { return port; }

    const std::string& getHostname()
    { return hostname; }

    void print();
private:
    // hostname/ip of connection
    std::string hostname;
    uint32_t port;

    int mCon;

    int mUid;
};

#endif
