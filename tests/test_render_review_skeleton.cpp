#include "quant_review/render_review_skeleton.hpp"

#include <gtest/gtest.h>

#include <string>

using quant_review::render_review_skeleton;

namespace {

const char* kDay28FixtureReply =
    "realized_total: +2\n"
    "unrealized_hint_total: +10\n"
    "win_rate: 1/2\n"
    "anomaly_alerts:\n"
    "- [consecutive_closed_losses] consecutive closed losses >= 2\n";

}  // namespace

TEST(RenderReviewSkeleton, Day28FixtureSurfacesPlusTwoPlusTenAndOneHalf) {
  const std::string skeleton = render_review_skeleton(kDay28FixtureReply);
  EXPECT_NE(skeleton.find("# 复盘报告骨架"), std::string::npos);
  EXPECT_NE(skeleton.find("## 已平总和"), std::string::npos);
  EXPECT_NE(skeleton.find("+2"), std::string::npos);
  EXPECT_NE(skeleton.find("## 未平浮动"), std::string::npos);
  EXPECT_NE(skeleton.find("+10"), std::string::npos);
  EXPECT_NE(skeleton.find("## 胜率"), std::string::npos);
  EXPECT_NE(skeleton.find("1/2"), std::string::npos);
  EXPECT_NE(skeleton.find("## 异常提示"), std::string::npos);
  EXPECT_NE(skeleton.find("[consecutive_closed_losses]"), std::string::npos);
  // Guardrail: fixture skeleton ≠ calling a model / opening a network
  EXPECT_EQ(skeleton.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(skeleton.find("http"), std::string::npos);
  EXPECT_EQ(skeleton.find("api_key"), std::string::npos);
}

TEST(RenderReviewSkeleton, EmptyFixtureStillEmitsHeadings) {
  const std::string skeleton = render_review_skeleton("");
  EXPECT_NE(skeleton.find("# 复盘报告骨架"), std::string::npos);
  EXPECT_NE(skeleton.find("## 已平总和"), std::string::npos);
  EXPECT_NE(skeleton.find("## 未平浮动"), std::string::npos);
  EXPECT_NE(skeleton.find("## 胜率"), std::string::npos);
  EXPECT_NE(skeleton.find("## 异常提示"), std::string::npos);
  EXPECT_NE(skeleton.find("（无）"), std::string::npos);
  EXPECT_EQ(skeleton.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(skeleton.find("http"), std::string::npos);
  EXPECT_EQ(skeleton.find("api_key"), std::string::npos);
}

TEST(RenderReviewSkeleton, NoneAlertStillShowsMissingTip) {
  const std::string fixture =
      "realized_total: +2\n"
      "unrealized_hint_total: +10\n"
      "win_rate: 1/2\n"
      "anomaly_alerts:\n"
      "- (none)\n";
  const std::string skeleton = render_review_skeleton(fixture);
  EXPECT_NE(skeleton.find("+2"), std::string::npos);
  EXPECT_NE(skeleton.find("（无）"), std::string::npos);
  EXPECT_EQ(skeleton.find("- (none)"), std::string::npos);
}
