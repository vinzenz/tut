#ifndef EXT_EXCEPTION_HPP
#define EXT_EXCEPTION_HPP

namespace ext
{

/** Default exception class
 */
class extension_error : public std::exception
{
public:
    /** Ctor
     * @param p_message message
     */
    explicit extension_error(const std::string& p_message)
        : m_message(p_message)
    {
    }

    /** Get exception message
     * @return message
     */
    virtual const std::string& getMessage() const
    {
        return m_message;
    }

    /** Dtor
     */
    ~extension_error() throw()
    {
    }

private:
    const std::string m_message;
};

/** Exception class - load error extension
 */
struct extension_load_error : public extension_error
{
    /** Ctor
     * @param p_message message
     */
    explicit extension_load_error(const std::string& p_message)
        : extension_error(p_message)
    {
    }

    /** Dtor
     */
    ~extension_load_error() throw()
    {
    }
};

/** Exception class - extension not found
 */
struct extension_not_found : public extension_error
{
    /** Ctor
     * @param p_message message
     */
    explicit extension_not_found(const std::string& p_message)
        : extension_error(p_message)
    {
    }

    /** Dtor
     */
    ~extension_not_found() throw()
    {
    }
};

/** Exception class - event not found
 */
struct event_not_found : public extension_error
{
    /** Ctor
     * @param p_message message
     */
    explicit event_not_found(const std::string& p_message)
        : extension_error(p_message)
    {
    }

    /** Dtor
     */
    ~event_not_found() throw()
    {
    }
};

} // namespace ext

#endif

