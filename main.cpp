#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include "data.hpp"
//libfmt to use fmt::print

int main()
{
    Data_::Data<int> idata;
    idata.input_data();

    idata.printf_input_data();

    return 0;
#if 0
    typedef LIRSCache::Cache Cache;
    Cache = Cache();

    unsigned nhits = 0;

    typedef std::vector vector;
    for(vector<T>::iterator i = data_begin(Data); i != data_end(Data); ++i)
    {
        nhits += Cache::process_request(Data);
    }   

    Data_::print_nhits(); 
#endif      
}