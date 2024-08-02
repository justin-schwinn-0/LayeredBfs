#include "LayeredBfsAlg.h"

#include "Utils.h"

LayeredBfsAlg::LayeredBfsAlg(Node& n):
    rNode(n)
{
}

void LayeredBfsAlg::handleMsg(std::string msg)
{
    auto message = decode(msg);

    switch(message.msgId)
    {
        case PARENT:
            handleParentMsg(message);
            break;
        case CHILD_ACK:
            handleChildAckMsg(message);
            break;
        case REF_ACK:
            handleRefAckMsg(message);
            break;
        case LAYER_BC:
            handleLayerBcMsg(message);
            break;
        case LAYER_CC:
            handleLayerCcMsg(message);
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
        mDepth = Utils::strToInt(msg.payload)+1;
        Utils::log("setting depth", mDepth);
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
    children.push_back(msg.uid);

    if(converge())
    {
        handleAddLayerConverge();
    }
}


void LayeredBfsAlg::handleRefAckMsg(Message msg)
{
    Utils::log("got ref ack message", msg.uid);

    if(converge())
    {
        handleAddLayerConverge();
    }
}

void LayeredBfsAlg::handleLayerBcMsg(Message msg)
{
    Utils::log("got layer message", msg.uid);

    expectedConverges = 0;
    for(int n : rNode.getConnectedUids())
    {
        if( n != parent)
        {
            sendMsg(n,PARENT,std::to_string(mDepth));
            expectedConverges++;
        }
    }
}

void LayeredBfsAlg::handleLayerCcMsg(Message msg)
{
    Utils::log("got layer cc message", msg.uid);

    if(converge())
    {
        broadcastDown();
    }
}

void LayeredBfsAlg::handleAddLayerConverge()
{
    if(parent != -1)
    {
        Utils::log("converge up");
        // converge cast up
        sendMsg(parent,LAYER_CC,std::to_string(children.size()));
    }
    else
    {
        broadcastDown();
    }
}

void LayeredBfsAlg::init()
{
    // send to all neighbors that i am the parent

    mDepth = 0;
    rNode.flood(getParentMsg(mDepth));
    expectedConverges = rNode.getNeighborsSize();
    Utils::log("expect converge",expectedConverges);
}

bool LayeredBfsAlg::converge()
{
    Utils::log("see converge",expectedConverges);
    expectedConverges--;
    if(expectedConverges == 0)
    {
        Utils::log("did converge");
        return true;
    }
    Utils::log("didnt converge",expectedConverges);
    return false;
}

void LayeredBfsAlg::broadcastDown()
{
    // broadcast down
    for(int c : children)
    {
        sendMsg(c,LAYER_BC,"none");
    }
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
