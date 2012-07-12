#include <vector>
#include <utility>
#include <cfloat>
#include <iomanip>
#include <boost/random.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/algorithm/minmax.hpp>
#include <tbb/tbb.h>

#include <parallel_min_max/parallel_min_max.hpp>

using namespace std;
using namespace tbb;

const int NUM_SAMPLES = 10000000;

typedef vector<double> DataHolder;


int main()
{
    tbb::task_scheduler_init init; //(1);

    //for (int test = 0; test < 10 ; ++test)
    //{
        boost::mt19937 rng;
        rng.seed(static_cast<unsigned int>(std::time(0)));
        boost::uniform_real<> unif_distrib(0.f, 100.f);

        boost::variate_generator<boost::mt19937&, boost::uniform_real<> > gen(rng, unif_distrib);

        DataHolder samples(NUM_SAMPLES);

        // using uniform dice to generate 1M samples
        generate(samples.begin(), samples.end(), gen);

        tick_count t0_p = tick_count::now();

        DataHolder::value_type min_tbb, max_tbb;

        tbb_experiments::parallel_min_max(samples.data(),
                                          samples.size(),
                                          min_tbb, max_tbb);

        tick_count t1_p = tick_count::now();

        double t_tbb = (t1_p - t0_p).seconds();
        cout << left << setw(15) << "TBB.MinMax: " << setw(20) << t_tbb;
        //cout << "max = " << setw(20) << max_tbb;
        //cout << "min = " << setw(20) << min_tbb << endl;

        tick_count t0_stl = tick_count::now();

        DataHolder::value_type max_stl = *max_element(samples.begin(), samples.end());
        DataHolder::value_type min_stl = *min_element(samples.begin(), samples.end());

        tick_count t1_stl = tick_count::now();

        double t_stl = (t1_stl - t0_stl).seconds();
        cout << left << setw(15) << "Serial (STL): " << setw(20) << t_stl;
        //cout << "max = " << setw(20) << max_stl;
        //cout << "min = " << setw(20) << min_stl << endl;

        if ( min_stl != min_tbb ||
             abs(min_stl - min_tbb) > 0.0001 )
        {
            cerr << std::endl
                 << "Mismatch min tbb vs. min STL: "
                 << setprecision(10) << min_tbb
                 << " != "
                 << setprecision(10) << min_stl
                 << std::endl;

            return -1;
        }

        if ( abs(max_stl - max_tbb) > 0.0001 )
        {
            cerr << std::endl
                 << "Mismatch max tbb vs. max STL: "
                 << setprecision(10) << max_tbb
                 << " != "
                 << setprecision(10) <<  max_stl
                 << std::endl;

            return -1;
        }

        std::cout << "Speed up = " << t_stl/t_tbb; // << std::endl;

        tick_count t0_boost = tick_count::now();

        pair<DataHolder::iterator, DataHolder::iterator> minmax = boost::minmax_element(samples.begin(), samples.end());

        tick_count t1_boost = tick_count::now();
        double t_boost = (t1_boost - t0_boost).seconds();

        cout << left << setw(15) << "Boost.MinMax: " << setw(20) << t_boost;
        // cout << "max = " << setw(20) << *minmax.second;
        // cout << "min = " << setw(20) << *minmax.first;
        // cout << endl;

        if ( *minmax.first != min_tbb ||
             abs(*minmax.first - min_tbb) > 0.0001 )
        {
            cerr << std::endl
                 << "Mismatch min tbb vs. min Boost: "
                 << setprecision(10) << min_tbb
                 << " != "
                 << setprecision(10) << *minmax.first
                 << std::endl;

            return -1;
        }

        if ( abs(*minmax.second - max_tbb) > 0.0001 )
        {
            cerr << std::endl
                 << "Mismatch max tbb vs. max Boost: "
                 << setprecision(10) << max_tbb
                 << " != "
                 << setprecision(10) << *minmax.second
                 << std::endl;

            return -1;
        }
        std::cout << "Speed up = " << t_boost/t_tbb << std::endl;
    //}

    return 0;

}
