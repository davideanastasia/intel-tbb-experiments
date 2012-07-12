#ifndef TBB_EXPERIMENTS_VECTOR_POW_H
#define TBB_EXPERIMENTS_VECTOR_POW_H

#include <cmath>
#include <algorithm>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>

namespace tbb_experiments
{

namespace detail
{
template <typename OutputType, typename ExponentType>
struct pow
{
    explicit pow(ExponentType exp)
        : exp_(exp)
    {}

    template <typename InputType>
    inline
    OutputType operator()(InputType input) const
    {
        return static_cast<OutputType>( std::pow( input, exp_ ) );
    }

private:
    ExponentType exp_;
};
}

//! \brief Calculates pow of the input vector and stores it into the output
//! vector
template <typename InputType,
          typename OutputType,
          typename ExponentType>
class parallel_pow_t
{
public:
    parallel_pow_t(const InputType* input_v,
                   OutputType* output_v,
                   ExponentType exponent)
        : input_vector_(input_v)
        , output_vector_(output_v)
        , exponent_(exponent)
    {}

    void operator( )(const tbb::blocked_range< size_t >& r) const
    {
        using namespace std;

        typedef detail::pow<OutputType, ExponentType> pow_;

        transform( input_vector_ + r.begin(),
                   input_vector_ + r.end(),
                   output_vector_ + r.begin(),
                   pow_(exponent_) );
    }

private:
    const InputType* input_vector_;
    OutputType* output_vector_;
    ExponentType exponent_;
};


template <typename InputType,
          typename OutputType,
          typename ExponentType>
inline
void parallel_pow(const InputType* input_v,
                  OutputType* output_v,
                  ExponentType exponent,
                  size_t num_elems)
{
    using namespace tbb;

    typedef tbb_experiments::parallel_pow_t<InputType,
            OutputType,
            ExponentType> ParallelPow;

    parallel_for( blocked_range<size_t>( 0, num_elems ),
                  ParallelPow( input_v, output_v, exponent ),
                  auto_partitioner() );
}

}

#endif
