/******************************************
*
* Official Name:  Hossain M. Delwar
*
* E-mail:  hdelwar@syr.edu
*
* Assignment:  Final Project
*
* Environment/Compiler:  Visual Studio Community 2022
*
* Date submitted:  May 6, 2022
*
* References:  https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-Detection for collision detection between blocks and ball
*
* Interactions:   Press a/d to move left or right and space to officalling start playing once passed the starting page
*                 Press r to reset, Press i to go to the winner page
*                 Use your left mouse button to select your choice to begin!
*
*******************************************/

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.1415
#define STARTING 0
#define PLAYING 1
#define LOSER 2
#define WINNER 3

using namespace std;

// Globals.
GLsizei w = 0, h = 0;
float blockx[] = { 
    2,2,2,2,2,2,2,2,2,2,2,2,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    10,10,10,10,10,10,10,10,10,10,
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
    18,18,22,22,18,18,18,18,26,26,26,26,26,26
};
float blocky[] = { 
    1, 3,5,7,-1,-3,-5,-7,-9,-11,-13,-15,
    1, 3,5,7,9,11,13,15,-9,-11,-13,-15, -17,-19,
    1, 3,5,7,-9,-11,-13,-15, -17,-19,
    1, 3,-1,-3,-5,-7,-9,-11,-13,-15,-21,-23,13,15,21,23,
    -1,-3,-1,-3,9,11,13,15,1, 3,5,7,9,11
};
int sideways = 0;
int num = sizeof blockx / sizeof * blockx;
bool way = true;
static int isAnimate = 1;
static int animationPeriod = 10;
int barmove = 0;
int gamestate = STARTING;
const int numoffiles = 10;
static unsigned int texture[numoffiles];
int id = 0;
float ballX = 0, ballY = -34, ballZ = 0;
int ballangle = 80;
bool startmoving = false;
int lives = 3;


void colorp(float r, float g, float b) {
    glColor3f(r / 100.0, g / 100.0, b / 100.0);
}

void colornum(float r, float g, float b) {
    glColor3f(r / 255.0, g / 255.0, b / 255.0);
}


class Block
{
public:
   Block();
   Block(float x, float y, unsigned char r, 
       unsigned char g, unsigned char b);
   float getLeft() { return leftside; }
   float getTop() { return top; }
   void statusdown() { status -= 1; if (status == 0) Block(); }
   bool CheckCollision();
   void draw();
   int status;

private:
    float centerX, centerY, leftside, rightside, top, bottom;
   unsigned char color[3];
};

Block::Block()
{
    centerX = 0.0;
    centerY = 0.0;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    status = 0;
}

Block::Block(float x, float y, unsigned char r, 
    unsigned char g, unsigned char b)
{
    centerX = x;
    centerY = y;
    leftside = x - 2;
    top = y + 1;
    color[0] = r;
    color[1] = g;
    color[2] = b;
    status = 2;
}

bool Block :: CheckCollision() //reference
{
    bool collisionX = getLeft() + sideways + 4 >= ballX &&
        ballX + 1 >= getLeft() + sideways;
    bool collisionY = getTop() + 2 >= ballY &&
        ballY + 1 >= getTop();
    return collisionX && collisionY;
}

void Block::draw()
{
    if (status != 0) {
        if (CheckCollision()) {
            statusdown();
            if (ballangle <= 180)ballangle = 360 - ballangle;
            else if (ballangle <= 270) ballangle = 540 - ballangle;
            else ballangle = 360 - ballangle;
        }
        
    
        glPushMatrix();
        glTranslatef(centerX+sideways, centerY, 0);


        glBindTexture(GL_TEXTURE_2D, texture[status]);

        glPushMatrix();
        glTranslatef(0, 0, 3);
        glColor3f(1, 0, 0);
        glScalef(2, 1, 1);
        glutSolidCube(2);
        glPopMatrix();
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-2, -1, 0.5);
        glTexCoord2f(1.0, 0.0); glVertex3f(2, -1, 0.5);
        glTexCoord2f(1.0, 1.0); glVertex3f(2, 1, 0.5);
        glTexCoord2f(0.0, 1.0); glVertex3f(-2, 1, 0.5);
        glEnd();


        glPopMatrix();
    }
    else {
        centerX = 100;
        centerY = 100;
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        status = 0;
    }
}

Block bl[2*(sizeof blockx / sizeof * blockx)];
Block bl2[2 * (sizeof blockx / sizeof * blockx)];

struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char* data;
};

