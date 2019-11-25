# 42-RT: Ray Tracing engine from scratch

![RT screenshot](./pictures/1.png)

## 1. Overview
The project implements a Ray Tracing engine from scratch in C and OpenCL for rendering volumetric figures.
The engine supports the following features:
- **5 simple figures**: `plane`, `sphere`, `cylinder`, `[capped] cone`, `box` + **1 complex figure**: `wineglass`;
- **4 operations** that can be applied to simple objects to construct the complex ones: `union`, `intersection`, `difference`, `clipping`;
- **4 light source types**: `ambient`, `point`, `parallel`, `spot`;
- Reflective and transparent materials;
- Camera movement in 3 dimensions with variable FOV;


## 2. Installation
Requires SDL2, OpenCL

## 3. Usage
`./RT scene_file`

Pre-defined scenes are located in `./scenes`

**Controls:**

`w`, `a`, `s`, `d` - XY axes movement;

`q`, `e` - Z axis movement;

`up`, `down` - pitch angle change;

`left`, `right` - yaw angle change;
