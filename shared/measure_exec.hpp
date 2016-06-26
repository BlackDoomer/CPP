#include <chrono>

namespace shared {
  template< typename time_unit = std::chrono::milliseconds >
  struct measure
  {
    template< typename F, typename ...Args >
    static typename time_unit::rep execution( F func, Args&&... args )
    {
      const auto exec_start = std::chrono::high_resolution_clock::now();
      func( std::forward<Args>(args)... );
      const auto exec_end = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<time_unit>
        (exec_end - exec_start).count();
    }
  };
}
