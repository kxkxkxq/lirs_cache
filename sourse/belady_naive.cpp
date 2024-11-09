#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "../include/data.hpp"
#include "../include/belady.hpp"

int main()
{
    data::Data<int, std::string> idata;
    
    if(!idata.input_data())
        return 1;
    
    caches::belady<int, std::string> bcache(idata.csize, idata.requests);

    unsigned nhits = 0;
    for(auto i = idata.requests.begin(), e = idata.requests.end(); i != e; ++i)
        nhits += bcache.process_request(*i);

    std::cout << nhits << '\n';
}