#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include "../../include/data.hpp"
#include "../../include/cache.hpp"
#include "../../include/belady.hpp"

//  DATA FOR COMPARISON OF BELADY AND LIRS CACHE ALGORITHMS

void save_result(const unsigned bh, const unsigned lh, const unsigned nr);

TEST(EFFICIENCE_COMPARISON, CountHits)
{
    enum Cache
    {
        cache_size = 1000,
    };

    enum Requests
    {
        iterations     = 10,
        nrequests      = 100000
    };

    std::cout << "Starting counting the number of hits for Belady and LIRS\n";

    for(unsigned it = 0; it < iterations; ++it)
    {
        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        std::vector<data::data_t<int, std::string>> v;
        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            v.emplace_back(data::data_t<int, std::string>{k});
        }

        caches::belady<int, std::string> belady_cache{cache_size, v};
        caches::lirs<int, std::string> lirs_cache{cache_size};

        unsigned belady_hits = 0, lirs_hits = 0;
        for(auto i =v.begin(), e = v.end(); i != e; ++i)
        {
            belady_hits += belady_cache.process_request(*i);
            lirs_hits += lirs_cache.process_request(*i);
        }

        save_result(belady_hits, lirs_hits, nrequests);
        std::cout << "Test" << it << "\n";
    }

    std::cout << "Finished counting\n";   
}

void save_result(const unsigned bh, const unsigned lh, const unsigned nr)
{
    std::ofstream file("results.txt", std::ios::app);

    if(!file.is_open())
        std::cerr << "failed to open the file\n";
    
    file << bh << " " << lh << " " << nr << "\n";
    file.close();
}