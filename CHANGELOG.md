# Changelog

## 2.9.2 — GitHub-ready maintainer package

- Updated mod metadata to identify `Banuarta` as project maintainer.
- Kept mod ID `market-island-dock` to preserve the existing mod identity
  and stored settings.
- Added in-mod documentation.
- Added a guard so a zero-strength fallback shadow does not leave
  residual pixels.
- Prepared repository README, architecture notes, and technical
  references.
- Documented the independent `ShapeOpacity` and `FluentGlassTint`
  controls.
- Documented the Yahoo Finance endpoint and its terms-of-use
  limitations.

## 2.9.1 — Independent shape opacity

- Added `ShapeOpacity` setting (0–100%).
- Kept `FluentGlassTint` independent from overall shape opacity.
- Applied shape opacity to the Composition surface and fallback
  background paths.
- Kept text and interactive controls readable.
