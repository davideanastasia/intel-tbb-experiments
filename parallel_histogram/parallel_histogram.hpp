#ifndef TBB_EXPERIMENTS_VECTOR_POW_H
#define TBB_EXPERIMENTS_VECTOR_POW_H

#include <cmath>
#include <algorithm>
#include <vector>
#include <limits>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/cache_aligned_allocator.h>

namespace tbb_experiments
{

template<typename InputType, int LowerBound = 0, int Upperbound = 255>
class parallel_histogram_t
{
public:
    typedef std::vector<int, tbb::cache_aligned_allocator<int> > HistogramType;

    //! \brief ctor
    parallel_histogram_t(const InputType* input_v)
        : input_vector_(input_v)
        , histogram_(Upperbound - LowerBound + 1)
    {
//        std::fill(histogram_.begin(),
//                  histogram_.end(),
//                  0);
    }
    
    //! \brief Special TBB split copy constructor
    parallel_histogram_t(const parallel_histogram_t& hp, tbb::split )
        : input_vector_(hp.input_vector_)
        , histogram_(hp.histogram_.size())
    {}
    
    //! \brief Parallel block!
    void operator()(const tbb::blocked_range<size_t>& r)
    {
        const InputType* curr_p = input_vector_ + r.begin();
        const InputType* end_p = input_vector_ + r.end();

        while ( curr_p != end_p )
        {
            histogram_[*curr_p - LowerBound]++;

            curr_p++;
        }
    }
    
    void join(const parallel_histogram_t& hp)
    {
        // sum into this instance the value of hp's histogram
        std::transform(histogram_.begin(),
                       histogram_.end(),
                       hp.histogram_.begin(),
                       histogram_.begin(),
                       std::plus<HistogramType::value_type>() );
    }
    
    //! \brief Access to final histogram
    const HistogramType& getHistogram() const
    {
        return histogram_;
    }
    
private:
    const InputType* input_vector_;
    HistogramType histogram_;
};

}

#endif
