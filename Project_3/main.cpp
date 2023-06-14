//Saishnu Ramesh Kumar (300758706)
//CSCI 173 - Project 3 (Model Loader with VBO)

#include <string.h>

#define GLEW_STATIC     // Static
#include <GL/glew.h>    // GLEW Library
#include <GL/gl.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <math.h>
#include <SOIL.h>

#define PI 3.14159

using namespace std;

bool WireFrame= false;
float i = 0;

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

float xpos =0;
float ypos =0;
float cameraZoom = 0;
float Wwidth,Wheight;

//Quaternion Rotations
GLfloat matrixX[16];
GLfloat matrixY[16];
GLfloat matrixZ[16];
GLfloat x, y, z, w;

static GLint RotateX=0; // model rotation X index
static GLint RotateY=0; // model rotation Y index
static GLint RotateZ=0; // model rotation Z index
GLuint vboID, iboID;    // VBO Handler (Vertexes) and IBO Handler (Indexes)
GLuint tex;

//Models to load
int cow, trunk, leaves;
int currentModel; //Pick which model will show up

bool hasVT = false; //Checks if VT exists

typedef struct Vec3 //v && vn && face
{
    float x; //The x-axis
    float y; //The y-axis
    float z; //The z-axis
}Vec3; //Three for each coords

typedef struct Vec2 //vt
{
    float x; //x-axis
    float y; //y-axis
}Vec2; //vt only has two points

typedef struct Vec4 // face quad
{
    int ver1, ver2, ver3, ver4, vext1, vext2, vext3, vext4, vnorm1, vnorm2, vnorm3, vnorm4;
    int count;
}Vec4; //For the face that has a quad

int ver = 10; // The ver stands for vertex
int verT = 10; // The verT stands for vertex Texture
int face = 10; // For face
int verN = 10; // The verN stands for vertex Normals

Vec3* normals = (Vec3*)malloc(verN * sizeof(Vec3));
Vec2* textureV = (Vec2*)malloc(verT * sizeof(Vec2));
Vec3* vertexs = (Vec3*)malloc(ver * sizeof(Vec3));
Vec4* faces = (Vec4*)malloc(face * sizeof(Vec4));

