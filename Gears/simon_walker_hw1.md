> Simon Walker   
> 9/5/2023  
> HW1

# 1

<u>300 x 300</u>  
Getting ~115 FPS

<u>1x1  </u>  
Getting about the same FPS. (~115 FPS)

<u>Full Screen **3456 x 2234**</u>  
Still the same. (113-118 FPS)

**I would expect** the fps to decrease with screen size. I notice with this particular
animation, the model is being scaled such that it looks roughly the same on any screen size.
Also, perhaps while the window size is scaling, the resolution is not.

# 2

My system is not causing the program to have a constant framerate. It varies quite a lot.
It could be constant, like 60, 72, or 80 fps if the sytsem has V-Sync enabled which locks
the framerate to the refreshrate of the monitior. This is useful for preventing screen tearing
and never creating more frames than the monitor can display. My system does not have V-Sync enabled.

I was also able to get my FPS to spike to a few thousand when hiding the window which makes
sense since the program is not rendering anything. Further confirmation that V-Sync is not enabled.

# 3

Assignment took about 20m (excluding installing openGL).