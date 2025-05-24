# Chronovyan Runtime Semantics

## Introduction

This document specifies the runtime behavior of the Chronovyan language, providing a precise definition of how each language construct executes, how state changes propagate, and how temporal operations affect the program. It serves as the authoritative reference for the dynamic semantics of Chronovyan, ensuring consistent behavior across all implementations.

The semantics are organized to reflect both the philosophical duality of Chronovyan (Order/Conformity and Flux/Rebellion) and the three-phase execution model (ANTECEDENCE, CONCURRENCY, CONSEQUENCE) that defines the language's approach to temporal manipulation.

## Execution Model

### Program Lifecycle

A Chronovyan program executes in a well-defined sequence:

1. **Initialization**
   - Resource allocation (Aethel, Chronon)
   - PARADOX_LEVEL initialization (baseline of 0.1)
   - System stability metrics setup

2. **ANTECEDENCE Phase**
   - All declarations processed sequentially
   - WEAVE_PATTERNs defined but not executed
   - Resources are harvested and initialized
   - Chronon stream allocation
   - No execution of actual operations

3. **CONCURRENCY Phase**
   - Parallel streams execute concurrently
   - WEAVE_PATTERNs applied according to their strategies
   - Temporal operations executed
   - Resource consumption occurs
   - PARADOX_LEVEL updated continuously

4. **CONSEQUENCE Phase**
   - Paradox checks evaluated
   - Error handling through CATCH_TEMPORAL
   - Rewards determined through LOOT_TABLE
   - Timeline auditing via DEJAVU
   - Final system state established

5. **Termination**
   - Resources released or preserved
   - PARADOX_LEVEL final assessment
   - Program output finalized

### Execution Context

Each operation in Chronovyan executes within a specific context that contains:

- **Timeline**: The current execution path
- **Temporal Position**: The current point in the timeline
- **Resource State**: Available Aethel and Chronon
- **PARADOX_LEVEL**: Current system instability
- **Variable Environment**: All accessible variables
- **Anchor Points**: Available REWIND_TO destinations

Contexts can be nested (e.g., within sandboxes) with defined inheritance rules.

## Variable Semantics

### Type System

#### Variable Types

1. **CONF (Conformist)**
   - State changes follow strict rules
   - Changes are synchronized across the timeline
   - Cannot exist in superposition
   - Modifications tracked and validated against paradox conditions
   - Contributes to system stability

2. **REB (Rebellious)**
   - Can exist in multiple states simultaneously
   - Changes can propagate non-linearly
   - Can be affected by temporal operations
   - Less predictable behavior
   - Higher PARADOX_LEVEL contribution

3. **ADAPTIVE**
   - Can shift between CONF and REB behavior based on context
   - Behavior determined by PARADOX_LEVEL and system conditions
   - "Middle path" variables that balance stability and flexibility

#### Variable Flags

1. **::STATIC**
   - Value cannot change after initialization
   - Reduces PARADOX_LEVEL by 0.01 per variable
   - Exempt from temporal fluctuations
   - Behavior: Any attempt to modify raises `StaticModificationError`

2. **::FLUX**
   - Value can change unpredictably
   - Increases PARADOX_LEVEL by 0.02 per variable
   - Subject to quantum effects
   - Behavior: Value can spontaneously change within defined bounds when PARADOX_LEVEL > 0.5

3. **::ANCHOR**
   - Serves as a stability point for timelines
   - Can be targeted by REWIND_TO operations
   - Creates a checkpoint in the execution history
   - Behavior: When referenced by REWIND_TO, execution state reverts to the point of anchor declaration

4. **::WEAVER**
   - Can manipulate other variables' temporal properties
   - Influences timeline branching and merging
   - Higher Aethel consumption
   - Behavior: Can be used in operations that modify timeline structure

5. **::ECHO**
   - Maintains history of all past values
   - Accessible via temporal access operations
   - Higher memory consumption
   - Behavior: All past states are preserved and can be retrieved with TEMPORAL_ACCESS

