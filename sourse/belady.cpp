#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "../include/belady.hpp"

int main()
{
    long long size = 0, nrequests = 0;
    std::cin >> size >> nrequests;
    
    if(std::cin.fail() || (size < 0) || (nrequests < 0))
    {
        std::cerr << "Input error\n";
        return 1;
    }
    
    std::list<int> requests;
    for(unsigned i = 0; i < nrequests; ++i)
    {
        int k = 0;
        std::cin >> k;
        if(std::cin.fail())
        {
            std::cerr << "Input error\n";
            return 1;
        }
        requests.push_back(k);
    }

    caches::belady<int> bcache(size, requests);
    unsigned nhits = 0;
    int s = 0;
    for(auto i = requests.begin(), e = requests.end(); i != e; ++i, ++s)
        nhits += bcache.process_request(*i);

    std::cout << nhits << '\n';
}