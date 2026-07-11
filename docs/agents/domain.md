# Domain Docs

This is a single-context repository. These rules describe how engineering skills consume its domain documentation.

## Before exploring, read these

- `CONTEXT.md` at the repository root.
- Relevant ADRs under `docs/adr/`.

If either location does not exist, proceed silently. Do not propose creating domain documentation merely because it is absent; domain-modeling and related Matt Pocock skills create it when terminology or decisions actually need to be recorded.

## File structure

```text
/
├── CONTEXT.md
├── docs/adr/
└── project/
```

## Use the glossary's vocabulary

When output names a domain concept in a ticket, refactor proposal, hypothesis, or test, use the term defined in `CONTEXT.md`. Do not drift to synonyms the glossary explicitly avoids.

If a needed concept is absent, reconsider whether new language is necessary or note a real gap for domain modeling.

## Flag ADR conflicts

If proposed work contradicts an existing ADR, surface the conflict explicitly rather than silently overriding the decision.
