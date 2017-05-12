#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

namespace meshIO
{
    template <class Vertex>
    bool writeFile (std::vector<Vertex> const& vertices, char const* flName, std::string const& comment="")
    {
        std::ofstream fl;
        fl.open(flName, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
        if (!fl.is_open())
        {
            std::cerr<<"Error in call to WriteFile: file failed to open"<<std::endl;
            return false;
        }
        std::ostringstream tempStr;
        tempStr<<vertices.size();
        std::string header = (comment==""?"":comment+"\n")+tempStr.str()+"\n";
        fl.write(header.c_str(), header.length());
        fl.write(reinterpret_cast<char const*>(vertices.data()), vertices.size()*sizeof(Vertex));
        fl.close();
        return (bool)fl; //Evaluates to false if fl raised an error when attempting to write
    }

    template <class Vertex>
    bool readFile (char const* flName, std::vector<Vertex>& outVertices, bool const& overwrite=true)
    {
        if (overwrite)
        {
            if (outVertices.size() > 0)
                std::cerr<<"Warning in call to readFile: outVertices is non-empty and will be overwritten."
                           "Overwrites can be controlled with the final flag."<<std::endl;
            outVertices.resize(0);
        }
        std::ifstream fl;
        fl.open(flName, std::ifstream::in | std::ifstream::binary);
        fl.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        size_t sz; fl>>sz;
        outVertices.resize(outVertices.size()+sz);
        fl.ignore();
        fl.read(reinterpret_cast<char*>(outVertices.data()), sz*sizeof(Vertex));
        return (bool)fl;
    }
}
