// Saishnu Ramesh Kumar (300758706)
// CSCI 173 - Project 2

#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <time.h>

#define radius 1.3 //Radius of sphere
using namespace std;

clock_t startTime;  // Clock

bool WireFrame= false;

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

GLfloat Wwidth, Wheight;
GLfloat x, y, z; // Position of balls
GLfloat xpos = 0, ypos = 0, zpos = 0; // x, y, z positions
GLfloat dx = 1, dy = 1, dz = 1; // Direction of ball travel
GLfloat speedX, speedY, speedZ; // Speed of ball traveling

GLfloat cameraAngle = 0, cameraZoom = 0; // To rotate left and right and to zoom in and out
GLfloat t = 0.05; // Ball speed
GLfloat bound = 4.0; // Sets boundary for collision
GLfloat q = bound - 0.1;
GLfloat spin = 0.0;
GLfloat sphereX = 0.0, sphereY = 0.0, sphereZ = 0.0; //For collision detection of spheres
GLfloat color[3]; // Changes color during collision

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    double Ratio;
    Wwidth = (float)width;
    Wheight = (float)height;
    Ratio= (double)width /(double)height;

    glViewport(0,0,(GLsizei) width,(GLsizei) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (50.0f, Ratio, 0.1f, 100.0f);
}

// Collision Function
int collision(float xpos, float ypos, float zpos)
{
    float distance = sqrt((xpos - sphereX) * (xpos - sphereX) + (ypos - sphereY) * (ypos - sphereY) + (zpos - sphereZ) * (zpos - sphereZ)); // Calculates the distance

    if (distance <= radius + 0.5) // Sphere collision detected
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;

        // Computes new direction vector for the ball
        float nx = (xpos - sphereX) / distance;
        float ny = (ypos - sphereY) / distance;
        float nz = (zpos - sphereZ) / distance;
        float n_length = sqrt(nx * nx + ny * ny + nz * nz);
        nx /= n_length;
        ny /= n_length;
        nz /= n_length;

        // Reflects the direction vector about the normal to the sphere's surface
        float dot = dx * nx + dy * ny + dz * nz;
        dx = dx - 2 * dot * nx;
        dy = dy - 2 * dot * ny;
        dz = dz - 2 * dot * nz;

        // Moves the ball back along its direction vector until it's just touching the sphere's surface
        xpos = sphereX + radius * nx;
        ypos = sphereY + radius * ny;
        zpos = sphereZ + radius * nz;
    }

    if (xpos >= bound) // Right wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dx = -dx;
        xpos = bound - radius;
        return 1;
    }

    if (xpos <= -bound) // Left wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dx = -dx;
        xpos = -bound + radius;
        return 2;
    }

    if (ypos >= bound) // Top wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dy = -dy;
        ypos = bound - radius;
        return 3;
    }

    if (ypos <= -bound) // Bottom wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dy = -dy;
        ypos = -bound + radius;
        return 4;
    }

    if (zpos >= bound) // Front wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dz = -dz;
        zpos = bound - radius;
        return 5;
    }

    if (zpos <= -bound) // Back wall hit
    {
        color[0] = (float)rand() / RAND_MAX;
        color[1] = (float)rand() / RAND_MAX;
        color[2] = (float)rand() / RAND_MAX;
        dz = -dz;
        zpos = -bound + radius;
        return 6;
    }
    return 0;
}

// Timer Function
static void timer(int value)
{

    float dirX = (float) (rand()) / (float) (RAND_MAX/q);
    float dirY = (float) (rand()) / (float) (RAND_MAX/q);
    float dirZ = (float) (rand()) / (float) (RAND_MAX/q);

    // Acts according to collision functions outcome
    if(collision(x,y,z) == 1)
    {
        dx = -dirX;
    }
    else if(collision(x,y,z) == 2)
    {
        dx = dirX;
    }
    else if(collision(x,y,z) == 3)
    {
        dy = -dirY;
    }
    else if(collision(x,y,z) == 4)
    {
        dy = dirY;
    }
    else if(collision(x,y,z) == 5)
    {
        dz = -dirZ;
    }
    else if(collision(x,y,z) == 6)
    {
        dz = dirZ;
    }

    glutPostRedisplay();
}

static void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 2, 20, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    if(WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      //Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);      //Toggle WIRE FRAME

    //your code here
    glTranslated(0, 0, cameraZoom); //To zoom in, and zoom out
    glRotatef(cameraAngle, 0.0, 5.0, 0.0);

    // Sphere Generation
    glPushMatrix();
    glTranslated(x,y,z); // Ball moves along these axis
    glColor3f(color[0], color[1], color[2]); // Random color generated per collision
    glutSolidSphere(radius,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-x,y,-z); // Ball moves along these axis
    glColor3f(color[0], color[1], color[2]); // Random color generated per collision
    glutSolidSphere(radius,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(x,-y,z); // Ball moves along these axis
    glColor3f(color[0], color[1], color[2]); // Random color generated per collision
    glutSolidSphere(radius,20,20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-x,y,z); // Ball moves along these axis
    glColor3f(color[0], color[1], color[2]); // Random color generated per collision
    glutSolidSphere(radius,20,20);
    glPopMatrix();

    // Wireframe Cube Generation
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glutWireCube(10); //Wireframe Cube
    glPopMatrix();

    glutTimerFunc(500,timer,0);

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'q':
        exit(0);
        break;
    }
}

void Specialkeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT : //Rotate camera left
        cameraAngle += 10.0;
        break;

    case GLUT_KEY_RIGHT : //Rotate camera right
        cameraAngle -= 10.0;
        break;

    case GLUT_KEY_UP : //Zoom in with camera
        cameraZoom += 0.5;
        break;

    case GLUT_KEY_DOWN : //Zoom out with camera
        cameraZoom -= 0.5;
        break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    if(clock() - startTime > 25) // Slows down the sphere's movement in the cube
    {
        spin += 0.5;
        speedX = t * (dx); // Speed multiplied by direction traveling to
        x = x +  speedX; // Adds the speed to current position

        speedY = t * (dy); // Speed multiplied by direction traveling to
        y = y + speedY; // Adds the speed to current position

        speedZ = t * (dz); // Speed multiplied by direction traveling to
        z = z + speedZ; // Adds the speed to current position

        startTime = clock();
    }

    glutPostRedisplay();
}

static void init(void)
{

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
}

/* Program entry point */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(800,600);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Moving Spheres (Project 2)");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);
    glutIdleFunc(idle);
    glutMainLoop();

    return EXIT_SUCCESS;
}
