---
name: default-base-foundry-2604
description: "Default build/dev base is Foundry Linux / Kubuntu 26.04 (ubuntu:26.04 for containers), not 24.04"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: d0436cb4-bd6b-40ff-8710-3fe7b8b4395b
---

Default to **Foundry Linux / Kubuntu 26.04** as the target environment. For build
containers (e.g. drmon's `linux/Dockerfile`) use `FROM ubuntu:26.04` — the base
Foundry/Kubuntu 26.04 sits on — not `ubuntu:24.04`.

**Why:** Will runs Foundry Linux (his Kubuntu-26.04-based distro; see foundrylinux.org).
26.04 ships the modern toolchain packages as first-class — e.g. `libcppdap-dev` 1.58.0a
is a normal apt package on 26.04 but absent/awkward on 24.04, which is what broke the
drmon DAP build until the base was bumped. He explicitly said: "who cares about 24.04? …
you should be using foundry linux kubuntu 26.04 as your default."

**How to apply:** When creating or fixing Dockerfiles/CI/dev-env for his projects, target
26.04. Don't pin to 24.04 or chase 24.04-specific package workarounds. If a dep is missing,
first check it's a proper 26.04 package before vendoring/building from source.
