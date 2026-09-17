#include "quant_review/flag_anomaly.hpp"
#include "quant_review/render_review_json.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using quant_review::AnomalyFlag;
using quant_review::ReviewSummaryReport;
using quant_review::render_review_json;

TEST(RenderReviewJson, Day23MockContainsPlusTwoPlusTenAndOneHalf) {
  // Day23: already-computed Day18-style card → fixed JSON (no LLM)
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  summary.unrealized_hint_total = 10.0;
  summary.wins = 1;
  summary.closed_count = 2;

  const std::string json = render_review_json(summary);
  EXPECT_NE(json.find("\"realized_total\": 2"), std::string::npos);
  EXPECT_NE(json.find("\"unrealized_hint_total\": 10"), std::string::npos);
  EXPECT_NE(json.find("\"wins\": 1"), std::string::npos);
  EXPECT_NE(json.find("\"closed_count\": 2"), std::string::npos);
  // Day26: key present even when no flags were passed
  EXPECT_NE(json.find("\"anomaly_alerts\": []"), std::string::npos);
  // Guardrail: this step is a template dump, not an LLM call site
  EXPECT_EQ(json.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(json.find("http"), std::string::npos);
  // Not a Markdown re-dump
  EXPECT_EQ(json.find("# 复盘草稿"), std::string::npos);
}

TEST(RenderReviewJson, EmptySummaryIsZeroOverZeroJson) {
  const std::string json = render_review_json({});
  EXPECT_NE(json.find("\"realized_total\": 0"), std::string::npos);
  EXPECT_NE(json.find("\"unrealized_hint_total\": 0"), std::string::npos);
  EXPECT_NE(json.find("\"wins\": 0"), std::string::npos);
  EXPECT_NE(json.find("\"closed_count\": 0"), std::string::npos);
  EXPECT_NE(json.find("\"anomaly_alerts\": []"), std::string::npos);
}

TEST(RenderReviewJson, Day26AttachesOneAnomalyAlertWithoutReFlagging) {
  // Day26: hang already-computed flags into JSON — does not re-run Day24 rule.
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  summary.unrealized_hint_total = 10.0;
  summary.wins = 1;
  summary.closed_count = 2;

  const std::vector<AnomalyFlag> flags = {
      {"consecutive_closed_losses", "consecutive closed losses >= 2"},
  };

  const std::string json = render_review_json(summary, flags);
  EXPECT_NE(json.find("\"realized_total\": 2"), std::string::npos);
  EXPECT_NE(json.find("\"anomaly_alerts\": ["), std::string::npos);
  EXPECT_NE(json.find("\"code\": \"consecutive_closed_losses\""),
            std::string::npos);
  EXPECT_NE(json.find("\"message\": \"consecutive closed losses >= 2\""),
            std::string::npos);
  // Still not a gate / LLM step
  EXPECT_EQ(json.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(json.find("gate"), std::string::npos);
}

TEST(RenderReviewJson, Day26EmptyFlagsStillEmitsAnomalyAlertsKey) {
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  const std::string json = render_review_json(summary, {});
  EXPECT_NE(json.find("\"anomaly_alerts\": []"), std::string::npos);
}

TEST(RenderReviewJson, Day26EscapesQuotesInFlagMessage) {
  ReviewSummaryReport summary;
  const std::vector<AnomalyFlag> flags = {
      {"tip", "say \"hello\""},
  };
  const std::string json = render_review_json(summary, flags);
  EXPECT_NE(json.find("\"message\": \"say \\\"hello\\\"\""), std::string::npos);
}
