#include "quant_review/render_review_json.hpp"

#include <gtest/gtest.h>

#include <string>

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
}
