#include "base91.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

void writeFile(
    const std::vector<uint8_t>& vec,
    const std::string& filename,
    bool  formatted = false
    )
{
    std::ofstream out( filename, std::ios::out | std::ios::binary );
    if (!out)
    {
        throw std::runtime_error(
            "Could not open file " + filename + " for writing" );
    }
    if (!formatted)
    {
        std::copy(
            vec.begin(),
            vec.end(),
            std::ostream_iterator<uint8_t>( out )
            );
    }
    else
    {
        int count = 0;
        for (auto c : vec)
        {
            out << c;
            if ( ++count == 64 )
            {
                out << "\r\n"; // windows line-endings hard coded for now
                count = 0;
            }
        }
    }
    out.close();
}

std::vector<uint8_t> readFile( const std::string& filename )
{
    std::ifstream in( filename, std::ios::in | std::ios::binary );
    if (!in)
    {
        throw std::runtime_error(
            "Could not open file " + filename + " for reading" );
    }
    in.seekg( 0, std::ios_base::end );
    std::streampos fileSize = in.tellg();
    std::vector<uint8_t> vec( fileSize );

    in.seekg( 0, std::ios_base::beg );
    in.read( reinterpret_cast<char*>(&vec[ 0 ]), fileSize );
    return vec;
}

int usage()
{
    std::cout << "Usage: base91codec [-e|-f|-d] [infile] [outfile]\n"
                 "       -e encodes, -f encodes formatted (for ease of copy/paste)\n"
                 "       and -d decodes.\n\n";
    return 1;
}

int main( int argc, char* argv[] )
{
    try
    {
        std::vector<std::string> args( argv, argv + argc );
        if( args.size() != 4 )
        {
            return usage();
        }
        auto in = readFile( args[2] );
        if ( args[1] == "-e" || args[1] == "-f" )
        {
            // encode
            std::vector<uint8_t> out;
            out.reserve( in.size() ) ; // it's going to be bigger but this avoids initial resizing
            auto enc = base91::encode( out, in );
            writeFile( out, args[3], args[1] == "-f" );
            return 0;
        }
        if ( args[1] == "-d" )
        {
            // decode
            std::vector<uint8_t> out;
            out.reserve( in.size() ) ; // it's going to be a bit smaller
            auto enc = base91::decode( out, in );
            writeFile( out, args[3] );
            return 0;
        }
        return usage();
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << "\n\n";
        return 2;
    }
}