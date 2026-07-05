# Requirements
- Yazi

# What is this?
This is a tool to help you identify which proton prefix is which, using symlinks to create folders with the game's name.

# Features
- For Steam games, the program sends a request to Steam in order to retrieve the game name, making the process for these games automatic.
- For non-Steam games, contents of popular gamesave folders are shown, such as all of AppData and Documents. If the user still can't identify the game, yazi will open to let them navigate the prefix freely.

# Building
## Requirements
- CMake
- Ninja
```shell
cmake -B build -S . -G Ninja
cmake --build build/
```

Binary should be inside `build/`