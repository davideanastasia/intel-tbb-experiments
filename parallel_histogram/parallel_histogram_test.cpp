#include <list>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <ctime>
#include <valarray>

#include <boost/random.hpp>

#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

#include <parallel_histogram/parallel_histogram.hpp>

using namespace std;
using namespace tbb;

typedef std::vector<int> Container;

namespace test
{

class serial_histogram_t
{
public:
    typedef std::vector<int> Histogram;

    //! \brief ctor
    serial_histogram_t(int lower_bound, int upper_bound)
        : histogram_(Histogram(upper_bound-lower_bound+1))
        , lower_bound_(lower_bound)
        , upper_bound_(upper_bound)
    {
        fill(histogram_.begin(), histogram_.end(), Histogram::value_type());
    }

    void operator()(const int& value)
    {
        histogram_[value - lower_bound_]++;
    }

    //! \brief Access to final histogram
    const Histogram& getHistogram() const
    {
        return histogram_;
    }

private:
    Histogram histogram_;
    int lower_bound_;
    int upper_bound_;
};

}

const int NUM_SAMPLES = 10000000;
const int LOWER_BOUND = 0;
const int UPPER_BOUND = 255;

int main()
{
    tbb::task_scheduler_init init;

    //for (int test = 0; test < 10 ; ++test)
    //{
        boost::mt19937 rng;
        rng.seed(static_cast<unsigned int>(std::time(0)));
        boost::uniform_int<> rand_img_generator(LOWER_BOUND,UPPER_BOUND);

        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(rng, rand_img_generator);

        Container test_data(NUM_SAMPLES);

        generate(test_data.begin(), test_data.end(), die);

        test::serial_histogram_t hist_serial(LOWER_BOUND,UPPER_BOUND);
        tick_count t0_s = tick_count::now();

        hist_serial = for_each(test_data.begin(), test_data.end(), hist_serial);

        tick_count t1_s = tick_count::now();
        double t_s = (t1_s - t0_s).seconds();

        const test::serial_histogram_t::Histogram& hist = hist_serial.getHistogram();

        typedef tbb_experiments::parallel_histogram_t<Container::value_type, LOWER_BOUND,UPPER_BOUND> ParallelHistogram;

        ParallelHistogram hist_parallel(test_data.data());
        tick_count t0_p = tick_count::now();

        parallel_reduce(tbb::blocked_range<size_t>(0, test_data.size()),
                        hist_parallel,
                        tbb::auto_partitioner());

        tick_count t1_p = tick_count::now();
        double t_p = (t1_p - t0_p).seconds();

        const ParallelHistogram::HistogramType& hist_p = hist_parallel.getHistogram();

        for (size_t idx = 0; idx < hist_p.size(); ++idx)
        {
            if ( hist[idx]  != hist_p[idx] )
            {
                std::cerr << "Index "
                          << idx
                          << " failed: "
                          << hist[idx] << " != " << hist_p[idx]
                          << std::endl;

                return -1;
            }
        }

        cout << "Serial: " << t_s << ", ";
        cout << "Parallel: " << t_p << ", ";
        cout << "Speed-up: " << t_s/t_p << std::endl;
    //}
    return 0;
}
