#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "../include/data.hpp"
#include "../include/cache.hpp"

int main()
{
    data::Data<int, std::string> idata;
    
    if(!idata.input_data())
        return 1;
    
    caches::lirs<int, std::string> lcache{idata.csize};

    unsigned nhits = 0;
    for(auto i = idata.requests.begin(); i != idata.requests.end(); ++i)
        nhits += lcache.process_request(*i); 
         
    std::cout << nhits << '\n';        
}