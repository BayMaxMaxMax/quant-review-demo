#include "quant_review/build_review_prompt_payload.hpp"

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

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

std::string build_review_prompt_payload(const ReviewSummaryReport& summary) {
  return build_review_prompt_payload(summary, {});
}

std::string build_review_prompt_payload(const ReviewSummaryReport& summary,
                                        const std::vector<AnomalyFlag>& flags) {
  // Deterministic LLM-facing draft only. Aggregates + tips; never raw CSV rows.
  std::ostringstream payload;
  payload << "Review summary for wording only.\n"
          << "Use aggregates below; do not invent trades.\n"
          << "\n"
          << "realized_total: " << format_signed_number(summary.realized_total)
          << "\n"
          << "unrealized_hint_total: "
          << format_signed_number(summary.unrealized_hint_total) << "\n"
          << "win_rate: " << summary.wins << '/' << summary.closed_count
          << "\n"
          << "anomaly_alerts:\n";

  if (flags.empty()) {
    payload << "- (none)\n";
  } else {
    for (const AnomalyFlag& flag : flags) {
      payload << "- [" << flag.code << "] " << flag.message << "\n";
    }
  }

  return payload.str();
}

}  // namespace quant_review
