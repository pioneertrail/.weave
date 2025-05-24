# The Weaver's Archive: Directory Structure

## The Temporal Atlas

Greetings, Navigator of the Chronovyan Archives! This scroll maps the physical manifestation of our knowledge, organizing the sacred texts into a structure that reflects both logical order and philosophical meaning. Like the branching timelines of Chronos Prime, our directory structure flows from foundational concepts to specific implementations, guiding Weavers of all castes through the repository.

**Purpose**: This document outlines the recommended organization of the Chronovyan project files, helping contributors locate existing documents and place new ones appropriately within the established hierarchy.

**Lore Tidbit**: The Archive Keepers of the Anchor Caste maintain a crystalline matrix of temporal threads, each representing a document in the Chronovyan Codex. The physical organization of this matrix—mirrored in our directory structure—is said to influence the stability of the knowledge itself.

## Root Directory: `/Chronovyan`

The root of all temporal threads, containing the highest-level navigation documents:

```
/Chronovyan
├── README.md                       # The entry point for all travelers
├── ROADMAP.md                      # The temporal map of past and future development
├── CHANGELOG.md                    # The chronicle of changes across versions
└── docs/                           # The main repository of knowledge
```

## The Knowledge Repository: `/Chronovyan/docs`

The central archive containing all documentation, organized by purpose and caste:

```
/Chronovyan/docs
├── Chronovyan_Documentation_Index.md   # The master index of all documentation
├── Directory_Structure.md              # This navigational guide
├── Chronovyan_Manifesto.md             # The foundational philosophy document
│
├── philosophy/                     # Foundational philosophical concepts
│   ├── The_Great_Duality_Order_and_Flux.md
│   └── Programming_as_Temporal_Weaving.md
│
├── specifications/                 # Technical specifications (Anchor domain)
│   ├── Chronovyan_Formal_Grammar.md
│   ├── Chronovyan_Runtime_Semantics.md
│   ├── Chronovyan_Data_Types.md
│   ├── Chronovyan_Variable_Interaction_Rules.md
│   └── Chronovyan_Loop_Stability_Metrics.md
│
├── narrative/                      # Lore-rich guides (Seeker domain)
│   ├── Chronoscript_Lore_and_Narrative_Guide.md
│   ├── Chronoscript_Language_Specification.md
│   ├── Chronoscript_Standard_Library_Reference.md
│   └── Chronoscript_Runtime_Mechanics_Guide.md
│
├── phase_summaries/               # Chronicles of completed phases
│   ├── Phase_1_Completion_Summary.md
│   └── Phase_2_Completion_Summary.md
│
├── implementation/                # Phase 3 implementation guides (future)
│   └── [Reserved for future documents]
│
└── TECHNICAL_DEBT.md              # Record of issues requiring future attention
```

## Suggested Organization of New Documents

When adding new scrolls to the Archive, follow these guidelines:

### For the Anchor Caste (Order-Focused)

Technical specifications, formal definitions, and implementation details belong in:
- `/Chronovyan/docs/specifications/` for language definitions
- `/Chronovyan/docs/implementation/` for reference implementation details

### For the Seeker Caste (Flux-Focused)

Narrative guides, lore expansions, and creative explorations belong in:
- `/Chronovyan/docs/narrative/` for lore-rich guides
- `/Chronovyan/docs/philosophy/` for new philosophical concepts

### For the Rebel Weaver Caste (Balanced)

Integration documents, summaries, and user-focused content belong in:
- `/Chronovyan/docs/` (root) for major indices and overviews
- `/Chronovyan/docs/phase_summaries/` for milestone documentation

## Migration Guide

To align with this structure, we recommend the following relocations:

1. Move all Chronoscript narrative guides from `/docs/` to `/docs/narrative/`
2. Ensure all technical specifications are consolidated in `/docs/specifications/`
3. Create the `/docs/philosophy/` directory if it doesn't exist, and move relevant documents there
4. Update cross-references in all documents to reflect the new paths

## Timeline Integration

This structure is designed to evolve with the Chronovyan project. As we progress through Phase 3 and beyond, new directories may emerge to accommodate implementation artifacts, testing frameworks, and community contributions.

**Lore Tidbit**: The Archive structure itself is said to be a temporal program, with directories as WEAVE_PATTERNs and files as temporal operations. Maintaining its harmony is as important as the content it contains, for a fractured Archive can lead to a fractured understanding.

---

*"Order in structure begets order in thought; clarity in organization yields clarity in execution. The Weaver who can navigate the Archive with ease shall navigate the complexities of temporal programming with equal grace."* — Archive Keeper's Creed 