6. **::SOURCE_INFUSED**
   - Connected to the fundamental power of The Source
   - Can modify system-level constraints
   - Extremely high Aethel consumption
   - Behavior: Can override normal paradox constraints with severe PARADOX_LEVEL consequences

7. **::VOLATILE**
   - Rapid state changes
   - No guaranteed persistence between operations
   - Unpredictable behavior at high PARADOX_LEVEL
   - Behavior: Value may not persist between operations if PARADOX_LEVEL > 0.7

### Variable Interactions

1. **CONF-CONF Interaction**
   - Predictable, stable outcomes
   - No PARADOX_LEVEL increase
   - Linear execution flow
   - Operations are atomic and consistent

2. **REB-REB Interaction**
   - Potential for superposition of outcomes
   - PARADOX_LEVEL increase of 0.02-0.05 per interaction
   - Non-linear effects possible
   - Operations may produce multiple potential outcomes

3. **CONF-REB Interaction**
   - CONF attempts to stabilize REB
   - REB may corrupt CONF at high PARADOX_LEVEL
   - PARADOX_LEVEL increase of 0.01-0.03
   - Outcome depends on PARADOX_LEVEL and variable flags

4. **ADAPTIVE Interaction Rules**
   - Behaves like CONF when PARADOX_LEVEL < 0.3
   - Exhibits mixed behavior between 0.3-0.7
   - Behaves like REB when PARADOX_LEVEL > 0.7
   - Adapts based on interaction partners

## Resource Semantics

### Chronons

1. **Allocation**
   - Allocated at program initialization
   - Required for all operations
   - Consumed at different rates by different operations
   - Can be harvested from CHRONON_STREAM sources

2. **Consumption Rules**
   - Basic operations: 1 Chronon
   - Control structures: 2 Chronons
   - Temporal operations: 5-20 Chronons depending on complexity
   - WEAVE_PATTERN application: Sum of contained operations + 5

3. **Depletion Effects**
   - At 20%: Operations slow by 50%
   - At 10%: Only CONF operations permitted
   - At 5%: Warning issued, PARADOX_LEVEL +0.1
   - At 0%: Program terminates with ChrononsDepletedError

### Aethel

1. **Generation**
   - CONF operations generate 0.1 Aethel per operation
   - Successful WEAVE_PATTERN execution generates 1-5 Aethel
   - HARVEST operations generate 10-50 Aethel
   - System stability above 0.8 generates 0.5 Aethel per cycle

2. **Consumption**
   - REB operations consume 1-10 Aethel
   - Temporal manipulations (REWIND_FLOW, etc.) consume 10-50 Aethel
   - PARADOX_LEVEL reduction consumes Aethel proportional to reduction amount
   - SOURCE_INFUSED operations consume 50-100 Aethel

3. **Depletion Effects**
   - At 20%: REB operations cost 2x Aethel
   - At 10%: PARADOX_LEVEL +0.2
   - At 5%: Only CONF operations permitted
   - At 0%: AethelDepletionError, forced timeline stabilization

### Resource Interaction

1. **Chronon-Aethel Exchange**
   - Chronons can be converted to Aethel at 1:0.1 ratio
   - Aethel can be converted to Chronons at 10:1 ratio
   - Conversion increases PARADOX_LEVEL by 0.05
   - Conversion requires explicit operation

## Temporal Operations

### Loop Semantics

1. **FOR_CHRONON**
   - Conformist, predictable iteration
   - Consumes 1 Chronon per iteration
   - PARADOX_LEVEL impact: +0.01 per 10 iterations
   - Exit conditions evaluated at start of each iteration
   - Runtime behavior: Standard iterator pattern with consistent progression

