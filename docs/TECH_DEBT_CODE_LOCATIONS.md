# Technical Debt Code Locations

This document identifies specific locations in the codebase where technical debt comments should be added, following the format specified in the Technical Debt Tracking document.

## StateController.cpp

1. **Error Handling Complexity** (TD-1)
   ```cpp
   // TODO(TECH-DEBT): Refactor error handling to use a polymorphic error type system - Improves error handling extensibility - v1.2
   ```
   Add to `handleErrorState` method

2. **Fallback Mode Duplication** (TD-1)
   ```cpp
   // TODO(TECH-DEBT): Consolidate fallback and error handling logic to remove duplication - Reduces maintenance burden - v1.2
   ```
   Add to `handleFallbackMode` method

3. **Test-Specific Handling** (TD-4)
   ```cpp
   // TODO(TECH-DEBT): Extract test-specific handling to dedicated test support class - Improves separation of concerns - v1.3
   ```
   Add to `updateMode` method where test-specific handling occurs

## ModeDecisionEngine.cpp

1. **Duplicate Recovery Logic** (TD-2)
   ```cpp
   // TODO(TECH-DEBT): Refactor sensor recovery logic into separate method to remove duplication - Reduces bug risk - v1.2
   ```
   Add to `evaluate_metrics` and `makeDecision` methods where recovery logic is duplicated

2. **String-Based Reason Codes** (TD-6)
   ```cpp
   // TODO(TECH-DEBT): Replace string-based reason codes with enum values - Reduces error-prone string comparisons - v1.4
   ```
   Add at the top of the file

3. **Magic Number Thresholds** (TD-5)
   ```cpp
   // TODO(TECH-DEBT): Extract magic number thresholds to configurable constants - Improves configurability - v1.4
   ```
   Add near the kHighLoadThreshold and kLowLoadThreshold usage

4. **Debug Output** (TD-3)
   ```cpp
   // TODO(TECH-DEBT): Create centralized logging framework instead of scattered debug prints - Improves debugging experience - v1.3
   ```
   Add near the DEBUG_MODE sections

## Other Files

For all files with #ifdef DEBUG_MODE sections, add:
```cpp
// TODO(TECH-DEBT): Implement centralized logging framework - Improves consistency of debug output - v1.3
```

## Implementation Plan

1. Add these comments to the respective files in the next maintenance cycle
2. Update the `TECHNICAL_DEBT.md` document when new items are identified
3. Track resolution of these items in the "Recently Addressed Technical Debt" section 