#ifndef LAYERED_BFS_ALG_H
#define LAYERED_BFS_ALG_H

#include <string>

#include "Node.h"

const std::string ID_DELIM = "__";
const std::string DATA_DELIM = "--";

const int PARENT = 0;
const int CHILD_ACK = 1;
const int REF_ACK = 2;
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

    void handleChildAckMsg(Message msg);

    void handleRefAckMsg(Message msg);

    void init();

    void sendMsg(int uid, int msgId, std::string msg);

    Message decode(std::string msg);

    std::string getParentMsg(int depth);
private:
    Node& rNode;

    int parent = -1;
    int mDepth = -1;
};

#endif
