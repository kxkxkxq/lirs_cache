#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "../include/lirs.hpp"

int main()
{
    long long size = 0, nrequests = 0;
    std::cin >> size >> nrequests;
    
    if(std::cin.fail() || (size < 2) || (nrequests < 0))
    {
        std::cerr << "Input error\n";
        if(size < 2)
            std::cerr << "Minimum allowed size for LIRS cache : 2\n";
        
        return 1;
    }

    caches::lirs<int> lcache{size};
                            //  add check that csize != 1
    unsigned nhits = 0;
    for(unsigned i = 0; i < nrequests; ++i)
    {
        int k = 0;
        std::cin >> k;
        if(std::cin.fail())
        {
            std::cerr << "Input error\n";
            return 1;
        }

        nhits += lcache.process_request(k);
    }

    std::cout << nhits << '\n';        
}