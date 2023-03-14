#include <cstdio>
#include "stl_vector.h"

int
main( int,
      const char**)
{

    stl::Vector<int> arr{}; //( 10);// = std::vector<int>( 10, 0);

    arr.push_back( 123);
    arr.push_back( 13);
    arr.push_back( 12);
    arr.push_back( 1);
    arr.push_back( 43);
    arr.push_back( 3);
    arr.push_back( 42);
    arr.push_back( 11);
    /*
    for ( stl::Vector<int>::Iterator it = arr.begin(); it != arr.end(); ++it )
    {
        std::printf( "iterator");
        std::printf( "elem = %d \n", *it);
    }
    */

    std::sort( arr.begin(), arr.end());
    //int x = *std::find_if(arr.begin(), arr.end(), [](int i){ return i == 1; });
    //std::printf( "found = %d \n", x);

    auto as = arr.begin() + 3;

    for ( auto&& elem : arr )
    {
        std::printf( "elem = %d \n", elem);
    }
    /*
    arr.push_back( 123);
    arr.push_back( 123);
    arr.push_back( 123);
    arr.push_back( 1);
    arr.push_back( 2);
    arr.push_back( 1);
    */

    /*
    for ( stl::Vector<int>::Iterator it = arr.begin(); it != arr.end(); ++it )
    {
        std::printf( "iterator");
        std::printf( "elem = %d \n", *it);
    }
    */

    /*
    for ( auto&& elem : arr )
    {
        std::printf( "elem = %d \n", elem);
    }
    */

    //std::sort( arr.begin(), arr.end());

    std::vector<int> arf(10);// = std::vector<int>( 10, 0);
    arf.push_back(123);
    arf.push_back(123);
    arf.push_back(123);
    arf.push_back(123);
    arf.push_back(1);
    arf.push_back(2);
    arf.push_back(1);
    //arf[0] = 123;
    //arf[1] = 1;
    //arf[2] = 13;
    //arf[3] = 13;
    //arf[4] = 13;
    //arf[5] = 13;

    std::sort( arf.begin(), arf.end());
    for ( auto&& elem : arf )
    {
        std::printf( "std = %d \n", elem);
    }

    /*
    for ( auto&& elem : arr )
    {
        std::printf( "elem = %d \n", elem);
    }
    */

    //arr.resize( 0x100000000U);

    //int x = *std::find_if(arr.begin(), arr.end(), [](int i){ return i == 1; });
    //std::printf( "found = %d \n", x);

    return 0;
}
