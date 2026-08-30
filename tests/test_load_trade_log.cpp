#include "quant_review/load_trade_log.hpp"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

using quant_review::LoadError;
using quant_review::TradeLogRow;
using quant_review::load_trade_log_csv;

namespace {

std::string FixturePath(const char* relative) {
  return std::string(QUANT_REVIEW_DATA_DIR) + "/" + relative;
}

}  // namespace

TEST(LoadTradeLogCsv, LoadB1TwoRowsHasTwoDataRows) {
  // Day19 lock: skip header → vector length == 2 (not 1, not 3).
  std::vector<TradeLogRow> rows;
  LoadError err;
  ASSERT_TRUE(load_trade_log_csv(FixturePath("examples/b1-two-rows.csv"), rows,
                                 err))
      << err.message;
  EXPECT_EQ(rows.size(), 2u);
}

TEST(LoadTradeLogCsv, FirstRowIsT001SecondIsT002) {
  std::vector<TradeLogRow> rows;
  LoadError err;
  ASSERT_TRUE(load_trade_log_csv(FixturePath("examples/b1-two-rows.csv"), rows,
                                 err))
      << err.message;
  ASSERT_EQ(rows.size(), 2u);

  EXPECT_EQ(rows[0].trade_id, "t001");
  ASSERT_TRUE(rows[0].realized_pnl.has_value());
  EXPECT_DOUBLE_EQ(*rows[0].realized_pnl, 6.0);
  EXPECT_FALSE(rows[0].unrealized_pnl.has_value());

  EXPECT_EQ(rows[1].trade_id, "t002");
  EXPECT_FALSE(rows[1].realized_pnl.has_value());
  ASSERT_TRUE(rows[1].unrealized_pnl.has_value());
  EXPECT_DOUBLE_EQ(*rows[1].unrealized_pnl, 10.0);
}

TEST(LoadTradeLogCsv, RejectsMissingFile) {
  std::vector<TradeLogRow> rows;
  LoadError err;
  EXPECT_FALSE(load_trade_log_csv(FixturePath("examples/no-such-file.csv"),
                                  rows, err));
  EXPECT_FALSE(err.message.empty());
}

TEST(LoadTradeLogCsv, HeaderOnlyYieldsEmptyVector) {
  const std::string path = FixturePath("examples/_tmp-header-only.csv");
  {
    std::ofstream out(path);
    ASSERT_TRUE(out);
    out << "trade_id,datetime,symbol,direction,offset,price,volume,"
           "commission_open,commission_close,realized_pnl,unrealized_pnl\n";
  }
  std::vector<TradeLogRow> rows;
  LoadError err;
  ASSERT_TRUE(load_trade_log_csv(path, rows, err)) << err.message;
  EXPECT_TRUE(rows.empty());
  std::remove(path.c_str());
}
