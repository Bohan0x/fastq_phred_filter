//
//  main.cpp
//  quality_filter
//
//  Created by Bo HAN on 4/26/14.
//  Copyright (c) 2014 Bo HAN. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "boost/program_options.hpp"
#include "piper.hpp"

using namespace std;
namespace po = boost::program_options;

int main(int argc, const char * argv[])
{
    po::options_description opts { R"(
This program filter qualities of FastQ format.
)"
    };
    string input_file_name;
    string output_file_name;
    int minimal_phred;
    try
    {
        opts.add_options ()
        ("help,h", "display this help message and exit")
        ("in,i", po::value<string>(&input_file_name)->default_value ("stdin"), "the input fastq file")
        ("out,o", po::value<string>(&output_file_name)->default_value ("stdout"), "output fastq file")
        ("min,m", po::value<int>(&minimal_phred)->default_value(10), "minimal phred score allowed")
        ;
        po::variables_map vm;
        po::store (po::parse_command_line(argc, argv, opts), vm);
        po::notify (vm);
        if ( vm.count("help") )
        {
            std::cerr << opts << std::endl;
            exit (1);
        }

    } catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << opts << std::endl;
        exit (1);
    }
    
    istream* in_file {&cin};
    ostream* ot_file {&cout};
    if (input_file_name != "stdin" )
    {
        in_file = new ifstream {input_file_name};
        if (! *in_file)
        {
            cerr << "error: cannot open file " << input_file_name << endl;
            exit (1);
        }
    }
    if (output_file_name != "stdout" )
    {
        ot_file = new ofstream {output_file_name};
        if (! *ot_file)
        {
            cerr << "error: cannot open file " << output_file_name << endl;
            exit (1);
        }
    }
    Piper <Fastq, Filter> {in_file, ot_file, static_cast<uint8_t> ( minimal_phred + 33)  } ();
    if (in_file != &cin)
    {
        dynamic_cast<ifstream*>(in_file)->close ();
        
    }
    if (ot_file != &cout)
    {
        dynamic_cast<ofstream*>(ot_file)->close ();
        
    }
    return 0;
}