2. **WHILE_EVENT**
   - Conformist conditional iteration
   - Consumes 2 Chronons per iteration
   - PARADOX_LEVEL impact: +0.01 per 5 iterations
   - Condition evaluated at start of each iteration
   - Runtime behavior: Continues until condition is false or resources depleted

3. **REWIND_FLOW**
   - Rebel operation that resets execution to a previous point
   - Consumes 10 Aethel per rewind
   - PARADOX_LEVEL impact: +0.05 per rewind
   - Condition evaluated after each iteration
   - Runtime behavior: When condition is true, execution jumps back to start of loop body
   - State preservation: Variables marked ::ECHO retain values, others reset
   - Limit: Maximum of 3 rewinds before PARADOX_LEVEL +0.1 per additional rewind

4. **TEMPORAL_ECHO_LOOP**
   - Rebel operation that creates multiple parallel timelines
   - Consumes 20 Aethel per iteration
   - PARADOX_LEVEL impact: +0.07 per iteration
   - Runtime behavior: Each iteration executes in parallel timeline
   - Results from all timelines are merged at completion
   - Conflict resolution: Last-write-wins for CONF, superposition for REB
   - Limit: Maximum parallel timelines = min(10, available Aethel / 20)

5. **CHRONO_DILATE_LOOP**
   - Rebel operation that expands temporal perception within loop
   - Consumes 15 Aethel per iteration
   - PARADOX_LEVEL impact: +0.06 per iteration
   - Runtime behavior: Operations within loop execute at "higher resolution"
   - Effect: More precise temporal operations, access to sub-Chronon events
   - Limit: Dilation factor limited by available Aethel

### Timeline Operations

1. **REWIND_TO**
   - Jumps execution to a previously established anchor point
   - Consumes 30 Aethel
   - PARADOX_LEVEL impact: +0.1
   - Runtime behavior: Execution state reverts to anchor point
   - State effects: CONF::STATIC variables unchanged, others reset to anchor state
   - Restrictions: Cannot rewind past ANTECEDENCE phase

2. **BRANCH_TIMELINE**
   - Creates a new execution path from current point
   - Consumes 40 Aethel
   - PARADOX_LEVEL impact: +0.15
   - Runtime behavior: Creates separate execution context
   - Resource division: Resources split between original and branch
   - Completion: Branches must be explicitly merged or terminated

3. **MERGE_TIMELINE**
   - Combines branched timelines back into single execution path
   - Consumes 40 Aethel
   - PARADOX_LEVEL impact: +0.15
   - Runtime behavior: Consolidates execution contexts
   - State reconciliation: CONF variables must match or ParadoxError occurs
   - REB variables may exist in superposition post-merge

4. **TEMPORAL_TRANSFORM**
   - Modifies a variable through temporal manipulation
   - Consumes 20 Aethel
   - PARADOX_LEVEL impact: +0.08
   - Runtime behavior: Variable state changes based on transform function
   - Historical preservation: Previous states remain accessible if ::ECHO flag set
   - Paradox check: Automatic check for timeline consistency

### WEAVE_PATTERN Semantics

1. **Pattern Definition**
   - Purely declarative, no execution
   - Consumes 1 Chronon per statement in pattern
   - Stores operation sequence for later application
   - Validates structure for consistency
   - PARADOX_LEVEL assessment for future execution

2. **SEQUENTIAL Strategy**
   - Executes operations in strict order
   - Consumes Chronons sequentially
   - PARADOX_LEVEL impact: Sum of individual operations
   - Runtime behavior: Each operation completes before next begins
   - Failure handling: Stops at first failed operation

3. **PARALLEL Strategy**
   - Executes operations concurrently
   - Consumes Chronons simultaneously
   - PARADOX_LEVEL impact: max(individual operations) + 0.05
   - Runtime behavior: All operations execute in parallel
   - Failure handling: Independent operation results
   - Conflict resolution required at completion

