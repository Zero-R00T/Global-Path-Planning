#include <iostream>
#include <fstream>
#include "json/json.h"


enum lane
{
    empty,_1, _2, _3, _4, _5, _1HL, _1HR, _1TL, _1TR
};

typedef struct Node * NodePtr;
typedef struct Node
{
    std::string road_id;
    int cost;
    lane lanes[10];
    NodePtr * HeadAd;
    NodePtr link;
};

NodePtr MakeNode()
{
    NodePtr temp = new Node;
    temp->road_id = {};
    temp->cost = 0;
    *(temp->lanes) = {};
    temp->HeadAd = NULL;
    temp->link = NULL;
    return temp;
}

void match_lane(NodePtr * temp, std::string str, int k)
{
    if(str == "1")
        (*temp)->lanes[k] = _1;
    else if(str == "2")
        (*temp)->lanes[k] = _2;
    else if(str == "3")
        (*temp)->lanes[k] = _3;
    else if(str == "4")
        (*temp)->lanes[k] = _4;
    else if(str == "5")
        (*temp)->lanes[k] = _5;
    else if(str == "1HL")
        (*temp)->lanes[k] = _1HL;
    else if(str == "1HR")
        (*temp)->lanes[k] = _1HR;
    else if(str == "1TL")
        (*temp)->lanes[k] = _1TL;
    else if(str == "1TR")
        (*temp)->lanes[k] = _1TR;
    else
        (*temp)->lanes[k] = empty;
}

int main()
{
    /* --------------------json_parsing--------------------- */
    std::ifstream ist("MAP_Sang-Am.json");
    
    std::string str;
    for (char p; ist >> p;)
    {
        str += p;
    }
    // std::cout << str << std::endl;

    Json::Reader reader;
    Json::Value root;
    bool parsingRet = reader.parse(str, root);

    if (!parsingRet)
    {
        std::cout << "Parsing Failed" << std::endl;
        return 1;
    }
    Json::Value roads = root["roads"];

    /* ---------------------------------------------------- */

    int road_num = (int)roads.size();

    NodePtr *HeadList = new NodePtr[road_num];
    for (int i = 0; i < road_num ; i++)
    {
        HeadList[i] = NULL;
    }
    // std::cout << "Number of roads : " << (int)roads.size() << std::endl << std::endl;
    for (int i = 0; i < (int)roads.size(); i++) // 34개의 도로! 노드 생성들어간다!
    {
        Json::Value connection = roads[i]["connection"];
        std::cout << "Headnode : " << roads[i]["road"].asString() << " ";
        std::cout << "Connected road : ";
        for (int j = 0; j < (int)connection.size(); j++)
        {
            NodePtr LastNode;
            NodePtr temp;
            
            if(j == 0)
            {                
                temp = MakeNode();
                temp->road_id = connection[j]["road"].asString();
                temp->cost += connection[j]["cost"].asFloat();
                for(int k = 0; k < (int)connection[j]["lane_cond"].size(); k++)
                {
                    std::string str = connection[j]["lane_cond"][k].asString();
                    match_lane(&temp, str, k);
                }
                temp->HeadAd = &HeadList[connection[j]["road_id"].asInt() - 1]; 
                std::cout << temp->road_id<< "  Cost : " << temp->cost << "  connected_lane : ";
                for(int i=0;i<10;i++)
                {
                    std::cout << (int)temp->lanes[i] << " ";
                }
                std::cout << "  Adress :"<< temp->HeadAd << "    ";
                HeadList[i] = temp;
                LastNode = temp;
            }
            else
            {
                temp = MakeNode();
                temp->road_id = connection[j]["road"].asString();
                temp->cost += connection[j]["cost"].asFloat();
                for(int k = 0; k < (int)connection[j]["lane_cond"].size(); k++)
                {
                    std::string str = connection[j]["lane_cond"][k].asString();
                    match_lane(&temp, str, k);
                }
                temp->HeadAd = &HeadList[connection[j]["road_id"].asInt() - 1];
                std::cout << temp->road_id<< "  Cost : " << temp->cost << "  connected_lane : ";
                for(int i=0;i<10;i++)
                {
                    std::cout << (int)temp->lanes[i] << " ";
                }
                std::cout << "  Adress :"<< temp->HeadAd << "    ";
                LastNode->link = temp;
                LastNode = temp;
            }
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < (int)roads.size(); i++) // 34개의 도로! 노드 삭제들어간다!
    {
        delete HeadList[i];
    }

    return 0;
}