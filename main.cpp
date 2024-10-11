#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "data.hpp"
#include "cache.hpp"

int main()
{
    data::Data<int, std::string> idata;
    idata.input_data();
    caches::lirs<int, std::string> lcache{idata.csize};

    unsigned nhits = 0;
    for(unsigned i = 0; i < idata.nrequests;  ++i)
        nhits += lcache.process_request(idata.requests[i]); 
    
    data::print_nhits(nhits);        
}