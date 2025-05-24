# Chronoscript Runtime Mechanics Guide

## Introduction
Welcome, Weaver, to the heart of Chronoscript’s temporal engine, where timelines bend and paradoxes lurk. On Chronos Prime, the runtime is the Anchor’s crucible, enforcing order amid the chaos of `WEAVE_PATTERN`s. This guide unravels the mechanics of `PARADOX_LEVEL` management, paradox detection, and sandboxing, arming you to craft stable time-threads or risk a Great Unraveling. Packed with examples and lore, it’s your map to mastering Chronoscript’s core.

**Purpose**: This guide equips developers with the runtime’s rules for managing temporal instability, detecting conflicts, and isolating risky weaves. For RPG fans, it’s a saga of balancing power and peril in a universe where time is both ally and foe.

**Lore Tidbit**: The runtime hums with the Pulse of Time, a rhythm Anchors swear to protect. Its code was forged in the aftermath of the First Fracture, when a Seeker’s hubris nearly unwove reality.

## PARADOX_LEVEL Management
The `PARADOX_LEVEL` (0.0–1.0) is the global gauge of timeline instability, the Weaver’s constant shadow.

### Initialization
- **Base Value**: Starts at 0.1, ambient noise of Chronos Prime.  
- **Adjustments**:  
  - +0.05 per `||` stream or `BRANCH`.  
  - +0.1 for low `AETHEL_RESERVE` (< 100).  
  - -0.05 for high `CHRONON_EFFICIENCY` (> 0.9).  
  - +0.02 per nested strategy (e.g., `PARALLEL` in `SEQUENTIAL`).  
- **Example**: 2 streams, low `Aethel`, nested `CONDITIONAL` → `0.1 + 0.05*2 + 0.1 + 0.02 = 0.32`.

### Accumulation
- **Instability_Index**: Added per `WEAVE_PATTERN`:  
  - **Sequential**: `sum(Index_i)`.  
  - **Parallel**: `max(Index_i)`.  
  - **Conditional**: `sum(p_i * Index_i)`, where `p_i` is condition probability.  
- **Operation Modifiers**:  
  - `REWIND_TO`: +0.05.  
  - `FAST_FORWARD`: +0.03.  
  - `BRANCH`: +0.07.  
  - `CYCLE`: +0.01 per iteration.  
- **Conflict Penalties**:  
  - Resource Contention: 0.03 (`SEQUENTIAL`), 0.1 (`PARALLEL`), 0.05 (`CONDITIONAL`), 0.12 (nested).  
  - Temporal Interference: 0.07 (`SEQUENTIAL`), 0.15 (`PARALLEL`), 0.1 (`CONDITIONAL`), 0.1 (nested).  
  - State Divergence: 0.1 (`SEQUENTIAL`), 0.2 (`PARALLEL`), 0.15 (`CONDITIONAL`), 0.18 (nested).  

### Decay and Reduction
- **Natural Decay**: -0.01 per 5 stable ticks (no conflicts).  
- **Active Reduction**:  
  - `INFUSE`: Reduces by `Aethel_Spent * 0.001` (max 0.2).  
  - `stabilize_timeline`: Up to -0.3, costing `Aethel`.  
- **Example**:  
  ```chronoscript
  New_Risk = stabilize_timeline(Energy, 100); // Reduces PARADOX_LEVEL by 0.1
  ```

### Thresholds
- 0.0–0.3: Stable.  
- 0.31–0.5: Caution (`TRACE` logs, +10% `Aethel` cost).  
- 0.51–0.79: High Risk (`NOTIFY_ANCHOR_CASTE`, restrict `BRANCH`).  
- 0.8+: Critical (`ParadoxOverflowError`).  

**Lore Tidbit**: A `PARADOX_LEVEL` above 0.8 is a “Void Whisper,” a dire omen that summons the Anchor Caste to intervene.

## Paradox Detection
The runtime vigilantly detects conflicts to prevent timeline fractures.

### Mechanism
- **Dependency Graph**: Tracks `REB::FLUX` variables, resources, and anchors.  
- **Pre-Execution**: `validate_pattern` simulates execution, flagging risks.  
- **Runtime Monitoring**: Checks resources, anchors, and states via `SNAPSHOT`s.  
- **Synchronization**: Reconciles states at strategy boundaries (e.g., end of `PARALLEL`).  

