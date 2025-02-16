#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "belady.hpp"

int main()
{
    long long size = 0, nRequests = 0;
    std::cin >> size >> nRequests;
    
    if(std::cin.fail() || (size < 0) || (nRequests < 0))
    {
        std::cerr << "Input error\n";
        return 1;
    }
    
    std::list<int> requests;
    for(unsigned i = 0; i < nRequests; ++i)
    {
        int key = 0;
        std::cin >> key;
        if(std::cin.fail())
        {
            std::cerr << "Input error\n";
            return 1;
        }
        requests.push_back(key);
    }

    caches::belady<int> bCache(size, requests.begin(), requests.end());
    unsigned nHits = 0;
    for(auto i : requests)
        nHits += bCache.process_request(i);

    std::cout << nHits << '\n';
}