//
//  format.hpp
//  quality_filter
//
//  Created by Bo HAN on 4/26/14.
//  Copyright (c) 2014 Bo HAN. All rights reserved.
//

#ifndef quality_filter_format_hpp
#define quality_filter_format_hpp

#include <string>
#include <cassert>
#include <limits> // for numeric_limits
#include "policy.hpp"

struct Fastq
{
    std::string __header;
    std::string __sequence;
    std::string __quality;
};

template < >
struct Reader < Fastq >
{
    static Fastq __read (std::istream* __in)
    {
        Fastq __f;
        char first_char = __in->get ();
        assert (first_char=='@');
        getline (*__in, __f.__header);
        getline (*__in, __f.__sequence);
        __in->ignore (std::numeric_limits<std::streamsize>::max(), '\n');
        getline (*__in, __f.__quality);
        return __f;
    }
};

template < >
struct Writer < Fastq >
{
    static void __write (std::ostream* __out, const Fastq& __f)
    {
        *__out << '@' << __f.__header << '\n'
              << __f.__sequence << "\n+\n"
              << __f.__quality << '\n' ;
    }
};

template < >
class Filter < Fastq >
{
public:
    static bool __execute (const Fastq& fp, uint8_t __phred_threadshold)
    {
        bool ret = true;
        for (const auto& c : fp.__quality)
        {
            if ( c < __phred_threadshold)
            {
                ret = false;
                break;
            }
        }
        return ret;
    }
};

struct Fasta
{
    std::string __name;
    std::string __sequence;
};

template < >
struct Reader < Fasta >
{
    static Fasta __read (std::istream* __in)
    {
        Fasta __f;
        std::string tmp;
        char first_char = __in->get ();
        assert (first_char == '>');
        getline (*__in, __f.__name);
        while ( __in->good () && __in->peek () != '>' )
        {
            getline (*__in, tmp);
            __f.__sequence += tmp;
        }
        return __f;
    }
};

template < >
struct Writer < Fasta >
{
    static void __write (std::ostream* __out, const Fasta& __f)
    {
        *__out << '>' << __f.__name << '\n'
        << __f.__sequence << '\n';
    }
};

template < >
class Filter < Fasta >
{
public:
    static bool __execute (const Fasta& fa, uint8_t __phred_threadshold)
    {
        bool ret = true;
        for (const auto& c : fa.__sequence)
        {
            if ( c == 'N')
            {
                ret = false;
                break;
            }
        }
        return ret;
    }
};

#endif
