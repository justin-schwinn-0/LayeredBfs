#include "LayeredBfsAlg.h"

#include "Utils.h"

LayeredBfsAlg::LayeredBfsAlg(Node& n):
    rNode(n)
{
}

void LayeredBfsAlg::handleMsg(std::string msg)
{
    Utils::log("got",msg);
    auto message = decode(msg);

    switch(message.msgId)
    {
        case PARENT:
            handleParentMsg(message);
            break;
        default:
            Utils::log("unknown message Id:", message.msgId);
            break;
    }
}

void LayeredBfsAlg::handleParentMsg(Message msg)
{
    Utils::log("got parent message", msg.uid);
    if(parent == -1)
    {
        Utils::log("setting parent to", msg.uid);
        parent = msg.uid;
        mDepth = Utils::strToInt(msg.payload);
        Utils::log("sending child ack");
        sendMsg(msg.uid,CHILD_ACK,"none");
    }
    else
    {
        Utils::log("sending refuse to",msg.uid);
        sendMsg(msg.uid,REF_ACK,"none");
    }
}

void LayeredBfsAlg::handleChildAckMsg(Message msg)
{
    Utils::log("got child ack message", msg.uid);
}

void LayeredBfsAlg::init()
{
    // send to all neighbors that i am the parent

    mDepth = 0;
    rNode.flood(getParentMsg(mDepth));
}

void LayeredBfsAlg::sendMsg(int uid, int msgId, std::string msg)
{
    std::string structuredMessage = std::to_string(rNode.getUid())+ID_DELIM+std::to_string(msgId) + ID_DELIM + msg;
    rNode.sendTo(uid,structuredMessage); 
}

Message LayeredBfsAlg::decode(std::string msg)
{
    auto idSegments = Utils::split(msg,ID_DELIM);

    int uid = Utils::strToInt(idSegments[0]);
    int msgId = Utils::strToInt(idSegments[1]);

    return {uid,msgId,idSegments[2]};
}

std::string LayeredBfsAlg::getParentMsg(int depth)
{
    return std::to_string(rNode.getUid())+ID_DELIM+std::to_string(PARENT)+ID_DELIM+std::to_string(depth);
}
