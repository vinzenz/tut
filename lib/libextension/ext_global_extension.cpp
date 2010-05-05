#include <ext_global_extension.hpp>

namespace ext
{

namespace detail
{
extension_manager* extension_manager_owner::m_extensionManager = 0;
} // namespace detail

detail::extension_manager_owner g_extensionManager;

} // namespace 

