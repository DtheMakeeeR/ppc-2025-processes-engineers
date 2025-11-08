#include "golovanov_d_matrix_max_elem//seq/include/ops_seq.hpp"

#include <float.h>

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
  size_t columns = std::get<0>(GetInput());
  size_t strokes = std::get<1>(GetInput());
  return (columns > 0) && (strokes > 0) && (std::get<2>(GetInput()).size() == strokes * columns) && (GetOutput() == 0);
}

bool GolovanovDMatrixMaxElemSEQ::PreProcessingImpl() {
  return true;
}

bool GolovanovDMatrixMaxElemSEQ::RunImpl() {
  std::vector<double> elems = std::get<2>(GetInput());
  double max = elems[0];
  for (size_t i = 1; i < elems.size(); i++) {
    if (elems[i] > max) {
      max = elems[i];
    }
  }
  GetOutput() = max;
  return true;
}

bool GolovanovDMatrixMaxElemSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace golovanov_d_matrix_max_elem
