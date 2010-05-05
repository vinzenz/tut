#ifndef FTRACE_TYPE_HPP
#define FTRACE_TYPE_HPP

namespace ftrace
{

class fun_type
{
public:
    typedef std::shared_ptr<iextension> ExtensionType;

    ExtensionListType::const_iterator begin() const { return m_list.begin(); }

    ExtensionListType::const_iterator end() const { return m_list.end(); }

    bool empty() const { return m_list.empty(); }

private:
    void* m_functionPtr;
    void* m_callFunctionPtr;
};

} // namespace ftrace

#endif

