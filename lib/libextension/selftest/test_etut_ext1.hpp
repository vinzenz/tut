#ifndef TEST_EXTENSION_HPP
#define TEST_EXTENSION_HPP

#include <ext_extension.hpp>
#include <test_events.hpp>

namespace etut
{

/** Keep function order information
 */
class func_order
{
public:
    func_order()
        : a(0)
    {
    }
    int a;

};

} // namespace etut

namespace ext
{

/** Extension example
 */
template<typename T>
class etut_func_analysis : public extension<T, etut_func_analysis<T>>
{
    template<typename TData, typename TExtension> friend class extension;

    typedef extension<T, etut_func_analysis<T>> Extension;
    typedef typename Extension::template event_handler<event_func_enter, int> EventFuncEnter;
    typedef typename Extension::template event_handler<event_func_exit,  int> EventFuncExit;
    
public:
    /** Default Ctor
     */
    etut_func_analysis()
        : m_id("etut_func_analysis")
    {
        init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this)) );
        init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this, detail::PRIORITY_LOW)) );
        //init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this, false, detail::PRIORITY_LOW)) );
        //init( std::shared_ptr<EventFuncEnter>(new EventFuncEnter(*this, true)) );
    }

private:
    /** Set value
     * @param p_data value
     */
    void setImpl(const T& p_data)
    {
        assert(&p_data);
        m_data = p_data;
    }

    /** Get value
     * @return value
     */
    T& getImpl()
    {
        assert(&m_data);
        return m_data;
    }

    void onEnableImpl()
    {
        std::cout << "ENABLE IMPLEMENTATION\n";
    }

    void onDisableImpl()
    {
        std::cout << "DISABLE IMPLEMENTATION\n";
    }
  
    T m_data;
    const std::string& identifyImpl() const { return m_id; }
    const std::string m_id;
};

namespace detail
{
template std::shared_ptr<etut_func_analysis<etut::func_order>> EXTENSION();
} // namespace detail

/**
 *
 * @param p_param
 */
template<>
template<>
void etut_func_analysis<etut::func_order>::event_handler<event_func_enter, int>::
     handleImpl(const int& p_param)
{
    std::clog << (*base()).a << std::endl;
    std::clog << "It's works\n" << p_param << std::endl;
}

/**
 *
 * @param p_param
 */
template<>
template<>
void etut_func_analysis<etut::func_order>::event_handler<event_func_exit, int>::
     handleImpl(const int& p_param)
{
}

} // namespace ext

#endif

