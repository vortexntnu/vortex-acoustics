#include "arm_math.h"

{ % if namespace % }
namespace {
{ namespace }
}
{
  { % endif % } {
    { input_type }
  } input_data{{ input_dimensions }} = { {% for value in input_data %}{{ value }},{% endfor %} };
  {
    { result_type }
  } reference_data{{ result_dimensions }} = { {% for value in result_data %}{{ value }},{% endfor %} };
  { % if namespace % }
}
{ % endif % }
