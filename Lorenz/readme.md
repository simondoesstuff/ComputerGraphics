> Simon Walker  
> 9/6/2023  
> Lorenz Attractor

![](./demo.gif)

# Build

Build the project with make `make`.
Run (unix) with `make test` or `./lorenz`.

# Usage

The simulations is parametrized by `S`, `B`, and `R`.
Use `S/s` to decrease/increase S respectively. Similar keybinds for B and R.

Use `space` to **reset** the simulation.
  - (The parameters can be changed while the simulation is running)

Use arrow keys to orbit.

# Time

It took me 8 hours to complete this project.
I spent the first 10m converting to C++ and setting up the project.
The next 4 hours wondering why the origin wasn't centered.
Another hour realizing it was because the makefile wasn't compiling and linking properly, even though there were no errors.

3 hours to implement the project once I had my feet on the ground.
  - 1 hour to implement the Lorenz system, basic graphics, and camera controls.
  - 2 hours for touch ups.

# Technical

## Point Generation
The animation is produced by taking the previous point and applying the Lorenz function to it to
march to the next point. The time between generating points is recorded and used as the 
$\Delta t$ for the next point and ensures a constant amount of points computed per second.
All points are computed while the simulation idles (not rendering).

## Coloring
Points are colored by mapping their velocities to hue and converting to RGB.