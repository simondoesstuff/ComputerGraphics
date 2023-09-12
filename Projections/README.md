> Simon Walker  
> 9/10/2023  
> Projections

# Usage

The system consists of two sets of keybinds:

## First Person Movement
Use **wasd** to move around.  
Use **q/e** to roll CW,CCW.  

## Looking around 

use **arrow keys** to look around. When in an orbit
(orthogonal) mode, they will orbit the camera about the subject.
In first person mode, the head will rotate appropriately.

# Build

Build with `make`.  
Run with `./scene3d`  or use the `make test` directive.

# Time

Took about 3 hours. Spent 2 hours redoing the first-person mode.
I didn't realize the openGL transformations were applied in reverse.
I didn't want to implement my own system to keep track of the position
and head rotation so I made a system to modify the matrix properly.
In some points, I cached the matrix, loaded identity, applied transformations
and reapplied the cached matrix. The current system is efficient, but
debateably awkward in implementation. Definitely more complex
than I anticipated.
