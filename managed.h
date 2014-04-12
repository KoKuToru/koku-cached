#ifndef KOKUMANAGED
#define KOKUMANAGED
#include <memory>

namespace koku {
/**
* Forces a class to be used with shared_ptr
*/
template <typename T, typename P = std::shared_ptr<T> >
struct managed {
  /**
  * @brief operator new, disabled. Use new_instance
  */
  static void* operator new(std::size_t sz) = delete;
  /**
  * @brief operator new, disabled. Use new_instance
  */
  static void* operator new[](std::size_t sz) = delete;
  /**
  * @brief operator&, disabled. Use references instead
  */
  T* operator&() = delete;
  typedef P pointer;
  /**
   * Creates a new instance and returns a shared_ptr
   */
  template <typename... U>
  static pointer new_instance(U... u) {
    return pointer(::new T(u...));
  }
};
}
#endif
