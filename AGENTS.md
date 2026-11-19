# Sonaria — agent guide

Sonaria is an open-source vocal pitch editor (VST3 / AU / ARA2 plugin and standalone app), a hard fork of [PitchNet](https://github.com/SessionLoops/PitchNet) (itself derived from [HachiTune](https://github.com/KCKT0112/HachiTune)). Its goals are editing speed, harmony creation and a hybrid synthesis engine (neural vocoder / PSOLA / spectral-envelope).

## Where things are

- Requirements: `docs/requirements/` (`REQ-{SCOPE}-{SEQ}`; start at `overview.md`)
- Specifications: `docs/specifications/` (`architecture.md` first)
- Plans: `docs/implementation/roadmap.md`, then `docs/implementation/M{n}-*/README.md`
- Decisions: `docs/adr/`
- Session handoffs: `docs/handoff/` (never at the repository root)

## Rules

- **License**: AGPL-3.0 (ADR-0001). Keep every upstream copyright and attribution notice. Do not remove the PitchNet / HachiTune attribution from README and `THIRD_PARTY_NOTICES.txt`.
- **Engines** take `DspSynthesisRequest` and return the same span of samples, so they stay interchangeable (`docs/specifications/engine.md`). Never mix two engine outputs in the same frame except inside a boundary crossfade (ADR-0003).
- **UI** stays JUCE. No WebView (ADR-0005).
- **Hosts**: verify ARA behaviour in Cubase and Studio One. Logic is build-only (ADR-0007).
- **Plan status** lives only in the table of each milestone `README.md`. Do not move files to express status.
- **Language**: requirements, plans and ADRs in Japanese; code, comments, commit messages and README in English.
- **Commits**: Conventional Commits (`feat:`, `fix:`, `docs:`, `ci:`, `build:`, `refactor:`, `test:`, `chore:`), one line with a concrete description. One commit per logical change. Commit directly to `main`; push only after the user agrees.
- **Docs consistency**: when finishing a piece of work, check whether it affects the requirements, specifications or plans under `docs/`, and if it does, ask the user whether to update them.