4. **CONDITIONAL Strategy**
   - Selects operations based on conditions
   - Consumes 2 Chronons for evaluation plus operation costs
   - PARADOX_LEVEL impact: Weighted average based on condition probability
   - Runtime behavior: Evaluates all conditions, executes matching operations
   - Default behavior: If no conditions match, NoConditionMetError unless default provided

5. **Sandbox Execution**
   - All WEAVE_PATTERNs execute in isolated context
   - Resource limits based on Instability_Index
   - PARADOX_LEVEL contained within sandbox
   - Escalation rules for unhandled paradoxes
   - Context inheritance follows sandbox level

## Paradox Management

### PARADOX_LEVEL Dynamics

1. **Initial Value**
   - Base value: 0.1
   - Adjustment for streams: +0.05 per stream
   - Adjustment for WEAVE_PATTERNs: Based on Instability_Index
   - Adjustment for resources: +0.1 if low Aethel

2. **Update Mechanisms**
   - Temporal operations: Direct increases per operation
   - Control structures: Accumulated over iterations
   - Resource consumption: Increases on depletion thresholds
   - Timeline manipulation: Significant increases

3. **Reduction Mechanisms**
   - Natural decay: -0.01 per 5 stable operations
   - INFUSE operation: Reduces by Aethel_Spent * 0.001
   - stabilize_timeline: Targeted reduction consuming Aethel
   - CONF operations: Minimal reductions over time

4. **Threshold Effects**
   - 0.0-0.3: Normal operation
   - 0.31-0.5: Warning level, increased Aethel costs
   - 0.51-0.79: Danger level, restricted operations
   - 0.8+: Critical level, ParadoxOverflowError

### Paradox Detection

1. **Detection Mechanisms**
   - Dependency tracking through variable access
   - Resource consumption monitoring
   - Timeline consistency validation
   - State transition verification

2. **Conflict Types**
   - Resource conflicts: Same resource used inconsistently
   - State conflicts: Incompatible variable states
   - Timeline conflicts: Causality violations
   - Anchor conflicts: Invalid REWIND_TO target

3. **Detection Timing**
   - Pre-execution: validate_pattern simulation
   - During execution: Continuous monitoring
   - Post-operation: State consistency check
   - On timeline merge: State reconciliation

4. **paradox_check Operation**
   - Explicitly validates dependencies
   - Returns boolean indicating safety
   - Consumes 5 Chronons
   - Does not itself affect PARADOX_LEVEL
   - Runtime behavior: Examines dependency graph for conflicts

### Error Handling

1. **CATCH_TEMPORAL**
   - Intercepts specific temporal errors
   - Consumes 10 Chronons on setup
   - No direct PARADOX_LEVEL impact
   - Runtime behavior: Executes handler block when matching error occurs
   - Error propagation: Unhandled errors propagate to parent context

2. **Error Types**
   - ParadoxOverflowError: PARADOX_LEVEL exceeds 0.8
   - AethelDepletionError: Aethel reserve exhausted
   - TimelineCorruptionAlert: Timeline consistency violated
   - NoConditionMetError: No matching condition in CONDITIONAL
   - TemporalInstabilityError: General stability failure

3. **Recovery Actions**
   - REWIND_TO: Return to safe state
   - INFUSE: Stabilize affected components
   - stabilize_timeline: Reduce PARADOX_LEVEL
   - Resource reallocation: Manage Aethel/Chronon distribution

4. **Unrecoverable States**
   - Simultaneous ParadoxOverflowError and AethelDepletionError
   - Recursive TimelineCorruptionAlert
   - Invalid state after multiple REWIND_TO attempts
   - Corrupted ANCHOR points

## Special Operations

### LOOT System

1. **LOOT_TABLE Resolution**
   - Probabilistic reward determination
   - Consumes 5 Chronons
   - No direct PARADOX_LEVEL impact
   - Runtime behavior: Random roll against probability table
   - Result: CHRONON_STREAM or special item

