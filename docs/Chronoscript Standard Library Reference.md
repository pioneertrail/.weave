# Chronoscript Standard Library Reference

## Introduction
Greetings, Weaver, to the **Chronoscript Standard Library**, your arsenal for bending time on Chronos Prime. These functions empower Seekers to explore rifts, Anchors to stabilize timelines, and Rebels to defy the Void. This reference details key functions for temporal manipulation, resource management, and pattern composition, with examples to spark your weaves. Dive in, but beware: every call risks a paradox.

**Purpose**: The Standard Library provides reusable tools to craft `WEAVE_PATTERN`s, manage `Aethel` and `Chronon`, and navigate the `PARADOX_LEVEL`. Designed for RPG enthusiasts, it blends code with Chronovyan lore, making your programs feel like epic quests.

**Lore Tidbit**: The library’s origins trace to the Codex Temporis, a mythic tome etched in `Aethel` by the First Anchor. Its functions are sacred incantations, guarded by the Weaver Caste.

## Function Categories
The library is organized into five categories, each serving a distinct role in temporal weaving.

### 1. Temporal Manipulation
Control the flow of time with precision.

- **`rewind_to(anchor: TIMESTAMP): BOOLEAN`**  
  Rewinds the timeline to a specified anchor.  
  - **Parameters**: `anchor` (e.g., `@Fork`).  
  - **Returns**: `true` if successful, `false` if anchor is invalid.  
  - **Aethel Cost**: 50.  
  - **Instability**: +0.05 to `PARADOX_LEVEL`.  
  - **Example**:  
    ```chronoscript
    Success = rewind_to(@Origin);
    ```

- **`fast_forward(ticks: INT): BOOLEAN`**  
  Advances the timeline by a number of ticks.  
  - **Parameters**: `ticks` (positive integer).  
  - **Returns**: `true` if successful, `false` if beyond `Temporal_Scope`.  
  - **Aethel Cost**: 30.  
  - **Instability**: +0.03 to `PARADOX_LEVEL`.  
  - **Example**:  
    ```chronoscript
    Success = fast_forward(10);
    ```

**Lore Tidbit**: `rewind_to` is whispered to echo the First Seeker’s gambit, unraveling a doomed thread to save Chronos Prime.

### 2. Aethel and Chronon Management
Harness the lifeblood of temporal power.

- **`initiate_harvest(source: CHRONON_STREAM): AETHEL_RESERVE`**  
  Harvests `Aethel` from a `Chronon` source.  
  - **Parameters**: `source` (e.g., `Time_Crystals`).  
  - **Returns**: `AETHEL_RESERVE` (e.g., 100).  
  - **Instability**: +0.1 if source is unstable (e.g., “Temporal_Rift”).  
  - **Example**:  
    ```chronoscript
    DECLARE REB::FLUX Energy : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    ```

- **`infuse_pattern(target: WEAVE_PATTERN, source: CHRONON_STREAM, intent: STRING): WEAVE_PATTERN`**  
  Enhances a pattern with `Chronon`, adjusting its metadata.  
  - **Parameters**: `target` (pattern), `source` (e.g., `LOOT`), `intent` (e.g., “BoostStability”).  
  - **Returns**: New `WEAVE_PATTERN` with reduced `Instability_Index` or `Aethel_Cost`.  
  - **Aethel Cost**: 20.  
  - **Example**:  
    ```chronoscript
    Enhanced = infuse_pattern(Stabilization, LOOT("Aethel_Shard"), "BoostStability");
    ```

**Lore Tidbit**: `infuse_pattern` channels the Weaver’s Oath, binding raw `Chronon` to stabilize fragile threads.

### 3. WEAVE_PATTERN Management
Craft and manipulate temporal blueprints.

- **`define_pattern(name: STRING, operations: STMT[]): WEAVE_PATTERN`**  
  Creates a `WEAVE_PATTERN` with specified operations.  
  - **Parameters**: `name` (unique identifier), `operations` (e.g., `INFUSE`, `GUARD`).  
  - **Returns**: Immutable `WEAVE_PATTERN`.  
  - **Aethel Cost**: 10 per operation.  
  - **Example**:  
    ```chronoscript
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    ```

- **`compose_pattern(name: STRING, patterns: WEAVE_PATTERN[], strategy: STRING, [conditions: CONDITION[]]): WEAVE_PATTERN`**  
  Combines patterns using `SEQUENTIAL`, `PARALLEL`, or `CONDITIONAL` strategies.  
  - **Parameters**: `name`, `patterns` (array), `strategy`, optional `conditions` for `CONDITIONAL`.  
  - **Returns**: Composite `WEAVE_PATTERN`.  
  - **Instability**: Varies by strategy (e.g., +0.02 for nested).  
  - **Example**:  
    ```chronoscript
    DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = compose_pattern("Master", [Stabilization, Exploration], "PARALLEL");
    ```

