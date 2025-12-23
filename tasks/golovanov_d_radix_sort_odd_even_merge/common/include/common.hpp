#pragma once

#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

using InType = std::tuple<int, int, std::vector<int>, std::vector<float>, std::vector<double>>;
using OutType = bool;
using TestType = std::tuple<int, int, bool>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace golovanov_d_radix_sort_odd_even_merge
