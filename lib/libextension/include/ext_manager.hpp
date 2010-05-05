#ifndef EXT_MANAGER_HPP
#define EXT_MANAGER_HPP

#include <memory>
#include <cassert>

#include <ext_extension.hpp>
#include <ext_common.hpp>
#include <ref_reflection.hpp>

namespace ext
{

/** Common class to handle extensions
 */
class extension_manager
{
public:
    typedef std::shared_ptr<iextension> ExtensionType;
    typedef std::vector<ExtensionType> ExtensionListType;

    /** Ctor
     * @param p_loadBin if true then load binary extension
     */
    extension_manager(bool p_loadBin = true);

    /** Load extension
     * @param p_path path for compilation unit with extension
     * @param p_regex pattern to extension method
     */
    void load(const std::string& p_path, const std::string& p_regex = detail::g_extensionRegExp);

    /** Load whole dir searching for extensions
     * @param p_path path to directory
     */
    void loadDir(const std::string& p_path);

    /** Enable all extensions
     */
    void enableAll() const;

    /** Disable all extensions
     */
    void disableAll() const;

    /** Extension list iterator begin
     * @return iterator begin
     */
    ExtensionListType::const_iterator begin() const { return m_list.begin(); }

    /** Extension list iterator end
     * @return iterator end
     */
    ExtensionListType::const_iterator end() const { return m_list.end(); }

    /** Check if extension list is empty
     * @return true if is empty
     */
    bool empty() const { return m_list.empty(); }

    /** Return extension which can be used with parameter
     * @param p_extension extension name
     *
     * @return found extension
     */
    ExtensionType find(const std::string& p_extension) const;

    /** Return extension from name in more convenient way
     * @param p_extension extension name
     *
     * @return found extension
     */
    ExtensionType operator[](const std::string& p_extension) const;

#ifdef EXT_USE_ASYNCHRONOUS_EVENTS
    void setTime(unsigned int p_time);
#endif

private:
    /** Add extension
     * @param p_extension extension
     */
    void add(const ExtensionType& p_extension);

    /** Remove extension
     * @param p_element extension
     */
    void remove(const ExtensionListType::iterator p_element);

    /** For all extension do function
     * @param p_f function
     */
    template<typename F>
    void forAll(F p_f) const
    {
        assert(p_f);
        std::for_each(m_list.begin(), m_list.end(), std::bind(p_f, std::placeholders::_1));
    }

    /** Split string to list
     * @param p_src source string
     * @param p_separator separator
     *
     * @return list of separated strings
     */
    std::vector<std::string> split(const std::string& p_src, const std::string& p_separator) const;

    /** Mkpath from strings
     * @param p_path init path
     * @param p_name name to be added
     *
     * @return joined path
     */
    const std::string mkpath(const std::string& p_path, const std::string& p_name) const;

    /** Append path
     * @param p_src source path
     * @param p_next next element
     *
     * @return joined pathc
     */
    const std::string append(const std::string& p_src, const std::string& p_next) const;

    ExtensionListType m_list;
};

} // namespac ext

#endif

