# make.krhub.uk

Free ESP32 builds, code and guides. Code is MIT, guides are CC BY 4.0.

## Layout
- `index.html` — the website (GitHub Pages)
- `firmware/version.json` — the update manifest every robot checks at boot
- `firmware/projectx/` — Project X desk buddy firmware
- `CREDITS.md` — original makers who inspired each build

## Release flow
1. Build the new firmware.
2. Flash it to Kyle's car in London and test it.
3. Publish the `.bin` as a GitHub Release (tag `projectx-vX.Y.Z`).
4. Update `firmware/version.json` (version, url, sha256, tested_on).
5. Ryan's and Ethan's cars pick it up next time they're idle and plugged in.

If a new version fails to start, the car rolls back to the previous one automatically.
