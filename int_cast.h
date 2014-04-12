#ifndef KOKU_INT_CAST
#define KOKU_INT_CAST

#include <limits>
#include <exception>
#include <typeinfo>
#include <sstream>
#include <string.h>

namespace koku {
template <typename Ta, typename Tb>
class bad_cast_out_of_range : public std::bad_cast {
 private:
  std::string text;

 public:
  bad_cast_out_of_range(Ta a, Tb b, Ta c, Tb d) throw() {
    std::stringstream ss;
    ss << "((" << typeid(a).name() << ")" << std::to_string(a) << " > ("
       << typeid(b).name() << ")" << std::to_string(b) << ") || (("
       << typeid(c).name() << ")" << std::to_string(c) << " < ("
       << typeid(d).name() << ")" << std::to_string(d) << "))";
    text = ss.str();
  }

  virtual ~bad_cast_out_of_range() throw() {}

  virtual const char* what() const throw() { return text.c_str(); }
};

template <typename Toutput, typename Tinput>
Toutput int_cast(Tinput input) {
  static_assert(
      std::is_integral<Tinput>::value && std::is_integral<Toutput>::value,
      "Only works on integral types");
  // check if within range: -> doens't work int/unsigned
  if (std::numeric_limits<Toutput>::is_signed) {
    // Toutput is signed
    if (std::numeric_limits<Tinput>::is_signed) {
      // TInput is signed
      if (std::numeric_limits<Toutput>::digits <
          std::numeric_limits<Tinput>::digits) {
        // we have less digits
        if ((static_cast<Tinput>(std::numeric_limits<Toutput>::min()) >
             input) ||
            (static_cast<Tinput>(std::numeric_limits<Toutput>::max()) <
             input)) {
          throw bad_cast_out_of_range<Toutput, Tinput>(
              std::numeric_limits<Toutput>::min(),
              input,
              std::numeric_limits<Toutput>::max(),
              input);
        }
      }
    } else {
      // Tinput is unsigned
      if (std::numeric_limits<Toutput>::digits <
          std::numeric_limits<Tinput>::digits) {
        // we have less digits
        if (static_cast<Tinput>(std::numeric_limits<Toutput>::max()) < input) {
          throw bad_cast_out_of_range<Toutput, Tinput>(
              std::numeric_limits<Toutput>::min(),
              input,
              std::numeric_limits<Toutput>::max(),
              input);
        }
      }
    }
  } else {
    // Toutput is unsigned
    if (std::numeric_limits<Tinput>::is_signed) {
      // TInput is signed
      if ((input != 0) && (input < 1)) {
        // smaller than 0
        throw bad_cast_out_of_range<Toutput, Tinput>(
            std::numeric_limits<Toutput>::min(),
            input,
            std::numeric_limits<Toutput>::max(),
            input);
      }
      if (std::numeric_limits<Toutput>::digits <
          std::numeric_limits<Tinput>::digits) {
        // we have less digits
        if (static_cast<Tinput>(std::numeric_limits<Toutput>::max()) < input) {
          throw bad_cast_out_of_range<Toutput, Tinput>(
              std::numeric_limits<Toutput>::min(),
              input,
              std::numeric_limits<Toutput>::max(),
              input);
        }
      }
    } else {
      // Tinput is unsigned
      if (std::numeric_limits<Toutput>::digits <
          std::numeric_limits<Tinput>::digits) {
        // we have less digits
        if (static_cast<Tinput>(std::numeric_limits<Toutput>::max()) < input) {
          throw bad_cast_out_of_range<Toutput, Tinput>(
              std::numeric_limits<Toutput>::min(),
              input,
              std::numeric_limits<Toutput>::max(),
              input);
        }
      }
    }
  }
  return static_cast<Toutput>(input);
}
}

#endif
