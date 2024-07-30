#ifndef LAYERED_BFS_ALG_H
#define LAYERED_BFS_ALG_H

#include <string>

#include "Node.h"

const std::string ID_DELIM = "__";
const std::string DATA_DELIM = "--";

const int PARENT = 0;


struct Message
{
    int uid;
    int msgId;
    std::string payload;
};

class LayeredBfsALg 
{
public:
    LayeredBfsALg(Node& n); 

    void handleMsg(std::string msg);

    void init();

    void sendMsg(int uid, int msgId, std::string msg);

    Message decode(std::string msg);

private:
    Node& rNode;
};

#endif
