#include "golovanov_d_bcast//seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "golovanov_d_bcast//common/include/common.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_bcast {

GolovanovDBcastSEQ::GolovanovDBcastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = false;
}

bool GolovanovDBcastSEQ::ValidationImpl() {
  //int index = std::get<0>(GetInput());
  int n = std::get<1>(GetInput());
  size_t size = static_cast<size_t>(n);
  std::cout << "Validation n: " << size << "size int: " << std::get<2>(GetInput()).size() << "\n" <<
                                           "size float: " << std::get<3>(GetInput()).size() << "\n" <<
                                           "size double: " << std::get<4>(GetInput()).size() << "\n" <<
                                           "getotput: " << GetOutput() << "\n";
  return ((n > -1) &&  (std::get<2>(GetInput()).size() == size) && 
                      (std::get<3>(GetInput()).size() == size) &&
                      (std::get<4>(GetInput()).size() == size) &&
         (GetOutput() == false));
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
