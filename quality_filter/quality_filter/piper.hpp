//
//  piper.hpp
//  quality_filter
//
//  Created by Bo HAN on 4/26/14.
//  Copyright (c) 2014 Bo HAN. All rights reserved.
//

#ifndef quality_filter_piper_hpp
#define quality_filter_piper_hpp

#include <vector>
#include <deque>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "format.hpp"

template<   typename N,
            template <typename ...> class DO,
            int MAX_BUFFER_SIZE = 10,
            template <typename ...> class T = std::vector
        >
class Piper :   Reader <N>,
                Writer <N>,
                DO <N>
{
private:
    std::istream*  __in;
    std::ostream*  __out;
    T <N> __buffer;
    std::mutex __mx;
    std::condition_variable __not_empty;
    std::condition_variable __not_full;
    uint8_t __phred_threshold;
public:
    explicit Piper (std::istream* _in, std::ostream* _out, uint8_t _phred_threshold = 5):
        __in (_in),
        __out (_out),
        __phred_threshold (_phred_threshold)
    { }
    void __read ()
    {
        while (1)
        {
            std::unique_lock <std::mutex> __lock (__mx);
            while ( __buffer.size () == MAX_BUFFER_SIZE )
            {
                __not_full.wait (__lock);
            }
            while ( __in->good () && __buffer.size () < MAX_BUFFER_SIZE )
            {
                __buffer.push_back ( Reader <N>::__read (__in) );
            }
            __lock.unlock ();
            __not_empty.notify_one ();
            if (! __in->good ())
            {
                break;
            }
        }
    }

    void __write ()
    {
        T<N> __buffer2;
        while (1)
        {
            std::unique_lock <std::mutex> __lock (__mx);
            while ( __buffer.size () == 0 )
            {
                __not_empty.wait (__lock);
            }
            while (__buffer.size ())
            {
                __buffer2.push_back (__buffer.back () );
                __buffer.pop_back ();
            }
            __lock.unlock ();
            __not_full.notify_one ();
            for (const auto& f: __buffer2)
            {
                if (DO<N>::__execute (f, __phred_threshold))
                {
                    Writer <N>::__write (__out, f);
                }
            }
            __buffer2.clear ();
            if ( __buffer.size () == 0 && ! __in->good () )
            {
                break;
            }
        }
    }
    void operator () ( )
    {
        std::thread reader { &Piper::__read , this };
        std::thread writer { &Piper::__write, this };
        reader.join ();
        writer.join ();
    }
};


#endif
