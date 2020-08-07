
#ifndef LIBS_HEADER_PROFILE_H
#define LIBS_HEADER_PROFILE_H

#include <chrono>


namespace LNF
{

    template <typename clock_type>
    class ScopeTimer
    {
     public:
        ScopeTimer() {
            m_tpInit = clock_type::now();
        }
        
        ~ScopeTimer() {
            auto td = clock_type::now() - m_tpInit;
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
            
            printf("scope timer: %.4fs\n", (float)(ns*1e-09));
        }
        
     private:
        typename clock_type::time_point   m_tpInit;
    };

    using HighPrecisionScopeTimer = ScopeTimer<std::chrono::high_resolution_clock>;

};


#endif  // #ifndef LIBS_HEADER_PROFILE_H
