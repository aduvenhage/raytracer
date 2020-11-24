#ifndef LIBS_HEADER_STATS_H
#define LIBS_HEADER_STATS_H

#include "constants.h"


namespace LNF
{
    // calculate mean and variance as you get samples
    class RunningStat
    {
     public:
        RunningStat()
            :m_n(0),
             m_oldM(0),
             m_newM(0),
             m_oldS(0),
             m_newS(0)
        {}

        void clear(){
            m_n = 0;
            m_newM = 0;
            m_oldS = 0;
            m_newS = 0;
        }

        void push(double x) {
            m_n++;
            
            if (m_n == 1) {
                m_newM = x;
                m_newS = 0.0;
            }
            else {
                m_newM = m_oldM + (x - m_oldM) / m_n;
                m_newS = m_oldS + (x - m_oldM) * (x - m_newM);
            }
            
            m_oldM = m_newM;
            m_oldS = m_newS;
        }

        int numDataValues() const {
            return m_n;
        }

        double mean() const {
            return m_newM;
        }

        double variance() const {
            return ((m_n > 1) ? m_newS / (m_n - 1) : 0.0);
        }

        double standardDeviation() const {
            return sqrt(variance());
        }

     private:
        int         m_n;
        double      m_oldM;
        double      m_newM;
        double      m_oldS;
        double      m_newS;
    };

    

};  // namespace LNF



#endif  // #ifndef LIBS_HEADER_STATS_H
