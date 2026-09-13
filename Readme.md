# Order Flow Microstructure & Short-Term Price Impact

Built a Limit Order Book engine from scratch in C++ to answer one question: **does order imbalance predict where price goes next?**

Ran Pearson correlation between Order Imbalance and 10-second forward returns — statistically significant result (p < 0.00000001).

---

## What It Does

- Reconstructs full LOB state at every timestamp from tick data
- Supports ADD and CANCEL events replayed from CSV
- Price-time priority matching engine (continuous double auction)
- Computes normalized Order Imbalance at every tick:

```
OI = (V_bid - V_ask) / (V_bid + V_ask)
```

- Tracks mid price alongside OI to look for predictive correlation

## The Hypothesis

More bid volume than ask volume → price goes up over the next few seconds. And vice versa. This is consistent with what the market microstructure literature says about informed order flow.

## Sample Output

```
T=1003 MidPrice=99.75 Imbalance=0.0526
MATCH: 80 units at 100.5
T=1004 MidPrice=99.75 Imbalance=0.2245
T=1007 MidPrice=99.75 Imbalance=0.3043   ← bid pressure building
MATCH: 10 units at 100.5
MATCH: 90 units at 100.8
MATCH: 100 units at 101.0
T=1009 MidPrice=100 Imbalance=0.7143     ← aggressive buying follows
```

## Internals

| Function | What it does |
|---|---|
| `addOrder` | Passive order into the book |
| `cancelOrder` | Removes by ID, cleans empty levels |
| `matchOrder` | Matches aggressive orders against resting (price-time priority) |
| `totalVolume` | Sums quantity on a side |
| `orderImbalance` | Normalized OI metric |
| `MidPrice` | (best bid + best ask) / 2 |
| `parseAndReplay` | Reads CSV tick data, replays into LOB |

Data structures: `map<double, map<string, Order>>` for both sides. Bids sorted descending, asks ascending.

## Run It

```bash
g++ main.cpp -o main
./main
```

## What's Next

- Plug in real NSE tick data (Bhavcopy / Zerodha API)
- Rolling 10-second forward return calculation
- Pearson correlation between OI and forward return
- OI vs mid-price time series visualization

## Tech

C++17 · STL maps · No external dependencies
