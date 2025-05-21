# Chronovyan: Core Syntax & Constructs (Conceptual Overview)

This preliminary guide outlines the conceptual syntax for declaring variables, defining basic loop structures, and invoking key temporal commands within the Chronovyan programming language.

## 1. Variable Declaration: The Duality of State

Variables in Chronovyan are declared with a primary alignment (`CONF` for Conformist, `REB` for Rebel) and can be further specialized with flags that define their behavior and interaction with time.

**General Syntax:**
`DECLARE [CONF|REB]::FLAG <variable_name> : <DataType> [= <initial_value>];`

* **`CONF` (Conformist):** Variables bound to stability and the established temporal order.
* **`REB` (Rebel):** Variables embracing flux, mutability, and defiance of temporal norms.
* **`::FLAG`:** Optional specifiers like `STATIC`, `ANCHOR`, `FLUX`, `WEAVER`, `GLITCH`, `ECHO`, `SOURCE_INFUSED`, `CHRONOS_BIND`.
* **`<DataType>`:** Can range from standard types (e.g., `INT`, `STRING`, `BOOLEAN`) to Chronovyan-specific types (e.g., `AETHEL_POOL`, `TEMPORAL_SIGNATURE`, `CHRONON_COUNT`).

**Examples:**

```chronovyan
// Conformist Variables
DECLARE CONF::STATIC System_Boot_Time : TIMESTAMP = "EpochZero";
DECLARE CONF::ANCHOR Main_Scheduler_PID : INT;
DECLARE CONF::WEAVER Life_Sustain_Pattern : DATA_STREAM;

// Rebel Variables
DECLARE REB::FLUX Current_Paradox_Index : FLOAT = 0.0;
DECLARE REB::WEAVER Hope_Synthesis_Algorithm : FUNCTION;
DECLARE REB::ECHO Potential_Futures : ARRAY<TEMPORAL_SIGNATURE>;
DECLARE REB::SOURCE_INFUSED Reality_Core_Handle : POINTER; // Rare and powerful

2. Basic Loop Structures: Cycles of Time
Loops provide the foundational structures for repetitive operations, reflecting both ordered cycles and potentially mutable flows.

Conceptual Loop Syntax:

Counted Loop (Conformist Focus):

Code snippet

CYCLE FOR <iterator> FROM <start> TO <end> [STEP <increment>] {
    // Code to execute in each predictable cycle
    IF <condition> THEN EXIT_CYCLE; // Standard break
}
Conditional Loop (While True):

Code snippet

WHILE_STREAM (<condition_is_true>) {
    // Code continues as long as condition holds
}
Conditional Loop (Until True):

Code snippet

REPEAT_WEAVE {
    // Code to execute
} UNTIL_ECHO (<condition_is_met>);
3. Rebel Temporal Commands: Defying the Flow
These commands allow programs to interact with and manipulate time directly, often consuming Aethel and carrying inherent risks.

Conceptual Invocation Syntax:

Rewinding Loop Iterations:

Code snippet

// Assuming 'my_rebel_loop' is an identified loop structure
INVOKE REWIND_FLOW (target: my_rebel_loop, steps: 3, cost: AETHEL_HIGH);
// Attempts to revert the last 3 iterations of 'my_rebel_loop'
Suspending a Loop's Timeline:

Code snippet

ACTIVATE SUSPEND_TIMELINE (target: critical_process_loop, duration: INDEFINITE, trigger: RESUME_SIGNAL);
// Freezes the loop until 'RESUME_SIGNAL' is received
Dilating a Loop's Tempo:

Code snippet

MODULATE CHRONO_DILATE_LOOP (target: observation_loop, factor: 0.1, stability_risk: MEDIUM);
// Makes 'observation_loop' run at 10% of normal perceived speed
Forcefully Shattering a Loop:

Code snippet

COMMAND SHATTER_ITERATION (target: compromised_loop, ripple_effect: MINIMAL);
// Abruptly terminates 'compromised_loop', potentially causing minor temporal ripples
This overview provides a glimpse into how Chronovyan syntax might begin to capture the language's core philosophies. The keywords and structures aim to be evocative of their function within this time-centric paradigm.

