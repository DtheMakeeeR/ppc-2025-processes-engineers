#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace golovanov_d_radix_sort_odd_even_merge {

using InType = std::vector<int>;
using OutType = std::vector<int>;
using TestType = std::tuple<std::string, std::vector<int>>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace golovanov_d_radix_sort_odd_even_merge
