#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include "../include/lirs.hpp"
#include "../include/belady.hpp"

//  LIRS
TEST(LIRSTests, Test1)
{
    caches::lirs<int> tcache{0};
    unsigned nhits = 0;
    nhits += tcache.process_request(0);

    EXPECT_EQ(nhits, 0);
}

TEST(LIRSTests, Test2)
{
    enum Cache
    { 
        cache_size = 10
    };

    enum Requests
    {
        req            = 1,
        expected_nhits = 9,
        nrequests      = 10
    };

    caches::lirs<int> tcache{cache_size};

    unsigned nhits = 0;
    for(size_t i = 0; i < nrequests; ++i)
        nhits += tcache.process_request(req);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test3)
{
    enum Cache
    {
        hirs_size  = 1, 
        lirs_size  = 2, 
        cache_size = 3
    };

    enum Requests
    {
        expected_nhits = 0,
        nrequests     = 10
    };

    caches::lirs<int> tcache{cache_size};
    ASSERT_EQ(tcache.lhirs_cap(), hirs_size);
    ASSERT_EQ(tcache.llirs_cap(), lirs_size);

    unsigned nhits = 0;
    for(int i = 0; i < nrequests; ++i)
        nhits += tcache.process_request(i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test4)
{
    enum Cache
    {
        cache_size = 4
    };

    enum Requests
    {
        expected_nhits = 3,
        nrequests = 10
    };

    caches::lirs<int> tcache{cache_size};
    std::vector<int> r = {1, 2, 3, 4, 5, 4, 6, 4, 6, 6};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);

}

TEST(LIRSTests, Test5)
{
    enum Cache
    {
        cache_size = 10
    };

    enum Requests
    {
        expected_nhits = 6,
        nrequests = 10
    };

    caches::lirs<int> tcache{cache_size};
    std::vector<int> r = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);

}

TEST(LIRSTests, Test6)
{
    enum Cache
    {
        cache_size = 10
    };   

    enum Requests
    {
        expected_nhits = 6,
        nrequests      = 10
    };

    caches::lirs<int> tcache{cache_size};
    std::vector<int> r = {1, 2, 3, 4, 3, 3, 3, 3, 3, 3};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test7)
{
    enum Cache
    {
        cache_size = 2
    };

    enum Requests
    {
        expected_nhits = 8,
        nrequests      = 10
    };

    caches::lirs<int> tcache{cache_size};
    std::vector<int> r = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}
TEST(LIRSTests, Test8)
{   
    enum Cache
    {
        cache_size = 4
    };

    enum Requests
    {
        expected_nhits = 6,
        nrequests      = 12
    };

    caches::lirs<int> tcache{cache_size};
    std::vector<int> r = {1, 2, 3, 4, 5, 2, 1, 1, 2, 4, 3, 4};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test9)
{
    enum Cache
    {
        cache_size = 2
    };

    enum Requests
    {
        expected_nhits = 99999,
        nrequests      = 100000
    };

    caches::lirs<int> tcache{cache_size};

    unsigned nhits = 0;
    for(size_t i = 0; i < nrequests; ++i)
        nhits += tcache.process_request(1);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test10)
{
    enum Cache
    {
        cache_size = 10,
    };

    enum Requests
    {
        iterations     = 100,
        nrequests      = 100000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        caches::lirs<int> tcache{cache_size};
        std::vector<int> r;
        r.reserve(nrequests);

        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            r.emplace_back(k);
        }

        unsigned niterations = 0;
        for(auto i = r.begin(), e = r.end(); i != e; ++i)
        {
            tcache.process_request(*i);
            ++niterations;
        }

        EXPECT_EQ(niterations, nrequests);
    }    
}

TEST(LIRSTests, Test11)
{
    enum Cache
    {
        cache_size = 250000,
    };

    enum Requests
    {
        iterations     = 100,
        nrequests      = 1000000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        caches::lirs<int> tcache{cache_size};
        std::vector<int> r;
        r.reserve(nrequests);

        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            r.emplace_back(k);
        }

        unsigned niterations = 0;
        for(auto i = r.begin(), e = r.end(); i != e; ++i)
        {
            tcache.process_request(*i);
            ++niterations;
        }

        EXPECT_EQ(niterations, nrequests); 
    }
}

//  BELADY

TEST(BELADYTests, Test1)
{
    std::list<int> l = {0};
    caches::belady<int> tcache{0, l};
    unsigned nhits = 0;
    nhits += tcache.process_request(0);

    EXPECT_EQ(nhits, 0);
}

TEST(BELADYTests, Test2)
{
    enum Cache
    { 
        cache_size = 10
    };

    enum Requests
    {
        expected_nhits = 9
    };

    std::list<int> l = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test3)
{
    enum Cache
    {
        cache_size = 3
    };

    enum Requests
    {
        expected_nhits = 0
    };

    std::list<int> l = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test4)
{
    enum Cache
    {
        cache_size = 4
    };

    enum Requests
    {
        expected_nhits = 6
    };

    std::list<int> l = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2};
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test5)
{
    enum Cache
    {
        cache_size = 2
    };

    enum Requests
    {
        expected_nhits = 8
    };

    std::list l = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3};
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test6)
{   
    enum Cache
    {
        cache_size = 4
    };

    enum Requests
    {
        expected_nhits = 7
    };

    std::list l = {1, 2, 3, 4, 5, 2, 1, 1, 2, 4, 3, 4};
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test7)
{
    enum Cache
    {
        cache_size = 2,
        req        = 1
    };

    enum Requests
    {
        expected_nhits = 99999,
        nrequests      = 100000
    };

    std::list<int> l(nrequests, req);
    caches::belady<int> tcache{cache_size, l};

    unsigned nhits = 0;
    for(auto i = l.begin(), e = l.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(BELADYTests, Test8)
{
    enum Cache
    {
        cache_size = 10,
    };

    enum Requests
    {
        iterations     = 50,
        nrequests      = 100000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        std::list<int> l;
        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            l.emplace_back(k);
        }

        caches::belady<int, std::string> tcache{cache_size, l};

        unsigned niterations = 0;
        for(auto i = l.begin(), e = l.end(); i != e; ++i)
        {
            tcache.process_request(*i);
            ++niterations;
        }

        EXPECT_EQ(niterations, nrequests);
    }    
}

TEST(BELADYTests, Test9)
{
    enum Cache
    {
        cache_size = 250000,
    };

    enum Requests
    {
        iterations     = 100,
        nrequests      = 1000000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        std::list<int> l;
        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            l.emplace_back(k);
        }

        caches::belady<int, std::string> tcache{cache_size, l};

        unsigned niterations = 0;
        for(auto i = l.begin(), e = l.end(); i != e; ++i)
        {
            tcache.process_request(*i);
            ++niterations;
        }

        EXPECT_EQ(niterations, nrequests);
    }   
}