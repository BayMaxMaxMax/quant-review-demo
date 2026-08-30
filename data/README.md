# Mock data

All files under `data/` are **fictional**, for software-engineering learning only.

They are unrelated to any real account, instrument, or market.

## B1 schema (**locked** · review journal · single table)

> **Locked for theme-map B1** (aligned names: `docs/data: align schema names to industry`).  
> This file is a **review journal projection** (one teaching table for post-hoc review).  
> It is **not** a full OMS model and **not** a claim that brokers export a single file this way.  
> **W3 first:** keep one table. A later split into e.g. `trades` + `positions` may land in W4+ without renaming PnL semantics.  
> Parser / tests (**B2+**) must follow this file; schema changes need a new `docs/data:` commit.

### Design rules

1. **Industry-oriented names** where they match common trade/position vocabulary (vn.py-style `direction` / `offset` / `symbol` / `datetime`; PnL as `realized_pnl` / `unrealized_pnl`).  
2. **Scheme A (no bucket/status column):** which PnL column is set encodes the review bucket — do **not** use `status` (collides with order lifecycle status in OMS models).  
3. **Empty means absent** (not zero). Generators must omit the unused PnL cell; never use `0` to mean “no value”.  
4. **Subset only:** no order book, gateway, yd/td split, account, ticks, or signals.

### Row meaning

One row = one **review journal line** for teaching:

- **Realized line:** a closed-round teaching result → set `realized_pnl` only.  
- **Unrealized line:** an open-position mark → set `unrealized_pnl` only.

Partially filled multi-trade orders are **out of scope** for W2; keep one logical line per row.

### Columns

| Column | Type (logical) | Required | Meaning (review-only) |
|---|---|---|---|
| `trade_id` | string | yes | Stable unique id for the row (derived key OK) |
| `datetime` | ISO-8601 UTC+8 or naive local | yes | Event time for the row |
| `symbol` | string | yes | Mock symbol only (e.g. `MOCK_FUT`) |
| `direction` | enum | yes | `long` \| `short` |
| `offset` | enum | yes | `open` \| `close` |
| `price` | number | yes | Fill price; for unrealized mark rows use mark price |
| `volume` | number | yes | Size (mock units) |
| `commission_open` | number | if known | Fee already incurred on open |
| `commission_close` | number | if closed | Fee on close; **empty/0** on unrealized mark rows |
| `realized_pnl` | number | if realized line | Closed-round PnL **after fees** (teaching); **must be empty** on unrealized lines |
| `unrealized_pnl` | number | if unrealized line | Open mark-to-market hint only; **must be empty** on realized lines |

### Explicitly **not** in this subset

- Order lifecycle / order `status` enums  
- Yesterday vs today position (`yd` / `td`) and `CLOSETODAY`  
- Account balance / frozen / available  
- Tick / depth / live gateway fields  
- Strategy signals, targets, or backtest fills  
- A separate `pnl_bucket` / journal `status` column (Scheme A)

### Aggregation rules (must match public Day5–D8 cards)

1. **Sum `realized_pnl` only** over rows where `realized_pnl` is present.  
2. **Report `unrealized_pnl` separately** where present — never add into realized total as a “settled” number.  
3. Reject rows that set both `realized_pnl` and `unrealized_pnl`.  
4. Reject rows that set neither (every journal line must be exactly one bucket).  
5. **Win-rate draft:** denominator = rows with `realized_pnl` present; wins = among those with `realized_pnl > 0`. Unrealized-only rows do **not** enter. Scratch/`==0` controversy left open (currently in denominator, not a win).

### Header line

```text
trade_id,datetime,symbol,direction,offset,price,volume,commission_open,commission_close,realized_pnl,unrealized_pnl
```

### Example fixture

See [`examples/b1-two-rows.csv`](examples/b1-two-rows.csv).

```text
trade_id,datetime,symbol,direction,offset,price,volume,commission_open,commission_close,realized_pnl,unrealized_pnl
t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,
t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10
```

- Row `t001`: realized teaching net **+6** (aligns Day4).  
- Row `t002`: unrealized **+10** (aligns Day5); do **not** treat +6 and +10 as one settled +16.

### Status

- **Locked** (single-table review journal · Scheme A · industry-aligned names).  
- Supersedes earlier draft columns (`timestamp` / `instrument` / `status` / `realized_net` / `floating_indication`).  
- Aggregate (Day12): `sum_realized_pnl` sums rule 1 only.  
- Report floating (Day13): `sum_unrealized_pnl` / `report_separate_pnl` cover rule 2 (separate bucket).  
- Win rate (Day14–17): `win_rate_closed` covers rule 5 (closed denominator only; scratch `== 0` in denom, not a win; empty / no closed → 0/0, not 0%; all closed losses → 0/n, which may be said as 0%).  
- One-shot summary (Day18): `report_review_summary` returns realized total + unrealized hint + wins/closed together (Day14 mock → +2 / +10 / 1/2); no file I/O, no dirty-row gate.  
- Load whole file (Day19): `load_trade_log_csv` skips the header and returns a `vector` of data rows (`b1-two-rows.csv` → 2 rows); parse failure aborts load; dirty-row / validate gates stay Day20.
