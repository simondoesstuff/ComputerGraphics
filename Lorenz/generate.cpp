/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 */

#include <stdio.h>

struct lorenzPoint {
   float x;
   float y;
   float z;
   float velocity;
};

lorenzPoint marchPoint(lorenzPoint &point, float dt, float s, float b, float r) {
   float dx = s*(point.y-point.x);
   float dy = point.x*(r-point.z)-point.y;
   float dz = point.x*point.y - b*point.z;
   float velocity = dx*dx + dy*dy + dz*dz;

   return lorenzPoint {
      point.x + dt*dx,
      point.y + dt*dy,
      point.z + dt*dz,
      velocity
   };
}

lorenzPoint* generate(int amount, float s, float b, float r)
{
   /*  Coordinates  */
   float x = 1;
   float y = 1;
   float z = 1;
   float velocity = 0;
   /*  Time step  */
   float dt = 0.001;
   
   lorenzPoint* output = new lorenzPoint[amount];

   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (int i=0;i < amount;i++)
   {
      lorenzPoint next = {
         x, y, z, velocity
      };

      output[i] = next;
      
      float dx = s*(y-x);
      float dy = x*(r-z)-y;
      float dz = x*y - b*z;
      velocity = dx*dx + dy*dy + dz*dz;

      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
   }
   
   return output;
}

lorenzPoint* generate(int amount) {
   return generate(amount, 10.0, 2.6666, 28.0);
}