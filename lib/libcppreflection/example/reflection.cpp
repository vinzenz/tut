#include <iostream>
#include <ref_reflection.hpp>

int main(int argc, char **argv)
{
    using namespace reflection;

    try
    {
        Reflection ref;
        ref.allow(".*");
        ref.load(std::string((argc < 2) ? argv[0] : argv[1]));
        std::cout << ref.toString() << std::endl;
    }
    catch(Failure &ex)
    {
        std::cout << ex.message() << std::endl;
    }

    return 0;
}
