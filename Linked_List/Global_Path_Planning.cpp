#include <iostream>
#include "Linked_List.h"
#include "Path_Finder.h"

int main()
{
    NodePtr * Map;
    MessagePtr Path;
    Linked_List list;
    Path_Finder finder;
    int ego(1), goal(1);

    Map = list.MakeList();
    list.CostFunction(Map);
    // list.ShowList(Map);
    
    while(1)
    {
        std::cout << "ego, goal : ";
        std::cin >> ego >> goal ;;
        if (ego != -1 && goal != -1)
            Path = finder.FindPath(Map, ego, goal);
        else
            break;
        finder.ShowPath(Path);
        finder.DeletePath(Path);
    }
    list.DeleteList(Map);
    
    std::cout << "Program is ended" << std::endl;

    return 0;
}