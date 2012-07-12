#ifndef TBB_EXPERIMENTS_VECTOR_MIN_MAX_H
#define TBB_EXPERIMENTS_VECTOR_MIN_MAX_H

#include <cmath>
#include <algorithm>
#include <limits>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>

namespace tbb_experiments
{

template <typename InputType>
class parallel_min_max_t
{
public:
    //! \brief constructor
    parallel_min_max_t(const InputType* input_vector)
        : input_vector_(input_vector)
        , min_( std::numeric_limits< InputType >::max() )
        , max_( -std::numeric_limits< InputType >::min() )
    {}

    //! \brief split copy constructor
    parallel_min_max_t(const parallel_min_max_t& p, tbb::split)
        : input_vector_(p.input_vector_)
//        , min_(p.min_)
//        , max_(p.max_)
        , min_( std::numeric_limits< InputType >::max() )
        , max_( -std::numeric_limits< InputType >::min() )
    {}

    void operator()(const tbb::blocked_range< size_t >& r)
    {
        const InputType* curr_value = input_vector_ + r.begin();
        const InputType* end_value = input_vector_ + r.end();

        if ( r.size() % 2 ) // odd
        {
            InputType sample = *curr_value;

            if (sample < min_) min_ = sample;
            if (sample < max_) max_ = sample;

            curr_value++;
        }

        while ( curr_value != end_value )
        {
            InputType sample1 = *curr_value;
            InputType sample2 = *(curr_value + 1);

            if (sample1 < sample2)
            {
                if (sample1 < min_) min_ = sample1;
                if (sample2 > max_) max_ = sample2;
            } else {
                if (sample2 < min_) min_ = sample2;
                if (sample1 > max_) max_ = sample1;
            }

            curr_value += 2;
        }

//        while ( curr_value != end_value )
//        {
//            InputType sample1 = *curr_value;

//            if (sample1 < min_) {
//                min_ = sample1;
//            } else {
//                max_ = sample1;
//            }

//            curr_value ++;
//        }
    }

    void join(const parallel_min_max_t& hp)
    {
        if (min_ > hp.min_) min_ = hp.min_;
        if (max_ < hp.max_) max_ = hp.max_;
    }

    inline
    InputType getMin()
    {
        return min_;
    }

    inline
    InputType getMax()
    {
        return max_;
    }

private:
    const InputType* input_vector_;

    InputType min_;
    InputType max_;
};


template <typename InputType>
inline
void parallel_min_max(const InputType* input_v,
                      size_t num_elems,
                      InputType& min,
                      InputType& max)
{
    using namespace tbb;

    typedef tbb_experiments::parallel_min_max_t<InputType> ParallelMinMax;

    ParallelMinMax min_max(input_v);

    parallel_reduce(blocked_range<size_t>(0, num_elems),
                    min_max,
                    auto_partitioner());

    min = min_max.getMin();
    max = min_max.getMax();
}

}


#endif
