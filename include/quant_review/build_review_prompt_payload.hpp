#pragma once

#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/flag_anomaly.hpp"

#include <string>
#include <vector>

namespace quant_review {

/// Day27: build a deterministic prompt payload from already-computed summary
/// (+ optional static flags). Template only — **no network, no LLM, no API key**.
/// Does **not** recompute flags or include raw trade rows / trade_id.
/// Day27 teaching mock (+2 / +10 / 1/2) must appear as values in the payload.
std::string build_review_prompt_payload(const ReviewSummaryReport& summary);

std::string build_review_prompt_payload(const ReviewSummaryReport& summary,
                                        const std::vector<AnomalyFlag>& flags);

}  // namespace quant_review
