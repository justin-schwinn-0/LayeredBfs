#include "LayeredBfsAlg.h"

#include "Utils.h"

LayeredBfsAlg::LayeredBfsAlg(Node& n):
    rNode(n)
{
}

void LayeredBfsAlg::handleMsg(std::string msg)
{

}

void LayeredBfsAlg::init()
{
    // send to all neighbors that i am the parent

    rNode.flood(std::to_string(rNode.getUid())+ID_DELIM+"test"+DATA_DELIM+std::to_string(1));
}

void LayeredBfsAlg::sendMsg(int uid, int msgId, std::string msg)
{
    std::string structuredMessage = std::to_string(uid)+ID_DELIM+std::to_string(msgId) + ID_DELIM + msg;
    rNode.sendTo(uid,structuredMessage); 
}

Message LayeredBfsAlg::decode(std::string msg)
{
    auto idSegments = Utils::split(msg,ID_DELIM);

    int uid = Utils::strToInt(idSegments[0]);
    int msgId = Utils::strToInt(idSegments[1]);

    return {uid,msgId,idSegments[2]};
}
