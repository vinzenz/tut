#include <tut.h>

namespace tut
{
    struct basicCompareGroup{};
 
    typedef test_group<basicCompareGroup> typeTestgroup;
    typedef typeTestgroup::object testobject;
    typeTestgroup basicCompareGroup("basicCompare");
 
    template<>
    template<>
    void testobject::test<1>()
    {
        ensure("null",0 == 0);
    }
};

