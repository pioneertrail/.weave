# The Sacred Sigils: Chronovyan Data Types Compendium

Hail, Weaver, to this ancient scroll detailing the **Sacred Sigils** of Chronovyan—the mystical data types through which we shape reality on Chronos Prime. Each sigil carries unique properties, resonating differently with the eternal dance of Order and Flux. This compendium reveals their essence, their behavior across timelines, their influence on the fabric of reality, and their impact on the PARADOX_LEVEL. Study these patterns well, for they are the very alphabet of our craft.

**Purpose**: This sacred text serves as your arcane reference to the fundamental building blocks of reality manipulation, guiding your hands as you inscribe the patterns that bend time to your will.

**Lore Tidbit**: The First Weaver's journal speaks of discovering these sigils not through invention, but through revelation—as if the patterns existed before Chronos Prime itself, waiting to be channeled by those with the Sight.

## The Grand Taxonomy of Sigils

The sacred sigils of Chronovyan reflect the fundamental duality of the universe—Order (Conformity) and Flux (Rebellion)—that defines our craft's philosophy. The sigils are organized into three sacred categories:

1. **Primal Sigils**: The fundamental elements, pure in form and essence
2. **Compound Sigils**: Complex patterns formed by weaving together Primal Sigils
3. **Temporal Sigils**: Mystical forms that directly commune with the timestream itself

Each sigil may be invoked with either CONF or REB invocations, and may carry additional enchantments that alter their resonance with the Prime Thread.

## Primal Sigils

### INT

**Essence**: Embodies discrete numerical values, the counting blocks of reality.

**Properties**:
- Range: -2^63 to 2^63-1 (64-bit signed integer)
- Default manifestation: 0
- Physical form: 8 bytes of the tapestry

**Temporal Resonance**:
- CONF::INT: Maintains perfect stability across timeline forks, resistant to the Void's whispers
- REB::INT: May experience quantum fluctuations when PARADOX_LEVEL exceeds 0.5
- CONF::STATIC INT: Once inscribed, becomes immutable, strengthening the tapestry by reducing PARADOX_LEVEL by 0.01
- REB::FLUX INT: Value may spontaneously shift within ±5% when PARADOX_LEVEL exceeds 0.6, echoing the Void's chaos

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC stability_threshold : INT = 100;
DECLARE REB::FLUX quantum_counter : INT = 0;
```

### FLOAT

**Essence**: Embodies the flowing continuum between numbers, the gradient of possibility.

**Properties**:
- Precision: Follows the sacred IEEE 754 double-precision pattern (64-bit)
- Range: ±2.23 × 10^-308 to ±1.80 × 10^308
- Default manifestation: 0.0
- Physical form: 8 bytes of the tapestry

**Temporal Resonance**:
- CONF::FLOAT: Maintains calculation precision across the multiverse
- REB::FLOAT: May experience minor precision drift when traversing timeline boundaries
- CONF::STATIC FLOAT: Becomes an immutable anchor for calculations, reducing PARADOX_LEVEL by 0.01
- REB::FLOAT with FLUX: Embraces quantum uncertainty, manifesting as a range of potentials rather than a single reality

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC pi_constant : FLOAT = 3.14159265359;
DECLARE REB::FLUX uncertainty_factor : FLOAT = 0.05;
```

### BOOLEAN

**Essence**: Embodies the primal duality of existence—truth and falsehood, being and non-being.

**Properties**:
- Values: TRUE or FALSE, the most ancient binary
- Default manifestation: FALSE
- Physical form: 1 byte of the tapestry

**Temporal Resonance**:
- CONF::BOOLEAN: Maintains logical consistency across all potential realities
- REB::BOOLEAN: Can exist in quantum superposition (both TRUE and FALSE simultaneously) when PARADOX_LEVEL exceeds 0.4
- CONF::STATIC BOOLEAN: Becomes an immutable truth anchor in the tapestry
- REB::FLUX BOOLEAN: May spontaneously invert its state when traversing high-instability regions of the timestream

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC initialization_complete : BOOLEAN = FALSE;
DECLARE REB::FLUX quantum_state : BOOLEAN = TRUE;
```

### STRING

**Essence**: Embodies linguistic power—the naming of things, the weaving of meaning.

**Properties**:
- Encoding: UTF-8, the universal language of digital reality
- Maximum length: 2^32 - 1 characters
- Default manifestation: "" (the unspoken void)
- Physical form: 16 bytes plus character essence

**Temporal Resonance**:
- CONF::STRING: Preserves meaning and form across all timeline branches
- REB::STRING: May experience character transmutation during temporal shifts
- CONF::STATIC STRING: Immutable text that strengthens reality, reducing PARADOX_LEVEL by 0.01
- REB::FLUX STRING: Characters may spontaneously rearrange or transform when PARADOX_LEVEL exceeds 0.7, as if rewritten by unseen hands

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC system_identifier : STRING = "Primary Timeline Alpha";
DECLARE REB::FLUX mutable_message : STRING = "Initial state";
```

