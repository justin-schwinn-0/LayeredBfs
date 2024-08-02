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
        case FIN:
            handleFinish(message);
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
        sendMsg(msg.uid,CHILD_ACK,"1");
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

    int childAdds = Utils::strToInt(msg.payload);

    nodesAdded += childAdds;
    Utils::log("decendednts added", childAdds, "from this connection,",nodesAdded,"total");

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

    if(children.size() == 0)
    {
        expectedConverges = 0;
        for(int n : rNode.getConnectedUids())
        {
            if( n != parent)
            {
                sendMsg(n,PARENT,std::to_string(mDepth));
                expectedConverges++;
            }
        }

        if(expectedConverges == 0)
        {
            Utils::log("no children here, tree branch complete before send");
            sendMsg(parent,LAYER_CC,std::to_string(nodesAdded)+DATA_DELIM+std::to_string(highestDegree));
        }
    }
    else
    {
        nodesAdded = 0;
        broadcastDown();
    }
}

void LayeredBfsAlg::handleLayerCcMsg(Message msg)
{
    Utils::log("got layer cc message", msg.uid);

    auto splits = Utils::split(msg.payload,DATA_DELIM);

    int nodes = Utils::strToInt(splits[0]);
    int degree = Utils::strToInt(splits[1]);
    nodesAdded += nodes;

    compareDegree(degree);


    if(converge())
    {
        if(parent != -1)
        {
            // sum up the children sizes and send
            sendMsg(parent,LAYER_CC,std::to_string(nodesAdded)+DATA_DELIM+std::to_string(highestDegree));
        }
        else
        {
            finishPhase();
        }
    }
}
void LayeredBfsAlg::handleFinish()
{
    Utils::log("Print finish and propogate!") 

    for(int c : children)
    {
        sendMsg(c,FIN,"none");
    }
}

void LayeredBfsAlg::handleAddLayerConverge()
{
    if(parent != -1)
    {
        Utils::log("====== LAYER ADDED");
        // converge cast up
        compareDegree(children.size()+1);
        sendMsg(parent,LAYER_CC,std::to_string(nodesAdded)+DATA_DELIM+std::to_string(highestDegree));
    }
    else
    {
        finishPhase();
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
    expectedConverges = 0;
    for(int c : children)
    {
        sendMsg(c,LAYER_BC,"none");
        expectedConverges++;
    }
}

void LayeredBfsAlg::compareDegree(int d)
{
    if(d > highestDegree)
    {
        highestDegree = d;
    }
}

void LayeredBfsAlg::finishPhase()
{
    Utils::log("===========FINISHED PHASE===========");
    Utils::log("tree added", nodesAdded, " nodes");
    

    if(nodesAdded != 0)
    {
        broadcastDown();
    }
    else
    {
        sendMsg(rNode.getUid(),FIN,"none");
    }
    nodesAdded = 0;
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
