//Saishnu Ramesh Kumar (300758706)
//CSCI 173 - Project 1

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
#define PI 3.14159

using namespace std;
bool WireFrame= false;

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

clock_t startTime;
GLfloat camera = 0; //Using arrow keys to rotate left and right

float xpos = 0, ypos = 0, zpos = 0; //x, y, z pos
float xp, yp, xm, ym; //x and y position and mouse
float t; //Timer
float Wwidth,Wheight;

const int maxControlPoints = 15; //Max Control Points
int numberControlPoints = 0;  //Number of control points
float ctrlPoints[maxControlPoints][3]; //Control Points Array

bool spaceBar = false; // Toggle spacebar to rest the whole scene
bool displayControlPoints = true; //Displays control points of curve

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
    gluPerspective (45.0f,Ratio,0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//Factorial
float factorial(int n)
{
    int result = 1;
    for(int i = 1; i <= n; i++)
    {
        result *= i;
    }
    return result;
}

//Binomial Coefficient
float binomialCoefficient(int n, int i)
{
    return (factorial(n)/((factorial(i)) * factorial(n - i)));
}

//Bezier Curve
void bezierCurve(float t, float &x, float &y, float &z)
{
    x = 0;
    y = 0;
    z = 0;
    for(int i = 0; i <= numberControlPoints; i++)
    {
        float bernsteinCoefficient = (factorial(numberControlPoints) / (factorial(i) * factorial(numberControlPoints - i))*(pow((1 - t),(numberControlPoints - i)))* pow(t,i));
        x += ctrlPoints[i - 1][0] * bernsteinCoefficient;
        y += ctrlPoints[i - 1][1] * bernsteinCoefficient;
        z += ctrlPoints[i - 1][2] * bernsteinCoefficient;
    }
}

//Bezier Display
void bezierDisplay()
{
    glBegin(GL_POINTS);
    //Helps generate the bezier curve
    for(float t = 0; t <= 1; t += 0.01)
    {
        float x, y, z;
        bezierCurve(t, x, y, z);
        glVertex3f(x, y, z);
    }
    glEnd();
}

//Control Point Display
void controlPointDisplay()
{
    glBegin(GL_POINTS);
    //Generates the 3 control points
    for (int i = 0; i <= numberControlPoints; i++)
    {
        if (ctrlPoints[i][0] == 0 && ctrlPoints[i][1] == 0 && ctrlPoints[i][2] == 0)
        {
            break;
        }
        glVertex3f(ctrlPoints[i - 1][0], ctrlPoints[i - 1][1], ctrlPoints[i - 1][2]);
    }
    glEnd();
}

static void idle(void)
{
    // Use parametric equation with t increment for xpos and y pos
    // Don't need a loop
    if(clock() - startTime > 10)
    {
        t += 0.01;
        startTime = clock();
    }
    xpos = 0;
    ypos = 0;
    zpos =0;
    for(int i = 0; i <= numberControlPoints; i++)
    {
        float binomial = binomialCoefficient(numberControlPoints, i);
        float ti = pow(t, i);
        float ti1 = pow(1 - t, numberControlPoints-i);

        xpos += binomial * ti1 * ti * ctrlPoints[i - 1][0];
        ypos += binomial * ti1 * ti * ctrlPoints[i - 1][1];
        zpos += binomial * ti1 * ti * ctrlPoints[i - 1][2];
    }
    if(t > 1) t = 1;

    glutPostRedisplay();
}

//Mouse
void mouse(int btn, int state, int x, int y)
{
    // get new mouse coordinates for x, y
    // use scale to match right
    float scale = 70 * (Wwidth/Wheight);
    xm = (float) (x-Wwidth/2)/scale;
    ym = (float) (Wheight/2-y)/scale;

    switch(btn)
    {
    //Left mouse click to plot coordinate points to draw curve
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN)
        {
            if(numberControlPoints < 15)
            {
                float xCoordinate = xm;
                float yCoordinate = ym;

                ctrlPoints[numberControlPoints][0] = xCoordinate;
                ctrlPoints[numberControlPoints][1] = yCoordinate;
                ctrlPoints[numberControlPoints][2] = rand() % 3;

                numberControlPoints++;

                t = 0;
                xp = xpos;
                yp = ypos;
            }

        }
        break;

    //Right mouse click to toggle between hide/show control points
    case GLUT_RIGHT_BUTTON:

        if(state == GLUT_DOWN){
            displayControlPoints = !displayControlPoints;
        }
        break;

    //Middle mouse click to RESET
    case GLUT_MIDDLE_BUTTON:
        if(spaceBar == GLUT_DOWN){
            camera = 0.0;
            numberControlPoints = 0;
            spaceBar = false;
        }
        break;
    }
    glutPostRedisplay();
};

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,0,10,0.0,0.0,0.0,0.0,1.0,100.0);

    if(WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //Toggle WIRE FRAME

    //your code here
    //Rotates the camera left and right
    glRotatef(camera, 0.0, 5.0, 0.0);

    //Generates the control points
    glPushMatrix();
    if(displayControlPoints)
    {
        glPointSize(5);
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_POINTS);
        for (int i = 0; i < numberControlPoints; i++)
        {
            glVertex3f(ctrlPoints[i][0], ctrlPoints[i][1], ctrlPoints[i][2]);
        }
    glEnd();
    }
    glPopMatrix();

    //Generates the control points
    glPushMatrix();
    glPointSize(2);
    glBegin(GL_POINTS);
    for(float t = 0; t <= 1; t += 0.001)
    {
        float x, y, z;
        bezierCurve(t, x, y, z);
        glVertex3f(x, y, z);
    }
    glEnd();
    glPopMatrix();

    //Generates the sphere
    glPushMatrix();
    glColor3f(0.0, 1.0, 2.5);
    glTranslatef(xpos, ypos, zpos);
    glutSolidSphere(0.5,40,40);
    glPopMatrix();

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

    case 'w':
        WireFrame =!WireFrame;
        break;

    //Repeats animation by pressing ENTER key
    case 13:
        for(int i = 0; i < 15; i++){
            t=0;
        }
        break;

    //Spacebar to RESET
    case ' ':
        spaceBar = true;
        if(spaceBar){
            camera = 0.0;
            numberControlPoints = 0;
            spaceBar = false;
        }
        break;
    }
}

void Specialkeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT : //Rotate camera left
        camera -= 2.0;
        break;

    case GLUT_KEY_RIGHT : //Rotate camera right
        camera += 2.0;
        break;
    }
    glutPostRedisplay();
}

static void init(void)
{
    glClearColor(0.5f, 0.5f, 1.0f, 0.0f); // assign a color you like

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

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    xp = 0;
    yp = 0;

    xm = 4.0;
    ym = 4.0;

    startTime = clock();
}
/* Program entry point */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Bezier Curve (Project 1)");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);
    glutIdleFunc(idle);
    glutMainLoop();
    return EXIT_SUCCESS;
}

