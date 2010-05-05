#ifndef EXT_EVENT_HPP
#define EXT_EVENT_HPP


#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

#include <ext_common.hpp>

namespace ext
{

//forward declaration
class iextension;
class iextension_manager;

/** 
 *
 */
class ievent_identify
{
public:
    /** Identify class
     * @return name
     */
    const std::string& identify() const
    {
        return identifyImpl();
    }

private:
    /** Identify class implementation
     * @return name
     */
    virtual const std::string& identifyImpl() const = 0;
};

/** Common basic extension functionality
 */
class ievent_manager
{
public:
    /** enable extension
     */
    void enable()
    {
        enableImpl();
    }

    /** Disable extension
     */
    void disable()
    {
        disableImpl();
    }

    /** Check if extension is enabled
     * @return true if extension is enabled
     */
    bool isEnabled() const
    {
        return isEnabledImpl();
    }

private:
    /** Enable extension implementation
     */
    virtual void enableImpl() = 0;

    /** Disable extension implementation
     */
    virtual void disableImpl() = 0;

    /** Check if extension is enabled implementation
     * @return true if extension is enabled
     */
    virtual bool isEnabledImpl() const = 0;
};

/** Class to determine priority for event
 */
class ievent_priority_manager : public ievent_manager, public ievent_identify
{
public:
    /** Set extension priority, defines when event will be handled
     * @param p_priority priority
     */
    void setPriority(detail::EPriority p_priority)
    {
        setPriorityImpl(p_priority);
    }

    /** Get extension priority
     * @return priority
     */
    detail::EPriority getPriority() const
    {
        return getPriorityImpl();
    }

    /** 
     * @return 
     */
    const iextension_manager& getExtension() const
    {
        return getExtensionImpl();
    }

private:
    /** Set extension priority implementation
     * @param p_priority priority
     */
    virtual void setPriorityImpl(detail::EPriority p_priority) = 0;

    /** Get extension priority implementation
     * @return priority
     */
    virtual detail::EPriority getPriorityImpl() const = 0;

    /** 
     * @return 
     */
    virtual const iextension_manager& getExtensionImpl() const = 0;
};

/** Common event interface
 */
class ievent : public ievent_identify
{
public:

    /** On begin event
     *  executed just before event handler
     */
    void onEnter(const iextension_manager& p_ext)
    {
        onEnterImpl(p_ext);
    }

    /** On end event
     *  executed just after event handler
     */
    void onExit(const iextension_manager& p_ext)
    {
        onExitImpl(p_ext);
    }

private:
    /** On begin event implementation
     */
    virtual void onEnterImpl(const iextension_manager&) = 0;

    /** On end event implementation
     */
    virtual void onExitImpl(const iextension_manager&) = 0;
};

/** Common handler interface
 */
template<typename TParam = detail::DefaultType>
class ievent_handler : public ievent_priority_manager
{
public:
    /** More convenient way to handle event
     * @param p_param evnet parameter
     */
    void operator()(const TParam& p_param)
    {
        handleImpl(p_param);
    }

    /** Use for comparision
     * @param p_first first element
     * @param p_second second element
     * @return true if first element is less then second
     */
    bool operator<(const ievent_handler& p_element) const
    {
        return this->getPriority() < p_element->getPriority();
    }

    /** Action which will be executed after event
     * @param p_param event parameter
     */
    void handle(const TParam& p_param)
    {
        handleImpl(p_param);
    }

private:
    /** Action implementation
     * @param p_param event parameter
     */
    virtual void handleImpl(const TParam& p_param) = 0;
};

/** 
 * 
 *
 */
template<typename TEvent>
struct base_event
{
    static TEvent& getEvent()
    {
        static TEvent s_event;
        return s_event;
    }
};

/** Keep event's information
 */
template<typename TEvent, typename TParam = detail::DefaultType>
class event
{
    typedef std::shared_ptr<ievent_handler<TParam>> EventType;
    typedef std::vector<EventType> EventsListType;

public:
    /** Ctor
     * Trigger for event handler
     * @param p_param event parameter
     */
    explicit event(const TParam& p_param = detail::g_defaultValue)
    {
        //asynchronouse
        //1. ctor add to queue
        //2. timer is trigger for handleEvent and clear queue

        //check queue to handle

        //TODO mutex's
        handleEvent(p_param);
    }

    /** Init event to the list
     * @param p_event event
     */
    static void init(const EventType& p_eventHandler)
    {
        //TODO mutex's
        getList().push_back(p_eventHandler);
        sortByPriority();
    }

    /** Sort event's by priority
     */
    static void sortByPriority()
    {
        std::sort(getList().begin(), getList().end());
    }

private:
    /** Handle event
     * @param p_param parameter
     */
    static void handleEvent(const TParam& p_param)
    {
        for (auto l_it = getList().begin(); l_it != getList().end(); ++l_it)
        {
            if ((*l_it)->isEnabled())
            {
                base_event<TEvent>::getEvent().onEnter((*l_it)->getExtension());
                (*l_it)->handle(p_param);
                base_event<TEvent>::getEvent().onExit((*l_it)->getExtension());
            }
        }
    }

    /** Get static list initalization
     * @return static list
     */
    static EventsListType& getList()
    {
        static EventsListType s_list;
        return s_list;
    }
};

/** Common event handler class
 */
template<typename TEvent, typename TParam = detail::DefaultType>
class base_event_handler : public ievent_handler<TParam>
{
public:
    /** Ctor
     * Init event handler
     * @param p_state default event state
     */
    base_event_handler(bool p_state = true, detail::EPriority p_priority = detail::PRIORITY_LOW)
        : m_state(p_state), m_priority(p_priority)
    {
        event<TEvent, TParam>::init(std::shared_ptr<ievent_handler<TParam>>(this));
    }

private:
   /** Check if event handler is enabled
    * @return true if event handler is enabled
    */
    bool isEnabledImpl() const
    {
        return m_state;
    }

    /** Enable event handler
     */
    void enableImpl()
    {
        m_state = true;
    }

    /** Disable event handler
     */
    void disableImpl()
    {
        m_state = false;
    }

    /** Set extension priority
     * @param p_priority priority
     */
    void setPriorityImpl(detail::EPriority p_priority)
    {
        m_priority = p_priority;
        event<TEvent, TParam>::sortByPriority();
    }

    /** Get extension priority
     * @return priority
     */
    detail::EPriority getPriorityImpl() const
    {
        return m_priority;
    }

    bool m_state;
    detail::EPriority m_priority;
};

} // namespace ext

#endif

