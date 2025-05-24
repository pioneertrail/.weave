# Chronoscript Language Specification

## Introduction
Welcome, Weaver, to **Chronoscript**, a temporal programming language born on Chronos Prime, where time is both canvas and crucible. Crafted for Seekers, Anchors, and Rebel Weavers, Chronoscript lets you weave timelines, harvest `Aethel`, and defy paradoxes. This spec outlines the syntax, semantics, and runtime mechanics, blending code with the lore of a universe where every tick risks a Great Unraveling. Ready to inscribe your Tapestry of Eternity?

**Purpose**: Chronoscript enables developers to manipulate temporal states, manage resources (`Aethel`, `Chronon`), and compose complex workflows (`WEAVE_PATTERN`) with narrative-driven mechanics inspired by RPGs. It’s ideal for crafting simulations, games, or systems where time is a core mechanic.

**Lore Tidbit**: Legend speaks of the First Weaver, who wove the Prime Thread from raw `Chronon`. Her code still echoes in the `ANTECEDENCE` block, a sacred ritual for all Weavers.

## Syntax (EBNF)
Chronoscript’s syntax is precise, reflecting the discipline of Anchors. Below is a simplified EBNF grammar for core constructs. Full grammar available in the repo.

```
program = "ANTECEDENCE", "{", { antecedent_stmt }, "}",
          "CONCURRENCY", "{", { concurrency_stmt }, "}",
          "CONSEQUENCE", "{", { consequence_stmt }, "}";

antecedent_stmt = declare_stmt | define_pattern_stmt | compose_pattern_stmt;
declare_stmt = "DECLARE", ("CONF" | "REB"), "::", flag, identifier, ":", data_type, ["=", expr], ";";
define_pattern_stmt = "DECLARE", "CONF", "::STATIC", identifier, ":", "WEAVE_PATTERN", "=", "define_pattern", "(", string, ",", "[", stmt, { ",", stmt }, "]", ")", ";";
compose_pattern_stmt = "DECLARE", "CONF", "::STATIC", identifier, ":", "WEAVE_PATTERN", "=", "compose_pattern", "(", string, ",", "[", identifier, { ",", identifier }, "]", ",", strategy, [",", "[", condition, { ",", condition }, "]"], ")", ";";

concurrency_stmt = stream_stmt | apply_pattern_stmt | validate_stmt | paradox_check_stmt;
stream_stmt = "||", identifier, ":", stmt;
apply_pattern_stmt = identifier, "=", "apply_pattern", "(", identifier, ",", identifier, ")", ";";
validate_stmt = identifier, "=", "validate_pattern", "(", identifier, ")", ";";
paradox_check_stmt = "?!", "paradox_check", "(", "[", identifier, { ",", identifier }, "]", ")", ";";

consequence_stmt = catch_temporal_stmt | dejavu_stmt | loot_table_stmt | if_stmt;
catch_temporal_stmt = "CATCH_TEMPORAL", error_type, "{", { stmt }, "}";
dejavu_stmt = "DEJAVU", "(", identifier, ",", identifier, ",", string, ")", ";";
loot_table_stmt = "LOOT_TABLE", "{", number, ":", "LOOT", "(", string, ")", ";", "}";

data_type = "TIMESTAMP" | "CHRONON_STREAM" | "AETHEL_RESERVE" | "TEMPORAL_SIGNATURE" | "WEAVE_PATTERN" | "CONF_ARRAY" | "REB_STREAM";
flag = "STATIC" | "FLUX" | "ANCHOR" | "SOURCE_INFUSED";
strategy = "SEQUENTIAL" | "PARALLEL" | "CONDITIONAL";
condition = temporal_metric | resource_threshold | pattern_outcome | boolean_expr;
error_type = "ParadoxOverflowError" | "AethelDepletionError" | "TimelineCorruptionAlert" | "NoConditionMet";
```

**Copy-Paste Example**:
```chronoscript
ANTECEDENCE {
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
}
```

**Lore Tidbit**: The `?!` operator for `paradox_check` is etched in Weaver glyphs, a warning to tread lightly lest the timeline frays.

## Core Constructs
Chronoscript revolves around `WEAVE_PATTERN`, a structured sequence of temporal operations, and composition strategies to craft complex time-threads.

### WEAVE_PATTERN
A `WEAVE_PATTERN` is an immutable (`CONF::STATIC`) blueprint of statements (e.g., `INFUSE`, `REWIND_TO`, `GUARD`), akin to a spell or function. It encapsulates:
- **Header**: Metadata (`Aethel_Cost`, `Chronon_Usage`, `Instability_Index`, `Temporal_Scope`).
- **Body**: Ordered statements with dependencies and temporal offsets.
- **Signature**: A `TEMPORAL_SIGNATURE` for integrity.

