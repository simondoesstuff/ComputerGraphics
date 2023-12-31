#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include "models/buff.h"
// #include "geometry.h"

struct orbit
{
   float pitch = 0;
   float yaw = 0;
   const float speed = 10;
   const float dim = 15;
} orbit;

Box *scene;

#define LEN 8192 //  Maximum length of text string
void Print(const char *format, ...)
{
   char buf[LEN];
   char *ch = buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args, format);
   vsnprintf(buf, LEN, format, args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

// -------- Draw Hook -------- //
void draw()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //  Undo previous transformations
   glLoadIdentity();

   //  Set view angle
   glRotatef(orbit.pitch, 1, 0, 0);
   glRotatef(orbit.yaw, 0, 1, 0);

   {
      //----------------- Axis
      glColor3f(.3, .3, .3);
      //  Draw axes
      const double len = 1; //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(len, 0.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, len, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, 0.0, len);
      glEnd();
      //  Label axes
      glRasterPos3d(len, 0.0, 0.0);
      Print("X");
      glRasterPos3d(0.0, len, 0.0);
      Print("Y");
      glRasterPos3d(0.0, 0.0, len);
      Print("Z");
   }

   scene->draw();
   glutSwapBuffers();
   glFlush();
}

// -------- Idle Hook -------- //
void idle()
{
}

// -------- Window Reshape -------- //
void reshape(int width, int height)
{
   //  Set the viewport to the entire window
   glViewport(0, 0, RES * width, RES * height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   const double dim = orbit.dim;
   double asp = (height > 0) ? (double)width / height : 1;
   glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim * 2);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void key(unsigned char ch, int x, int y)
{
}

// -------- Special -------- //
void special(int key, int x, int y)
{
   float motion = orbit.speed;

   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      orbit.yaw += motion;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      orbit.yaw -= motion;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      orbit.pitch -= motion;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      orbit.pitch += motion;
   //  Keep angles to +/-360 degrees

   if (orbit.yaw > 360)
      orbit.yaw -= 360;
   if (orbit.yaw < 0)
      orbit.yaw += 360;
   if (orbit.pitch > 360)
      orbit.pitch -= 360;
   if (orbit.pitch < 0)
      orbit.pitch += 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// -------- Visibility -------- //
void visible(int vis)
{
   if (vis == GLUT_VISIBLE)
      glutIdleFunc(idle);
   else
      glutIdleFunc(NULL);
}

void initScene()
{
   auto bgColor = Vec3{.4, .5, 1} / 4;
   glClearColor(bgColor.x, bgColor.y, bgColor.z, 1);

   auto mainBuff = buildBuff()
                       ->rotate({0, (float)(rand() % 360), 0});

   auto altBuff = buildBuff()
                      ->scale(.4)
                      ->move({2, -1.3, -4})
                      ->rotate({0, (float)(rand() % 360) + 0, 0});

   auto altBuff2 = buildBuff()
                       ->scale(.4)
                       ->move({6, -1.3, -6})
                       ->rotate({0, (float)(rand() % 360) + 0, 0});

   auto altBuff3 = buildBuff()
                       ->scale(.4)
                       ->move({-5, -1.3, -3})
                       ->rotate({0, (float)(rand() % 360) + 0, 0});

   auto altBuff4 = buildBuff()
                       ->scale(.4)
                       ->move({6, -1.3, 5})
                       ->rotate({0, (float)(rand() % 360) + 0, 0});

   auto floor = Polygon({
                            {0, 0, 0},
                            {0, 0, 1},
                            {1, 0, 1},
                            {1, 0, 0},
                        },
                        {.6, .8, 1})
                    .extrude({0, -.1, 0})
                    ->varigatePaint(.25)
                    ->boxed()
                    ->scale({16, 1, 16})
                    ->move({-8, -2, -8});

   scene = new Box({mainBuff, altBuff, altBuff2, altBuff3, altBuff4, floor});
}

int main(int argc, char *argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc, argv);

#ifdef USEGLEW
   if (glewInit() != GLEW_OK)
   {
      fprintf(stderr, "Error initializing GLEW\n");
      exit(1);
   }
#endif

   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600, 600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   //  Create the window
   glutCreateWindow("3D Scene");

   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   // Enable face culling
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);

   initScene();

   glutDisplayFunc(draw);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(key);
   glutSpecialFunc(special);
   glutVisibilityFunc(visible);

   glutMainLoop();
   return 0; /* ANSI C requires main to return int. */
}