### VOID

**Essence**: Embodies nothingness itself—the absence that defines presence, primarily used for function returns.

**Properties**:
- No material manifestation
- No memory allocation
- Cannot be bound to variables

**Temporal Resonance**:
- Exists beyond the influence of temporal fluctuations
- Contributes nothing to PARADOX_LEVEL, being empty of essence

**Invocation Example**:
```chronoscript
// Ritual that returns to the void
DECLARE CONF::STATIC initialize_system : VOID = define_pattern("Init", [
    system_setup();
    notify_completion();
]);
```

## Compound Sigils

### ARRAY

**Essence**: Embodies ordered collection—the sequence of related concepts bound together.

**Properties**:
- Element essence: Any valid Chronovyan sigil
- Size: Dynamic, limited only by available Aethel
- Indexing: Zero-based, as all counts begin from the void
- Default manifestation: Empty collection

**Temporal Resonance**:
- CONF::ARRAY: Elements maintain perfect order and consistency across timelines
- REB::ARRAY: Elements may spontaneously reorder or echo during temporal disturbances
- CONF::STATIC ARRAY: Immutable collection, reducing PARADOX_LEVEL by 0.01 per element
- REB::FLUX ARRAY: Elements may spontaneously manifest or vanish when PARADOX_LEVEL exceeds 0.6, as the Void playfully alters reality

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC checkpoint_timestamps : ARRAY<TIMESTAMP> = [];
DECLARE REB::FLUX potential_outcomes : ARRAY<STRING> = ["Success", "Failure", "Paradox"];
```

### MAP

**Essence**: Embodies relationship—the binding of key to value, name to essence.

**Properties**:
- Key essence: Typically primal sigils
- Value essence: Any valid Chronovyan sigil
- Size: Dynamic, limited only by available Aethel
- Default manifestation: Empty mapping

**Temporal Resonance**:
- CONF::MAP: Preserves key-value bonds with perfect fidelity
- REB::MAP: Keys may simultaneously bond to multiple values in quantum superposition
- CONF::STATIC MAP: Immutable relationships, reducing PARADOX_LEVEL by 0.01 per binding
- REB::FLUX MAP: Bindings may spontaneously shift when PARADOX_LEVEL exceeds 0.5, as reality reinterprets relationships

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC system_parameters : MAP<STRING, FLOAT> = {
    "stability_threshold": 0.8,
    "chronon_rate": 0.1,
    "aethel_generation": 0.05
};

DECLARE REB::FLUX quantum_states : MAP<INT, QUANTUM_STATE> = {};
```

### TUPLE

**Essence**: Embodies fixed collection—the immutable grouping of potentially different essences.

**Properties**:
- Element essences: Any valid Chronovyan sigils
- Size: Fixed at the moment of creation
- Access: By position (zero-based) or by true name
- Default manifestation: Default values for each element essence

