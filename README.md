# 🧩 Maze Pathfinding Visualizer

**Course:** Introduction to Artificial Intelligence  
**Author:** Merim Jusufbegović  
**University:** University of Sarajevo – Faculty of Electrical Engineering (ETF)

---

## 📋 Overview

An interactive desktop application that visualizes three classic graph-search algorithms navigating randomly generated mazes. Built with C++17 and the [natGUI]([https://www.natGUI.co](https://github.com/idzafic/natID/) cross-platform UI framework, the tool lets you watch BFS, Dijkstra, and A* explore the grid in real time — step by step or in animated mode.

---

## ✨ Features

| Feature | Details |
|---|---|
| **Three algorithms** | BFS · Dijkstra · A* (Manhattan heuristic) |
| **Live animation** | Adjustable speed slider (instant → 2 s/step) |
| **Step-by-step mode** | Advance the search one node at a time |
| **Interactive maze editor** | Click cells to toggle walls, drag start/goal markers |
| **Configurable grid** | 5 × 5 up to 80 × 80 cells |
| **Wall density slider** | 5 % – 45 % random obstacles |
| **Diagonal movement** | Optional 8-connectivity (cost = 2) |
| **Guaranteed-path mode** | Carves a clear corridor so a solution always exists |
| **Colour customisation** | Pick colours for visited cells and the final path |
| **Status bar** | Reports nodes visited · path length · elapsed time (ms) |
| **Multilingual UI** | English and Bosnian (`res/tr/EN` / `res/tr/BA`) |

---

## 🎨 Visualisation Legend

| Colour | Meaning |
|---|---|
| ⬛ Black | Wall |
| ⬜ White | Free, unvisited cell |
| 🟡 Yellow *(default)* | Visited / closed set |
| 🔵 Cyan | Frontier / open set |
| 🟢 Green *(default)* | Final path |
| 🔷 Blue border | Start cell |
| 🔴 Red border | Goal cell |

Visited and path colours can be changed at runtime with the colour pickers.

---

## 🤖 Algorithms

### Breadth-First Search (BFS)
Explores all neighbours layer by layer; guarantees the **shortest path** on an unweighted grid.

### Dijkstra's Algorithm
Priority-queue variant of BFS. On a uniform-cost grid it behaves identically to BFS, but correctly handles the optional diagonal moves (cost 2 vs. 1).

### A\* Search
Extends Dijkstra with a **Manhattan distance heuristic** (`h = |Δrow| + |Δcol|`). Typically visits far fewer nodes than the other two while still finding an optimal path.

---

## 🏗️ Project Structure

```
ProjAI_Maze_Jusufbegovic/
├── CMakeLists.txt          # CMake build entry point
├── astarmaze.cmake         # Source-list helper
├── src/
│   ├── main.cpp            # Application entry point
│   ├── Application.h       # App initialisation & lifecycle
│   ├── MainWindow.h        # Top-level window (menu, toolbar, statusbar)
│   ├── MainView.h          # Control panel + canvas layout
│   ├── MazeCanvas.h        # Drawing surface & user-interaction handler
│   ├── MazeModel.h         # Grid data, maze generation & search logic
│   ├── MenuBar.h           # Menu bar definition
│   ├── ToolBar.h           # Toolbar (Start/Stop, New maze, Step)
│   ├── StatusBar.h         # Status information panel
│   └── Constants.h         # Shared numeric IDs for menus/actions
└── res/
    ├── main.xml            # UI resource descriptors
    ├── DevRes.xml          # Development resources
    ├── appIcon/            # Application icons (Windows .ico, macOS .icns)
    └── tr/
        ├── EN/main.xml     # English translations
        └── BA/main.xml     # Bosnian translations
```

---

## ⚙️ Build Requirements

| Requirement | Version |
|---|---|
| C++ Standard | C++17 |
| CMake | ≥ 3.18 |
| natGUI framework | Set up in `~/Work/DevEnv/` |

> **Note:** This project is built on top of the **natGUI** cross-platform framework used in the Introduction to AI course at ETF Sarajevo. The framework must be installed and configured at `$HOME/Work/DevEnv/` before building.

### Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

---

## 🖱️ Usage

### Running a search

1. Launch the application — a random 25 × 25 maze is generated immediately.
2. Select an algorithm from the **Algorithm** drop-down (*BFS*, *Dijkstra*, or *A**).
3. Press **Start** (toolbar or *Animation* menu) to begin animated search, or press **Step** to advance one node at a time.
4. The status bar shows nodes visited, path length, and elapsed time when the search finishes.

### Editing the maze

Select an edit mode from the **Edit** combo box:

| Mode | Click / drag action |
|---|---|
| **Wall** | Toggle walls on/off |
| **Start** | Move the start marker (blue) |
| **Goal** | Move the goal marker (red) |

### Generating a new maze

Click **New Maze** in the toolbar or use the *Maze → New* menu item.  
Adjust **Density** with the slider before generating to control wall coverage.

---

## 📊 Algorithm Comparison

| Algorithm | Optimal? | Complete? | Speed (typical) |
|---|---|---|---|
| BFS | ✅ (unweighted) | ✅ | Moderate |
| Dijkstra | ✅ | ✅ | Moderate |
| A* | ✅ | ✅ | **Fastest** |

A* consistently explores the fewest nodes thanks to its heuristic guidance, which is especially visible on large, open mazes.

---

## 📄 License

This project was developed as a course assignment at the University of Sarajevo – Faculty of Electrical Engineering. All rights reserved by the author.
