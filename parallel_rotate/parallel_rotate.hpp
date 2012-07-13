#ifndef TBB_EXPERIMENTS_ROTATE_H
#define TBB_EXPERIMENTS_ROTATE_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>

namespace tbb_experiments
{

struct tag_cw_t {};
struct tag_ccw_t {};

template <typename InputType,
          typename RotateTag>
class parallel_rotate_t
{
public:
    parallel_rotate_t(const InputType* input_vector,
                      InputType* output_vector,
                      size_t width,
                      size_t height,
                      RotateTag)
        : input_vector_(input_vector)
        , output_vector_(output_vector)
        , width_(width)
        , height_(height)
    {}

    void operator()(const tbb::blocked_range2d< size_t >& r,
                    typename boost::enable_if< boost::is_same<RotateTag, tag_cw_t> >::type* p = 0) const
    {
        using namespace std;

        // std::cout << "[" << r.rows().begin()<< "," << r.cols().begin() << "] -> [" << r.rows().end() << "," << r.cols().end() << "]" << std::endl;

        for (size_t i = r.rows().begin(), i_stop = r.rows().end();
             i != i_stop;
             ++i)
        {
            for (size_t j = r.cols().begin(), j_stop = r.cols().end();
                 j != j_stop;
                 ++j)
            {
                output_vector_[(j+1)*height_ - 1 - i] = input_vector_[i*width_ + j];
            }
        }
    }

//    void operator()(const tbb::blocked_range2d< size_t >& r,
//                    typename boost::enable_if< boost::is_same<RotateTag, tag_ccw_t> >::type* p = 0) const
//    {
//        using namespace std;
//
//        // std::cout << "[" << r.rows().begin()<< "," << r.cols().begin() << "] -> [" << r.rows().end() << "," << r.cols().end() << "]" << std::endl;
//
//        for (size_t i = r.rows().begin(), i_stop = r.rows().end();
//             i != i_stop;
//             ++i)
//        {
//            for (size_t j = r.cols().begin(), j_stop = r.cols().end();
//                 j != j_stop;
//                 ++j)
//            {
//                output_vector_[(width_ - j - 1)*height_ + i] = input_vector_[i*width_ + j];
//            }
//        }
//    }

private:
    const InputType* input_vector_;
    InputType* output_vector_;
    size_t width_;
    size_t height_;
};

} // tbb_experiments

template <typename InputType,
          typename RotateTag>
void parallel_rotate(const InputType* input,
                     InputType* output,
                     size_t width,
                     size_t height,
                     RotateTag)
{
    using namespace tbb;

    typedef tbb_experiments::parallel_rotate_t<InputType, RotateTag> ParallelRotate;

    affinity_partitioner ap;

    parallel_for( blocked_range2d< size_t>(0, height, 144,
                                           0, width, 144),
                  ParallelRotate(input, output, width, height, RotateTag()),
                  ap);
}

#endif