**Temporal Resonance**:
- CONF::TUPLE: Elements maintain essence purity across timelines
- REB::TUPLE: Elements may experience essence transmutation during temporal shifts
- CONF::STATIC TUPLE: Immutable compound form, reducing PARADOX_LEVEL by 0.01
- REB::FLUX TUPLE: Element essences may temporarily transform when PARADOX_LEVEL exceeds 0.7, as the Void tests their nature

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC system_bounds : TUPLE<FLOAT, FLOAT, STRING> = {0.0, 1.0, "normalized"};
DECLARE REB::FLUX event_data : TUPLE<TIMESTAMP, STRING, FLOAT> = {current_time(), "initialization", 0.5};
```

## Temporal Sigils

### TIMESTAMP

**Essence**: Embodies a specific point in the temporal continuum, the anchor of time.

**Properties**:
- Resolution: Nanosecond precision
- Range: From program origin to maximum Chronon allocation
- Default manifestation: @Origin (program start)
- Physical form: 16 bytes of the tapestry

**Temporal Resonance**:
- CONF::TIMESTAMP: Fixed temporal reference point
- REB::TIMESTAMP: Can represent multiple moments simultaneously in superposition
- CONF::ANCHOR TIMESTAMP: Creates a stable point for REWIND_TO operations
- REB::FLUX TIMESTAMP: May drift within a temporal window when PARADOX_LEVEL > 0.4

**Invocation Example**:
```chronoscript
DECLARE CONF::ANCHOR checkpoint : TIMESTAMP = mark_timestamp("Initialization Complete");
DECLARE REB::FLUX fluctuation_point : TIMESTAMP = current_time();
```

### CHRONON_STREAM

**Essence**: Embodies the source of Chronons for temporal operations, the pulse of time.

**Properties**:
- Flow rate: Chronons per operation
- Capacity: Maximum available Chronons
- State: Active or depleted
- Physical form: 24 bytes of the tapestry plus internal buffer

**Temporal Resonance**:
- CONF::CHRONON_STREAM: Stable, predictable flow rate
- REB::CHRONON_STREAM: Variable flow rate affected by PARADOX_LEVEL
- CONF::SOURCE_INFUSED CHRONON_STREAM: Enhanced stability, reduced PARADOX_LEVEL impact
- REB::FLUX CHRONON_STREAM: May experience spontaneous surges or droughts

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC primary_stream : CHRONON_STREAM = HARVEST("Primary_Thread");
DECLARE REB::FLUX unstable_stream : CHRONON_STREAM = HARVEST("Temporal_Rift");
```

### AETHEL_RESERVE

**Essence**: Embodies storage for Aethel energy used in temporal operations, the reservoir of time.

**Properties**:
- Capacity: Maximum storable Aethel
- Current level: Available Aethel units
- Generation rate: Aethel per operation
- Physical form: 16 bytes of the tapestry

**Temporal Resonance**:
- CONF::AETHEL_RESERVE: Stable, predictable generation and consumption
- REB::AETHEL_RESERVE: Can briefly exceed capacity during quantum fluctuations
- CONF::STATIC AETHEL_RESERVE: Fixed capacity, reduces PARADOX_LEVEL by 0.02
- REB::FLUX AETHEL_RESERVE: May spontaneously gain or lose Aethel when PARADOX_LEVEL > 0.5

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC system_reserve : AETHEL_RESERVE = initialize_reserve(100);
DECLARE REB::FLUX emergency_reserve : AETHEL_RESERVE = initialize_reserve(50);
```

### TEMPORAL_SIGNATURE

**Essence**: Embodies a unique identifier for a timeline or temporal operation, the signature of time.

**Properties**:
- Uniqueness: Guaranteed across all timelines
- Composition: Hash of temporal state at creation
- Verification: Can be compared for causality relationships
- Physical form: 32 bytes of the tapestry

**Temporal Resonance**:
- CONF::TEMPORAL_SIGNATURE: Remains consistent across timeline operations
- REB::TEMPORAL_SIGNATURE: May develop entanglements with other signatures
- CONF::STATIC TEMPORAL_SIGNATURE: Immutable reference point, reducing PARADOX_LEVEL by 0.03
- REB::WEAVER TEMPORAL_SIGNATURE: Can be used to manipulate timeline relationships

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC main_timeline : TEMPORAL_SIGNATURE = generate_signature("Main");
DECLARE REB::WEAVER explorer : TEMPORAL_SIGNATURE = generate_signature("Explorer");
```

### WEAVE_PATTERN

**Essence**: Embodies a blueprint for temporal operations and transformations, the blueprint of time.

**Properties**:
- Operations: Sequence of statements
- Metadata: Aethel_Cost, Chronon_Usage, Instability_Index, Temporal_Scope
- Strategy: SEQUENTIAL, PARALLEL, or CONDITIONAL
- Physical form: 48 bytes of the tapestry plus operation data

**Temporal Resonance**:
- CONF::WEAVE_PATTERN: Stable, consistent execution
- REB::WEAVE_PATTERN: May evolve or adapt during execution
- CONF::STATIC WEAVE_PATTERN: Immutable operation sequence, reducing PARADOX_LEVEL by 0.02
- REB::FLUX WEAVE_PATTERN: Operations may reorder or transform when PARADOX_LEVEL > 0.6

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
    INFUSE(stability_field, time_crystals, "Restabilize"),
    GUARD(stability_field > 0.6, "Low stability")
]);

