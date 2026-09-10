#include "quant_review/render_review_json.hpp"

#include <cmath>
#include <sstream>
#include <string>

namespace quant_review {

namespace {

std::string format_json_number(double value) {
  std::ostringstream out;
  if (std::floor(value) == value && std::ceil(value) == value) {
    out << static_cast<long long>(value);
  } else {
    out << value;
  }
  return out.str();
}

}  // namespace

std::string render_review_json(const ReviewSummaryReport& summary) {
  std::ostringstream json;
  json << "{\n"
       << "  \"realized_total\": " << format_json_number(summary.realized_total)
       << ",\n"
       << "  \"unrealized_hint_total\": "
       << format_json_number(summary.unrealized_hint_total) << ",\n"
       << "  \"wins\": " << summary.wins << ",\n"
       << "  \"closed_count\": " << summary.closed_count << "\n"
       << "}\n";
  return json.str();
}

}  // namespace quant_review
