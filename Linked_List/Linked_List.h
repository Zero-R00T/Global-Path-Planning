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
    int road_id;
    float cost;
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
            NodePtr LastNode = NULL;
            g_connection = g_roads[i]["connection"];

            for (int j = 0; j < (int)g_connection.size(); j++)
            {
                
                std::string str;
                   
                NodePtr NewNode = MakeNode(); //노드 생성
                NewNode->road_id = g_connection[j]["road_id"].asInt(); //첫째칸 road_id 입력
                NewNode->cost += g_connection[j]["cost"].asFloat(); //둘재칸 cost offset 입력
                for(int k = 0; k < (int)g_connection[j]["lane_cond"].size(); k++) //셋째칸 차선 조건 배열 입력
                {
                    str = g_connection[j]["lane_cond"][k].asString();
                    NewNode->lanes[k] = MatchLane(str);
                }
                str = g_connection[j]["pass_type"].asString();
                NewNode->passtype = MatchPasstype(str);
                NewNode->HeadAd = &HeadList[NewNode->road_id - 1]; //HeadList 주소 입력
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
        for (int i = 0; HeadList[i] != NULL; i++)
        {
            std::cout << "Head index : " << i << "  ";
            ShowNode(HeadList[i]);
            std::cout << std::endl;
        }
    }

    void DeleteList(NodePtr * HeadList)
    {
        for (int i=0; HeadList[i] != NULL; i++) // HeadList 마지막에 NULL 들어있음
            DeleteNode(HeadList[i]);
        delete []HeadList;
    }

    void CostFunction(NodePtr * HeadList)
    {
        float road_length = 0;
        int lane_num = 0;
        Passtype passtype = P_empty;
        int lane_cond_num = 0;
        for (int i = 0; HeadList[i] != NULL; i++)
        {
            NodePtr LastNode = HeadList[i];
            g_connection = g_roads[i]["connection"];
            road_length = g_roads[i]["road_length"].asFloat();
            lane_num = (int)g_roads[i]["lane"].size();
            for (int j = 0; j < (int)g_connection.size(); j++)
            {
                passtype = MatchPasstype(g_connection[j]["pass_type"].asString());
                lane_cond_num = (int)g_connection[j]["lane_cond"].size();
                LastNode->cost += CaluculateCost(road_length, lane_num, passtype, lane_cond_num);
                LastNode = LastNode->link;              
            }
        }
    }
    
    private:

    float CaluculateCost(float road_length, int lane_num, Passtype passtype, int lane_cond_num)
    {
        //임시적인 cost 계산법 추후에 수정필요
        float cost_sum = 0;
        float coef_road_length(1), coef_lane_num(1), coef_lane_cond_num(1);
        float coef_passtype[5] = {10000000, 100, 85, 150, 350};

        cost_sum += coef_road_length * road_length;
        cost_sum += coef_lane_num * lane_num;
        cost_sum += coef_lane_cond_num * lane_cond_num;
        cost_sum += coef_passtype[passtype];

        return cost_sum;
    }

    NodePtr MakeNode()
    {
        NodePtr temp = new Node;
        temp->road_id = 0;
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
        std::cout << "[Road : R" << temp->road_id << "  Cost : " << temp->cost << "  connected_lane : ";
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

    Lane MatchLane(std::string str)
    {
        if(str == "1")
            return _1;
        else if(str == "2")
            return _2;
        else if(str == "3")
            return _3;
        else if(str == "4")
            return _4;
        else if(str == "5")
            return _5;
        else if(str == "1HL")
            return _1HL;
        else if(str == "1HR")
            return _1HR;
        else if(str == "1TL")
            return _1TL;
        else if(str == "1TR")
            return _1TR;
        else
            return L_empty;
    }

    Passtype MatchPasstype(std::string str)
    {
        if(str == "Straight")
            return Straight;
        else if(str == "Right")
            return Right;
        else if(str == "Left")
            return Left;
        else if(str == "U_Turn")
            return U_Turn;
        else
            return P_empty;
    }
};