/* Copyright 2012-present Facebook, Inc.
 * Licensed under the Apache License, Version 2.0 */
#pragma once
#include <unordered_map>
#include "watchman_synchronized.h"

struct watchman_clock {
  uint32_t ticks;
  time_t timestamp;
};
typedef struct watchman_clock w_clock_t;

struct w_query_ctx;
struct w_query_since;

struct ClockPosition {
  uint32_t rootNumber{0};
  uint32_t ticks{0};

  ClockPosition() = default;
  ClockPosition(uint32_t rootNumber, uint32_t ticks)
      : rootNumber(rootNumber), ticks(ticks) {}

  w_string toClockString() const;
};

enum w_clockspec_tag {
  w_cs_timestamp,
  w_cs_clock,
  w_cs_named_cursor
};

struct w_clockspec {
  enum w_clockspec_tag tag;
  union {
    time_t timestamp;
    struct {
      uint64_t start_time;
      int pid;
      ClockPosition position;
    } clock;
    struct {
      w_string cursor;
    } named_cursor;
  };

  w_clockspec();
  w_clockspec(const ClockPosition& position);
  ~w_clockspec();

  /** Evaluate the clockspec against the inputs, returning
   * the effective since parameter.
   * If cursorMap is passed in, it MUST be unlocked, as this method
   * will acquire a lock to evaluate a named cursor. */
  struct w_query_since evaluate(
      const ClockPosition& position,
      const uint32_t lastAgeOutTick,
      watchman::Synchronized<std::unordered_map<w_string, uint32_t>>*
          cursorMap = nullptr) const;
};

std::unique_ptr<w_clockspec> w_clockspec_parse(const json_ref& value);
void w_clockspec_init(void);