### Strategy-Specific Conflicts
- **Sequential**:  
  - Resource Dependency Violation: Consumed `LOOT` unavailable later (+0.03).  
  - Anchor Inconsistency: `REWIND_TO` invalid anchor (+0.07).  
  - State Invalidation: `FAST_FORWARD` skips critical setup (+0.1).  
- **Parallel**:  
  - Resource Contention: Concurrent `Aethel` overuse (+0.1).  
  - Temporal Interference: Overlapping `REWIND`/`BRANCH` (+0.15).  
  - State Divergence: Inconsistent `REB::FLUX` states (+0.2).  
- **Conditional**:  
  - Resource Contention: Uncertain resource needs (+0.05).  
  - Temporal Interference: Condition-driven `REWIND` (+0.1).  
  - State Divergence: Retroactive condition invalidation (+0.15).  
- **Cross-Strategy**: Nested conflicts amplify penalties (e.g., +0.12 for nested contention).  

**Copy-Paste Example**:
```chronoscript
?! paradox_check([Stability_Field, Scout, Energy_Harvest]);
```

**Lore Tidbit**: `paradox_check` is the Weaver’s Sight, a mystic scan that peers into the Void’s cracks.

## Sandboxing
Sandboxing isolates nested `WEAVE_PATTERN`s to contain paradoxes, limiting their “blast radius.”

### Containment Levels
- **Low-Risk** (`Instability_Index < 0.3`):  
  - Full `AETHEL_RESERVE` access.  
  - Contained failures (no parent impact).  
  - Example: Simple `Stabilization`.  
- **Medium-Risk** (0.3–0.6):  
  - 50% `AETHEL_RESERVE`, capped `Chronon`.  
  - Escalates if unhandled.  
  - Example: `CONDITIONAL` with risky conditions.  
- **High-Risk** (>0.6):  
  - 25% `AETHEL_RESERVE`, single `Chronon` stream.  
  - Always escalates, halts sandbox.  
  - Example: Nested `PARALLEL` with `BRANCH`.  

### Resource Quota
- Formula: `Quota = Parent_Aethel * (1 - Instability_Index)`.  
- Example: `Dual_Timeline` (Index=0.7) in `Master_Weave` (200 `Aethel`) gets `200 * 0.3 = 60 Aethel`.

### CATCH_TEMPORAL Integration
- **Local Handling**: Sandbox-level `CATCH_TEMPORAL` contains failures.  
- **Escalation**: Unhandled medium/high-risk paradoxes escalate to parent.  
- **Example**:
  ```chronoscript
  CATCH_TEMPORAL TimelineCorruptionAlert {
      REWIND_TO @Origin;
      NOTIFY_ANCHOR_CASTE("Nested fracture contained!");
  }
  ```

**Lore Tidbit**: Sandboxing is a “Temporal Ward,” a shimmering shield woven by Anchors to cage reckless weaves.

## Example Program
A weave showcasing runtime mechanics:
```chronoscript
ANTECEDENCE {
    DECLARE REB::SOURCE_INFUSED Time_Crystals : CHRONON_STREAM = HARVEST("Prime_Thread");
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift"),
        REWIND_TO @Fork
    ]);
    DECLARE CONF::STATIC Dual_Timeline : WEAVE_PATTERN = compose_pattern("Dual", [Stabilization, Exploration], "PARALLEL");
}

CONCURRENCY {
    || Stream_1: DECLARE REB::FLUX Energy : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    Risk = validate_pattern(Dual_Timeline);
    Success = apply_pattern(Dual_Timeline, System_Stabilize);
}

CONSEQUENCE {
    ?! paradox_check([Stability_Field, Scout, Energy]);
    CATCH_TEMPORAL ParadoxOverflowError {
        REWIND_TO @Origin;
        INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
        New_Risk = stabilize_timeline(Energy, 100);
    }
    DEJAVU(Dual_Timeline, Risk, "Runtime Audit");
    IF Success {
        Reward = loot_drop({ 0.95: LOOT("Legendary_Thread") });
    }
}
```

## Contribute
Join our repo to fortify the runtime! Share paradox-handling tricks or lore for the Anchor Caste. Let’s weave a stable Chronos Prime.

**Lore Tidbit**: A Weaver who masters the runtime is said to hear the Void’s heartbeat, a secret known only to the High Anchors.