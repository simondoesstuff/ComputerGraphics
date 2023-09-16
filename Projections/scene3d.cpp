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

struct control
{
   const float dim = 15;
   float orbitSpeed = 10;
   float walkSpeed = 3;
   float lookSpeed = 7;
   float orbitYaw = 0;
   float orbitPitch = 10;
   int proj = 0; // 0,1,2 => ortho, prespective, first person
} control;

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
   glPushMatrix();

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
   glPopMatrix();
}

// -------- Idle Hook -------- //
void idle()
{
}

void adjustLook(float angleX, float angleY, float angleZ)
{
   if (control.proj != 2)
   {
      // orthographic mode
      control.orbitYaw += angleY;
      control.orbitPitch += angleX;
      glLoadIdentity();
      glTranslatef(0, 0, -control.dim);
      glRotatef(control.orbitPitch, 1, 0, 0);
      glRotatef(control.orbitYaw, 0, 1, 0);
   }
   else
   {
      // first person mode
      // rotate according to (angleX, angleY) but apply
      // the transformation before the current transformations
      float matrix[16];
      glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
      glLoadIdentity();
      glRotatef(angleX, 1, 0, 0);
      glRotatef(angleY, 0, 1, 0);
      glRotatef(angleZ, 0, 0, 1);
      glMultMatrixf(matrix);
   }

   glutPostRedisplay();
}

void adjustWalk(float dX, float dY)
{
   // transformations need to be applied "before" the current transformations
   float matrix[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
   glLoadIdentity();
   glTranslatef(dX, 0, dY);
   glMultMatrixf(matrix);
   glutPostRedisplay();
}

void updateProjection(int newProj)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   double asp = (glutGet(GLUT_WINDOW_HEIGHT) > 0) ? (double)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT) : 1;

   switch (newProj)
   {
   case 0:
   {
      // Orthographic projection
      const double dim = control.dim;
      glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim * 2);
      break;
   }
   default:
      // Perspective projection
      // (glu is not allowed, only able to use glFrustum)
      glFrustum(-asp * .08, +asp * .08, -.08, +.08, .1, 1000);
      break;
   }

   glMatrixMode(GL_MODELVIEW);
   adjustLook(0, 0, 0);

   if (newProj == control.proj)
   {
      return;
   }

   if (control.proj == 2)
   {
      // first person or left first person
      glLoadIdentity();
      glTranslatef(0, 0, -control.dim);
      glRotatef(control.orbitPitch, 1, 0, 0);
      glRotatef(control.orbitYaw, 0, 1, 0);
   }

   control.proj = newProj;
}

// -------- Window Reshape -------- //
void reshape(int width, int height)
{
   //  Set the viewport to the entire window
   glViewport(0, 0, RES * width, RES * height);
   updateProjection(control.proj);
}

void key(unsigned char ch, int x, int y)
{
   if (ch == 'm')
   {
      auto next = (control.proj + 1) % 3;
      updateProjection(next);
      glutPostRedisplay();
   }
   else if (ch == 'M')
   {
      auto next = (control.proj - 1);
      if (next < 0)
         next = 2;
      updateProjection(next);
      glutPostRedisplay();
   }

   // WASD controls first person view movement
   auto motion = control.walkSpeed;
   if (control.proj == 2)
   {
      if (ch == 'w')
         adjustWalk(0, motion);
      else if (ch == 's')
         adjustWalk(0, -motion);
      else if (ch == 'a')
         adjustWalk(motion, 0);
      else if (ch == 'd')
         adjustWalk(-motion, 0);
      else if (ch == 'q')
         adjustLook(0, 0, -control.lookSpeed);
      else if (ch == 'e')
         adjustLook(0, 0, control.lookSpeed);

      glutPostRedisplay();
   }
}

// -------- Special -------- //
void special(int key, int x, int y)
{
   float motion = control.orbitSpeed;

   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      adjustLook(0, motion, 0);
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      adjustLook(0, -motion, 0);
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      adjustLook(-motion, 0, 0);
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      adjustLook(motion, 0, 0);
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
   glutCreateWindow("Projections");

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
