#pragma once

#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/flag_anomaly.hpp"

#include <string>
#include <vector>

namespace quant_review {

/// Day23 / Day26: render a fixed intermediate JSON string from an already-computed
/// summary (and optional static anomaly flags). Deterministic template only —
/// **no network, no LLM**. Does **not** recompute flags; callers pass flags in.
/// Day26: always emits `"anomaly_alerts"` (empty array when none).
std::string render_review_json(const ReviewSummaryReport& summary);

std::string render_review_json(const ReviewSummaryReport& summary,
                               const std::vector<AnomalyFlag>& flags);

}  // namespace quant_review
