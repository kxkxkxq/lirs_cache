#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <vector>
#include <map>
#include <queue>

namespace data
{
    template <typename T> struct Data
    {
        using tvec = typename std::vector<T>;
        tvec requests; 

        using titerator = typename std::vector<T>::iterator;
        //titerator begin() const {return requests.begin(); };
        //titerator end() const {return requests.end(); };

    public:
        unsigned csize = 0, nrequests = 0;//remove to private
        //Data();
        //~Data();

        void input_data();
        void fill_requests_with_data();
        void printf_input_data();
    };

    inline void print_nhits(const unsigned n) {std::cout << n; };
}

template <typename T> void data::Data<T>::input_data()
{
    std::cin >> csize >> nrequests;
    requests.reserve(nrequests);

    for(unsigned i = 0; i < nrequests; ++i)
    {
        unsigned r;
        std::cin >> r;
        requests.push_back(r);
    }
}

template <typename T> void data::Data<T>::printf_input_data()
{
    std::cout << "csize == " << csize << "\nnrequests == " << nrequests << '\n';
    
    for(unsigned i = 0; i < nrequests; ++i)
        std::cout << requests.at(i) << '\n';
}

#endif