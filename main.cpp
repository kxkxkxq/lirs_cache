#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include "data.hpp"
#include "cache.hpp"

//libfmt to use fmt::print

int main()
{
    data::Data<int, std::string> idata;
    idata.input_data();
    caches::lirs<int, std::string> lcache{idata.csize};

#if 1
    std::cerr << "      main() : \n";
    idata.printf_input_data();
#endif 

    unsigned nhits = 0;
    for(unsigned i = 0; i < idata.nrequests;  ++i)
        nhits += lcache.process_request(idata.requests[i]); 
    
    data::print_nhits(nhits);        
}