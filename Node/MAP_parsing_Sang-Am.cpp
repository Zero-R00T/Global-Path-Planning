#include <iostream>
#include <fstream>
#include "json/json.h"

int main()
{
    std::ifstream ist("MAP_Sang-Am.json");
    
    std::string str;
    for (char p; ist >> p;)
    {
        str += p;
    }
    std::cout << str << std::endl;

    Json::Reader reader;
    Json::Value root;
    bool parsingRet = reader.parse(str, root);

    if (!parsingRet)
    {
        std::cout << "Parsing Failed" << std::endl;
        return 1;
    }
    // std::cout << "Map name is : " << root["name"] << std::endl;
    Json::Value roads = root["roads"];

    std::cout << "Number of roads : " << (int)roads.size() << std::endl << std::endl;

    for (int index = 0; index < (int)roads.size();index++)
    {
        std::cout << "road id : " << roads[index]["road"].asString() << std::endl;
        std::cout << "road's lane : ";
        for (Json::ValueIterator it = roads[index]["lane"].begin(); it != roads[index]["lane"].end(); it++)
        {
            std::cout << (*it).asString() << " ";
        }
        std::cout << std::endl;
        if (roads[index]["lane"].size() != roads[index]["lane_id"].size())
        {
            std::cout << "R" << index + 1 << "'s nume of lanes & lane_ids are not matched" << std::endl;
            return 1;
        }

        Json::Value connection = roads[index]["connection"];
        std::cout << "Number of connected_roads : " << (int)connection.size() << std::endl;
        for (int index = 0; index < (int)connection.size(); index++)
        {
            std::cout << "connected_road id : " << connection[index]["road_id"].asString() << std::endl;
            // std::cout << "lane_cond : " << connection[index]["lane_cond"] << std::endl;
            // std::cout << "Cost : " << connection[index]["Cost"].asInt() << std::endl;
        }
        std::cout<<std::endl <<std::endl;
    }


    std::cout << "Finshed" << std::endl;

    return 0;
}