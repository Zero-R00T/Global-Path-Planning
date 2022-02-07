#include <iostream>
#include "Linked_List.h"

int main()
{
    NodePtr * MAP;
    Linked_List list;

    MAP = list.MakeList();
    list.ShowList(MAP);
    list.CostFunction(MAP);
    list.ShowList(MAP);
    list.DeleteList(MAP);
    
    return 0;
}