BitMapFile* getBMPData(string filename)
{
    BitMapFile* bmp = new BitMapFile;
    unsigned int size, offset, headerSize;

    // Read input file name.
    ifstream infile(filename.c_str(), ios::binary);

    // Get the starting point of the image data.
    infile.seekg(10);
    infile.read((char*)&offset, 4);

    // Get the header size of the bitmap.
    infile.read((char*)&headerSize, 4);

    // Get width and height values in the bitmap header.
    infile.seekg(18);
    infile.read((char*)&bmp->sizeX, 4);
    infile.read((char*)&bmp->sizeY, 4);

    // Allocate buffer for the image.
    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];

    // Read bitmap data.
    infile.seekg(offset);
    infile.read((char*)bmp->data, size);

    // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i + 2];
        bmp->data[i + 2] = temp;
    }

    return bmp;
}

void loadExternalTextures()
{
    BitMapFile* image[numoffiles+1];

    image[0] = getBMPData("hdelwarTextures/background.bmp");
    image[1] = getBMPData("hdelwarTextures/blue.bmp");
    image[2] = getBMPData("hdelwarTextures/red.bmp");
    image[3] = getBMPData("hdelwarTextures/start.bmp");
    image[4] = getBMPData("hdelwarTextures/logo.bmp");
    image[5] = getBMPData("hdelwarTextures/exit.bmp");
    image[6] = getBMPData("hdelwarTextures/lost.bmp");
    image[7] = getBMPData("hdelwarTextures/background2.bmp");
    image[8] = getBMPData("hdelwarTextures/congrats.bmp");
    image[9] = getBMPData("hdelwarTextures/background3.bmp");
    for (int i = 0; i < numoffiles; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[i]->sizeX, image[i]->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, image[i]->data);
    }
}
void starting() {
    startmoving = !startmoving;
    sideways = 0;
    lives -= 1;
    ballangle = 80;
    ballY = -34;
}


void animate(int value)
{
    if (startmoving)
    {
        if (way && sideways < 32) sideways += 1;
        else way = false;
        if (way == false && sideways > -32) sideways -= 1;
        else way = true;
        if ((ballY + 1 >= 33.75)) {
            if (ballangle <= 90) ballangle = 360 - ballangle;
            else if (ballangle <= 180)ballangle = 360 - ballangle;
        }
        else if (ballX - 0.5 <= -60) {
            if (ballangle < 180) ballangle = 180 - ballangle;
            else ballangle = 540 - ballangle;
        }
        else if (ballX + 0.5 >= 60) {
            if (ballangle < 90) ballangle = 180 - ballangle;
            else ballangle = 540 - ballangle;
        }
        else if (ballY - 0.5 <= -35 && ((ballX + 0.5 <= barmove + 10) && (ballX - 0.5 >= barmove - 10))) {
            if (ballangle <= 270) ballangle = 540 - ballangle;
            else ballangle = 360 - ballangle;
        }
        else if (ballY + 0.5 <= -35) starting();
        ballangle = ballangle % 360;

        ballX = ballX + 0.5*cos(PI * ballangle/180);
        ballY = ballY + 0.5 * sin(PI * ballangle / 180);
    }
    if (gamestate == WINNER) {
        ballangle += 3;
    }
    glutTimerFunc(animationPeriod, animate, 1);
    glutPostRedisplay();
}

void ball() {
    glPushMatrix();
    if (!startmoving) {
        ballX = barmove;
    }
    glTranslatef(ballX, ballY, ballZ);
    glColor3f(0, 1, 0);
    glutSolidSphere(0.5, 360, 360);
    glPopMatrix();
    for (int i = 0; i < lives; i++) {
        glPushMatrix();
        glTranslatef(48+(i*4), 31, ballZ);
        glColor3f(1, 0, 0);
        glutSolidSphere(1, 360, 360);
        glPopMatrix();
    }
}

void bar() {
    glPushMatrix();
    glTranslatef(barmove, -35, 5);
    colornum(165, 42, 42);
    glRotatef(90, 1, 0, 0);
    gluCylinder(gluNewQuadric(), 10, 10, 2, 4, 4);
    glPopMatrix();
}

void trophy() {
    glEnable(GL_LIGHTING);
    float matAmb[] = { 185/(float)255, 242/ (float)255, 255/ (float)255, 1.0 };
    float matShine[] = { 100 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmb);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glPushMatrix();
    glRotatef(ballangle, 0, 1, 0);
    glPushMatrix();
    glTranslatef(0, -20, 0);
    colornum(185, 242, 255);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(gluNewQuadric(), 0.1, 15, 15, 7, 5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, -3, 0);
    colornum(185, 242, 255);
    glRotatef(180, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(gluNewQuadric(), 10, 15, 2, 7, 5);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_LIGHTING);
}

