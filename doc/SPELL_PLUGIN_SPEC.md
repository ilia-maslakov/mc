# Spell Plugin Specification (Final)

## 0. Architecture Contract

* Editor plugins may depend on MC core/plugin API.
* MC core/editor must not depend on any specific editor plugin implementation.
* MC core/editor must not know about `HAVE_ASPELL`.
* MC core/editor must not contain `#ifdef HAVE_ASPELL` around editor-plugin registration.
* Core must provide only a generic mechanism: "register all built editor-plugins".
* Core/editor code must not call plugin-specific functions directly (no spell-only init, cleanup, settings, feature checks).
* All plugin behavior must be reached only through generic editor plugin callbacks/dispatch.
* Core menu visibility and action availability must be based on generic plugin state queries, not on plugin-specific helpers.
* Core must not contain any spell-specific special cases.

---

## 1. Plugin Registration

* Plugin id: `spell`
* Label: `Spell`
* Menu flag: `MC_EPF_HAS_MENU`
* Registration must use generic editor-plugin registration flow.
* Spell plugin must always be built.
* Missing aspell/hunspell must not disable plugin build.
* No compile-time `#ifdef HAVE_ASPELL` may remove the whole Spell plugin.
* Backend availability is runtime state reported via `query_state`.

Source tree layout:

* `src/editor-plugins/spell/*`
* `src/editor-plugins/etags/*`
* `src/editor-plugins/<plugin-name>/*`

---

## 2. Plugin Callbacks

Mandatory callbacks:

* `activate(edit)`
  Runs the main plugin action only.

* `configure(edit)`
  Opens plugin settings only.

* `handle_action(edit, command)`
  Handles explicit command dispatch.

* `query_state(edit) -> { available: bool, enabled: bool, reason: optional string }`
  Reports runtime capability/state.

Optional callback:

* `handle_event(edit, event_id, payload)`
  Handles editor events.

Lifecycle:

* `open/close` use generic host wrapper callbacks.

Normative semantics:

* `activate` must not be reused for settings.
* `configure` must not start spell-check action.
* `query_state.available = false` means plugin cannot run in current runtime environment.
* `query_state.enabled = false` means plugin exists but action is temporarily disabled.
* `query_state.reason` provides a user-facing explanation.

---

## 3. Invocation Semantics

* `Plugins -> Spell` calls `activate(edit)` and runs full-file spell check.
* `Options -> Plugin info -> Spell` calls `configure(edit)` through generic API.
* No spell-specific special cases in core.
* Legacy `activate(NULL)` settings path is deprecated and must be removed after migration.

Before starting spell check, plugin must validate backend availability:

* Aspell: dynamic library/symbol load must succeed.
* Hunspell: `hunspell` binary must be available in `PATH`.

If no backend is available:

* Spell check must not start.
* Plugin must show a user-visible error.
* Plugin must return `MC_EPR_NOT_SUPPORTED`.

Backend probing must be performed at runtime (at least on `activate`).

---

## 4. Commands Handled by `handle_action`

* `CK_SpellCheckCurrentWord` -> suggest current word
* `CK_SpellCheck` -> spellcheck file
* `CK_SpellCheckSelectLang` -> change spelling language

If `edit == NULL`, only `CK_SpellCheckSelectLang` is accepted; others return `MC_EPR_NOT_SUPPORTED`.

If backend is unavailable, commands must not execute and must return `MC_EPR_NOT_SUPPORTED`.

---

## 5. Editor Menu Integration

Core integration must be generic and plugin-driven:

* Core shows plugin entry if plugin reports menu capability.
* Core enables/disables actions based on `query_state.enabled`.
* Core may display `query_state.reason` when action is disabled.
* Core must not call spell-specific helpers (e.g. `spell_is_enabled()`).
* Spell-specific commands are exposed through plugin metadata and callbacks, not hardcoded in core.
* Menu items must be provided through generic plugin action metadata.

---

## 6. Persistent Plugin Settings

Config section: `EditorPluginSpell`

Keys:

* `engine` (`aspell` or `hunspell`)
* `language` (e.g. `en`, `ru`, `en_US`)

Defaults:

* `engine=aspell`
* `language=en`

Settings dialog includes:

* engine selector (`Aspell` / `Hunspell`)
* language input

Settings must be accessible even if no backend is installed.

---

## 7. Backend Behavior

Backend selection is runtime, not compile-time.

Selected backend comes from settings key: `engine=aspell|hunspell`.

Aspell backend:

* Uses dynamic `libaspell` loading (`dlopen`/`dlsym`).
* Must not require aspell headers at build time.
* Supports check, suggest, add-to-dictionary.

Hunspell backend:

* Uses external CLI (`hunspell -a -d <lang>`) and parses output.
* Does not support add-to-dictionary.

Missing backend dependency is a runtime error, not a build error.

---

## 8. Runtime Errors and User Messaging

If selected backend is unavailable:

* Plugin must report a concrete runtime error.
* Plugin must show a user-visible message.
* Plugin must not silently fall back.

If no backend is available:

* Plugin must still operate as a shell:

  * opens from plugin menu;
  * opens settings dialog;
  * allows changing `engine`;
  * on spell-check action, shows clear runtime error explaining missing backend and installation hints.

Error messages must include:

* What is missing.
* How to install it.
* Distro-specific hints (at least Ubuntu/Debian and RHEL/Fedora examples).

Automatic backend switching is not allowed.

---

## 9. API Migration Rule

* Dedicated `configure` callback is mandatory in target API.
* `activate` must not be used for settings.
* Legacy `activate(NULL)` behavior is deprecated and must be removed after migration.

---

## 10. Core Decoupling Requirements

* Remove plugin-specific includes from `src/editor/*`.
* Remove direct spell lifecycle calls from core (`aspell_init()` / `aspell_clean()`).
* Remove spell-specific menu visibility logic from core.
* Remove any `#ifdef HAVE_ASPELL` from core/editor registration paths.
* Core must rely only on generic plugin registry and dispatch.
* Core must use `query_state` for menu and action decisions.
* Core must support dedicated `configure` callback in generic API.
* Core may support generic `handle_event` callback; events must not be tunneled through action commands.
