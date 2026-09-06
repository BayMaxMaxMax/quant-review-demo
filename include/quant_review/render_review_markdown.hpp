#pragma once

#include "quant_review/aggregate_trade_log.hpp"

#include <string>

namespace quant_review {

/// Day22: render a fixed Markdown draft from an already-computed summary.
/// Deterministic string template only — **no network, no LLM**.
/// Day22 teaching mock (+2 / +10 / 1/2) must include heading and those figures.
std::string render_review_markdown(const ReviewSummaryReport& summary);

}  // namespace quant_review
