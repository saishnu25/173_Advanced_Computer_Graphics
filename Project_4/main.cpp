//Saishnu Ramesh Kumar (300758706)
//CSCI 173 - Project 4

#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <SOIL.h>

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

float i = 0, t = 0;
float Wwidth, Wheight;

float depth = 100.0;

float xMax = 1.0;
float xMin = 0.0;
float yMax = 0.0;
float yMin = 1.0;

clock_t startTime1; //First start time
clock_t startTime2; //Second start time

GLuint tex1; //For parallax texture
GLuint tex2; //For sprite sheet texture

const int numberControlPoints = 10;
float ctrlPoints[numberControlPoints][10];

int crntFrame = 0;

bool viewSwitch = false;

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

    gluPerspective (45.0f,Ratio,0.1f, depth);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//Texture Loader Function
void TLoad(char* fileName, GLuint& tex)
{
	int width, height;

	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, tex);

	image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGBA); // let soil load image
	if (!image){
	    cout << "Image not Found" << endl;
    }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

//Control Point Display
void controlPointDisplay()
{
    float x = -0.5;
    for (int i = 0; i < numberControlPoints; ++i)
    {
        ctrlPoints[i][0] = x;
        ctrlPoints[i][1] = float(rand()) / float(RAND_MAX);
        x += 0.5;
    }
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

//Bezier Curve
void bezierCurve(float t)
{
    float x = 0.0;
    float y = 0.0;
    for(int i = 0; i <= numberControlPoints; i++)
    {
        float bernsteinCoefficient = (factorial(numberControlPoints) / (factorial(i) * factorial(numberControlPoints - i))*(pow((1 - t),(numberControlPoints - i)))* pow(t,i));
        x += ctrlPoints[i][0] * bernsteinCoefficient;
        y += ctrlPoints[i][1] * bernsteinCoefficient;
    }
    glPushMatrix();
    glTranslatef(x, y - 0.8, 0.0);
    glutSolidSphere(0.01, 20, 20);
    glPopMatrix();
}

//Control Point Lines
void displayCtrlPointLines()
{
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(5.0f);
    for (int i = 0; i < numberControlPoints; ++i)
    {
        glBegin(GL_POINTS);
        glVertex3f(ctrlPoints[i][0], ctrlPoints[i][1] - 0.8, 0.0);
        glEnd();
    }
}

//Control Point Movement
void ctrlPointsMove()
{
    for (int i = 0; i < numberControlPoints - 1; ++i)
    {
        ctrlPoints[i][0] = ctrlPoints[i + 1][0] - 0.5;
        ctrlPoints[i][1] = ctrlPoints[i + 1][1];
    }
    ctrlPoints[numberControlPoints - 1][0] = ctrlPoints[numberControlPoints - 2][0] + 0.5;
    ctrlPoints[numberControlPoints - 1][1] = float(rand()) / float(RAND_MAX);
}

//Tangent Line
void tangentLine(float t)
{
    float x = 0.0, y = -1.0;
    for (int i = 0; i < numberControlPoints; ++i)
    {
        float bernsteinCoefficient = (factorial(numberControlPoints - 1)/(factorial(i) * factorial(numberControlPoints - 1 - i)))*pow(t, i) * pow(1 - t, numberControlPoints - 1 - i);
        x += ctrlPoints[i][0] * bernsteinCoefficient;
        y += ctrlPoints[i][1] * bernsteinCoefficient;
    }
}

//Displays Filled Color Under Curve
void areaUnderCurve(){

    glColor3f(2.0f, 1.5f, 1.0f);
    glBegin(GL_TRIANGLE_STRIP);
    for (float t = 0.0; t <= 1.0; t += 0.01)
    {
        float x = 0.0, y = 0.0;
        for (int i = 0; i < numberControlPoints; ++i)
        {
            float bernsteinCoefficient = (factorial(numberControlPoints - 1)/(factorial(i) * factorial(numberControlPoints - 1 - i)))*pow(t, i) * pow(1 - t, numberControlPoints - 1 - i);
            x += ctrlPoints[i][0] * bernsteinCoefficient;
            y += ctrlPoints[i][1] * bernsteinCoefficient;
        }
        glVertex3f(x, y - 0.8, 0.0);
        glVertex3f(x, -1.05, 0.0);
    }
    glEnd();
}

//Parallax Function
void parallax(float width, float height){

    glDisable(GL_LIGHTING);
    glScalef(10.0f, 10.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glBegin(GL_QUADS);
        glTexCoord2f(xMin, yMin); glVertex3f(-1*Wwidth/Wheight, -1, -8.0);
        glTexCoord2f(xMax, yMin); glVertex3f(Wwidth/Wheight, -1, -8.0);
        glTexCoord2f(xMax, yMax); glVertex3f(Wwidth/Wheight,  1, -8.0);
        glTexCoord2f(xMin, yMax); glVertex3f(-1*Wwidth/Wheight,  1, -8.0);
    glEnd();
}

//Sprite Sheet Function
void spriteSheet(float width, float height, int xfrm, int yfrm, int totalXFrm, int totalYFrm, float t) {
    glBindTexture(GL_TEXTURE_2D, tex2);

    float x = 0.0, y = 0.0;
    for (int i = 0; i < numberControlPoints; ++i) {
        float bernsteinCoefficient = (factorial(numberControlPoints - 1)/(factorial(i) * factorial(numberControlPoints - 1 - i)))*pow(t, i) * pow(1 - t, numberControlPoints - 1 - i);
        x += ctrlPoints[i][0] * bernsteinCoefficient;
        y += ctrlPoints[i][1] * bernsteinCoefficient;
    }

    float spriteX = float(xfrm) / float(totalXFrm);
    float spriteY = float(yfrm) / float(totalYFrm);

    glBegin(GL_QUADS);
        glTexCoord2f(spriteX, spriteY); glVertex3f(x - (width / 2.0), y - (height / 2.0), 0.0);
        glTexCoord2f(spriteX + (1.0 / float(totalXFrm)), spriteY); glVertex3f(x + (width / 2.0), y - (height / 2.0), 0.0);
        glTexCoord2f(spriteX + (1.0 / float(totalXFrm)), spriteY + (1.0 / float(totalYFrm))); glVertex3f(x + (width / 2.0), y + (height / 2.0), 0.0);
        glTexCoord2f(spriteX, spriteY + (1.0 / float(totalYFrm))); glVertex3f(x - (width / 2.0), y + (height / 2.0), 0.0);
    glEnd();
}

//Timer Function
void timer(int)
{
    crntFrame = (crntFrame + 1) % 5; // Number of frames in animation
    glutPostRedisplay();
    glutTimerFunc(100, timer, 0); // Animation speed
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(viewSwitch){
        gluLookAt(0, 0, 30, 1, 0.0, -30, 0.0, 1.0, 0.0);
    }
    else{
        gluLookAt(5, 0, 11, 1, 0.0, -30, 0.0, 1.0, 0.0);
    }

    if(WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		//Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		//Toggle WIRE FRAME

    //your code here
    glPushMatrix();
    parallax(Wwidth, Wheight); //Parallax and Bezier Curve render
    for (float t = 0.0; t <= 1.0; t += 0.002)
    {
        bezierCurve(t);
        for (float t = 0.0; t <= 1.0; t += 0.01)
        {
            tangentLine(t);
        }
    }
    displayCtrlPointLines();
    areaUnderCurve();
    glPopMatrix();

    //Sprite sheet render
    glPushMatrix();
    glTranslated(0, 0,0);
    for (int i = 0; i < numberControlPoints; ++i) {
        spriteSheet(2.5f, 2.0f, crntFrame, 0, 5, 2, 1.0f);
    }
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

    //Spacebar to change point of view
    case ' ':
        viewSwitch = !viewSwitch;
        break;
    }
}

static void idle(void)
{
    // Use parametric equation with t increment for xpos and y pos
    // Don't need a loop
    if(clock() - startTime1 > 20)
    {
        xMin += 0.001;
        xMax += 0.001;
    }
    if (clock() - startTime2 > 300)
    {
        ctrlPointsMove();
        startTime2 = clock();
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

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &tex1);
    TLoad("images/background.png",tex1);

    glGenTextures(1, &tex2);
    TLoad("images/human.png", tex2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    controlPointDisplay();

}

/* Program entry point */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1280,720);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("2D Animation (Project 4)");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMainLoop();
    return EXIT_SUCCESS;
}

