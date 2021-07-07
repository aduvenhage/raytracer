#ifndef LIBS_HEADER_RANDOM_H
#define LIBS_HEADER_RANDOM_H

#include <random>
#include <thread>


namespace LNF
{
    using default_rand_type = std::mt19937;

    // global random number generator
    template <typename generator_type = default_rand_type>
    auto &generator() {
        thread_local static auto tlInstance = generator_type();
        return tlInstance;
    }


    // seed the random number generator (this should be done per thread)
    template <typename generator_type = default_rand_type>
    void seed(typename generator_type::result_type _uSeed) {
        generator<generator_type>().seed(_uSeed);
    }


    // seed the random number generator with noise (this should be done per thread)
    template <typename generator_type = default_rand_type>
    void seed_auto() {
        thread_local static std::random_device noise;
        generator<generator_type>().seed(noise());
    }

};  // namespace LNF



#endif  // #ifndef LIBS_HEADER_RANDOM_H
