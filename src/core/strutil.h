
#ifndef CORE_STRUTIL_H
#define CORE_STRUTIL_H

namespace CORE
{
    namespace detail {
        // log param cast (designed to catch and convert paramaters not compatible with sprintf)
        template <typename T> struct cparam {
            static const T &str(const T &_value) {
                return _value;
            }
        };

        // log param cast for std::string
        template <> struct cparam<std::string> {
            static const char *str(const std::string &_value) {
                return _value.c_str();
            }
        };
        
    };  // namespace detail
    

    // Create a string using printf style formatting. Returns -1 on fail, or number of chars in full output.
    template <typename output_type, class... T>
    int getString(output_type &_output, T... t)
    {
        return snprintf(_output.data(), _output.size(), detail::cparam<T>::str(t) ...);
    }


    // create a string using printf style formatting
    template <class... T>
    std::string getString(T... t)
    {
        static thread_local std::vector<char> buffer(1024);
        for (;;) {
            int n = snprintf(buffer.data(), buffer.size(), detail::cparam<T>::str(t) ...) + 1;
            if (n > buffer.size()) {
                buffer.resize(n);
            }
            else {
                return std::string(buffer.data(), n);
            }
        }
    }



};  // namespace CORE


#endif // #ifndef CORE_STRUTIL_H
