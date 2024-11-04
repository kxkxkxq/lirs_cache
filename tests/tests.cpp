#include <gtest/gtest.h>
#include <random>

#include "../include/data.hpp"
#include "../include/cache.hpp"

TEST(LIRSTests, Test1)
{
    enum Cache
    {
        cache_size = 0
    };

    enum Requests
    {
        expected_nhits = 0,
        nrequests      = 0,
        req            = 0
    };

    caches::lirs<int, std::string> tcache{cache_size};
    unsigned nhits = 0;
    nhits += tcache.process_request(req);

    EXPECT_EQ(nhits, expected_nhits);
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

    caches::lirs<int, std::string> tcache{cache_size};

    unsigned nhits = 0;
    for(size_t i = 0; i < nrequests; ++i)
    {
        data::data_t<int, std::string> r{req};
        nhits += tcache.process_request(r);
    }

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

    caches::lirs<int, std::string> tcache{cache_size};
    ASSERT_EQ(tcache.lhirs_cap(), hirs_size);
    ASSERT_EQ(tcache.llirs_cap(), lirs_size);

    unsigned nhits = 0;
    for(int i = 0; i < nrequests; ++i)
    {
        data::data_t<int, std::string> r{i};
        nhits += tcache.process_request(r);
    }

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

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r = {1, 2, 3, 4, 5, 4, 6, 4, 6, 6};

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

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2};

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

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r = {1, 2, 3, 4, 3, 3, 3, 3, 3, 3};

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

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3};

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

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r = {1, 2, 3, 4, 5, 2, 1, 1, 2, 4, 3, 4};

    unsigned nhits = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
        nhits += tcache.process_request(*i);

    EXPECT_EQ(nhits, expected_nhits);
}

TEST(LIRSTests, Test9)
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

    caches::lirs<int, std::string> tcache{cache_size};

    unsigned nhits = 0;
    for(size_t i = 0; i < nrequests; ++i)
    {
        data::data_t<int, std::string> r{req};
        nhits += tcache.process_request(r);
    }

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
        nrequests      = 100000
    };

    caches::lirs<int, std::string> tcache{cache_size};
    std::vector<data::data_t<int, std::string>> r;
    r.reserve(nrequests);

    std::random_device rd;
    std::mt19937 gen(rd());    
    std::uniform_int_distribution<> req_dist{1, 10000};

    for(unsigned i = 0; i < nrequests; ++i)
    {
        int k = req_dist(gen);
        r.emplace_back(data::data_t<int, std::string>{k});
    }

    unsigned niterations = 0;
    for(auto i = r.begin(), e = r.end(); i != e; ++i)
    {
        tcache.process_request(*i);
        ++niterations;
    }

    EXPECT_EQ(niterations, nrequests);
}

TEST(LIRSTests, Test11)
{
    enum Cache
    {
        cache_size = 10,
    };

    enum Requests
    {
        iterations     = 100,
        nrequests      = 10000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        caches::lirs<int, std::string> tcache{cache_size};
        std::vector<data::data_t<int, std::string>> r;
        r.reserve(nrequests);

        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            r.emplace_back(data::data_t<int, std::string>{k});
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

TEST(LIRSTests, Test12)
{
    enum Cache
    {
        cache_size = 1000,
    };

    enum Requests
    {
        iterations     = 100,
        nrequests      = 100000
    };

    for(unsigned it = 0; it < iterations; ++it)
    {
        caches::lirs<int, std::string> tcache{cache_size};
        std::vector<data::data_t<int, std::string>> r;
        r.reserve(nrequests);

        std::random_device rd;
        std::mt19937 gen(rd());    
        std::uniform_int_distribution<> req_dist{1, 10000};

        for(unsigned i = 0; i < nrequests; ++i)
        {
            int k = req_dist(gen);
            r.emplace_back(data::data_t<int, std::string>{k});
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