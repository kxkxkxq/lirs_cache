#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include "data.hpp"
#include "cache.hpp"

//libfmt to use fmt::print

int main()
{
    data::Data<int> idata;
    idata.input_data();
    lirscache::Cache<int> icache{idata.csize};

#if 1
    std::cerr << "      main() : \n";
    idata.printf_input_data();
    std::cout << "csize == " << icache.csize << "\n";
#endif 

    unsigned nhits = 0;
    for(unsigned j = 0; j < idata.nrequests; ++j)
        nhits += icache.process_request(idata.requests[j]); 
    
    data::print_nhits(nhits);        
}