#pragma once

#define MAX_COST_SUM_VALUE 999999

#include <iostream>
#include "Linked_List.h"

typedef struct Bead * BeadPtr;
struct Bead
{
    BeadPtr upper_bead;
    int road_id;
    float cost_sum;
    int * checklist;
    BeadPtr link;
};
typedef struct Depth * DepthPtr;
struct Depth
{
    int depth;
    DepthPtr lower_depth;
    BeadPtr bead;
};
typedef struct Message * MessagePtr;
struct Message
{
    MessagePtr prev_link;
    int departure_id;
    int arrival_id;
    int lane_cond[MAX_LANE_NUM];
    int passtype;
    MessagePtr link;
};

class Path_Finder
{
    public:

    MessagePtr FindPath(NodePtr * HeadList, int ego, int goal)
    {
        bool is_bead_gone = false;
        bool did_find_path = false;
        int road_num;
        for (road_num = 0; HeadList[road_num] != NULL; road_num++); // HeadList의 도로 갯수

        BeadPtr final_bead = NULL;
        float Min_cost_sum = MAX_COST_SUM_VALUE; //CheckBeads()를 위해 선언

        MessagePtr HeadMessage = NULL;

        DepthPtr HeadDepth = MakeDepth();
        DepthPtr LastDepth = HeadDepth;
        BeadPtr NewBead = MakeBead(road_num);

        NewBead->road_id = ego; // 첫 번재 Depth 생성
        NewBead->checklist[ego - 1] += 1;
        LastDepth->bead = NewBead;

        // ShowDepth(HeadDepth);

        for (int i = 1; is_bead_gone == false; i++)
        {
            DepthPtr NewDepth = MakeDepth();
            BeadPtr BeadTail = NULL;
            int Bead_num = BeadCounter(LastDepth->bead);
            BeadPtr LastBead = LastDepth->bead;

            NewDepth->depth = i;

            for (int j = 0; j < Bead_num; j++)
            {
                int Node_num = NodeCounter(HeadList, LastBead->road_id);
                NodePtr NowNode = HeadList[LastBead->road_id - 1];
                for (int k = 0; k < Node_num; k++)
                {
                    BeadPtr NewBead = MakeBead(road_num);
                    NewBead->upper_bead = LastBead;
                    NewBead->road_id = NowNode->road_id;
                    NewBead->cost_sum = LastBead->cost_sum + NowNode->cost; // 부모 bead의 cost_sum에 현재 bead의 cost를 더해서 저장한다.
                    *(NewBead->checklist) = *(LastBead->checklist); // 부모 bead의 checklist를 복사한다.
                    NewBead->checklist[NewBead->road_id - 1] += 1; // 복사한 checklist에 현제 bead의 도로 id를 표시한다.

                    if(j == 0 && k == 0)
                        NewDepth->bead = NewBead; // 해당 Depth의 첫 번째 bead라서 Depth에 연결
                    else
                        BeadTail->link = NewBead; // 나머지 각 bead들은 맨마지막 bead에 연결
                    BeadTail = NewBead; // 맨마지막 bead 표시 최신화

                    NowNode = NowNode->link; // Linked_List 상의 다음 node로 이동
                }
                LastBead = LastBead->link;
            }
            LastDepth->lower_depth = NewDepth; // NewDepth 연결
            LastDepth = NewDepth; // LastDepth 이동

            CheckBeads(LastDepth, goal, &final_bead, &Min_cost_sum, road_num); // goal 찾았는지 확인, 희망없는 후보군 탈락

            if (LastDepth->bead != NULL)
            {
                // ShowDepth(LastDepth);
            }   
            else
            {
                if(final_bead != NULL) // goal까지 연결된 path를 찾은 경우 표시
                    did_find_path = true;
                is_bead_gone = true; // path candidate가 전부 사라졌기 때문에 for문 종료
            }
        }
        if (did_find_path == true)
            HeadMessage = LinkedMessage(HeadList, final_bead, ego, goal); // 만약 경로를 찾았다면 Message생성

        for (int i = 0; final_bead->upper_bead != NULL; i++)
        {
            std::cout << final_bead->road_id << "  ";
            final_bead = final_bead->upper_bead;
        }

        DeleteDepth(HeadDepth); // 생성했던 Depth, Bead들 전부 삭제

        return HeadMessage;
    }

    private:

    MessagePtr LinkedMessage(NodePtr * HeadList, BeadPtr final_bead, int ego, int goal)
    {
        MessagePtr TailMessage = MakeMessage();
        MessagePtr LastMessage = TailMessage;
        NodePtr NowNode = NULL;
        for (int i = 0; final_bead->upper_bead != NULL; i++)
        {   
            MessagePtr temp = NULL;
            if (i == 0)
            {
                temp = TailMessage;
                temp->arrival_id = goal;
                temp->departure_id = final_bead->road_id;
                NowNode = SearchLinkedList(HeadList, temp->departure_id , temp->arrival_id);
                *(temp->lane_cond) = *(NowNode->lanes);
                temp->passtype = NowNode->passtype;
            }
            else
            {
                
            }

            final_bead = final_bead->upper_bead; // 다음 bead로 이동
        }
        


        return Head_Message;
    }

