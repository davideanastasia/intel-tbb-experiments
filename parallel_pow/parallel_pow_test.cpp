#include <vector>
#include <list>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <boost/bind.hpp>
#include <tbb/tbb.h>

#include <parallel_pow/parallel_pow.hpp>

using namespace std;
using namespace tbb;

typedef std::vector<float> Container;

const Container::size_type NUM_ELEMS = 10000000;
const Container::value_type EXPONENT = 3.14f;

namespace test
{
template <typename ExponentType>
struct pow
{
    explicit pow(ExponentType exp)
        : exp_(exp)
    {}

    template <typename InputType>
    InputType operator()(InputType input) const
    {
        return std::pow( input, exp_ );
    }

private:
    ExponentType exp_;
};

typedef ::test::pow<Container::value_type> pow_;
}

int main()
{
    task_scheduler_init init;

    Container input(NUM_ELEMS);
    Container output_serial(NUM_ELEMS);
    Container output_parallel(NUM_ELEMS);

    generate(input.begin(), input.end(), &rand);

    tick_count t0_s = tick_count::now( );

    transform(input.begin(), input.end(),
             output_serial.begin(),
             test::pow_(EXPONENT) );

    tick_count t1_s = tick_count::now( );
    double t_s = (t1_s - t0_s).seconds( );
    cout << "Serial: " << t_s << endl;

    tick_count t0_p = tick_count::now( );

    tbb_experiments::parallel_pow( input.data(),
                                   output_parallel.data(),
                                   EXPONENT,
                                   input.size() );

    tick_count t1_p = tick_count::now( );
    double t_p = (t1_p - t0_p).seconds( );
    cout << "Parallel: " << t_p << endl;


    for (Container::size_type idx = 0 ; idx < NUM_ELEMS; ++idx)
    {
        if ( output_serial[idx] != output_parallel[idx] )
        {
            std::cerr << "Failed on index "
                      << idx
                      << "output_serial[idx] != output_parallel[idx] : "
                      << output_serial[idx] << " != " << output_parallel[idx]
                      << std::endl;
            return -1;
        }
    }
    std::cout << "Speed up = " << t_s/t_p << std::endl;

    return 0;
}