2. **LOOT Utilization**
   - Can be used in INFUSE operations
   - Special properties based on LOOT type
   - Consumption rules vary by item
   - Some LOOT reduces PARADOX_LEVEL when used
   - Rare LOOT may enable prohibited operations

### DEJAVU Operation

1. **Execution Semantics**
   - Analyzes execution history of a WEAVE_PATTERN
   - Consumes 15 Chronons
   - PARADOX_LEVEL impact: +0.02
   - Runtime behavior: Records analysis to specified identifier
   - Optional notification with provided message
   - No direct modification of execution flow

2. **Analysis Results**
   - Execution efficiency
   - Resource consumption patterns
   - PARADOX_LEVEL contribution
   - Potential optimization strategies
   - Conflict detection

## Implementation Constraints

1. **Resource Management**
   - Implementations must track Chronon and Aethel precisely
   - Resource exhaustion must trigger appropriate errors
   - Resource transfers must maintain conservation

2. **Paradox Handling**
   - PARADOX_LEVEL calculation must follow specified formulas
   - Paradox detection must identify all specified conflict types
   - Error propagation must respect context boundaries

3. **Timeline Management**
   - Context isolation must be maintained for sandboxes
   - State preservation and reversion must follow variable flag rules
   - Timeline operations must enforce causality constraints

4. **Optimization Constraints**
   - Implementations may optimize execution if semantics preserved
   - Apparent execution order must match specification
   - Resource consumption may be optimized but not eliminated
   - PARADOX_LEVEL effects must be accurately simulated

This document provides the authoritative specification for how Chronovyan constructs behave at runtime. Implementations must adhere to these semantics to ensure consistent behavior across different environments and platforms.

## Examples

The following examples demonstrate the runtime semantics of key Chronovyan constructs:

### Variable Behavior Example

```chronovyan
DECLARE CONF::STATIC anchor_value : INT = 42;
DECLARE REB::FLUX quantum_value : INT = 10;

// The following would fail at runtime with StaticModificationError
// anchor_value = 43;

// This is valid and consumes 1 Chronon
quantum_value = 20;

// This pattern has a PARADOX_LEVEL impact of +0.02 due to REB variable modification
DECLARE CONF::STATIC test_pattern : WEAVE_PATTERN = define_pattern("QuantumTest", [
    quantum_value = quantum_value * 2
]);

// This would execute the pattern, consuming Chronons and Aethel
// The quantum_value would become 40
Success = apply_pattern(test_pattern, standard_context);
```

### Loop Behavior Example

```chronovyan
// Conformist loop - predictable execution
// Consumes 10 Chronons (1 per iteration)
// PARADOX_LEVEL impact: +0.01
FOR_CHRONON (CHRONON c = 0; c < 10; c++) {
    process_data(c);
}

// Rebel loop - can rewind execution
// First iteration consumes 5 Chronons, plus 10 Aethel per rewind
// PARADOX_LEVEL impact: +0.05 per rewind
REWIND_FLOW (validation_failed) {
    attempt_process();
    check_validation();
}
```

### Error Handling Example

```chronovyan
// Paradox check consumes 5 Chronons
// Returns false if dependencies have conflicts
?! paradox_check([stability_field, quantum_state, energy_reserve]);

// Error handler consumes 10 Chronons on setup
// Executes only if ParadoxOverflowError occurs
CATCH_TEMPORAL ParadoxOverflowError {
    // REWIND_TO consumes 30 Aethel
    // PARADOX_LEVEL impact: +0.1
    // Execution state reverts to Origin anchor point
    REWIND_TO @Origin;
    
    // INFUSE consumes variable Aethel based on parameters
    // Reduces PARADOX_LEVEL by amount * 0.001
    INFUSE(stability_field, LOOT("Chronovyan_Tapestry"), "Restabilize");
}
```

These examples illustrate how Chronovyan's constructs behave at runtime, including resource consumption, PARADOX_LEVEL impact, and execution semantics. 