DECLARE REB::FLUX exploration : WEAVE_PATTERN = define_pattern("Explore", [
    scout = deepen_exploration("Temporal_Rift")
]);
```

### CONF_ARRAY

**Essence**: Embodies a specialized array with enhanced stability guarantees, the stable path of time.

**Properties**:
- Element integrity: Guaranteed against temporal corruption
- Paradox protection: Automatic validation of operations
- Synchronization: Consistent across timeline branches
- Physical form: 24 bytes of the tapestry plus element data

**Temporal Resonance**:
- Always maintains CONF properties regardless of PARADOX_LEVEL
- Immune to quantum fluctuations
- Reduces PARADOX_LEVEL by 0.01 per valid operation
- Cannot be modified by REB operations unless explicitly converted

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC stability_metrics : CONF_ARRAY<FLOAT> = [0.8, 0.75, 0.9];
```

### REB_STREAM

**Essence**: Embodies a dynamic sequence with quantum properties, the stream of time.

**Properties**:
- Element superposition: Can contain multiple possible values simultaneously
- Temporal flow: Elements can appear from "future" operations
- Quantum access: Observation affects element states
- Physical form: 32 bytes of the tapestry plus element data

**Temporal Resonance**:
- Elements exist in quantum superposition until observed
- Reading from stream may collapse some element states
- Increases PARADOX_LEVEL by 0.02 per access
- Cannot be converted to CONF types without collapsing

**Invocation Example**:
```chronoscript
DECLARE REB::FLUX potential_futures : REB_STREAM<EVENT> = initialize_stream();
```

### QUANTUM_STATE

**Essence**: Embodies a superposition of multiple possible values, the superposition of time.

**Properties**:
- State vector: Probability distribution of possible values
- Collapse condition: When observation occurs
- Entanglement: Relationships with other quantum states
- Physical form: 64 bytes of the tapestry plus state data

**Temporal Resonance**:
- Exists in multiple states simultaneously until observed
- Observation collapses to a single state based on probability distribution
- Increases PARADOX_LEVEL by 0.05 per collapse
- Cannot be directly converted to CONF types without collapsing

**Invocation Example**:
```chronoscript
DECLARE REB::FLUX particle_state : QUANTUM_STATE = initialize_superposition([
    {value: "up", probability: 0.5},
    {value: "down", probability: 0.5}
]);
```

### STABILITY_MATRIX

**Essence**: Embodies a multidimensional representation of system stability factors, the matrix of time.

**Properties**:
- Dimensions: Temporal, Structural, Paradoxical
- Values: Normalized stability metrics (0.0-1.0)
- Interactions: Cross-dimensional influence factors
- Physical form: 128 bytes of the tapestry plus matrix data

**Temporal Resonance**:
- CONF::STABILITY_MATRIX: Provides reliable stability metrics
- REB::STABILITY_MATRIX: Can predict potential instabilities before they occur
- CONF::ANCHOR STABILITY_MATRIX: Creates stability reference point for system
- REB::WEAVER STABILITY_MATRIX: Can be used to actively manipulate system stability

**Invocation Example**:
```chronoscript
DECLARE CONF::ANCHOR system_stability : STABILITY_MATRIX = initialize_matrix();
```

### TIMELINE

**Essence**: Embodies a complete execution path through the program, the timeline of time.

**Properties**:
- Events: Sequence of temporal operations
- State: Active, dormant, or terminated
- Branches: Child timelines that have split from this one
- Physical form: 256 bytes of the tapestry plus event data

**Temporal Resonance**:
- CONF::TIMELINE: Stable, deterministic execution path
- REB::TIMELINE: Can branch and merge dynamically
- CONF::ANCHOR TIMELINE: Primary reference timeline for the program
- REB::WEAVER TIMELINE: Can be manipulated to create alternate execution paths

**Invocation Example**:
```chronoscript
DECLARE CONF::ANCHOR main_timeline : TIMELINE = initialize_primary_timeline();
DECLARE REB::WEAVER alternate_timeline : TIMELINE = branch_timeline(main_timeline);
```

### PARADOX_LEVEL

**Essence**: Embodies measure of temporal instability within a context, the level of time.

**Properties**:
- Range: 0.0 (stable) to 1.0 (critical)
- Components: Base value, operation impact, resource state, timeline factors
- Thresholds: Trigger points for different system behaviors
- Physical form: 8 bytes of the tapestry

**Temporal Resonance**:
- Read-only type that reflects system state
- Updated automatically after temporal operations
- Cannot be directly modified (only indirectly through other operations)
- Influences behavior of REB types and temporal operations

