#include "quant_review/render_review_markdown.hpp"

#include <gtest/gtest.h>

#include <string>

using quant_review::ReviewSummaryReport;
using quant_review::render_review_markdown;

TEST(RenderReviewMarkdown, Day22MockContainsPlusTwoPlusTenAndOneHalf) {
  // Day22: already-computed Day18-style card → fixed Markdown draft (no LLM)
  ReviewSummaryReport summary;
  summary.realized_total = 2.0;
  summary.unrealized_hint_total = 10.0;
  summary.wins = 1;
  summary.closed_count = 2;

  const std::string md = render_review_markdown(summary);
  EXPECT_NE(md.find("# 复盘草稿"), std::string::npos);
  EXPECT_NE(md.find("已平总和：+2"), std::string::npos);
  EXPECT_NE(md.find("未平浮动：+10"), std::string::npos);
  EXPECT_NE(md.find("胜率：1/2"), std::string::npos);
  // Guardrail: this step is a template dump, not an LLM call site
  EXPECT_EQ(md.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(md.find("http"), std::string::npos);
}

TEST(RenderReviewMarkdown, EmptySummaryIsZeroOverZeroDraft) {
  const std::string md = render_review_markdown({});
  EXPECT_NE(md.find("# 复盘草稿"), std::string::npos);
  EXPECT_NE(md.find("已平总和：0"), std::string::npos);
  EXPECT_NE(md.find("未平浮动：0"), std::string::npos);
  EXPECT_NE(md.find("胜率：0/0"), std::string::npos);
}
