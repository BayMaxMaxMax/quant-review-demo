# quant-review-demo

> One-sentence scope: **read a mock trade-log CSV → aggregate stats → (later) call DeepSeek to draft a review report → write Markdown.**

This repository is a **90-day build-in-public learning demo** for a C++ backend engineer studying quant *operations* concepts and LLM engineering. It is **not** a trading system.

**Learning path:** topics outside review (broader quant engineering, richer trade models) may appear in private study notes later; **this repo stays review-only** — see [`SCOPE.md`](SCOPE.md). Field names may *align with* fuller trade models as a subset; they do **not** implement orders, live trading, or backtests here.

## What this project does

- Parse **fictional** mock CSV logs
- Compute deterministic review statistics (hourly PnL, instrument win-rate style summaries, static anomaly flags)
- Generate a structured review report (planned; DeepSeek chat API only)

## What this project does NOT do

- No live market data
- No order placement, signals, or strategy backtest
- No real accounts, keys for brokers, or production deployment guides
- No profit claims or investment advice

## Tech stack (planned)

- C++17 + CMake 3.16+
- cpp-httplib + OpenSSL (HTTPS)
- nlohmann/json
- Google Test
- DeepSeek API (`chat/completions`) for report wording only

## Build / run

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Library: `quant_review`
- `parse_trade_log_row` — map one CSV data line into `TradeLogRow` (`std::optional` for empty PnL cells; **空 ≠ 0**)
- `validate_direction_and_offset` — enums `long|short` / `open|close`; realized bucket requires `close`, unrealized requires `open` (reject mismatches before aggregate)
- `sum_realized_pnl` — sum `realized_pnl` only where present; unrealized rows do not enter this total


## Mock data

See [`data/README.md`](data/README.md) — **B1 mock CSV schema is locked** (single-table review journal · Scheme A · industry-aligned names).  
Fixture: [`data/examples/b1-two-rows.csv`](data/examples/b1-two-rows.csv).  
All samples are **fictional** and unrelated to any real account, instrument, or market.

## Compliance

This project is for **software-engineering learning only**. It does **not** constitute any trading or investment advice. Inputs are handwritten / generated mock data.

## 90-day learning log

Index of weekly public notes will be linked here as the Xiaohongshu series starts (commit SHA ↔ note).

## License

MIT — see [LICENSE](LICENSE).
