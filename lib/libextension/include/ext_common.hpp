#ifndef EXT_COMMON_HPP
#define EXT_COMMON_HPP

#define EXTENSION_NAMESPACE            ext::detail      // namespace
#define EXTENSION                      init_extension__ // method name

#define ADD_QUOTE(p_value)             #p_value
#define EXTENSION__                    EXTENSION_NAMESPACE::EXTENSION
#define EXTENSION_(value)              ADD_QUOTE(value)
#define EXTENSION_REG_EXP              EXTENSION_(EXTENSION__)

namespace common
{

/** 
 * 
 */
struct noncopyable
{
  noncopyable & operator=(const noncopyable&) = delete;
  noncopyable(const noncopyable&) = delete;
  noncopyable() = default;
};

} // namespace common

namespace ext
{

namespace detail
{

enum EPriority
{
    PRIORITY_HIGH                   = 0x00,
    PRIORITY_MEDIUM                 = 0x10,
    PRIORITY_LOW                    = 0x20
};

typedef int DefaultType;

const std::string g_extensionDirEnv = "EXTENSION_DIR";
const std::string g_extensionRegExp =  EXTENSION_REG_EXP;
const DefaultType g_defaultValue    =  0;

} // namespace detail

} // namespace ext

#undef ADD_QUOTE
#undef EXTENSION__
#undef EXTENSION_NAMESPACE
#undef EXTENSION_REG_EXP

#endif

