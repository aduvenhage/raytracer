#pragma once

#include "constants.h"
#include <cmath>


namespace CORE
{
    // calculate mean and variance as you get samples
    class RunningStat
    {
     public:
        RunningStat()
            :m_uCount(0),
             m_dMean(0),
             m_dM2(0)
        {}
    
        void push(double _x) {
            if (std::isnan(_x) == false) {
                m_uCount++;
                auto delta = _x - m_dMean;
                m_dMean += delta / m_uCount;
                
                auto delta2 = _x - m_dMean;
                m_dM2 += delta * delta2;
            }
        }
    
        double mean() const {
            return m_dMean;
        }

        double variance() const {
            return m_dM2 / m_uCount;
        }

        double sampleVariance() const {
            return m_dM2 / (m_uCount-1);
        }

        double standardDeviation() const {
            return sqrt(variance());
        }

     private:
        uint64_t    m_uCount;
        double      m_dMean;
        double      m_dM2;
    };
    
        
    

};  // namespace CORE


