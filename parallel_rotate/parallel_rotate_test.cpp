#include <iostream>
#include <vector>
#include <algorithm>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

#include <parallel_rotate/parallel_rotate.hpp>

const size_t NY = 3000;
const size_t NX = 4500;

namespace test
{

struct SeqInt
{
    SeqInt() : current_(0) {}

    int operator()() { return current_++; }

    int current_;
};

template <typename InputType>
void rotate_ccw(const InputType* input,
               InputType* output,
               size_t width,
               size_t height)
{
    for (size_t j = 0; j < height; j++)
    {
        for (size_t i = 0; i < width; i++)
        {
            output[(width - i - 1)*height + j] = input[j*width + i];
        }
    }
}

template <typename InputType>
void rotate_cw(const InputType* input,
               InputType* output,
               size_t width,
               size_t height)
{
    for (size_t j = 0; j < height; j++)
    {
        for (size_t i = 0; i < width; i++)
        {
            output[(i+1)*height - 1 - j] = input[j*width + i];
        }
    }
}

template <typename InputType>
void print_matrix(const InputType* matrix,
                  size_t width,
                  size_t height)
{
    for (size_t j = 0; j < height; j++)
    {
        for (size_t i = 0; i < width; i++)
        {
            std::cout << *matrix << " ";

            matrix++;
        }
        std::cout << std::endl;
    }
}

template <typename InputType>
bool compare_sequences(const InputType* seq1,
                       const InputType* seq2,
                       size_t num_elems)
{
    while (num_elems)
    {
        if ( *seq1 != *seq2 ) return false;

        num_elems--;
    }
    return true;
}


}

typedef std::vector< float > Container;

int main(int argc, char** argv)
{
    using namespace tbb;
    using namespace test;
    using namespace std;

    tbb::task_scheduler_init init;

    Container vector(NY * NX);
    Container vector_parallel(NY * NX);
    Container vector_serial(NY * NX);

    // fill vector with sequential numbers
    generate(vector.begin(), vector.end(), SeqInt());

    tick_count t0_s = tick_count::now( );

    // test::print_matrix(vector.data(), NX, NY);
    rotate_cw(vector.data(), vector_serial.data(), NX, NY);

    tick_count t1_s = tick_count::now( );
    double t_s = (t1_s - t0_s).seconds( );
    cout << "Serial: " << t_s << endl;

    tick_count t0_p = tick_count::now( );

    parallel_rotate(vector.data(), vector_parallel.data(), NX, NY, tbb_experiments::tag_cw_t());

    tick_count t1_p = tick_count::now( );
    double t_p = (t1_p - t0_p).seconds( );
    cout << "Parallel: " << t_p << endl;

    // test::print_matrix(vector_parallel.data(), NY, NX);
    // test::print_matrix(vector_serial.data(), NY, NX);

    if ( compare_sequences(vector_parallel.data(),
                           vector_serial.data(),
                           NX*NY) )
    {
        std::cout << "Speed up = " << t_s/t_p << std::endl;
        return 0;
    }

    std::cerr << "test failed" << std::endl;

    return -1;
}
