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

#include "generate.cpp"
#include "hsv2rgb.cpp"
#include <vector>

std::vector<lorenzPoint> points;
float vMin = 500000;
float vMax = 0;
const float orbitSpeed = 2.5;
const float paramAdjustmentSensitivity = .33;
float pitch = 0;
float yaw = 0;

float t0 = 0;
float pointDeltaTime = 0;

struct lorenzParams
{
   float s;
   float b;
   float r;
   float speed;
} lorenzParams = {10.0, 2.6667, 28.0, 0.0003};

void erasePoints()
{
   points = {{10, 12, 25, 0}};
}

void addPoint()
{
   float dt = lorenzParams.speed * pointDeltaTime;
   auto lastPoint = points.back();
   auto nextPoint = marchPoint(lastPoint, dt, lorenzParams.s, lorenzParams.b, lorenzParams.r);
   points.push_back(nextPoint);
   
   if (nextPoint.velocity < vMin) {
      vMin = nextPoint.velocity;
   }
   
   if (nextPoint.velocity > vMax) {
      vMax = nextPoint.velocity;
   }
}

void velocityToColor(float velocity, float color[3]) {
   float t = 0;
   if (vMax - vMin != 0) {
      t = (velocity - vMin) / (vMax - vMin);
   }
   
   float r, g, b;
   float h = t * 360;
   float s = 1;
   float v = 1;
   HSVtoRGB(r, g, b, h, s, v);
   
   // return
   color[0] = r;
   color[1] = g;
   color[2] = b;
}

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
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   //  Set view angle
   glRotatef(pitch, 1, 0, 0);
   glRotatef(yaw, 0, 1, 0);

   {
      glPushMatrix();
      const float scale = .03;
      glScalef(scale, scale, scale);

      glBegin(GL_LINES);
      glLineWidth(1.0f);

      for (int i = 0; i < points.size() - 1; i++)
      {
         auto pointA = points[i];
         auto pointB = points[i + 1];

         float color[3];
         velocityToColor(pointB.velocity, color);
         glColor3f(color[0], color[1], color[2]);
         glVertex3f(pointA.x, pointA.y, pointA.z);
         glVertex3f(pointB.x, pointB.y, pointB.z);
      }

      glEnd();
      glPopMatrix();
   }

   {
      //----------------- Axis
      glColor3f(.3, .3, .3);
      //  Draw axes
      const double len = 1.5; //  Length of axes
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
   
   {
      //-------------------- Specs
      glWindowPos2i(5, 5);
      Print("S %.2f, B %.2f, R %.2f", lorenzParams.s, lorenzParams.b, lorenzParams.r);
   }

   glutSwapBuffers();
   glFlush();
}

// -------- Idle Hook -------- //
void idle()
{
   const float newPointThreshold = .003;
   float dt = lorenzParams.speed * pointDeltaTime;
   
   if (dt > newPointThreshold) {
      addPoint();
      glutPostRedisplay();
      pointDeltaTime = 0;
      t0 = glutGet(GLUT_ELAPSED_TIME);
   }
   
   if (t0 == 0) t0 = glutGet(GLUT_ELAPSED_TIME);
   pointDeltaTime = glutGet(GLUT_ELAPSED_TIME) - t0;
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
   const double dim = 2.5;
   double asp = (height > 0) ? (double)width / height : 1;
   glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void key(unsigned char ch, int x, int y) {
   switch (ch) {
      case 's':
         lorenzParams.s += paramAdjustmentSensitivity;
         break;
      case 'S':
         lorenzParams.s -= paramAdjustmentSensitivity;
         break;
      case 'b':
         lorenzParams.b += paramAdjustmentSensitivity;
         break;
      case 'B':
         lorenzParams.b -= paramAdjustmentSensitivity;
         break;
      case 'r':
         lorenzParams.r += paramAdjustmentSensitivity;
         break;
      case 'R':
         lorenzParams.r -= paramAdjustmentSensitivity;
         break;
      case ' ':
         erasePoints();
         glutPostRedisplay();
         break;
      default:
         return;
   }
}

// -------- Special -------- //
void special(int key, int x, int y)
{
   float motion = orbitSpeed;

   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      yaw += motion;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      yaw -= motion;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      pitch += motion;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      pitch -= motion;
   //  Keep angles to +/-360 degrees

   if (yaw > 360)
      yaw -= 360;
   if (yaw < 0)
      yaw += 360;
   if (pitch > 360)
      pitch -= 360;
   if (pitch < 0)
      pitch += 360;
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

int main(int argc, char *argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc, argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600, 600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   //  Create the window
   glutCreateWindow("Lorenz Attractor");

#ifdef USEGLEW
   if (glewInit() != GLEW_OK)
   {
      fprintf(stderr, "Error initializing GLEW\n");
      exit(1);
   }
#endif

   erasePoints();

   glutDisplayFunc(draw);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(key);
   glutSpecialFunc(special);
   glutVisibilityFunc(visible);

   glutMainLoop();
   return 0; /* ANSI C requires main to return int. */
}
