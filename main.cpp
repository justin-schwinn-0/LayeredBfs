#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <functional>
#include <climits>

#include "Node.h"
#include "Connection.h"
#include "Utils.h"


struct NodeInfo
{
    int numNodes;
    Node n;
};

NodeInfo readConfig(std::string configFile, int popId = -1)
{
    std::ifstream file(configFile) ;
    Utils::log( "reading file: " , configFile );

    std::string wholeFile;
    std::string line;

    while(std::getline(file,line))
    {
        wholeFile += line + "\n";
    }
    auto lines = Utils::split(wholeFile,"\n");



    // sanitize lines to remove comments
    
    for(auto it = lines.rbegin(); it != lines.rend(); it++)
    {
        // erase the while line if it starts with #
        std::string& str = *it;
        if(str.at(0) == '#')
        {
            lines.erase((it+1).base());
        }
        else
        {
           size_t pos = 0;
           // if we find a #, erase it and everything after
           if((pos = str.find("#")) != std::string::npos)
           {
               str.erase(str.begin()+pos,str.end());
           }
        }
    }
    // use lines to create N nodes, and return 1 for this process
    auto firstLine = Utils::split(lines[0]," ");

    int numNodes;
    if(firstLine.size() != 1)
    {
        // if length of the first line isn't 1,
        // then something when wrong in the sanitization 
        // or the config is invalid
        Utils::log( "First valid line of Config is too long!" );
    }
    else
    {
        std::istringstream iss(firstLine[0]);

        iss >> numNodes;
    }


    std::vector<Node> nodes;
    for(int i = 1; i < numNodes+1;i++)
    {
        auto splitNode = Utils::split(lines[i]," ");

        if(splitNode.size() == 3)
        {
            std::istringstream uidSS(splitNode[0]); 
            std::istringstream portSS(splitNode[2]); 

            int uid;
            uint32_t port;

            uidSS >> uid;
            portSS >> port;

            Node n(uid,{uid,splitNode[1],port});
            nodes.push_back(n);
            //Utils::log( "adding node " , uid );

        }
        else
        {
            Utils::log( "node Line " , i , " is invalid" );
        }
        
    }

    // get distinguished node
    auto distLine = Utils::split(lines[numNodes+1]," ");
    int distNode;
    if(distLine.size() != 1)
    {
        Utils::log("Distinguished Node isnt defined properly!");
    }
    else
    {
        distNode = Utils::strToInt(distLine[0]);
    }


    for(int i = numNodes+2; i < lines.size(); i++)
    {
        std::string conLine = lines[i];
        auto connections = Utils::split(conLine," ");

        if(connections.size() == 1)
        {
            std::string endpoints = connections[0].substr(1,connections[0].size()-2);

            auto ends = Utils::split(endpoints,",");

            int end1 = Utils::strToInt(ends[0]);
            int end2 = Utils::strToInt(ends[1]);

            int end1Index = -1; 
            int end2Index = -1;

            for(int j = 0; j < nodes.size();j++)
            {
                if(nodes[j].getUid() == end1)
                {
                    end1Index = j;
                }
                else if(nodes[j].getUid() == end2)
                {
                    end2Index = j;
                }
            }

            auto addConToNode = [&](int n,int far)
            {
                nodes[n].addConnection(
                        {nodes[far].getUid(),
                         nodes[far].getOwnConnection().getHostname(),
                         nodes[far].getOwnConnection().getPort()});
            };

            addConToNode(end1Index,end2Index);
            addConToNode(end2Index,end1Index);
        }
        else
        {
            Utils::log("connection line invalid:", conLine);
        }

    }

/*    for(auto& n : nodes)
    {
        n.addConnection(
            {n.getUid(),
             n.getOwnConnection().getHostname(),
             n.getOwnConnection().getPort(),
             INT_MAX});
    }*/

    if(popId > 0)
    {
        for(auto n : nodes)
        {
            if(n.getUid() == popId)
            {
                return {numNodes,n};
            }
        }

        Utils::log( "could not find id: " , popId ); 
    }
    else
    {
        for(auto& n : nodes)
        {
            n.print();
        }
    }

    return {numNodes,nodes[0]};
}

/*template<class T>
void runAlg(NodeInfo& ni)
{
    ni.n.print();
    ni.n.openSocket();


    Sync sync(ni.n.getNeighborsSize());

    T t(ni.n,ni.numNodes);
    sync.setHandlers<T>(t);

    ni.n.setHandler(std::bind(&Sync::msgHandler,sync,std::placeholders::_1));

    ni.n.connectAll();

    ni.n.acceptNeighbors();
    sync.init();
    ni.n.listenToNeighbors(2000);
}*/

int main(int argc,char** argv)
{
    int uid;
    if(argc == 3)
    {
        uid = std::stoi(argv[2]);

        auto n = readConfig(argv[1],uid);

    }
    else
    {
        readConfig("testConfig.txt");
    }
    return 0;
}
