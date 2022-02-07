#pragma once // 추후 자세히 알아봐야함 일단은 헤더파일 선언이 한번만 되겠끔...?

#define MAX_LANE_NUM 10

#include <iostream>
#include <fstream>
#include "json/json.h"

Json::Value g_root;
Json::Value g_roads;
Json::Value g_connection;
static const char * g_Lane_str[] = {"", "1", "2", "3", "4", "5", "1HL", "1HR", "1TL", "1TR"};
static const char * g_Passtype_str[] = {"", "Straight", "Right", "Left", "U_Turn"};

enum Lane
{
    L_empty,_1, _2, _3, _4, _5, _1HL, _1HR, _1TL, _1TR
};
enum Passtype
{
    P_empty, Straight, Right, Left, U_Turn
};
typedef struct Node * NodePtr;
struct Node
{
    std::string road;
    int cost;
    Lane lanes[MAX_LANE_NUM];
    Passtype passtype;
    NodePtr * HeadAd;
    NodePtr link;
};

class Linked_List
{
    public:

    NodePtr * MakeList()
    {
        if(MapParsing() == 1)
            return NULL;

        int road_num = (int)g_roads.size();

        NodePtr * HeadList = new NodePtr[road_num + 1]; 
        for (int i = 0; i < road_num + 1 ; i++)
            HeadList[i] = NULL;
        // HeadList 초기화 & 동적할당 풀 때 편하게 하기 위해 끝에 NULL 넣음
        
        for (int i = 0; i < road_num; i++) // road 갯수만큼 노드 생성
        {
            g_connection = g_roads[i]["connection"];

            for (int j = 0; j < (int)g_connection.size(); j++)
            {
                NodePtr LastNode;
                NodePtr NewNode;
                std::string str;
                   
                NewNode = MakeNode(); //노드 생성
                NewNode->road = g_connection[j]["road"].asString(); //첫째칸 road name 입력
                NewNode->cost += g_connection[j]["cost"].asFloat(); //둘재칸 cost offset 입력
                for(int k = 0; k < (int)g_connection[j]["lane_cond"].size(); k++) //셋째칸 차선 조건 배열 입력
                {
                    str = g_connection[j]["lane_cond"][k].asString();
                    MatchLane(NewNode, str, k);
                }
                str = g_connection[j]["pass_type"].asString();
                MatchPasstype(NewNode, str);
                NewNode->HeadAd = &HeadList[g_connection[j]["road_id"].asInt() - 1]; //HeadList 주소 입력
                if(j == 0)
                    HeadList[i] = NewNode; //HealList에 연결
                else
                    LastNode->link = NewNode; //마지막 노드에 연결
                LastNode = NewNode; //마지막 노드 위치 저장
            }
        }
        return HeadList;
    }

    void ShowList(NodePtr * HeadList)
    {
        for (int road_num = 0; HeadList[road_num] != NULL; road_num++)
        {
            std::cout << "Head index : " << road_num << "  ";
            ShowNode(HeadList[road_num]);
            std::cout << std::endl;
        }
    }

    void DeleteList(NodePtr * HeadList)
    {
        for (int i=0; HeadList[i] != NULL; i++) // HeadList 마지막에 NULL 들어있음
            DeleteNode(HeadList[i]);
        delete HeadList;
    }
    
    private:

    NodePtr MakeNode()
    {
        NodePtr temp = new Node;
        temp->road = {};
        temp->cost = 0;
        for (int i=0; i < MAX_LANE_NUM; i++)
            temp->lanes[i] = L_empty;
        temp->passtype = P_empty;
        temp->HeadAd = NULL;
        temp->link = NULL;
        return temp;
    }
    
    void DeleteNode(Node * temp)
    {
        if(temp->link != NULL)
            DeleteNode(temp->link);
        delete temp;
    }

    void ShowNode(Node * temp)
    {
        if(temp->link != NULL)
        {
            ShowNode(temp->link);
        }
        std::cout << "[Road : " << temp->road << "  Cost : " << temp->cost << "  connected_lane : ";
        for(int i=0;i<MAX_LANE_NUM;i++)
            if (temp->lanes[i] != 0)
                std::cout << g_Lane_str[temp->lanes[i]] << " ";
        std::cout << " pass_type : " << g_Passtype_str[temp->passtype] << "  Adress : " << temp->HeadAd << "]   ";
    }

    int MapParsing()
    {
        std::ifstream ist("MAP_Sang-Am.json");
        std::string str;
        for (char p; ist >> p;)
        {
            str += p;
        }
        Json::Reader reader;
        bool parsingRet = reader.parse(str, g_root);

        if (!parsingRet) // 맵 파싱 실패시
        {
            std::cout << "Parsing Failed" << std::endl;
            return 1;
        }
        g_roads = g_root["roads"];

        return 0;
    }

    void MatchLane(Node * temp, std::string str, int k)
    {
        if(str == "1")
            temp->lanes[k] = _1;
        else if(str == "2")
            temp->lanes[k] = _2;
        else if(str == "3")
            temp->lanes[k] = _3;
        else if(str == "4")
            temp->lanes[k] = _4;
        else if(str == "5")
            temp->lanes[k] = _5;
        else if(str == "1HL")
            temp->lanes[k] = _1HL;
        else if(str == "1HR")
            temp->lanes[k] = _1HR;
        else if(str == "1TL")
            temp->lanes[k] = _1TL;
        else if(str == "1TR")
            temp->lanes[k] = _1TR;
        else
            temp->lanes[k] = L_empty;
    }

    void MatchPasstype(Node * temp, std::string str)
    {
        if(str == "Straight")
            temp->passtype = Straight;
        else if(str == "Right")
            temp->passtype = Right;
        else if(str == "Left")
            temp->passtype = Left;
        else if(str == "U_Turn")
            temp->passtype = U_Turn;
        else
            temp->passtype = P_empty;
    }
};