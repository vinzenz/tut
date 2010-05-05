#include <algorithm>
#include <cstdlib>
#include <dirent.h>

#include <ext_manager.hpp>
#include <ext_event.hpp>
#include <ext_exception.hpp>
#include <ref_type.hpp>

#ifdef __GNUC__
extern char* __progname_full;
#else
char* __progname_full = "";
#endif

namespace ext
{

extension_manager::extension_manager(bool p_loadBin)
{
    //load binary extensions
    if (p_loadBin)
    {
        load(__progname_full);
    }

    //load external extensions
    const char* l_extensionDirEnv = getenv(detail::g_extensionDirEnv.c_str());
    if (l_extensionDirEnv)
    {
        std::vector<std::string> l_list = split(l_extensionDirEnv, ":");
        for (auto l_it = l_list.begin(); l_it != l_list.end(); ++l_it)
        {
            loadDir(*l_it);
        }
    }
}

void extension_manager::add(const ExtensionType& p_extension)
{
    assert(p_extension);
    m_list.push_back(p_extension);
}

void extension_manager::remove(const ExtensionListType::iterator p_element)
{
    assert(*p_element);
    m_list.erase(p_element);
}

void extension_manager::enableAll() const
{
    forAll(&iextension::enable);
}

void extension_manager::disableAll() const
{
    forAll(&iextension::disable);
}

extension_manager::ExtensionType extension_manager::find(const std::string& p_extension) const
{
    for (auto l_it = m_list.begin(); l_it != m_list.end(); ++l_it)
    {
        if ((*l_it)->identify() == p_extension)
        {
            return *l_it;
        }
    }

    throw extension_not_found(p_extension);
}

extension_manager::ExtensionType extension_manager::operator[](const std::string& p_extension) const
{
    return find(p_extension);
}

void extension_manager::load(const std::string& p_path, const std::string& p_regex)
{
    std::vector<reflection::Method*> l_extensions;

    try
    {
        l_extensions = reflection::getMethodPtr(p_path, p_regex);
    }
    catch(const std::exception& p_ex)
    {
        throw extension_load_error(p_path + p_ex.what());
    }

    for (auto l_it = l_extensions.begin(); l_it != l_extensions.end(); ++l_it)
    {
        //init extension
        std::shared_ptr<iextension> p_el = std::shared_ptr<iextension>(
                *reinterpret_cast<iextension**>( ((void* (*)())(*l_it)->getPtr())() )
        );

        assert(p_el);
        add(p_el);
    }
}

void extension_manager::loadDir(const std::string& p_path)
{
    DIR* l_dir = ::opendir(p_path.c_str());

    if (l_dir)
    {
        for (dirent* l_ent = ::readdir(l_dir); l_ent; l_ent = ::readdir(l_dir))
        {
            std::string l_path = mkpath(p_path, l_ent->d_name);

            if (l_ent->d_name[0] == '.')
            {
                if (l_ent->d_name[1] == 0 || (l_ent->d_name[1] == '.' && l_ent->d_name[2] == 0))
                {
                    continue;
                }
            }

            load(l_path);
        }
    }

    ::closedir(l_dir);
}

std::vector<std::string> extension_manager::split(const std::string& p_src, const std::string& p_separator) const
{
    std::vector<std::string> l_ret;
    std::string::size_type l_begin = 0;

    while (p_separator.size() < p_src.size())
    {
        std::string::size_type l_pos = p_src.find(p_separator, l_begin);

        if (!l_pos)
        {
            l_begin += p_separator.size();
        }
        else
        if (l_pos == std::string::npos)
        {
            l_ret.push_back(p_src.substr(l_begin));
            l_begin = p_src.size();
        }
        else
        {
            l_ret.push_back(p_src.substr(l_begin, l_pos - l_begin));
            l_begin = l_pos + p_separator.size();
        }
    }

    return l_ret;
}

const std::string extension_manager::mkpath(const std::string& p_path, const std::string& p_name) const
{
    char* l_cwd = ::getcwd(0, 0);
    std::string l_ret;

    l_ret = (*p_path.begin() == '/') ?
        append(p_path, p_name) :
        append(l_cwd, append(p_path, p_name));

    ::free(l_cwd);

    return l_ret;
}

const std::string extension_manager::append(const std::string& p_src, const std::string& p_next) const
{
    return *p_src.rbegin() != '/' ? p_src + "/" + p_next : p_src + p_next;
}

} // namespace ext