/* GLUT callback Handlers */
//-- Load Object Function --
int loadObject(char* filename)
{
    int a = 1, b = 1, c = 1, f = 0, faceAmount = 0;
    ifstream file;
    file.open(filename); //Opens the file
    string line;
    if (!file.is_open())
    {
        cout << "File not opened!";
        return -1;
    }
    while (getline(file, line)) //While getting each line from the file it will enter the function.
    {
        string prefix = line.substr(0, 2); //Detects char

        if (prefix == "v ") //If v is detected, enters function
        {
            struct Vec3 vertices;
            sscanf(line.c_str(), "v %f %f %f", &vertices.x, &vertices.y, &vertices.z); //Reads and stores the data into float variables
            vertexs[a] = vertices; //Stores the structure in an array
            a++;
            if (a >= ver)
            {
                vertexs = (Vec3*)realloc(vertexs, 2 * ver * sizeof(Vec3));
                ver *= 2;   //Doubles the global variable for re-allocation for the dynamic array
            }
        }
        else if (prefix == "vt") //If vt is detected, enters function
        {
            hasVT = true;
            struct Vec2 vectex;
            sscanf(line.c_str(), "vt %f %f", &vectex.x, &vectex.y);
            textureV[b] = vectex;
            b++;
            if (b >= verT) //Increase size if needed
            {
                textureV = (Vec2*)realloc(textureV, 2 * verT * sizeof(Vec2));
                verT *= 2;
            }
        }
        else if (prefix == "vn") //If vn is detected, enters function
        {
            struct Vec3 vecnorm;
            sscanf(line.c_str(), "vn %f %f %f", &vecnorm.x, &vecnorm.y, &vecnorm.z);
            normals[c] = vecnorm;
            c++;
            if (c >= verN)
            {
                normals = (Vec3*)realloc(normals, 2 * verN * sizeof(Vec3));
                verN *= 2;
            }
        }
        else if (prefix == "f ") //If f is detected, enters function
        {
            faceAmount++;
            int counting = count(line.begin(), line.end(), '/'); //Counts the number of slashes in the face lists (6 - Triangle, 8 - Quad)
            struct Vec4 faceIn;
            if (counting == 6 && hasVT == true)
            {
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &faceIn.ver1, &faceIn.vext1, &faceIn.vnorm1, &faceIn.ver2, &faceIn.vext2, &faceIn.vnorm2, &faceIn.ver3, &faceIn.vext3, &faceIn.vnorm3); //Scans and read through the values to store
                faceIn.count = 3;
                faces[f] = faceIn;
                f++;
                if (f >= face)
                {
                    faces = (Vec4*)realloc(faces, 2 * face * sizeof(Vec4));
                    face *= 2;
                }
            }
            else if (counting == 6 && hasVT == false)
            {
                sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &faceIn.ver1, &faceIn.vnorm1, &faceIn.ver2, &faceIn.vnorm2, &faceIn.ver3, &faceIn.vnorm3); //Scans and read through the values to store
                faceIn.count = 3; // Number of faces in the current list is 3
                faces[f] = faceIn;
                f++;
                if (f >= face) //Increases the size if needed
                {
                    faces = (Vec4*)realloc(faces, 2 * face * sizeof(Vec4));
                    face *= 2;
                }
            }
            else if (counting == 8 && hasVT == true)
            {
                sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &faceIn.ver1, &faceIn.vext1, &faceIn.vnorm1, &faceIn.ver2, &faceIn.vext2, &faceIn.vnorm2, &faceIn.ver3, &faceIn.vext3, &faceIn.vnorm3, &faceIn.ver4, &faceIn.vext4, &faceIn.vnorm4); //Scans and read through the values to store
                faceIn.count = 4; // Number of faces in the current list is 4
                faces[f] = faceIn;
                f++;
                if (f >= face)
                {
                    faces = (Vec4*)realloc(faces, 2 * face * sizeof(Vec4));
                    face *= 2;
                }
            }
            else if (counting == 8 && hasVT == false)
            {
                sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &faceIn.ver1, &faceIn.vnorm1, &faceIn.ver2, &faceIn.vnorm2, &faceIn.ver3, &faceIn.vnorm3, &faceIn.ver4, &faceIn.vnorm4); //Scans and read through the values to store
                faceIn.count = 4;
                faces[f] = faceIn;
                f++;
                if (f >= face)
                {
                    faces = (Vec4*)realloc(faces, 2 * face * sizeof(Vec4));
                    face *= 2;
                }
            }

        }
    }

    int num;
    num = glGenLists(1);
    glNewList(num, GL_COMPILE);
    for (int i = 0; i < faceAmount; i++)
    {
        if (faces[i].count == 4)  //For Quads
        {
            glBegin(GL_QUADS);
            glNormal3f(normals[faces[i].vnorm1].x, normals[faces[i].vnorm1].y, normals[faces[i].vnorm1].z);
            if(hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext1].x, textureV[faces[i].vext1].y);
            }
            glVertex3f(vertexs[faces[i].ver1].x, vertexs[faces[i].ver1].y, vertexs[faces[i].ver1].z);

            glNormal3f(normals[faces[i].vnorm2].x, normals[faces[i].vnorm2].y, normals[faces[i].vnorm2].z);
            if(hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext2].x, textureV[faces[i].vext2].y);
            }
            glVertex3f(vertexs[faces[i].ver2].x, vertexs[faces[i].ver2].y, vertexs[faces[i].ver2].z);

            glNormal3f(normals[faces[i].vnorm3].x, normals[faces[i].vnorm3].y, normals[faces[i].vnorm3].z);
            if(hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext3].x, textureV[faces[i].vext3].y);
            }
            glVertex3f(vertexs[faces[i].ver3].x, vertexs[faces[i].ver3].y, vertexs[faces[i].ver3].z);

            glNormal3f(normals[faces[i].vnorm4].x, normals[faces[i].vnorm4].y, normals[faces[i].vnorm4].z);
            if(hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext4].x, textureV[faces[i].vext4].y);
            }
            glVertex3f(vertexs[faces[i].ver4].x, vertexs[faces[i].ver4].y, vertexs[faces[i].ver4].z);

            glEnd();
        }
        else if (faces[i].count == 3) //For Triangles
        {

            glBegin(GL_TRIANGLES);

            glNormal3f(normals[faces[i].vnorm1].x, normals[faces[i].vnorm1].y, normals[faces[i].vnorm1].z);
            if (hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext1].x, textureV[faces[i].vext1].y);
            }
            glVertex3f(vertexs[faces[i].ver1].x, vertexs[faces[i].ver1].y, vertexs[faces[i].ver1].z);

            glNormal3f(normals[faces[i].vnorm2].x, normals[faces[i].vnorm2].y, normals[faces[i].vnorm2].z);
            if (hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext2].x, textureV[faces[i].vext2].y);
            }
            glVertex3f(vertexs[faces[i].ver2].x, vertexs[faces[i].ver2].y, vertexs[faces[i].ver2].z);

            glNormal3f(normals[faces[i].vnorm3].x, normals[faces[i].vnorm3].y, normals[faces[i].vnorm3].z);
            if (hasVT == true)
            {
                glTexCoord2f(textureV[faces[i].vext3].x, textureV[faces[i].vext3].y);
            }
            glVertex3f(vertexs[faces[i].ver3].x, vertexs[faces[i].ver3].y, vertexs[faces[i].ver3].z);

            glEnd();
        }

    }
    glEndList(); //End of list
    return num; //Returns list ID
}

