# Sample project with tests using Catch2

Setup with:
```bash
cmake -S . -B build -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=1
if [ ! -e compile_commands.json ]; then
  ln -s build/compile_commands.json .
fi
```
run with
```bash
cmake --build build
```