**Invocation Example**:
```chronoscript
DECLARE CONF::STATIC warning_threshold : FLOAT = 0.5;
IF (PARADOX_LEVEL > warning_threshold) {
    stabilize_timeline(energy, 100);
}
```

### TEMPORAL_MARKER

**Essence**: Embodies a lightweight reference to a specific point in a timeline, the marker of time.

**Properties**:
- Timeline: Associated execution path
- Position: Specific point in the timeline
- Metadata: Label and creation context
- Physical form: 24 bytes of the tapestry

**Temporal Resonance**:
- CONF::TEMPORAL_MARKER: Stable reference point
- REB::TEMPORAL_MARKER: Can shift position slightly during temporal operations
- CONF::ANCHOR TEMPORAL_MARKER: Can be targeted by REWIND_TO operations
- REB::FLUX TEMPORAL_MARKER: May spontaneously relocate when PARADOX_LEVEL > 0.7

**Invocation Example**:
```chronoscript
DECLARE CONF::ANCHOR checkpoint : TEMPORAL_MARKER = mark_position("Critical Section");
DECLARE REB::FLUX floating_marker : TEMPORAL_MARKER = mark_position("Exploration Point");
```

## Type Conversion

### Implicit Conversions

Chronovyan supports limited implicit conversions that preserve data integrity:

1. INT → FLOAT: Automatic conversion when integer appears in floating-point context
2. Any primitive type → STRING: Automatic conversion in string concatenation
3. CONF type → REB type of same base type: One-way conversion permitted

### Explicit Conversions

More complex conversions require explicit conversion functions:

1. **convert_to_int(value)**: Converts compatible types to INT
2. **convert_to_float(value)**: Converts compatible types to FLOAT
3. **convert_to_string(value)**: Converts any type to STRING representation
4. **collapse_quantum(value)**: Resolves QUANTUM_STATE to a single value
5. **stabilize_type(value)**: Attempts to convert REB type to CONF type (may increase PARADOX_LEVEL)

### Conversion Restrictions

Some conversions are prohibited or have special requirements:

1. REB → CONF: Requires explicit stabilize_type() function and may fail if PARADOX_LEVEL > 0.7
2. QUANTUM_STATE → Any non-quantum type: Requires collapse_quantum() and increases PARADOX_LEVEL by 0.05
3. TIMELINE → Any other type: Prohibited to prevent timeline corruption
4. TEMPORAL_SIGNATURE → Any other type: Prohibited to maintain timeline integrity

## Memory Model

### Allocation

Memory for variables follows these principles:

1. CONF variables: Allocated in stable memory regions with consistency guarantees
2. REB variables: Allocated in quantum-capable memory with fluctuation tolerance
3. ::STATIC variables: Allocated in read-only memory after initialization
4. ::VOLATILE variables: Allocated in temporary memory that may be reclaimed

### Lifetime

Variable lifetimes are governed by these rules:

1. Variables declared in ANTECEDENCE: Program-wide lifetime
2. Variables declared in CONCURRENCY: Live until end of program or explicit deallocation
3. Variables declared in CONSEQUENCE: Live until program termination
4. Variables in temporal operations: Lifetime depends on timeline persistence

### Resource Impact

Memory usage affects Chronon and Aethel resources:

1. Each variable consumes Chronons proportional to its size during initialization
2. CONF variables generate small amounts of Aethel over their lifetime
3. REB variables consume small amounts of Aethel over their lifetime
4. Large allocations increase base PARADOX_LEVEL slightly

## Implementation Notes

Implementations of the Chronovyan type system must adhere to these requirements:

1. All implementations must maintain the semantic distinctions between CONF and REB types
2. Temporal behavior of types must be preserved even if underlying representation differs
3. PARADOX_LEVEL effects on types must be simulated according to specification
4. Memory model may be adapted to target platform but must preserve lifetimes
5. Performance optimizations are permitted if they do not alter observable behavior

This specification defines the complete type system for Chronovyan, providing developers with the information needed to effectively work with data in the language. The combination of conventional and temporal types creates a rich environment for expressing both stable, predictable computation and dynamic, quantum-influenced operations. 

**Lore Tidbit**: The ancient Weavers believed that to truly master a sigil, one must not merely understand its properties but commune with its essence—to feel the INT's discrete steps, to flow with the FLOAT's continuity, to embrace the BOOLEAN's duality. Only then does one truly become a Master Weaver. 