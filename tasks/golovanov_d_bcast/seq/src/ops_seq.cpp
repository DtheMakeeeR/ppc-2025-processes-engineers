#include "golovanov_d_bcast//seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "golovanov_d_bcast//common/include/common.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_bcast {

GolovanovDBcastSEQ::GolovanovDBcastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool GolovanovDBcastSEQ::ValidationImpl() {
  //int index = std::get<0>(GetInput());
  int n = std::get<1>(GetInput());
  return (n > -1) && ((std::get<2>(GetInput()).size() == std::get<3>(GetInput()).size()) == (std::get<4>(GetInput()).size() == static_cast<size_t>(n))) &&
         (GetOutput() == true);
}

bool GolovanovDBcastSEQ::PreProcessingImpl() {
  return true;
}

bool GolovanovDBcastSEQ::RunImpl() {
  GetOutput() = true;
  return true;
}

bool GolovanovDBcastSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_bcast
