# ★ STARBLAST

A top-down arcade space shooter built in **C + raylib**.

🎮 **[Play in browser →](https://YOUR_USERNAME.github.io/starblast/)**

---

## Controls

| Key | Action |
|-----|--------|
| WASD / Arrow Keys | Move |
| SPACE / Z | Shoot |
| P / ESC | Pause |
| ENTER | Confirm (menus) |

## Features

- 5 waves of escalating difficulty
- 3 enemy types: Scout, Tank, Boss
- Particle explosions, scrolling starfield
- Lives system + mercy invincibility
- Score tracking

## Build locally (MSYS2 UCRT64)

```bash
# Install raylib once
pacman -S mingw-w64-ucrt-x86_64-raylib

# Build and run
make run
```

## Tech

- Language: C (C11)
- Graphics: [raylib](https://www.raylib.com)
- Build: GNU Make
- Web: Emscripten → WebAssembly, deployed via GitHub Actions
