#include <tut/tut.hpp>
#include <memory>

//#define EXT_NOT_LOAD_BIN_EXTENSIONS

#include <ext_global_extension.hpp>
#include <test_etut_ext1.hpp>
#include <test_events.hpp>

namespace tut
{

struct extension_data
{
    extension_data()
    {
    }

    virtual ~extension_data() { }
};

typedef test_group<extension_data> factory;
typedef factory::object object;

} // namespace tut

namespace
{

tut::factory tf("libextension basic tests");

}

namespace tut
{

template<>
template<>
void object::test<1>()
{
    using namespace ext;

    std::unique_ptr<event<event_func_enter, int>>(new event<event_func_enter, int>(32));
    std::unique_ptr<event<event_func_enter, int>>(new event<event_func_enter, int>(1232));
}

template<>
template<>
void object::test<2>()
{
    using ext::g_extensionManager;

    for(auto l_it = (*g_extensionManager).begin(); l_it != (*g_extensionManager).end(); ++l_it)
    {
        std::clog << "\n: extension: " << (*l_it)->identify() << std::endl;
    }
}

template<>
template<>
void object::test<3>()
{
    using ext::g_extensionManager;

    (*g_extensionManager)["etut_func_analysis"]->enable();
    ensure_equals((*g_extensionManager)["etut_func_analysis"]->isEnabled(), true);

    (*g_extensionManager)["etut_func_analysis"]->disable();
    ensure_equals((*g_extensionManager)["etut_func_analysis"]->isEnabled(), false);

    (*g_extensionManager).enableAll();
    (*g_extensionManager).disableAll();
}

template<>
template<>
void object::test<4>()
{
    using namespace ext;
    using namespace etut;

    //g_extensionManager["etut_func_analysis"]->enable();

    func_order& l_funcOrder = (*g_extensionManager)["etut_func_analysis"]->get<func_order>();

    ////l_funcOrder.allow("public void class::.*");
    ////l_funcOrder.deny("public void class::.*");

    l_funcOrder.a = 432;

/*    l_funcOrder.add( &bla::funckja );*/
    //l_funcOrder.add( &bla::inna );
    //l_funcOrder.add( &bla::jeszcze inna );

    //l_funcOrder.startChecking();
    //....
    //l_funcOrder.endChecking();

    /*enusure_equals_functions()*/

    //l_funcWrapper.add( &bla1, &bla2, 3);//3 times

    ensure_equals((*g_extensionManager)["etut_func_analysis"]->get<func_order>().a, 432 );

    std::unique_ptr<event<event_func_enter, int>>(new event<event_func_enter, int>(32));
}

template<>
template<>
void object::test<5>()
{
    using ext::g_extensionManager;

    //throw
    (*g_extensionManager).load("not_exist_file.so");
    (*g_extensionManager).load("example_plugin.so");
    (*g_extensionManager).loadDir("not_exist_dir");
    (*g_extensionManager).loadDir("data/plugins");
}

template<>
template<>
void object::test<6>()
{
    using ext::g_extensionManager;

    (*g_extensionManager).enableAll();
    //ensure_equals( g_extensionManager.end() - g_extensionManager.begin(), 0 );

    (*g_extensionManager).disableAll();
    //ensure_equals( g_extensionManager.empty(), 0 );
}

template<>
template<>
void object::test<7>()
{
    using ext::g_extensionManager;

    //event handlers
    (*g_extensionManager)["etut_func_analysis"]->disable();
    (*(*g_extensionManager)["etut_func_analysis"])["event_func_enter"]->enable();

    (*(*g_extensionManager)["etut_func_analysis"])["event_func_enter"]->setPriority(ext::detail::PRIORITY_HIGH);
}

} // namespace tut

