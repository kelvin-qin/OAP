#include <arrow/type.h>
#include <gandiva/expression.h>

#include "codegen/arrow_compute/arrow_compute_code_generator.h"
#include "codegen/code_generator.h"
#include "codegen/compute_ext/compute_ext_code_generator.h"
#include "codegen/expr_visitor.h"
#include "codegen/gandiva/gandiva_code_generator.h"

arrow::Status CreateCodeGenerator(
    std::shared_ptr<arrow::Schema> schema_ptr,
    std::vector<std::shared_ptr<gandiva::Expression>> exprs_vector,
    std::vector<std::shared_ptr<arrow::Field>> ret_types,
    std::shared_ptr<CodeGenerator>* out) {
  ExprVisitor nodeVisitor;
  int codegen_type;
  auto status = nodeVisitor.create(exprs_vector, &codegen_type);
  switch (codegen_type) {
    case ARROW_COMPUTE:
      *out = std::make_shared<ArrowComputeCodeGenerator>(schema_ptr, exprs_vector,
                                                         ret_types);
      break;
    case GANDIVA:
      *out = std::make_shared<GandivaCodeGenerator>(schema_ptr, exprs_vector, ret_types);
      break;
    case COMPUTE_EXT:
      *out =
          std::make_shared<ComputeExtCodeGenerator>(schema_ptr, exprs_vector, ret_types);
      break;
    default:
      *out = nullptr;
      status = arrow::Status::TypeError("Unrecognized expression type.");
      break;
  }
  return status;
}
