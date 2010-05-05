#ifndef ETUT_CAST_HPP
#define ETUT_CAST_HPP

namespace etut
{

namespace detail
{

template<typename T>
void *void_cast(T ptr)
{
    return *reinterpret_cast<void**>(&ptr);
}

} // namespace detail

} // namespace etut

#endif

