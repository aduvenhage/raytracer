/*
    This file contains containers and test algorithms for my own training and research.
    (Not for production use!)
 */
 
#ifndef ALGO_PLAY_H
#define ALGO_PLAY_H
 
#include "constants.h"
#include "profile.h"

#include <iostream>
 

using namespace LNF;
 

/*
    This messy code is just me playing with some algorithms to see how they work.
 */
namespace LNF_DO_NOT_USE
{
 
    void print(int *_pBegin, int *_pEnd) {
        std::cout << "[";
        if (_pEnd > _pBegin) {
            for (int *p = _pBegin; p < _pEnd-1; p++) {
                std::cout << *p << ", ";
            }
            
            std::cout << *(_pEnd - 1);
        }
        
        std::cout << "]" << std::endl;
    }


    void swap(int &_a, int &_b) {
        auto v = _b;
        _b = _a;
        _a = v;
    }

    
    // very slow sorting
    void bubblesort(int *_pBegin, int *_pEnd) {
        bool bSorted = false;
        while (bSorted == false) {
            bSorted = true;
            
            for (int *p = _pBegin; p < _pEnd-1; p++) {
                if (*p > *(p+1)) {
                    swap(*p, *(p+1));
                    bSorted = false;
                }
            }
        }
    }


    // ok sorting
    void selectionsort(int *_pBegin, int *_pEnd) {
        for (int *p = _pBegin; p < _pEnd-1; p++) {

            int *m = p;

            for (int *q = p+1; q < _pEnd; q++) {
                if (*q < *m) {
                    m = q;
                }
            }
            
            swap(*m, *p);
        }
    }

    // recursive lowerbound
    int *lowerbound(int *_pBegin, int *_pEnd, int v) {
        int *p = _pBegin + (_pEnd - _pBegin) / 2;
        
        if (v == *p) {
            return p;
        }
        else if (p == _pBegin) {
            return _pEnd;
        }
        else if (v < *p) {
            return lowerbound(_pBegin, p, v);
        }
        else {
            return lowerbound(p, _pEnd, v);
        }
    }

    // lower bound in a loop
    int *lowerbound_flat(int *_pBegin, int *_pEnd, int v) {
        for (;;) {
            int *p = _pBegin + (_pEnd - _pBegin) / 2;
            
            if (v == *p) {
                return p;
            }
            else if (p == _pBegin) {
                return _pEnd;
            }
            else if (v < *p) {
                _pEnd = p;
            }
            else {
                _pBegin = p;
            }
        }
    }

    // my own flavour of partitioning -- take last element
    int *quickpartition1(int *_pA, int *_pB)
    {
        // selects last element as pivot point and then parition
        if (_pB-1 > _pA) {
            auto v = *(_pB - 1);
            
            for (;;) {
                if (*_pA > v) {
                    swap(*_pA, *(_pB-2));
                    swap(*(_pB-1), *(_pB-2));
                    
                    _pB--;
                }
                else {
                    _pA++;
                }

                if (_pA >= _pB)
                {
                    return _pB-1;
                }
            }
        }
        else {
            return _pB;
        }
    }


    // my own flavour of partitioning -- too many swaps
    int *quickpartition2(int *_pA, int *_pB)
    {
        // selects middle element as pivot point and then parition
        if (_pB-1 > _pA) {
            auto *pV = _pA + (_pB - _pA) / 2;
            
            for (;;) {
                if ( (_pA < pV) &&
                     (*_pA > *pV) ) {
                    swap(*_pA, *(pV-1));
                    swap(*pV, *(pV-1));
                    pV--;
                }
                else if ( (_pA > pV) &&
                          (*_pA < *pV) ) {
                    swap(*_pA, *(pV+1));
                    swap(*pV, *(pV+1));
                    pV++;
                }
                else {
                    _pA++;
                }
                
                if (_pA >= _pB)
                {
                    return pV;
                }
            }
        }
        else {
            return _pB;
        }
    }


    // cannot get this to work :-(
    int *quickpartition_hoare(int *_pA, int *_pB)
    {
        auto pivot = *(_pA + (_pB - _pA)/2);

        for (;;) {
            while (*_pA < pivot) {
                _pA += 1;
            }
            
            do {
                _pB -= 1;
            } while (*_pB > pivot);

            if (_pA >= _pB) {
                return _pB;
            }
            
            swap(*_pA, *_pB);
        }
    }


    // very fast sorting
    void quicksort(int *_pBegin, int *_pEnd)
    {
        if (_pEnd - _pBegin > 1)
        {
            //auto p = quickpartition1(_pBegin, _pEnd);
            auto p = quickpartition2(_pBegin, _pEnd);
            //auto p = quickpartition_hoare(_pBegin, _pEnd);
            
            //print(_pBegin, _pEnd);
            
            quicksort(_pBegin, p);
            quicksort(p, _pEnd);
        }
    }


    // profile and verify algorithms
    void testAlgorithms()
    {
        // generate data
        std::vector<int> data;
        RandomGen rand;
        std::uniform_int_distribution<int> numbers(0, 999999);
        
        for (int i = 0; i < 400000; i++) {
            data.push_back(numbers(rand));
        }
        
        //data = {6, 7, 3, 5, 2, 1, 8, 9, 0, 4};
        
        // profile sorting
        {
            printf("timing sort method...");
            HighPrecisionScopeTimer timer;
            
            //bubblesort(data.data(), data.data() + data.size());
            //selectionsort(data.data(), data.data() + data.size());
            quicksort(data.data(), data.data() + data.size());
            //std::sort(data.data(), data.data() + data.size());
        }

        // verify sorting
        bool bGood = true;
        for (int i = 1; i < data.size(); i++) {
            if (data[i] < data[i]-1) {
                printf("bad sorting at %d!\n", i);
                bGood = false;
                break;
            }
        }
        
        if (bGood) {
            printf("Good sorting :-)\n");
        }

        // check search
        auto p1 = lowerbound(data.data(), data.data() + data.size(), 4000);
        auto p2 = lowerbound_flat(data.data(), data.data() + data.size(), 4000);
        auto p3 = std::lower_bound(data.data(), data.data() + data.size(), 4000);

        return;
    }

};  // namespace LNF_DO_NOT_USE


#endif  // #ifndef ALGO_PLAY_H

