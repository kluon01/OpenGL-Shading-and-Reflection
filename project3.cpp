//---------------------------------------
// Program: project3.cpp
// Purpose: Create and display a background terrain surface
//          using Gourand shading and distance weighted diffuse reflection model
// Author:  Karshin Luong, John Gauch
// Date:    3/13/2019
//---------------------------------------
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Transformation variables
#define ROTATE 1
#define LIGHT1 2
#define LIGHT2 3
int xangle = 0;
int yangle = 0;
int zangle = 0;
int xpos = 0;
int ypos = 0;
int zpos = 0;
int mode = ROTATE;

// Light 1 global position variables
float Lx1 = 0.8;
float Ly1 = 1.0;
float Lz1 = 1.0;
// Light 2 global position variables
float Lx2 = 2.0;
float Ly2 = 1.0;
float Lz2 = 1.0;
// Light 1 color variables
float Lr1 = 0.0;
float Lg1 = 0.0;
float Lb1 = 1.0;
// Light 2 color variables
float Lr2 = 1.0;
float Lg2 = 1.0;
float Lb2 = 1.0;

// Light reach for each light
float lightreach1;
float lightreach2;

// Surface variables
#define SIZE 128
float Px[SIZE + 1][SIZE + 1];
float Py[SIZE + 1][SIZE + 1];
float Pz[SIZE + 1][SIZE + 1];
float Nx[SIZE + 1][SIZE + 1];
float Ny[SIZE + 1][SIZE + 1];
float Nz[SIZE + 1][SIZE + 1];
float R[SIZE + 1][SIZE + 1];
float G[SIZE + 1][SIZE + 1];
float B[SIZE + 1][SIZE + 1];
#define STEP 0.1

using namespace std;

float calcLightReach(float Lx, float Ly, float Lz, int i, int j)
{
  float a = 0.25;
  float b = 0.25;
  float c = 0.25;
  float Vx, Vy, Vz;
  float magnitude = 0.0;
  float lightDistMultiplier = 0.0;
  float dot = 0.0;
  float euclideanD = 0.0;
  // Calculate V from vertex point to light source
  Vx = Lx - Px[i][j];
  Vy = Ly - Py[i][j];
  Vz = Lz - Pz[i][j];
  // Calculate dot product of V with surface normal N
  dot =  Vx * Nx[i][j] + Vy * Ny[i][j] + Vz * Nz[i][j];
  // Multiply by 1/(a+bD+cD^2) where D = distance from light to vertex
  euclideanD = sqrt((Vx*Vx) + (Vy * Vy) + (Vz * Vz));
  lightDistMultiplier = 1/(a+b*euclideanD+c*euclideanD*euclideanD);
  magnitude = lightDistMultiplier * dot;
  return magnitude;
}

//---------------------------------------
// Calculate random value between [min..max]
//---------------------------------------
float myrand(float min, float max)
{
   return rand() * (max - min) / RAND_MAX + min;
}

//---------------------------------------
// Initialize surface points
//---------------------------------------
void init_surface(float Xmin, float Xmax, float Ymin, float Ymax)
{
   // Initialize (x,y,z) coordinates of surface
   for (int i = 0; i <= SIZE; i++)
   for (int j = 0; j <= SIZE; j++)
   {
      Px[i][j] = Xmin + i * (Xmax - Xmin) / SIZE;
      Py[i][j] = Ymin + j * (Ymax - Ymin) / SIZE;
      Pz[i][j] = 0;
   }

   // Add randoms waves to surface
   for (int wave = 1; wave <= 10; wave++)
   {
      int rand_i = rand() % SIZE / wave;
      int rand_j = rand() % SIZE / wave;
      float length = sqrt(rand_i * rand_i + rand_j * rand_j);
      if (length >= 40)
      for (int i = 0; i <= SIZE; i++)
      for (int j = 0; j <= SIZE; j++)
      {
         float angle = (rand_i * i + rand_j * j) / (length * length);
         Pz[i][j] += 0.01 * sin(angle * 2 * M_PI);
      }
   }
}