    NodePtr SearchLinkedList(NodePtr * HeadList, int departure_id, int arrival_id)
    {
        NodePtr temp = HeadList[departure_id - 1];
        for (int i = 0; temp->road_id != arrival_id; i++)
        {
            temp = temp->link;
        }

        return temp
    }

    MessagePtr MakeMessage()
    {
        MessagePtr temp = new Message;
        temp->prev_link = NULL;
        temp->departure_id = 0;
        temp->arrival_id = 0;
        for (int i=0; i < MAX_LANE_NUM; i++)
            temp->lane_cond[i] = L_empty;
        temp->passtype = P_empty;
        temp->link = NULL;
        return temp;
    }

    void CheckBeads(Depth * LastDepth, int goal, BeadPtr * final_bead_ad, float * Min_cost_sum, int road_num)
    {
        int Bead_num = BeadCounter(LastDepth->bead);

        BeadPtr * prev = &(LastDepth->bead);
        BeadPtr temp_1 = LastDepth->bead;
        BeadPtr temp_2 = LastDepth->bead;

        int * checkbeadlist = new int[Bead_num];
        for (int i = 0; i < Bead_num; i++) // 초기화
            checkbeadlist[i] = 0;

        for (int i = 0; i < Bead_num; i++) 
        {
            for(int j = 0; j < road_num; j++) // 루프 확인
                if (temp_1->checklist[j] >= 2)
                    checkbeadlist[i] += 10; // 만일 루프면 제거를 위해 표시

            if (temp_1->road_id == goal && temp_1->cost_sum < *Min_cost_sum) //해당 depth에서 goal과 일치하는 bead를 찾는다.
            {
                *final_bead_ad = temp_1->upper_bead;
                *Min_cost_sum = temp_1->cost_sum;
            }
            temp_1 = temp_1->link;
        }

        for (int i = 0; i < Bead_num; i++) 
        {
            if (temp_2->cost_sum >= *Min_cost_sum) // 찾은 비드보다 cost_sum이 높은 bead 삭제
                DeleteBead(&temp_2, prev);
            else if(checkbeadlist[i] >= 10) // 갔던 길을 다시 가는 경우의 bead 삭제
                DeleteBead(&temp_2, prev);
            else
            {
                prev = &(temp_2->link); // prev 현제 bead로 이동
                temp_2 = temp_2->link; // 다음 bead로 이동
            }
        }
        delete []checkbeadlist;
    }

    void DeleteDepth(DepthPtr temp)
    {
        if(temp->lower_depth != NULL)
            DeleteDepth(temp->lower_depth);

        DeleteBeads(temp->bead);
        delete temp;
    }

    void DeleteBeads(BeadPtr temp)
    {
        if(temp != NULL)
            if(temp->link != NULL)
                DeleteBeads(temp->link);

        delete temp;
    }

    void DeleteBead(BeadPtr * temp_2_ad, BeadPtr * prev) // 
    {
        BeadPtr TEMP = NULL;

        delete [](*temp_2_ad)->checklist; // bead안의 checklist 동적할당 제거

        *prev = (*temp_2_ad)->link; // 이전 link에 다음 bead 이어줌, 만약 다음 bead가 없으면 자동으로 NULL값 들어감
        TEMP = *temp_2_ad;
        *temp_2_ad = (*temp_2_ad)->link; // 다음 bead로 이동

        delete TEMP; // bead 동적 할당 제거
    }

    BeadPtr MakeBead(int road_num)
    {
        BeadPtr temp = new Bead;
        temp->upper_bead = NULL;
        temp->road_id = 0;
        temp->cost_sum = 0;
        temp->checklist = new int[road_num];
        for(int i=0; i < road_num; i++)
            temp->checklist[i] = 0;
        temp->link = NULL;
        return temp;
    }

    DepthPtr MakeDepth()
    {
        DepthPtr temp = new Depth;
        temp->depth = 0;
        temp->lower_depth = NULL;
        temp->bead = NULL;
        return temp;
    }

    int BeadCounter(BeadPtr temp)
    {
        int cnt = 0;
        for(cnt = 0; temp->link != NULL; cnt++)
        {
            temp = temp->link;
        }
        return cnt + 1;
    }

    int NodeCounter(NodePtr * HeadList, int road_id)
    {
        int cnt;
        NodePtr temp = HeadList[road_id - 1];
        for(cnt = 0; temp->link != NULL; cnt++)
        {
            temp = temp->link;
        }
        return cnt + 1;
    }

    void ShowDepth(DepthPtr temp)
    {
        std::cout << "[Depth : " << temp->depth << "  ";
        ShowBead(temp->bead);
        std::cout << "]" << std::endl;
    }

    void ShowBead(BeadPtr temp)
    {
        std::cout << "[upper_bead : " << temp->upper_bead << "  road_id : " << temp->road_id;
        std::cout << "  cost_sum : " << temp->cost_sum << "  next_bead ad : " << temp->link << "]";
        if(temp->link != NULL)
            ShowBead(temp->link);
    }
};