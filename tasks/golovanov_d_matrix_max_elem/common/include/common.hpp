#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace golovanov_d_matrix_max_elem {

using InType = std::tuple<size_t, size_t, std::vector<double>>;
using OutType = double;
using TestType = std::string;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace golovanov_d_matrix_max_elem
