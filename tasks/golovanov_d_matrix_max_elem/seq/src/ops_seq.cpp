#include "golovanov_d_matrix_max_elem//seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "golovanov_d_matrix_max_elem//common/include/common.hpp"
#include "util/include/util.hpp"

namespace golovanov_d_matrix_max_elem {

GolovanovDMatrixMaxElemSEQ::GolovanovDMatrixMaxElemSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool GolovanovDMatrixMaxElemSEQ::ValidationImpl() {
  int columns = std::get<0>(GetInput());
  int strokes = std::get<1>(GetInput());
  return (columns > 0) && (strokes > 0) && (std::get<2>(GetInput()).size() == static_cast<size_t>(strokes * columns)) &&
         (GetOutput() == 0);
}

bool GolovanovDMatrixMaxElemSEQ::PreProcessingImpl() {
  return true;
}

bool GolovanovDMatrixMaxElemSEQ::RunImpl() {
  std::vector<double> elems = std::get<2>(GetInput());
  double max = *std::max_element(elems.begin(), elems.end());
  GetOutput() = max;
  return true;
}

bool GolovanovDMatrixMaxElemSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_matrix_max_elem
