# Chronovyan Language Examples

This directory contains example Chronovyan programs (.cvy files) that demonstrate various features of the language. These examples serve as both a learning resource and a test suite for verifying interpreter functionality.

## Running the Examples

To run these examples, use the Chronovyan interpreter:

```bash
# Run an example file
chronovyan examples/hello_world.cvy

# Start the REPL and then copy/paste snippets from the examples
chronovyan
```

## Available Examples

### Basic Examples

1. **hello_world.cvy** - A simple "Hello, World!" program that demonstrates basic syntax, variables, and printing.

### Intermediate Examples

2. **temporal_branching.cvy** - Demonstrates timeline branching, time points, and managing paradox levels.

## Creating Your Own Examples

Feel free to create your own `.cvy` files using these examples as a starting point. The Chronovyan language supports:

- CONF (Conformist) and REB (Rebel) variables
- Temporal operations like BRANCH_TIMELINE and REWIND_FLOW
- Resource tracking for Chronons and Aethel
- Variable modifiers like ::ANCHOR and ::WEAVER
- Paradox level management

## Example File Structure

Each example follows this general structure:

1. Comments explaining the purpose of the example
2. Variable and function declarations
3. Main code with temporal operations
4. Resource and state reporting

## Learning Path

For new users, we recommend studying the examples in this order:

1. Start with hello_world.cvy to understand basic syntax
2. Move to temporal_branching.cvy to explore timeline manipulation
3. Create your own examples to experiment with the language 