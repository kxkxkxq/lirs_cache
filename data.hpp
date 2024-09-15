#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <vector>
#include <map>
#include <queue>

namespace Data_
{
    template <typename T> class Data;

    inline void print_nhits(const unsigned n) {std::cout << n; return;};
};

template <typename T> class Data_::Data
{
    using tvec = typename std::vector<T>;
    unsigned csize = 0, nrequests = 0;
    tvec requests; 

    using titerator = typename std::vector<T>::iterator;
    titerator data_begin() const {return requests.begin(); };
    titerator data_end() const {return requests.end(); };

public:
    //Data();
    //~Data();

    void input_data();
    void fill_requests_with_data();
    void printf_input_data();
};

template <typename T> void Data_::Data<T>::input_data()
{
    std::cin >> csize >> nrequests;

    requests.reserve(nrequests);
    fill_requests_with_data();

    return;
};

template <typename T> 
    void Data_::Data<T>::fill_requests_with_data()
{
    for(unsigned i = 0; i < nrequests; ++i)
        std::cin >> requests[i];

    return;
};

template <typename T> void Data_::Data<T>::printf_input_data()
{
    std::cout << "csize == " << csize << "\nnrequests == " << nrequests << '\n';
    
    for(unsigned i = 0; i < nrequests; ++i)
        std::cout << requests[i] << '\n';
};

#endif