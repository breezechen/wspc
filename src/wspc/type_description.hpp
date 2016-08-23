/*
 *  Copyright (c) 2016 Kajetan Swierk
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#ifndef WSPC_TYPE_DESCRIPTION_HPP_GUARD
#define WSPC_TYPE_DESCRIPTION_HPP_GUARD

#include <kl/ctti.hpp>
#include <kl/enum_reflector.hpp>
#include <kl/stream_join.hpp>

#include <sstream>
#include <type_traits>

namespace wspc {
namespace detail {

struct visitor
{
    explicit visitor(std::stringstream& strm) : strm_{strm} {}

    template <typename FieldInfo>
    void operator()(FieldInfo f)
    {
        if (current_field_index_)
            strm_ << ", ";
        strm_ << "  " << f.name()
              << " :: " << kl::ctti::name<typename FieldInfo::type>();
        visit_enum(std::forward<FieldInfo>(f),
                   std::is_enum<typename FieldInfo::type>{});
        ++current_field_index_;
    }

private:
    template <typename FieldInfo>
    void visit_enum(FieldInfo, std::true_type)
    {
        using enum_type = std::remove_cv_t<typename FieldInfo::type>;
        using enum_reflector = kl::enum_reflector<enum_type>;
        strm_ << " (";
        auto joiner = kl::make_outstream_joiner(strm_, ", ");
        for (size_t i = 0; i < enum_reflector::count(); ++i)
        {
            if (const auto str =
                    enum_reflector::to_string(static_cast<enum_type>(i)))
            {
                joiner = str;
            }
        }
        strm_ << ")";
    }

    template <typename FieldInfo>
    void visit_enum(FieldInfo, std::false_type) {}

    std::stringstream& strm_;
    std::size_t current_field_index_{0};
};

template <typename T>
std::string get_type_info_impl(std::true_type /*is_reflectable*/)
{
    std::stringstream strm;
    strm << kl::ctti::name<T>() << " { ";
    kl::ctti::reflect<T>(visitor{strm});
    strm << " }";
    return strm.str();
}

template <typename T>
std::string get_type_info_impl(std::false_type /*is_reflectable*/)
{
    return kl::ctti::name<T>();
}
} // namespace detail

template <typename T>
std::string get_type_info()
{
    return detail::get_type_info_impl<T>(kl::is_reflectable<T>{});
}
} // namespace wspc

#endif