**Example**:
```chronoscript
DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
    DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift")
]);
```

### Composition Strategies
`WEAVE_PATTERN`s are combined via `COMPOSE_PATTERN` with three strategies:
- **SEQUENTIAL**: Executes patterns in order, summing `Instability_Index`.  
- **PARALLEL**: Runs patterns concurrently, using `max(Instability_Index)`.  
- **CONDITIONAL**: Selects patterns based on conditions (e.g., `PARADOX_LEVEL < 0.3`), using weighted `Instability_Index`.

**Copy-Paste Example**:
```chronoscript
DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = COMPOSE_PATTERN("Master", [Stabilization, Exploration], "PARALLEL");
```

**Lore Tidbit**: `PARALLEL` weaves are called “Threadstorms” by Seekers, a daring act that tempts the Void’s wrath.

## Runtime Mechanics
The Chronoscript runtime ensures temporal integrity through paradox detection, `PARADOX_LEVEL` management, and sandboxing.

### PARADOX_LEVEL
A global metric (0.0–1.0) tracking timeline instability:
- **Initialization**: Starts at 0.1, adjusted by streams (+0.05 each) or low `AETHEL_RESERVE` (+0.1).
- **Accumulation**: Adds `Instability_Index` (strategy-specific) plus conflict penalties:
  - Resource Contention: 0.03–0.12.
  - Temporal Interference: 0.07–0.15.
  - State Divergence: 0.1–0.2.
- **Decay**: -0.01 per 5 stable ticks; `INFUSE` reduces by `Aethel_Spent * 0.001`.
- **Thresholds**: 0.8+ triggers `ParadoxOverflowError`.

### Paradox Detection
Detects conflicts via `paradox_check`:
- **Sequential**: Resource/anchor inconsistencies.
- **Parallel**: Concurrent state divergence.
- **Conditional**: Condition-driven conflicts.
- Cross-strategy: Nested conflicts (e.g., `PARALLEL` within `SEQUENTIAL`).

### Sandboxing
Nested patterns run in sandboxes to limit paradox “blast radius”:
- **Low-Risk** (`Instability_Index < 0.3`): Full resources, contained failures.
- **Medium-Risk** (0.3–0.6): 50% resources, escalates if unhandled.
- **High-Risk** (> 0.6): 25% resources, always escalates.

**Copy-Paste Example**:
```chronoscript
CATCH_TEMPORAL ParadoxOverflowError {
    REWIND_TO @Origin;
    INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
}
```

**Lore Tidbit**: Sandboxing is an Anchor’s “Temporal Ward,” a shimmering barrier etched in `Aethel` to shield reality.

## Standard Library Overview
Key functions include:
- `define_pattern(name: STRING, operations: STMT[]): WEAVE_PATTERN`: Creates a pattern.
- `apply_pattern(pattern: WEAVE_PATTERN, context: WEAVE_PATTERN): BOOLEAN`: Executes a pattern.
- `validate_pattern(pattern: WEAVE_PATTERN): PARADOX_LEVEL`: Checks for risks.
- `infuse_pattern(target: WEAVE_PATTERN, source: CHRONON_STREAM, intent: STRING): WEAVE_PATTERN`: Enhances a pattern.
- `stabilize_timeline(reserve: AETHEL_RESERVE, amount: INT): FLOAT`: Reduces `PARADOX_LEVEL`.

**Copy-Paste Example**:
```chronoscript
Risk = validate_pattern(Stabilization);
Success = apply_pattern(Stabilization, System_Stabilize);
```

## Example Program
A full weave combining strategies, paradox handling, and loot:
```chronoscript
ANTECEDENCE {
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift")
    ]);
    DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = COMPOSE_PATTERN("Master", [Stabilization, Exploration], "PARALLEL");
}

CONCURRENCY {
    || Stream_1: DECLARE REB::FLUX Energy_Harvest : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    Risk = validate_pattern(Master_Weave);
    Success = apply_pattern(Master_Weave, System_Stabilize);
}

CONSEQUENCE {
    ?! paradox_check([Stability_Field, Scout, Energy_Harvest]);
    CATCH_TEMPORAL ParadoxOverflowError {
        REWIND_TO @Origin;
        INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
    }
    DEJAVU(Master_Weave, PARADOX_LEVEL, "Master Audit");
    IF Success {
        LOOT_TABLE { 0.95: LOOT("Legendary_Thread"); }
    }
}
```

**Lore Tidbit**: The `DEJAVU` command channels the Weaver’s Sight, replaying a timeline’s echoes to uncover hidden flaws.

## Contribute
Join our repo to weave your own time-threads! Issues, PRs, and lore ideas welcome. Let’s craft the next Chronovyan saga together.