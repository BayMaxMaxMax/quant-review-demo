#pragma once

#include "quant_review/aggregate_trade_log.hpp"

#include <string>

namespace quant_review {

/// Day23: render a fixed intermediate JSON string from an already-computed
/// summary. Deterministic template only — **no network, no LLM**.
/// Day23 teaching mock (+2 / +10 / 1/2) must include those figures as values.
std::string render_review_json(const ReviewSummaryReport& summary);

}  // namespace quant_review
