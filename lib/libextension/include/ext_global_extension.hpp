#ifndef EXT_GLOBAL_EXTENSION_HPP
#define EXT_GLOBAL_EXTENSION_HPP

#include <ext_manager.hpp>

namespace ext
{

namespace detail
{

#ifndef EXT_EXPLICIT_INIT
bool g_extExplicitInit = false;
#else
bool g_extExplicitInit = true;
#endif

#ifndef EXT_NOT_LOAD_BIN_EXTENSIONS
bool g_extLoadBin = true;
#else
bool g_extLoadBin = false;
#endif

/** 
 * 
 */
class extension_manager_owner : public common::noncopyable
{
public:
    /** 
     * 
     */
    extension_manager_owner()
    {
        if (!g_extExplicitInit)
        {
            init();
        }
    }

    /** 
     * 
     */
   ~extension_manager_owner()
    {
        delete m_extensionManager;
        m_extensionManager = 0;
    }

   /** 
    * 
    */
    void init()
    {
        assert(!m_extensionManager);
        m_extensionManager = new extension_manager(g_extLoadBin);
    }

    /** 
     * 
     */
    void reset()
    {
        release();
        init();
    }
    
    /** 
     * 
     */
    void release()
    {
        assert(m_extensionManager);

        delete m_extensionManager;
        m_extensionManager = 0;
    }

    /** 
     * 
     * @return 
     */
    extension_manager& get()
    {
        assert(m_extensionManager);
        return *m_extensionManager;
    }

    /** 
     * 
     * @return 
     */
    extension_manager& operator*()
    {
        return get();
    }

    /** 
     * 
     * @return 
     */
    extension_manager* operator->()
    {
        assert(m_extensionManager);
        return m_extensionManager;
    }

    /** 
     * 
     * @return 
     */
    bool isInitialized() const
    {
        return m_extensionManager != 0;
    }

private:
    static extension_manager* m_extensionManager;
};

} // namespace detail

extern detail::extension_manager_owner g_extensionManager;

} // namespace ext

#endif

