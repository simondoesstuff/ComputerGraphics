#include "CSCIx229.h"

#include "models/buff.h"
#include "loadShader.h"

struct OrbitParams
{
   float pitch = 25;
   float yaw = 0;
   const float speed = 10;
   const float dim = 10;
} orbit;

struct LightParams {
   float speed = 1;
   float radius = 7.5;
   float height = 4;
   Vec3 pos = {0, height, 0};

   Vec3 globalAmbient = Vec3 {1,1,1} * .2;
   
   float intensity = 3;
   Vec3 color = (Vec3 {.5, 1, 1}).normalized() * intensity;
   float ambient = .2;
   float diffuse = 1;
   float specular = 1;
   
   float shininess = 16;
   bool directional = false;
   
   // these need to be freed
   float *ambientArray = (color * ambient).toArray();
   float *diffuseArray = (color * diffuse).toArray();
   float *specularArray = (color * specular).toArray();
} light;

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
   
    {
      //----------------- Draw the light source
      glColor3f(light.color.x, light.color.y, light.color.z);
      glPushMatrix();
      glTranslated(light.pos.x, light.pos.y, light.pos.z);
      glutSolidSphere(.1, 10, 10);
      glPopMatrix();
    }

   scene->draw();
   glutSwapBuffers();
   glFlush();
}

// -------- Idle Hook -------- //
void idle()
{
   // -------------- Handle orbitting light source
   float dt = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   light.pos.x = light.radius * cos(dt * light.speed);
   light.pos.z = light.radius * sin(dt * light.speed);
   int directional = !light.directional ? 1 : 0;
   const float pos[] = {light.pos.x, light.pos.y, light.pos.z, (float) directional};
   glLightfv(GL_LIGHT0, GL_POSITION, pos);
   glutPostRedisplay();
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
   auto theme = Vec3{.4, .5, 1}.normalized();
   auto bgColor = theme * .2;
   auto floorColor = theme * .8; 
   
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
                        floorColor)
                    .extrude({0, -.1, 0})
                    ->boxed()
                    ->scale({16, 1, 16})
                    ->move({-8, -2, -8});

   scene = new Box({mainBuff, altBuff, altBuff2, altBuff3, altBuff4, floor});
}

void initLighting() {
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
   glMaterialf(GL_FRONT, GL_SHININESS, light.shininess);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
   glShadeModel(GL_SMOOTH);

   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light.ambientArray);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light.diffuseArray);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light.specularArray);
   
   // disable default light
   auto global_ambient = light.globalAmbient.toArray();
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); // set global ambient light
   delete global_ambient;
   
   // load shader
   int shader1 = CreateShaderProg("pixlight.vert", "pixlight.frag");
   glUseProgram(shader1);
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

   auto version = glGetString(GL_VERSION);
   printf("OpenGL version: %s\n", version);

   initScene();
   initLighting();

   glutDisplayFunc(draw);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(key);
   glutSpecialFunc(special);
   glutVisibilityFunc(visible);

   glutMainLoop();
   return 0; /* ANSI C requires main to return int. */
}
