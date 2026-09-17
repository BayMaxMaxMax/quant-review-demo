#include "quant_review/render_review_json.hpp"

#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

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

std::string escape_json_string(const std::string& value) {
  std::string out;
  out.reserve(value.size() + 8);
  for (const unsigned char ch : value) {
    switch (ch) {
      case '\\':
        out += "\\\\";
        break;
      case '"':
        out += "\\\"";
        break;
      case '\b':
        out += "\\b";
        break;
      case '\f':
        out += "\\f";
        break;
      case '\n':
        out += "\\n";
        break;
      case '\r':
        out += "\\r";
        break;
      case '\t':
        out += "\\t";
        break;
      default:
        if (ch < 0x20) {
          char buf[7];
          std::snprintf(buf, sizeof(buf), "\\u%04x", ch);
          out += buf;
        } else {
          out += static_cast<char>(ch);
        }
        break;
    }
  }
  return out;
}

}  // namespace

std::string render_review_json(const ReviewSummaryReport& summary) {
  return render_review_json(summary, {});
}

std::string render_review_json(const ReviewSummaryReport& summary,
                               const std::vector<AnomalyFlag>& flags) {
  std::ostringstream json;
  json << "{\n"
       << "  \"realized_total\": " << format_json_number(summary.realized_total)
       << ",\n"
       << "  \"unrealized_hint_total\": "
       << format_json_number(summary.unrealized_hint_total) << ",\n"
       << "  \"wins\": " << summary.wins << ",\n"
       << "  \"closed_count\": " << summary.closed_count << ",\n"
       << "  \"anomaly_alerts\": [";

  for (std::size_t i = 0; i < flags.size(); ++i) {
    if (i > 0) {
      json << ",";
    }
    json << "\n    {\n"
         << "      \"code\": \"" << escape_json_string(flags[i].code) << "\",\n"
         << "      \"message\": \"" << escape_json_string(flags[i].message)
         << "\"\n"
         << "    }";
  }
  if (!flags.empty()) {
    json << "\n  ";
  }
  json << "]\n"
       << "}\n";
  return json.str();
}

}  // namespace quant_review
