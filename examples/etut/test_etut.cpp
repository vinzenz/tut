#include <tut/tut.hpp>
#include <memory>

#include <ext_global_extension.hpp>
#include <extensions/etut_func_analysis.hpp>

namespace tut
{

struct etut_data
{
    etut_data()
    {
    }

    virtual ~etut_data() { }
};

typedef test_group<etut_data> factory;
typedef factory::object object;

} // namespace tut

namespace
{

tut::factory tf("etut basic tests");

}

namespace tut
{

template<>
template<>
void object::test<1>()
{
    using namespace ext;
    using namespace ext::detail;

    g_extensionManager["etut_func_analysis"]->enable();

    func_analysis& l_funcAnalysis = g_extensionManager["etut_func_analysis"]->get<func_analysis>();
}

} // namespace tut

