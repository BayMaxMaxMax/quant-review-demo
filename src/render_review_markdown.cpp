#include "quant_review/render_review_markdown.hpp"

#include <cmath>
#include <sstream>
#include <string>

namespace quant_review {

namespace {

std::string format_signed_number(double value) {
  std::ostringstream out;
  if (value > 0.0) {
    out << '+';
  }
  if (std::floor(value) == value && std::ceil(value) == value) {
    out << static_cast<long long>(value);
  } else {
    out << value;
  }
  return out.str();
}

}  // namespace

std::string render_review_markdown(const ReviewSummaryReport& summary) {
  std::ostringstream md;
  md << "# 复盘草稿\n"
     << "\n"
     << "- 已平总和：" << format_signed_number(summary.realized_total) << "\n"
     << "- 未平浮动：" << format_signed_number(summary.unrealized_hint_total)
     << "\n"
     << "- 胜率：" << summary.wins << '/' << summary.closed_count << "\n";
  return md.str();
}

}  // namespace quant_review
