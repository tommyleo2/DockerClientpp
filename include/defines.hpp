#ifndef DEFINES_H
#define DEFINES_H

#include "json.hpp"

#include <type_traits>

namespace DockerClientpp {
#define FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

template <template <typename...> class Base_type, typename... Ts>
std::true_type is_base_of_template_I(const Base_type<Ts...> *);

template <template <typename...> class Base_type>
std::false_type is_base_of_template_I(...);

/**
 * @brief Test if Derived_type class is derived from Base_type class
 *
 * Using Substitution failure is not an error (SFINAE) technique. See Wikipedia
 *
 * The core idea of this method is to test if a `Derived_type *` can be
 * cast to `Base_type *` at compile time. Notice how the templates of
 * Derived_type class and Derived_type class are swapped in
 * `is_base_of_template_I`
 * that returns `std::true_type`. If that does not match, the false_type
 * version is
 * matched.
 */
template <template <typename...> class Base_type, typename Derived_type>
using is_base_of_template =
    decltype(is_base_of_template_I<Base_type>(std::declval<Derived_type *>()));

/**
 * @brief Type of socket that docker daemon use.
 */
enum SOCK_TYPE { UNIX, TCP };

using nlohmann::json;
using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;

namespace Http {
typedef std::string Uri;
typedef json Header;
typedef std::map<std::string, std::string> QueryParam;
}
}

#endif /* DEFINES_H */