//---------------------------------------
// Initialize surface normals
//---------------------------------------
void init_normals()
{
   // Initialize surface normals
   for (int i=0; i<=SIZE; i++)
   for (int j=0; j<=SIZE; j++)
   {
      // Get tangents S and T
      float Sx = (i<SIZE) ? Px[i+1][j] - Px[i][j] : Px[i][j] - Px[i-1][j];
      float Sy = (i<SIZE) ? Py[i+1][j] - Py[i][j] : Py[i][j] - Py[i-1][j];
      float Sz = (i<SIZE) ? Pz[i+1][j] - Pz[i][j] : Pz[i][j] - Pz[i-1][j];
      float Tx = (j<SIZE) ? Px[i][j+1] - Px[i][j] : Px[i][j] - Px[i][j-1];
      float Ty = (j<SIZE) ? Py[i][j+1] - Py[i][j] : Py[i][j] - Py[i][j-1];
      float Tz = (j<SIZE) ? Pz[i][j+1] - Pz[i][j] : Pz[i][j] - Pz[i][j-1];

      // Calculate N = S cross T
      float Slength = sqrt(Sx*Sx + Sy*Sy + Sz*Sz);
      float Tlength = sqrt(Tx*Tx + Ty*Ty + Tz*Tz);
      if ((Slength * Tlength) > 0)
      {
         Nx[i][j] = (Sy*Tz - Sz*Ty) / (Slength * Tlength);
         Ny[i][j] = (Sz*Tx - Sx*Tz) / (Slength * Tlength);
         Nz[i][j] = (Sx*Ty - Sy*Tx) / (Slength * Tlength);
      }
   }
}

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
   // Initialize OpenGL
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float radius = 1.2;
   glOrtho(-radius, radius, -radius, radius, -radius, radius);
   glEnable(GL_DEPTH_TEST);

   // Initialize random light colors
   Lr1 = myrand(0.2,1.0);
   Lg1 = myrand(0.2,1.0);
   Lb1 = myrand(0.2,1.0);
   Lr2 = myrand(0.2,1.0);
   Lg2 = myrand(0.2,1.0);
   Lb2 = myrand(0.2,1.0);

   // Initialize surface
   init_surface(-1.0, 1.0, -1.0, 1.0);
   init_normals();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
   // Incrementally rotate objects
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(xpos / 500.0, ypos / 500.0, zpos / 500.0);
   glRotatef(xangle, 1.0, 0.0, 0.0);
   glRotatef(yangle, 0.0, 1.0, 0.0);
   glRotatef(zangle, 0.0, 0.0, 1.0);

   // Draw line showing light 1 position
   glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(Lx1,Ly1,Lz1);
   glEnd();

   // Draw line showing light 2 position
   glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(Lx2,Ly2,Lz2);
   glEnd();

   // Loop through every vertex point and calculate the light reach for each
   for (int i = 0; i <= SIZE; i++)
      for (int j = 0; j <= SIZE; j++)
      {
        lightreach1 = calcLightReach(Lx1,Ly1,Lz1,i,j);
        lightreach2 = calcLightReach(Lx2,Ly2,Lz2,i,j);
        R[i][j] = Lr1 * lightreach1 + Lr2 * lightreach2;
        G[i][j] = Lg1 * lightreach1 + Lg2 * lightreach2;
        B[i][j] = Lb1 * lightreach1 + Lb2 * lightreach2;
      }

   // Draw the surface
   int i, j;
   for (i = 0; i < SIZE; i++)
      for (j = 0; j < SIZE; j++)
      {
	 //glBegin(GL_LINE_LOOP);
	 glBegin(GL_POLYGON);
	 glColor3f(R[i][j], G[i][j], B[i][j]);
	 glVertex3f(Px[i][j], Py[i][j], Pz[i][j]);
	 glColor3f(R[i][j], G[i][j], B[i][j]);
	 glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j]);
	 glColor3f(R[i][j], G[i][j], B[i][j]);
	 glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1]);
	 glColor3f(R[i][j], G[i][j], B[i][j]);
	 glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1]);
	 glEnd();
      }
   glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y)
{
   // Initialize random surface
   if (key == 'i')
   {
      Lr1 = myrand(0.5,1.0);
      Lg1 = myrand(0.5,1.0);
      Lb1 = myrand(0.5,1.0);
      Lr2 = myrand(0.5,1.0);
      Lg2 = myrand(0.5,1.0);
      Lb2 = myrand(0.5,1.0);
      init_surface(-1.0, 1.0, -1.0, 1.0);
      init_normals();
   }

   // ROTATE
   if (key == '3')
   {
      printf("Type x y z to decrease or X Y Z to increase ROTATION angles.\n");
      mode = ROTATE;
   }
   // Light 1 control
   else if (key == '1')
   {
     printf("Light 1 controls.\n");
     printf("Type x, y, z to decrease angles.\n");
     printf("Type X, Y, Z to increase angles.\n");
     printf("Type r, g, b to decrease red, green, or blue color value of the light.\n");
     printf("Type R, G, B to increase red, green, or blue color value of the light.\n");
     mode = LIGHT1;
   }
   // Light 2 control
   else if (key == '2')
   {
     printf("Light 2 controls.\n");
     printf("Type x, y, z to decrease angles.\n");
     printf("Type X, Y, Z to increase angles.\n");
     printf("Type r, g, b to decrease red, green, or blue color value of the light.\n");
     printf("Type R, G, B to increase red, green, or blue color value of the light.\n");
     mode = LIGHT2;
   }

   // Handle ROTATE
   if (mode == ROTATE)
   {
      if (key == 'x')
	 xangle -= 5;
      else if (key == 'y')
	 yangle -= 5;
      else if (key == 'z')
	 zangle -= 5;
      else if (key == 'X')
	 xangle += 5;
      else if (key == 'Y')
	 yangle += 5;
      else if (key == 'Z')
	 zangle += 5;
   }
   // Handle light 1
   if (mode == LIGHT1)
   {
    if (key == 'x')
    Lx1 -= 0.1;
        else if (key == 'y')
    Ly1 -= 0.1;
        else if (key == 'z')
    Lz1 -= 0.1;
        else if (key == 'X')
    Lx1 += 0.1;
        else if (key == 'Y')
    Ly1 += 0.1;
        else if (key == 'Z')
    Lz1 += 0.1;
        if (key == 'r')
    Lr1 -= 0.1;
        else if (key == 'g')
    Lg1 -= 0.1;
        else if (key == 'b')
    Lb1 -= 0.1;
        else if (key == 'R')
    Lr1 += 0.1;
        else if (key == 'G')
    Lg1 += 0.1;
        else if (key == 'B')
    Lb1 += 0.1;
   }
   // Handle light 2
   if (mode == LIGHT2)
   {
        if (key == 'x')
    Lx2 -= 0.1;
        else if (key == 'y')
    Ly2 -= 0.1;
        else if (key == 'z')
    Lz2 -= 0.1;
        else if (key == 'X')
    Lx2 += 0.1;
        else if (key == 'Y')
    Ly2 += 0.1;
        else if (key == 'Z')
    Lz2 += 0.1;
        if (key == 'r')
    Lr2 -= 0.1;
        else if (key == 'g')
    Lg2 -= 0.1;
        else if (key == 'b')
    Lb2 -= 0.1;
        else if (key == 'R')
    Lr2 += 0.1;
        else if (key == 'G')
    Lg2 += 0.1;
        else if (key == 'B')
    Lb2 += 0.1;
   }

   glutPostRedisplay();
}


//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[])
{
   // Create OpenGL window
   glutInit(&argc, argv);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(250, 250);
   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
   glutCreateWindow("Gourand");
   init();
   printf("Type 1 to enter Light 1 control mode.\n");
   printf("Type 2 to enter Light 2 control mode.\n");
   printf("Type 3 to enter ROTATE mode\n");

   // Specify callback function
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
