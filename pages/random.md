# Random Number Generation
It is important to have fast and reliable random number generation for the type of monte-carlo raytacing I'm doing in this project.

C++11 gave us a new flavour of random number generation and also split the concept of the 'random number generation' from the 'shape of the distribution' or output.  The C++11 mersenne twister generator (see `<random>`) is pretty good and fast enough for my purposes.  The distributions (for example `std::uniform_real_distribution<float>`) are very lightweight and in most cases can be created as local variables in the calling code: all state is kept in the generator; the distributions do not keep any state, and can be created and destroyed as required.


## Some Code
This is the random number generator I'm using now:
```C++

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

```

This is how I use it:
```C++
    std::uniform_real_distribution<float> uniform01(0, 1);
    auto value = uniform01(generator());

```


I'm using the generator as a singleton for easy access, but it wraps the C++11 generator in a thread local way, so that I get a unique generator per thread.   Another way of doing this would be to create a number generator object per worker thread and just pass it down into each call, but having the random number generator passed into each function/method that needs it becomes hard to manage.