void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    if(gamestate == PLAYING) glBindTexture(GL_TEXTURE_2D, texture[7]);
    else if (gamestate == WINNER) glBindTexture(GL_TEXTURE_2D, texture[9]);
    else glBindTexture(GL_TEXTURE_2D, texture[0]);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0); glVertex3f(-60, -40, 10);
    glTexCoord2f(1.0, 0.0); glVertex3f(60.0, -40, 10);
    glTexCoord2f(1.0, 1.0); glVertex3f(60.0, 33.75, 10);
    glTexCoord2f(0.0, 1.0); glVertex3f(-60.0, 33.75, 10);
    glEnd();

    if (gamestate == STARTING) {
        glBindTexture(GL_TEXTURE_2D, texture[4]);
        int x = 45, y = 30.3;
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-x, -y+10, 10);
        glTexCoord2f(1.0, 0.0); glVertex3f(x, -y+10, 10);
        glTexCoord2f(1.0, 1.0); glVertex3f(x, y, 10);
        glTexCoord2f(0.0, 1.0); glVertex3f(-x, y, 10);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, texture[3]);
        x = -45, y = 25;
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(x, -y - 10.7, 10);
        glTexCoord2f(1.0, 0.0); glVertex3f(x + 26.2, -y-10.7, 10);
        glTexCoord2f(1.0, 1.0); glVertex3f(x+26.2, -y, 10);
        glTexCoord2f(0.0, 1.0); glVertex3f(x, -y, 10);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, texture[5]);
        x = 45-26.2, y = 25;
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(x, -y - 10.7, 10);
        glTexCoord2f(1.0, 0.0); glVertex3f(x + 26.2, -y - 10.7, 10);
        glTexCoord2f(1.0, 1.0); glVertex3f(x + 26.2, -y, 10);
        glTexCoord2f(0.0, 1.0); glVertex3f(x, -y, 10);
        glEnd();
    }
    if (gamestate == PLAYING){
        glLineWidth(3);
        int blockcount = 0;
        for (int i = 0; i < num; i++) {
            if (bl[i].status == 0) blockcount += 1;
            else bl[i].draw();
            if (bl2[i].status == 0) blockcount += 1;
            else bl2[i].draw();
        }
        glDisable(GL_TEXTURE_2D);
        if (blockcount == 132) gamestate = WINNER;
        if (lives == 0) gamestate = LOSER;
        ball();
        bar();
    }
    if (gamestate == LOSER) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[6]);
        int x = 30;
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-x, -x, 10);
        glTexCoord2f(1.0, 0.0); glVertex3f(x,-x, 10);
        glTexCoord2f(1.0, 1.0); glVertex3f(x,x, 10);
        glTexCoord2f(0.0, 1.0); glVertex3f(-x, x, 10);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        
    }
    if (gamestate == WINNER) {
        glDisable(GL_TEXTURE_2D);
        trophy();
    }
    glutSwapBuffers();
}

void reset() {
    for (int i = 0; i < num; i++) {
        bl[i] = Block(blockx[i], blocky[i], 1, 0, 0);
        bl2[i] = Block(-blockx[i], blocky[i], 1, 0, 0);
    }
    sideways = 0;
    way = true;
    barmove = 0;
    gamestate = PLAYING;
    ballX = 0, ballY = -34, ballZ = 0;
    ballangle = 80;
    startmoving = false;
    lives = 3;
}

void setup(void)
{

    for (int i = 0; i < num; i++) {
        bl[i] = Block(blockx[i], blocky[i], 1, 0, 0);
        bl2[i] = Block(-blockx[i], blocky[i], 1, 0, 0);
    }
    glGenTextures(numoffiles, texture);

    loadExternalTextures();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    float globAmb[] = { 1, 1, 1, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
}
void mouseControl(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN && button == GLUT_LEFT)
    {
        x = (x / 10) - 60;
        y = (-y / 10) + 33.3;
        if (gamestate == STARTING) {
            if ((x > -45 && x < (-45 + 26.3)) && (y > -30.7) && (y < -25)) gamestate = PLAYING;
            if ((x > (45 - 26.3) && x < 45) && (y > -30.7) && (y < -25)) exit(0);
        }
        glutPostRedisplay();

    }
}

void resize(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-60, 60.0, -40, 33.75, -20.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    h = height;
    w = width;
}

void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case 'd': 
        if (barmove <50 && gamestate == PLAYING) barmove +=2.5;
        glutPostRedisplay();
        break;
    case 'a':
        if (barmove > -50 && gamestate == PLAYING) barmove -= 2.5;
        break;
    case ' ':
        if (gamestate == PLAYING && !startmoving) {
            startmoving = !startmoving;
        }
        glutPostRedisplay();
        break;
    case 'r':
        reset();
        glutPostRedisplay();
        break;
    case 'i':
        gamestate = WINNER;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

int main(int argc, char** argv)
{
    cout << "Interaction:" << endl;
    cout << "Press a/d to move left or right and space to officalling start playing once passed the starting page" << endl;
    cout << "Press r to reset" << endl;
    cout << "Use your left mouse button to select your choice to begin!" << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 737.5);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("hdelwarAssignment1");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutMouseFunc(mouseControl);
    glutTimerFunc(5, animate, 1);
    glutMainLoop();

    return 0;
}
