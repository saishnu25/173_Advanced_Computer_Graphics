// Saishnu Ramesh Kumar (300758706)
// CSCI 173 - Project 5

// To see a clearer wave movement, press W to view it using Wireframe

#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <SOIL.h>
#include <math.h>
#include <fstream>
#define PI 3.14159

using namespace std;

float Wwidth,Wheight;
float scl = 1.0;

GLuint tex[6]; //To generate textures for the skybox
GLfloat rotateCamera = 0.0; //This is to be used and rotate the cube
GLfloat rotateY = 0.0; //Rotate along the y-axis
GLfloat zoom = 0.0; //Zoom
GLfloat modelView[16]; //ModelView
GLfloat projection[16]; //Projection

unsigned int vs, fs, program;
GLint loc, mvloc, ploc;

GLfloat sizeOfGrid = 100.0f; //Grid Size
int numberOfLines = 200; //Number of Lines

bool WireFrame= false;
float i =0;
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


/* GLUT callback Handlers */
//Texture Loader Function
void TLoad(char* fileName, GLuint &tex) //Texture laoder
{
    int width, height;

    unsigned char* image;

    glBindTexture(GL_TEXTURE_2D, tex);

    image = SOIL_load_image(fileName,&width,&height,0,SOIL_LOAD_RGBA); // let the soil load the images.
    if(!image)
    {
        cout<< "Image not Found"<<endl;
    }

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);

    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

}

//Read Shader File Function
void readShaderFile(char* fileName, string &str)
{
    ifstream in(fileName);
    char tmp[1024];

    if(!in.is_open())
    {
        cout<< "The file "<<fileName << " Not Found "<<endl;
    }
    else{

        while(!in.eof())
        {
            in.getline(tmp,1024);
            str+=tmp;
            str +='\n';

            //cout<< tmp<<endl;     // Prints the Source Code (Line-By-Line)
        }
    }
}

//Load Shader Function
unsigned int loadShader(string &source, unsigned int mode)
{
    unsigned int id;
    char error[1024];

    id = glCreateShader(mode);

    const char *cSrc = source.c_str();

    glShaderSource(id,1,&cSrc,NULL);
    glCompileShader(id);

    glGetShaderInfoLog(id,1024,NULL,error);

    cout<<" Compile Status: \n"<< error<<endl;
    return id;
}

//Shader Initialization Function
void initShader(char* vName, char* fName)
{
    string source ="";

    //Vertex Shader
    readShaderFile(vName,source);
    vs = loadShader(source, GL_VERTEX_SHADER);
    source = "";

    //Fragment Shader
    readShaderFile(fName,source);
    fs = loadShader(source, GL_FRAGMENT_SHADER);
    source = "";

    //Creating and attaching the respective shaders
    program = glCreateProgram();
    glAttachShader(program,vs);
    glAttachShader(program,fs);

    glLinkProgram(program);

    //Detaching the shaders
    glDetachShader(program,vs);
    glDetachShader(program,fs);
}

//Grid Generation Function
void gridGenerate()
{
    GLfloat distanceBtwnGrid = sizeOfGrid / numberOfLines; //Getting the distance between each of the lines on the grid

    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numberOfLines; i++)
    {
        float xPos = -sizeOfGrid / 2 + i * distanceBtwnGrid;
        float xPos2 = xPos + distanceBtwnGrid;
        for (int j = 0; j < numberOfLines; j++)
        {
            float zPos = -sizeOfGrid / 2 + j * distanceBtwnGrid;
            float zPos2 = zPos + distanceBtwnGrid;

            // Drawing the triangles to generate grid
            glVertex3f(xPos, 0.0, zPos);
            glVertex3f(xPos, 0.0, zPos2);
            glVertex3f(xPos2, 0.0, zPos);

            glVertex3f(xPos2, 0.0, zPos);
            glVertex3f(xPos, 0.0, zPos2);
            glVertex3f(xPos2, 0.0, zPos2);
        }
    }
    glEnd();
    glPopMatrix();
}

static void resize(int width, int height)
{
    double Ratio;

    Wwidth = (float)width;
    Wheight = (float)height;

    Ratio = (double)width /(double)height;

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

    gluLookAt(0,0,10,0.0,0.0,0.0,0.0,1.0,100.0);

    if(WireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		//Draw Our Mesh In Wireframe Mesh
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		//Toggle WIRE FRAME

    //your code here
    glTranslatef(0.0, rotateY, zoom);
    glRotatef(rotateCamera, 0.0f, 1.0f, 0.0f);

    //-- SKYBOX --
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScalef(50.0,50.0,50.0);

    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal Front Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f); // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Top Left Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f, -1.0f); // Bottom Left Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normals Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f); // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f); // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Top Right Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[3]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom Right Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[4]);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f); // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, -1.0f); // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 1.0f); // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f); // Bottom Left Of The Texture and Quad
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[5]);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f); // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Top Left Of The Texture and Quad
    glEnd();
    glPopMatrix();

    //-- WAVE --
    glUseProgram(program);
    glPushMatrix();
    glTranslatef(0.0f, -10.0f, -2.0f);

    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);

    gridGenerate(); //Calling the grid generation function
    float waveMovement = glutGet(GLUT_ELAPSED_TIME) / 500.0f; //Controls the wave speed
    glUniform1f(glGetUniformLocation(program, "time"), waveMovement);
    glPopMatrix();

	if(loc !=-1) glUniform1f(loc,scl);
	if(mvloc !=-1) glUniformMatrix4fv(mvloc,1,GL_FALSE, modelView);
	if(ploc !=-1) glUniformMatrix4fv(ploc,1,GL_FALSE,projection);
	glUseProgram(0);

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

    }
}

void Specialkeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        rotateCamera -= 4.0; // Rotate camera left
        break;
    case GLUT_KEY_RIGHT:
        rotateCamera += 4.0; // Rotate camera right
        break;
    case GLUT_KEY_UP:
        zoom += 1.0; // Zoom In
        break;
    case GLUT_KEY_DOWN:
        zoom -= 1.0; // Zoom Out
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

static void init(void)
{
    glewInit();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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
    glGenTextures(6,tex);
    TLoad("images/front.jpg",tex[0]);
    TLoad("images/back.jpg",tex[1]);
    TLoad("images/top.jpg",tex[2]);
    TLoad("images/bottom.jpg",tex[3]);
    TLoad("images/left.jpg",tex[4]);
    TLoad("images/right.jpg",tex[5]);

    initShader("waveVertex.vs", "waveFragment.fs"); //Calling the fs and vs shader files

    glUseProgram(program);
    loc = glGetUniformLocation(program, "scale");
    mvloc = glGetUniformLocation(program, "mvMatrix");
    ploc = glGetUniformLocation(program, "pMatrix");
    glUseProgram(0);
}

/* Program entry point */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(1028,720);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Project 5 - Simulating Water");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);

    glutIdleFunc(idle);

    glutMainLoop();
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(program);
    return EXIT_SUCCESS;
}
