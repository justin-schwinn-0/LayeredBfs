#ifndef LAYERED_BFS_ALG_H
#define LAYERED_BFS_ALG_H

#include <string>

#include "Node.h"

const std::string ID_DELIM = "__";
const std::string DATA_DELIM = "--";

const int PARENT = 0;
const int TEST = 1000;


struct Message
{
    int uid;
    int msgId;
    std::string payload;
};

class LayeredBfsAlg 
{
public:
    LayeredBfsAlg(Node& n); 

    void handleMsg(std::string msg);

    void handleParentMsg(Message msg);

    void init();

    void sendMsg(int uid, int msgId, std::string msg);

    Message decode(std::string msg);

    std::string getParentMsg();

private:
    Node& rNode;
};

#endif
