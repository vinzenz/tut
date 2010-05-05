#ifndef EXT_EXTENSION_HPP
#define EXT_EXTENSION_HPP

#include <iostream>
#include <memory>
#include <algorithm>

#include <ext_common.hpp>
#include <ext_exception.hpp>
#include <ext_event.hpp>

namespace ext
{

namespace detail
{

/** Common register extension
 * @return extension new instance
 */
template<int N = 0, typename T>
std::shared_ptr<T> EXTENSION()
{
    return std::shared_ptr<T>(new T);
}

} // namespace detail

//forward declaration
template<typename T> class iparameter; 

/** Common basic interface for extension
 */
class iextension_manager : public ievent_identify
{
public:
    typedef std::shared_ptr<ievent_priority_manager> HandlerType;
    typedef std::vector<HandlerType> HandlerListType;

    /** Event handler iterator begin
     * @return iterator begin
     */
    HandlerListType::const_iterator begin() const { return beginImpl(); }

    /** Event handler iterator end
     * @return iterator end
     */
    HandlerListType::const_iterator end() const { return endImpl(); }

    /** Check if event handler list is empty
     * @return true if event handler list is empty
     */
    bool empty() const { return emptyImpl(); }

    /** Search element by name
     * @param p_name name
     * @return found element
     */
    HandlerType operator[](const std::string& p_name) const
    {
        return findImpl(p_name);
    }

    /** Search element by name
     * @param p_name name
     *
     * @return found element
     */
    HandlerType find(const std::string& p_name) const
    {
        return findImpl(p_name);
    }

    /** Method to get data from extension
     * @return iparameter reference
     */
    template<typename T>
    const T& get()
    {
        return dynamic_cast<iparameter<T>&>(*this).getImpl();
    }

private:
    /** Event handler iterator begin implementation
     * @return iterator begin
     */
    virtual HandlerListType::const_iterator beginImpl() const = 0;

    /** Event handler iterator end implementation
     * @return iterator end
     */
    virtual HandlerListType::const_iterator endImpl() const = 0;

    /** Check if event handler list is empty implementation
     * @return true if event handler list is empty
     */
    virtual bool emptyImpl() const = 0;

    /** Search element by name implementation
     * @param p_name name
     * @return found element
     */
    virtual HandlerType findImpl(const std::string& p_name) const = 0;
};

/** Common basic interface for extension
 */
struct iextension : public iextension_manager, public ievent_manager
{
    /** Event trigger when extension is changed to enable state
     */
    void onEnable()
    {
        onEnableImpl();
    }

    /** Event trigger when extension is changed to disable state
     */
    void onDisable()
    {
        onDisableImpl();
    }

    /** Method to get data from extension
     * @return iparameter reference
     */
    template<typename T>
    T& get()
    {
        return dynamic_cast<iparameter<T>&>(*this).getImpl();
    }

    /** Method to set extension data
     * @param p_value value
     */
    template<typename T>
    void set(const T& p_value)
    {
        return dynamic_cast<iparameter<T>&>(*this).setImpl(p_value);
    }

private:
    /** Event trigger when extension is changed to enable state implementation
     */
    virtual void onEnableImpl() = 0;

    /** Event trigger when extension is changed to disable state implementation
     */
    virtual void onDisableImpl() = 0;
};

/** Common interface to handle extension parameters
 */
template<typename T>
class iparameter
{
    friend class iextension;

public:
    /** 
     * 
     * @return 
     */
    T& operator*()
    {
        return getImpl();
    }

private:
    /** Set parameter implementation
     * @param T parameter
     */
    virtual void setImpl(const T&) = 0;

    /** Get parameter implementation
     * @return parameter
     */
    virtual T& getImpl() = 0;
};

/** Base class for written extension
 */
template<typename TData, typename TEextension>
class extension : public iextension, public iparameter<TData>
{
public:
    /** Handle events class
     */
    template<typename TEvent, typename TParam>
    class event_handler : public base_event_handler<TEvent, TParam>
    {
    public:
        /** Ctor
         * @param p_extension reference to extension
         * @param p_state default state enabled/disabled
         * @param p_priority handler priority (determine order)
         */
        event_handler(TEextension& p_extension, bool p_state = true, detail::EPriority p_priority = detail::PRIORITY_LOW)
            : base_event_handler<TEvent, TParam>(p_state, p_priority), m_base(p_extension) 
        {
        }
        
        /** Ctor
         * @param p_extension reference to extension
         * @param p_priority handler priority (determine order)
         * @param p_state default state enabled/disabled
         */
        event_handler(TEextension& p_extension, detail::EPriority p_priority)
            : base_event_handler<TEvent, TParam>(true, p_priority), m_base(p_extension)
        {
        }

    private:
        /** Handle event
         * @param TParam parameter
         */
        void handleImpl(const TParam&); //default event handler

        /** Event handler name
         * @return name
         */
        const std::string& identifyImpl() const
        {
            return base_event<TEvent>::getEvent().identify();
        }

        /** 
         * 
         * @return 
         */
        const iextension_manager& getExtensionImpl() const
        {
            assert(&m_base);
            return static_cast<iextension_manager&>(m_base);
        }

        /** 
         * 
         * @return 
         */
        inline TEextension& base()
        {
            assert(&m_base);
            return m_base; 
        }

        TEextension& m_base;
    };

    /** Initialize event handler
     * @param p_handler event handler
     */
    void init(const HandlerType& p_handler)
    {
        assert(p_handler);
        m_list.push_back(p_handler);
    }

private:
    /** Event handler iterator begin
     * @return iterator begin
     */
    HandlerListType::const_iterator beginImpl() const { return m_list.begin(); }

    /** Event handler iterator end
     * @return iterator end
     */
    HandlerListType::const_iterator endImpl() const { return m_list.end(); }

    /** Check if event handler list is empty
     * @return true if event handler list is empty
     */
    bool emptyImpl() const { return m_list.empty(); }

    /** Enable event handler implementation
     */
    void enableImpl()
    {
        forAll(&ievent_priority_manager::enable);
        onEnable();
    }

    /** Disable event handler implementation
     */
    void disableImpl()
    {
        forAll(&ievent_priority_manager::disable);
        onDisable();
    }

    /** Check if event handler is enabled
     * @return true if at least one event handler is enabled
     */
    bool isEnabledImpl() const
    {
        for (auto l_it = m_list.begin(); l_it != m_list.end(); ++l_it)
        {
            if ((*l_it)->isEnabled())
            {
                return true;
            }
        }

        return false;
    }

    /** Find element in event's list
     * @param p_name name
     * @return found element
     */
    HandlerType findImpl(const std::string& p_name) const
    {
        for (auto l_it = m_list.begin(); l_it != m_list.end(); ++l_it)
        {
            if ((*l_it)->identify() == p_name)
            {
                return *l_it;
            }
        }

        throw event_not_found(p_name);
    }

    /** Do function on every list elements
     * @param p_f function
     */
    template<typename F>
    void forAll(F p_f) const
    {
        assert(p_f);
        std::for_each(m_list.begin(), m_list.end(), std::bind(p_f, std::placeholders::_1));
    }

    HandlerListType m_list;
};

} // namespace ext

#endif

