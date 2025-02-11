#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

#include "lirs.hpp"
#include "belady.hpp"

namespace cachetests
{
//      structs for tests :    
    struct CacheHitParam 
    {
        size_t cacheSize;    
        const std::list<int> testData;  
        unsigned result;
    };

    struct CacheBigDataParam
    {
        size_t cacheSize;
        size_t testDataSize;
        size_t numberOfIterations;  //  number of repeated tests for given data 
    };

//     HIT TEST DATA
    static std::list<int>  l1(10, 1);
    static std::list<int>  l2(1000000, 1);
    static std::list<int>  l3{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    static std::list<int>  l4{1, 2, 3, 4, 5, 4, 6, 4, 6, 6};
    static std::list<int>  l5{1, 2, 3, 4, 1, 2, 3, 4, 1, 2};
    static std::list<int>  l6{1, 2, 3, 4, 3, 3, 3, 3, 3, 3};
    static std::list<int>  l7{1, 3, 1, 3, 1, 3, 1, 3, 1, 3};
    static std::list<int>  l8{1, 2, 3, 4, 5, 2, 1, 1, 2, 4, 3, 4};

    std::vector<CacheHitParam> lirsHitTestData = 
    {
        {10, l1, 9},
        { 2, l2, 999999},
        { 3, l3, 0},
        { 4, l4, 3},
        {10, l5, 6},
        {10, l6, 6},
        { 2, l7, 8},
        { 4, l8, 6}
    };

    std::vector<CacheHitParam> beladyHitTestData = 
    {
        {10, l1, 9},
        { 2, l2, 999999},
        { 3, l3, 0},
        { 4, l4, 4},
        {10, l5, 6},
        {10, l6, 6},
        { 2, l7, 8},
        { 4, l8, 7}   
    };

//      TESTS WITH BIG PSEUDORANDOM DATA 
    std::vector<CacheBigDataParam> bigTestData = 
    {
        {50, 1000, 10},
        {1000, 10000, 10},
        {5000, 100000, 10},
        {25000, 1000000, 10},
        {1000, 1000000, 10}, 
    };

    std::vector<CacheBigDataParam> beladyBigTestData = 
    {
        {50, 1000, 10},
        {10, 100000, 50},
        {5000, 100000, 10},
        {250000, 1000000, 100}, 
        {25000, 1000000, 10}
    };

//      func to fill list with pseudorandom numbers
    template <typename KeyT> void   
    fill_list_with_data(std::list<KeyT>& l, size_t size, const KeyT lb = 1, const KeyT ub = 10000)
    {
        std::random_device r;
        std::mt19937 gen(r());    
        std::uniform_int_distribution<> req_dist{lb, ub};

        for (size_t i = 0; i < size; ++i)
            l.push_back(req_dist(gen));
    };
}   //  namespace


//      CACHE HIT TESTS
class LIRSHitTests : public ::testing::TestWithParam<cachetests::CacheHitParam> {};
class BeladyHitTests : public ::testing::TestWithParam<cachetests::CacheHitParam> {};

//  LIRS
TEST_P(LIRSHitTests, HitTest)
{
    cachetests::CacheHitParam testParams = GetParam();
    caches::lirs<int> cache{testParams.cacheSize};

    unsigned nh = 0;
    for(auto i : testParams.testData)
        nh += cache.process_request(i);
    EXPECT_EQ(nh, testParams.result);
}

//  Belady
TEST_P(BeladyHitTests, HitTest)
{
    cachetests::CacheHitParam testParams = GetParam();
    caches::belady<int> cache{testParams.cacheSize, testParams.testData};

    unsigned nh = 0;
    for(auto i : testParams.testData)
        nh += cache.process_request(i);
    EXPECT_EQ(nh, testParams.result);
}

INSTANTIATE_TEST_SUITE_P( LIRS,
                          LIRSHitTests,
                          ::testing::ValuesIn(cachetests::lirsHitTestData) );

INSTANTIATE_TEST_SUITE_P( Belady,
                          BeladyHitTests,
                          ::testing::ValuesIn(cachetests::beladyHitTestData) );


//      BIG DATA TESTS
class LIRSBigDataTests : public ::testing::TestWithParam<cachetests::CacheBigDataParam> {};
class BeladyBigDataTests : public ::testing::TestWithParam<cachetests::CacheBigDataParam> {};

//  LIRS
TEST_P(LIRSBigDataTests, BigDataTest)
{
    cachetests::CacheBigDataParam testParams = GetParam();
    
    for(size_t i = 0; i < testParams.numberOfIterations; ++i)
    {
        std::unique_ptr<std::list<int>> dataPtr(new std::list<int>);
        cachetests::fill_list_with_data(*dataPtr, testParams.testDataSize);
        
        caches::lirs<int> cache{testParams.cacheSize};
        unsigned ni = 0;
        
        for(auto j : *dataPtr)
        {
            cache.process_request(j);
            ++ni;
        }
        EXPECT_EQ(ni, testParams.testDataSize);
    }
}

//  Belady
TEST_P(BeladyBigDataTests, BigDataTest)
{
    cachetests::CacheBigDataParam testParams = GetParam();
    
    for(size_t i = 0; i < testParams.numberOfIterations; ++i)
    {
        std::unique_ptr<std::list<int>> dataPtr(new std::list<int>);
        cachetests::fill_list_with_data(*dataPtr, testParams.testDataSize);
        
        caches::belady<int> cache{testParams.cacheSize, *dataPtr};
        unsigned ni = 0;
        
        for(auto j : *dataPtr)
        {
            cache.process_request(j);
            ++ni;
        }
        EXPECT_EQ(ni, testParams.testDataSize);
    }
}

INSTANTIATE_TEST_SUITE_P( LIRS,
                          LIRSBigDataTests,
                          ::testing::ValuesIn(cachetests::bigTestData) );

INSTANTIATE_TEST_SUITE_P( Belady,
                          BeladyBigDataTests,
                          ::testing::ValuesIn(cachetests::beladyBigTestData) );