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
    idata.printf_input_data();
    std::cout << "\ncsize == " << icache.csize << "\n";
#endif 
    unsigned nhits = 0;
#if 1

    size_t j = 0;
    for(auto i = idata.requests.begin(); i != idata.requests.begin(); ++i, ++j)
        nhits += icache.process_request(idata.requests[j]); 

    data::print_nhits(nhits); 
#endif
    return 0;       
}