- **`apply_pattern(pattern: WEAVE_PATTERN, context: WEAVE_PATTERN): BOOLEAN`**  
  Executes a pattern in a given context.  
  - **Parameters**: `pattern`, `context` (e.g., `System_Stabilize`).  
  - **Returns**: `true` if successful, `false` if paradox occurs.  
  - **Instability**: Adds pattern’s `Instability_Index`.  
  - **Example**:  
    ```chronoscript
    Success = apply_pattern(Master_Weave, System_Stabilize);
    ```

- **`validate_pattern(pattern: WEAVE_PATTERN): PARADOX_LEVEL`**  
  Simulates pattern execution to assess risk.  
  - **Parameters**: `pattern`.  
  - **Returns**: Estimated `PARADOX_LEVEL` (0.0–1.0).  
  - **Aethel Cost**: 5.  
  - **Example**:  
    ```chronoscript
    Risk = validate_pattern(Stabilization);
    ```

**Lore Tidbit**: `define_pattern` is a sacred rite, akin to inscribing a time-scroll in the Halls of Eternity.

### 4. Paradox Management
Safeguard timelines from unraveling.

- **`paradox_check(dependencies: IDENTIFIER[]): BOOLEAN`**  
  Checks for conflicts among dependencies.  
  - **Parameters**: `dependencies` (e.g., `[Stability_Field, Time_Crystals]`).  
  - **Returns**: `true` if safe, `false` if conflict detected.  
  - **Instability**: +0.01–0.2 based on conflict type.  
  - **Example**:  
    ```chronoscript
    ?! paradox_check([Stability_Field, Scout, Energy_Harvest]);
    ```

- **`stabilize_timeline(reserve: AETHEL_RESERVE, amount: INT): FLOAT`**  
  Reduces `PARADOX_LEVEL` using `Aethel`.  
  - **Parameters**: `reserve` (source), `amount` (e.g., 100).  
  - **Returns**: New `PARADOX_LEVEL`.  
  - **Aethel Cost**: `amount`.  
  - **Effect**: Reduces `PARADOX_LEVEL` by `amount * 0.001`.  
  - **Example**:  
    ```chronoscript
    New_Risk = stabilize_timeline(Energy, 100); // Reduces by 0.1
    ```

**Lore Tidbit**: `stabilize_timeline` invokes the Anchor’s Vigil, a ritual to mend fraying threads with `Aethel`’s light.

### 5. Exploration and Loot
Probe timelines and claim rewards.

- **`deepen_exploration(target: STRING): TEMPORAL_SIGNATURE`**  
  Probes a timeline or rift for insights.  
  - **Parameters**: `target` (e.g., “Temporal_Rift”).  
  - **Returns**: `TEMPORAL_SIGNATURE` for further weaves.  
  - **Aethel Cost**: 40.  
  - **Instability**: +0.1 for unstable targets.  
  - **Example**:  
    ```chronoscript
    DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift");
    ```

- **`loot_drop(table: LOOT_TABLE): CHRONON_STREAM`**  
  Resolves a `LOOT_TABLE` to yield rewards.  
  - **Parameters**: `table` (e.g., `{ 0.95: LOOT("Legendary_Thread") }`).  
  - **Returns**: `CHRONON_STREAM` or `LOOT`.  
  - **Aethel Cost**: 10.  
  - **Example**:  
    ```chronoscript
    Reward = loot_drop({ 0.95: LOOT("Legendary_Thread") });
    ```

**Lore Tidbit**: `loot_drop` mirrors the Seeker’s Hunt, where bold weaves yield treasures from the Void’s edge.

## Example Program
A full weave using the library, blending strategies and paradox handling:
```chronoscript
ANTECEDENCE {
    DECLARE REB::SOURCE_INFUSED Time_Crystals : CHRONON_STREAM = HARVEST("Prime_Thread");
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift")
    ]);
    DECLARE CONF::STATIC Dual_Timeline : WEAVE_PATTERN = compose_pattern("Dual", [Stabilization, Exploration], "PARALLEL");
}

CONCURRENCY {
    DECLARE REB::FLUX Energy : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    Risk = validate_pattern(Dual_Timeline);
    Success = apply_pattern(Dual_Timeline, System_Stabilize);
    IF Risk > 0.5 {
        New_Risk = stabilize_timeline(Energy, 100);
    }
}

CONSEQUENCE {
    ?! paradox_check([Stability_Field, Scout, Energy]);
    CATCH_TEMPORAL ParadoxOverflowError {
        REWIND_TO @Origin;
        INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
    }
    DEJAVU(Dual_Timeline, Risk, "Timeline Audit");
    IF Success {
        Reward = loot_drop({ 0.95: LOOT("Legendary_Thread") });
    }
}
```

## Contribute
Fork our repo and weave your own functions! Share paradox-busting algorithms or lore-inspired mechanics. Let’s build the Codex Temporis together.

**Lore Tidbit**: The library grows with each Weaver’s contribution, a living testament to Chronos Prime’s resilience.