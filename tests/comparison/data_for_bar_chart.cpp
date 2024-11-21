#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include "../../include/lirs.hpp"
#include "../../include/belady.hpp"

//  DATA FOR COMPARISON OF BELADY AND LIRS CACHE ALGORITHMS

void save_result(const unsigned bh, const unsigned lh, const unsigned nr, const unsigned cs);

TEST(EFFICIENCE_COMPARISON, CountHits)
{
    enum Cache
    {
        cache_size = 500,
    };

    enum Requests
    {
        iterations     = 10,
        nrequests      = 400000
    };

    std::cout << "Starting counting the number of hits for Belady and LIRS\n";

    for(unsigned it = 0; it < iterations; ++it)
    {
        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 1000};

        std::list<int> l;
        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            l.emplace_back(k);
        }

        caches::belady<int, std::string> belady_cache{cache_size, l};
        caches::lirs<int, std::string> lirs_cache{cache_size};

        unsigned belady_hits = 0, lirs_hits = 0;
        for(auto i = l.begin(), e = l.end(); i != e; ++i)
        {
            belady_hits += belady_cache.process_request(*i);
            lirs_hits += lirs_cache.process_request(*i);
        }

        save_result(belady_hits, lirs_hits, nrequests, cache_size);
        std::cout << "Test" << (it + 1) << " : ";
        std::cout << "lirs hits : " << lirs_hits << ", belady hits : " << belady_hits << "\n";
    }

    std::cout << "Finished counting\n";   
}

void save_result(const unsigned bh, const unsigned lh, const unsigned nr, const unsigned cs)
{
    std::ofstream file("results.txt", std::ios::app);

    if(!file.is_open())
        std::cerr << "failed to open the file\n";
    
    file << bh << " " << lh << " " << nr << " " << cs << "\n";
    file.close();
}