//-- Create VBO Function --
void createVBO()
{
    // Generating Buffers
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &iboID);

    size_t vSize = sizeof(ver);    // Size of Vertices
    size_t nSize = sizeof(verN);   // Size of Normals
    size_t tSize = sizeof(verT);   // Size of Tex Coords

    // Binding Buffers
    glBindBuffer(GL_ARRAY_BUFFER, vboID);   // Vertex Buffer
    glBufferData(GL_ARRAY_BUFFER, (vSize + nSize + tSize), 0, GL_STATIC_DRAW);    // Allocate Buffer Space
    glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertexs);       // Loading the Data to the Buffer
    glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);    // Loading the Data (Normals)
    glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, tSize, textureV);  // Loading the Data (Tex Coordinates)

    glBindBuffer(GL_ARRAY_BUFFER, 0);               // Unbind Buffer, Releasing
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);   // Index Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//-- Draw VBO Function --
void drawVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);

    glEnableClientState(GL_VERTEX_ARRAY);   // Enable -> Creates a pointer
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, (void*)0);  // Vertex Pointer
    glNormalPointer(GL_FLOAT, 0, (void*)sizeof(vertexs));  // Normal Pointer
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)sizeof(vertexs) + sizeof(normals));    // Tex Coordinates Pointer

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//-- Texture Loader Function --
void TLoad(char* fileName, GLuint &tex) // Texture Loader
{
    int width, height;

    unsigned char* image;

    glBindTexture(GL_TEXTURE_2D, tex);

    image = SOIL_load_image(fileName,&width,&height,0,SOIL_LOAD_RGBA); // let soil load image
    if(!image) {cout<< "Image not Found"<<endl;}

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);

    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

}

//-- Create Axis Angle Function --
void createFromAxisAngle(GLfloat X, GLfloat Y, GLfloat Z, GLfloat degree){

GLfloat angle = (GLfloat)((degree / 180.0f) * PI); //Converting degree to radians assuming angles are in radians

GLfloat result = (GLfloat)sin(angle / 2.0f); //Calculating sin(theta/2)

w = (GLfloat)cos( angle / 2.0f ); //Calculating w through cos(theta/2)

//Calculating x, y, z of quaternions
x = (GLfloat)(X * result);
y = (GLfloat)(Y * result);
z = (GLfloat)(Z * result);

}

