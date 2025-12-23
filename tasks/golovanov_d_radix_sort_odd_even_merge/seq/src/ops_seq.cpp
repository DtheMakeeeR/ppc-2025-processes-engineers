#include "golovanov_d_radix_sort_odd_even_merge/seq/include/ops_seq.hpp"

#include "golovanov_d_radix_sort_odd_even_merge/common/include/common.hpp"
// #include "util/include/util.hpp" clang-tidy попросил отключить

namespace golovanov_d_radix_sort_odd_even_merge {

GolovanovDRadixSortOddEvenMergeSEQ::GolovanovDRadixSortOddEvenMergeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = false;
}

bool GolovanovDRadixSortOddEvenMergeSEQ::ValidationImpl() {
  return true;
}

bool GolovanovDRadixSortOddEvenMergeSEQ::PreProcessingImpl() {
  return true;
}
bool GolovanovDRadixSortOddEvenMergeSEQ::RunImpl() {
  GetOutput() = true;
  return true;
}

bool GolovanovDRadixSortOddEvenMergeSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_radix_sort_odd_even_merge
