#include "quant_review/build_review_prompt_payload.hpp"
#include "quant_review/flag_anomaly.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using quant_review::AnomalyFlag;
using quant_review::ReviewSummaryReport;
using quant_review::build_review_prompt_payload;

TEST(BuildReviewPromptPayload, Day27MockContainsPlusTwoPlusTenAndOneHalf) {
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  summary.unrealized_hint_total = 10.0;
  summary.wins = 1;
  summary.closed_count = 2;

  const std::string payload = build_review_prompt_payload(summary);
  EXPECT_NE(payload.find("realized_total: +2"), std::string::npos);
  EXPECT_NE(payload.find("unrealized_hint_total: +10"), std::string::npos);
  EXPECT_NE(payload.find("win_rate: 1/2"), std::string::npos);
  EXPECT_NE(payload.find("anomaly_alerts:"), std::string::npos);
  EXPECT_NE(payload.find("- (none)"), std::string::npos);
  // Guardrail: assembling a draft ≠ calling a model
  EXPECT_EQ(payload.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(payload.find("http"), std::string::npos);
  EXPECT_EQ(payload.find("api_key"), std::string::npos);
  // Desensitization: no raw trade row / trade_id fields
  EXPECT_EQ(payload.find("trade_id"), std::string::npos);
  EXPECT_EQ(payload.find("MOCK_FUT"), std::string::npos);
}

TEST(BuildReviewPromptPayload, Day27IncludesAlreadyComputedFlagWithoutReFlagging) {
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  summary.unrealized_hint_total = 10.0;
  summary.wins = 1;
  summary.closed_count = 2;

  const std::vector<AnomalyFlag> flags = {
      {"consecutive_closed_losses", "consecutive closed losses >= 2"},
  };

  const std::string payload = build_review_prompt_payload(summary, flags);
  EXPECT_NE(payload.find("realized_total: +2"), std::string::npos);
  EXPECT_NE(payload.find("[consecutive_closed_losses]"), std::string::npos);
  EXPECT_NE(payload.find("consecutive closed losses >= 2"), std::string::npos);
  EXPECT_EQ(payload.find("- (none)"), std::string::npos);
  EXPECT_EQ(payload.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(payload.find("http"), std::string::npos);
}

TEST(BuildReviewPromptPayload, EmptySummaryStillEmitsTemplateKeys) {
  const std::string payload = build_review_prompt_payload({});
  EXPECT_NE(payload.find("realized_total: 0"), std::string::npos);
  EXPECT_NE(payload.find("win_rate: 0/0"), std::string::npos);
  EXPECT_NE(payload.find("- (none)"), std::string::npos);
}
