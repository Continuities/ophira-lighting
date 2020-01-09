# Ophira Lighting Simulator

Experimenting with a web-based environment so I can develop lighting patterns without access to the hardware.

## Install and Build
- `brew install emscripten`
- `brew install binaryen`
- `brew install llvm`
- Edit `~/.emscripten`
  - set `LLVM_ROOT` to `/usr/local/opt/llvm`
  - set `BINARYAN_ROOT` to `/usr/local`
- `yarn build`
- `yarn start`

## Use
Edit `scene.cpp` to set up whatever kind of scene you like using
the layers defined in LayerEngine. Write new layers, mix and match, see how things look!

When you're ready, run `yarn build` and then `yarn start`. Hit `http://localhost:3000` to see your patterns in action, no hardware needed!