//-- Create Matrix Function --
void createMatrix(GLfloat *pMatrix){

    //First row projection matrix
    pMatrix[0] = 1.0f - 2.0f * ( y * y + z * z );
    pMatrix[1] = 2.0f * (x * y + z * w);
    pMatrix[2] = 2.0f * (x * z - y * w);
    pMatrix[3] = 0.0f;

    //Second row projection matrix
    pMatrix[4] = 2.0f * ( x * y - z * w );
    pMatrix[5] = 1.0f - 2.0f * ( x * x + z * z );
    pMatrix[6] = 2.0f * (z * y + x * w );
    pMatrix[7] = 0.0f;

    //Third row projection matrix
    pMatrix[8] = 2.0f * ( x * z + y * w );
    pMatrix[9] = 2.0f * ( y * z - x * w );
    pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );
    pMatrix[11] = 0.0f;

    //Fourth row projection matrix
    pMatrix[12] = 0.0f;
    pMatrix[13] = 0.0f;
    pMatrix[14] = 0.0f;
    pMatrix[15] = 1.0f;

}

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

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,20,0.0,0.0,0.0,0.0,1.0,0.0);
    if(WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //Toggle WIRE FRAME

    // your code here
    glTranslated(0,0,cameraZoom); // To zoom in and out using END and HOME respectively

    //-- Quaternion Rotations for X, Y, Z --
    createMatrix(matrixX); //Quaternion for X
    createFromAxisAngle(1, 0, 0, RotateX);
    glMultMatrixf(matrixX);

    createMatrix(matrixY); //Quaternion for Y
    createFromAxisAngle(0, 1, 0, RotateY);
    glMultMatrixf(matrixY);

    createMatrix(matrixZ); //Quaternion for Z
    createFromAxisAngle(0, 0, 1, RotateZ);
    glMultMatrixf(matrixZ);

    //-- Model Implementations --
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glScalef(1.0, 1.0, 1.0);
        switch (currentModel){
            //Cow Model
            case 1:
                drawVBO();
                glCallList(cow);
                break;
            //Trunk Model
            case 2:
                drawVBO();
                glTranslated(0,-5,0);
                glCallList(trunk);
                break;
            //Leaves Model
            case 3:
                drawVBO();
                glTranslated(0,-10,0);
                glCallList(leaves);
                break;
        }
         glDisable(GL_TEXTURE_2D);
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
       //First model picked
       case '1':
            currentModel = 1;
            break;
       //Second model picked
        case '2':
            currentModel = 2;
            break;
       //Third model picked
        case '3':
            currentModel = 3;
            break;
    }
}

void Specialkeys(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            RotateX = (RotateX + 5) % 360;
            break;
        case GLUT_KEY_DOWN:
            RotateX = (RotateX - 5) % 360;
            break;
        case GLUT_KEY_LEFT:
            RotateY = (RotateY + 5) % 360;
            break;
        case GLUT_KEY_RIGHT:
            RotateY = (RotateY - 5) % 360;
            break;
        case GLUT_KEY_END:
            cameraZoom += 1.0;
            break;
        case GLUT_KEY_HOME:
            cameraZoom -= 1.0;
            break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    // Use parametric equation with t increment for xpos and y pos
    // Don't need a loop
    glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y){
    float scale = 100*(Wwidth/Wheight);
    switch(btn)
    {
        case GLUT_LEFT_BUTTON:
            if(state==GLUT_DOWN)
            {
               // get new mouse coordinates for x,y
               // use scale to match right
            }
            break;
    }
     glutPostRedisplay();
};

static void init(void)
{

    glewInit();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

    TLoad("images/grass.png",tex);
    createVBO();
    cow = loadObject("model/cow.obj"); //Loading cow model

    createVBO();
    trunk = loadObject("model/trunk.obj"); //Loading trunk model

    createVBO();
    leaves = loadObject("model/leaves.obj"); //Loading leaves model

    // De-allocating Dynamic Arrays
    free(vertexs);
    free(normals);
    free(textureV);
}

/* Program entry point */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1024,720);
    glutInitWindowPosition(400,200);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Project 3 - Model Loader with VBO");
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
