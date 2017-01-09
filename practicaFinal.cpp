/*****************************************************/
/*                                                   */
/*           PACTICA FINAL                           */
/*             David Alonso                          */
/*             Teodora Dinu                          */
/*****************************************************/

#include <GL/freeglut.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int ancho, alto;
GLfloat angx,angy,angz;
GLfloat alfa, beta;
GLfloat upz=1.0;;
GLdouble xesf1,yesf1,zesf1,eyex,eyey,eyez,atx,aty,atz;

GLfloat paso=0.05f;
GLdouble znear= 0.001f;

GLfloat tamobj= 0.2;
float zfar;


//Usuo de teclado avanzado
bool keypressed[256];
bool specialpressed[256];
//Raton
void PasiveMouse(int x, int y);

/*definiciones para las listas de visualización*/
GLuint fortalezaDL;
GLuint casa1DL;
GLuint casa2DL;
GLuint pozoDL;

//variable para el sonido
int andando=0;

//panel informacion
int HUDO=0;

//vista cenital
int Ccenital=0;

fstream objeto;

typedef struct{
    double x;
    double y;
    double z;
}Punto3D;

typedef struct{
    int A;
    int B;
    int C;
}Superficie;

typedef struct{
    double nx;
    double ny;
    double nz;
}Normal;

typedef struct{
    Superficie s;
    Normal n;
}Superficie3D;

Punto3D *vertices;
Superficie3D *superficies;

int numVertices,numSuperficies;



void lecturaObjeto(char* nombreFichero){
    objeto.open(nombreFichero,ios::in);
    string tag;
    double x,y,z,nx,ny,nz;
    int A,B,C,i,aux=0;
    tag=" ";

    //printf("aqui llega");
    while (!objeto.eof()){

        objeto >> tag;
        //std::cout << "tag a comparar: " << tag;
        if(tag.compare("*MESH_NUMVERTEX")==0){
            std::cout << tag;
            aux=1;}
        else if(tag.compare("*MESH_NUMFACES")==0){
            std::cout << tag;
            aux=2;}
        else if(tag.compare("*MESH_VERTEX_LIST")==0){
            std::cout << tag;
            aux=3;}
        else if(tag.compare("*MESH_FACE_LIST")==0){
            std::cout << tag;
            aux=4;}
        else if (tag.compare("*MESH_NORMALS")==0){
            std::cout << tag;
            aux=5;
        }

       // printf("aux: %d",aux);
        switch(aux)
        {
        case 1:

            objeto >> numVertices;
            vertices = (Punto3D*) malloc(numVertices*sizeof(Punto3D));
            //printf(" numVertices: %d",numVertices);
            aux=0;
            break;

        case 2:

            objeto >> numSuperficies;
            superficies= (Superficie3D*) malloc(numSuperficies*sizeof(Superficie3D));
            //printf(" numSuperficies: %d",numSuperficies);
            aux=0;
            break;

        case 3:

            //saltamos el { y loopeamos hasta el numero de vertices
            objeto >> tag; /*{*/
            //std::cout << tag;
            for (i=0;i<numVertices;i++){
                //        *MESH_VERTEX >> nº vertice >>>>>coordenadas
                objeto >> tag >> tag >> x >> y >> z;

                vertices[i].x=x; //printf(" x: %f",x);
                vertices[i].y=y; //printf(" y: %f",y);
                vertices[i].z=z; //printf(" z: %f",z);
            }
            aux = 0;
            break;

        case 4:
            //saltamos el { y loopeamos hasta el numero de superficies
            //objeto >> tag /*{*/

            for (i=0;i<numSuperficies;i++){
                    //con este bucle saltamos los datos de cada meshface quq
                    //no interesan
                    do
                            objeto >> tag;
                    while (tag.compare("*MESH_FACE") != 0);
        //       nºsup >> A: >>>>>coordenadas
                objeto >> tag >> tag >> A;
                objeto >> tag >> B;
                objeto >> tag >> C;

                superficies[i].s.A = A; //printf(" x: %d",A);
                superficies[i].s.B = B; //printf(" y: %d",B);
                superficies[i].s.C = C; //printf(" z: %d",C);
                }
                aux=0;
            break;

        case 5:

                for (i=0;i<numSuperficies;i++){
                    //con este bucle saltamos los datos de cada meshface quq
                    //no interesan
                    do
                            objeto >> tag;
                    while (tag.compare("*MESH_FACENORMAL") != 0);
        //       indice >>coordenadas
                objeto >> tag >> nx >> ny >> nz;

                superficies[i].n.nx = nx; //printf(" nx: %f",nx);
                superficies[i].n.ny = ny; //printf(" ny: %f",ny);
                superficies[i].n.nz = nz; //printf(" nz: %f",nz);
                }
                aux=0;
            break;

        }//fin switch


    }//fin while
    objeto.close();
}

void CalculaPosEsferica(void){
    xesf1=sin(angx)*cos(angy);
    yesf1=sin(angx)*sin(angy);

    zesf1=cos(angx);


    //printf("COORDENADAS ESF : x= %f y= %f z= %f\n",xesf1,yesf1,zesf1);
}
void CalcFirstPersonPos(void){

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    atx=eyex+znear*cos(beta)*cos(alfa);
    aty=eyey+znear*cos(beta)*sin(alfa);
    atz=eyez+znear*sin(beta);

    gluPerspective(45.0f,(float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT),0.01,zfar);
}
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}
void camaraCenital()
{
    ancho= glutGet(GLUT_WINDOW_WIDTH);
    alto = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, ancho, alto);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(50.0, -50.0, 25.0, -25.0, 10.0, 300.0);

    gluLookAt(0,0,100,0.1,0.01,0.00,1,0,1);

}
/* Funcion para inicializar algunos parametros de OpenGL */
void init(void)
{
    glClearColor(0.1,0.1,0.1,0.0);

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINEAR_ATTENUATION);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_DIFFUSE);

    ancho = glutGet(GLUT_WINDOW_WIDTH);
    alto  = glutGet(GLUT_WINDOW_HEIGHT);

    beta =  0.0f;
    alfa =  0.0f;
<<<<<<< HEAD
    //r=50;
=======
>>>>>>> origin/rama_teo

    glutSetCursor(GLUT_CURSOR_NONE); //Desactiva el culsor

    paso = 0.01f;
    eyex = 0.001;
    eyey = 0.001;
    eyez = 0.05;

    zfar=1000;// distancia maxima
    znear=0.01;
   // lecturaObjeto();

   //transparecia panel informacion
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //musica
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(22050, AUDIO_S16,2,512);

    Mix_Music *musica;
    musica = Mix_LoadMUS("ott.mp3");
    Mix_PlayMusic(musica,0);

    //uso de teclado avanzado
    glutIgnoreKeyRepeat(1);
    for (int i=0; i<256; i++)
    {
        keypressed[i]=false;
        specialpressed[i]=false;
    }


<<<<<<< HEAD
    //listas de visualizacion
    /*se cargan los objetos en la lista de visualización*/

=======
    /*se cargan los objetos en la lista de visualización*/


>>>>>>> origin/rama_teo
    lecturaObjeto("fortaleza.ase");


    fortalezaDL=glGenLists(1);
    glNewList(fortalezaDL,GL_COMPILE);// ahora se define el objeto en la escena

    glPushMatrix();
    glTranslated(2,0.5,-0.02);
    glRotated(-90,0,0,1);
    glScalef(0.0015,0.0015,0.0015);
    // glEnable(GL_TEXTURE_2D);
   // glBindTexture(GL_TEXTURE_2D, 5);

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<numSuperficies; i++){
            glNormal3d(superficies[i].n.nx,superficies[i].n.ny,superficies[i].n.nz);
            glVertex3d(vertices[superficies[i].s.A].x,vertices[superficies[i].s.A].y,vertices[superficies[i].s.A].z);
            glVertex3d(vertices[superficies[i].s.B].x,vertices[superficies[i].s.B].y,vertices[superficies[i].s.B].z);
            glVertex3d(vertices[superficies[i].s.C].x,vertices[superficies[i].s.C].y,vertices[superficies[i].s.C].z);
        }
        glEnd();

    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    glEndList();

    //fin de carga de fortaleza
    //carga de los siguientes objetos

    lecturaObjeto("casa1.ase");

    casa1DL=glGenLists(1);
    glNewList(casa1DL,GL_COMPILE);// ahora se define el objeto en la escena

    glPushMatrix();
    glTranslated(-1,1.5,-0.02);
    glScalef(0.0015,0.0015,0.0015);
   // glEnable(GL_TEXTURE_2D);
   // glBindTexture(GL_TEXTURE_2D, 5);

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<numSuperficies; i++){
            glNormal3d(superficies[i].n.nx,superficies[i].n.ny,superficies[i].n.nz);
            glVertex3d(vertices[superficies[i].s.A].x,vertices[superficies[i].s.A].y,vertices[superficies[i].s.A].z);
            glVertex3d(vertices[superficies[i].s.B].x,vertices[superficies[i].s.B].y,vertices[superficies[i].s.B].z);
            glVertex3d(vertices[superficies[i].s.C].x,vertices[superficies[i].s.C].y,vertices[superficies[i].s.C].z);
        }
        glEnd();

    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    glEndList();

    lecturaObjeto("casa2.ase");

    casa2DL=glGenLists(1);
    glNewList(casa2DL,GL_COMPILE);// ahora se define el objeto en la escena

    glPushMatrix();
    glTranslated(1.1,1.8,-0.02);
    glRotated(-210,0,0,1);
    glScalef(0.0015,0.0015,0.0015);
   // glEnable(GL_TEXTURE_2D);
   // glBindTexture(GL_TEXTURE_2D, 5);

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<numSuperficies; i++){
            glNormal3d(superficies[i].n.nx,superficies[i].n.ny,superficies[i].n.nz);
            glVertex3d(vertices[superficies[i].s.A].x,vertices[superficies[i].s.A].y,vertices[superficies[i].s.A].z);
            glVertex3d(vertices[superficies[i].s.B].x,vertices[superficies[i].s.B].y,vertices[superficies[i].s.B].z);
            glVertex3d(vertices[superficies[i].s.C].x,vertices[superficies[i].s.C].y,vertices[superficies[i].s.C].z);
        }
        glEnd();

    glPopMatrix();

    glPushMatrix();
    glTranslated(0,-1.8,-0.02);
    glRotated(-30,0,0,1);
    glScalef(0.0015,0.0015,0.0015);
   // glEnable(GL_TEXTURE_2D);
   // glBindTexture(GL_TEXTURE_2D, 5);

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<numSuperficies; i++){
            glNormal3d(superficies[i].n.nx,superficies[i].n.ny,superficies[i].n.nz);
            glVertex3d(vertices[superficies[i].s.A].x,vertices[superficies[i].s.A].y,vertices[superficies[i].s.A].z);
            glVertex3d(vertices[superficies[i].s.B].x,vertices[superficies[i].s.B].y,vertices[superficies[i].s.B].z);
            glVertex3d(vertices[superficies[i].s.C].x,vertices[superficies[i].s.C].y,vertices[superficies[i].s.C].z);
        }
        glEnd();

    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);
    glEndList();

    lecturaObjeto("pozo.ase");

    pozoDL=glGenLists(1);
    glNewList(pozoDL,GL_COMPILE);// ahora se define el objeto en la escena

    glPushMatrix();
    glTranslated(-0.5,0.0,-0.02);
    glScalef(0.0015,0.0015,0.0015);
   // glEnable(GL_TEXTURE_2D);
   // glBindTexture(GL_TEXTURE_2D, 5);

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<numSuperficies; i++){
            glNormal3d(superficies[i].n.nx,superficies[i].n.ny,superficies[i].n.nz);
            glVertex3d(vertices[superficies[i].s.A].x,vertices[superficies[i].s.A].y,vertices[superficies[i].s.A].z);
            glVertex3d(vertices[superficies[i].s.B].x,vertices[superficies[i].s.B].y,vertices[superficies[i].s.B].z);
            glVertex3d(vertices[superficies[i].s.C].x,vertices[superficies[i].s.C].y,vertices[superficies[i].s.C].z);
        }
        glEnd();

    glPopMatrix();

    //suelo
    glPushMatrix();
    glTranslatef(0.0f,0.0f,-0.08f);
    glScaled(35 ,20,0.5);
    glutSolidCube(tamobj);
    glPopMatrix();
    //muroizq
    glPushMatrix();
    glTranslated(0.0,2,-0.08);
    glScaled(35 ,1,8);
    glutSolidCube(tamobj);
    glPopMatrix();
    //muroder
    glPushMatrix();
    glTranslated(0.0,-2,-0.08);
    glScaled(35 ,1,8);
    glutSolidCube(tamobj);
    glPopMatrix();
    //murofondo
    glPushMatrix();
    glTranslated(3.5,0.0,-0.08);
    glScaled(1,20,8);
    glutSolidCube(tamobj);
    glPopMatrix();
    //murofrente
    glPushMatrix();
    glTranslated(-3.5,0.0,-0.08);
    glScaled(1,20,8);
    glutSolidCube(tamobj);
    glPopMatrix();

    //almenas
    glPushMatrix();
    glTranslated(3.5,2,-0.08);
    glScaled(2,2,10);
    glutSolidCube(tamobj);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-3.5,2,-0.08);
    glScaled(2,2,10);
    glutSolidCube(tamobj);
    glPopMatrix();

    glPushMatrix();
    glTranslated(3.5,-2,-0.08);
    glScaled(2,2,10);
    glutSolidCube(tamobj);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-3.5,-2,-0.08);
    glScaled(2,2,10);
    glutSolidCube(tamobj);
    glPopMatrix();

    glEndList();

<<<<<<< HEAD

}
void keyoperations()
{

    //teclas de entorno
    if (keypressed[27])
    {
        exit(0);
    }
    if (keypressed['f'] || keypressed['F'])
    {
        glutFullScreen();

        reshape(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
    }

    //teclas de movimiento
    if (keypressed['w'] || keypressed['W'])
    {
        if(Ccenital!=1)
        {
                eyex +=paso*cos(alfa);
                eyey +=paso*sin(alfa);

            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['a'] || keypressed['A'])
    {
        if(Ccenital!=1)
        {

                eyex =eyex-(paso*sin(alfa));
                eyey =eyey+(paso*cos(alfa));
            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['s'] || keypressed['S'])
    {
        if(Ccenital!=1)
        {

                eyex -=paso*cos(alfa);
                eyey -=paso*sin(alfa);
           CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['d'] || keypressed['D'])
    {
        if(Ccenital!=1)
        {

                eyex =eyex + (paso*sin(alfa));
                eyey =eyey - (paso*cos(alfa));
            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['r'] || keypressed['R'])
    {
        if(Ccenital!=1)
        {
            eyex=0.0;//recolocar en la poscion inicial
            eyey=45.0;
            alfa=-90.0;
            beta =0.0;
        }
    }

    //Teclas de camara
    if (specialpressed[GLUT_KEY_RIGHT])
    {
        alfa-=0.08;//solo actualizamos los valores de alpha y beta
      //  CalcFirstPersonPos();
    }
    if (specialpressed[GLUT_KEY_LEFT])
    {
        alfa+=0.08;//solo actualizamos los valores de alpha y beta
       // CalcFirstPersonPos();
    }
    if (specialpressed[GLUT_KEY_UP])
    {
        if(beta>-1.48353)  //-1,48 son -85 grados en radianes
        {
            beta-=0.08;//limitamos a -85º
        }
        // CalcFirstPersonPos();
    }

    if (specialpressed[GLUT_KEY_DOWN])
    {
        if(beta<1.48353)  //1,48 son 85 grados en radianes
        {
            beta+=0.08;//limitar a +85
        }
       //  CalcFirstPersonPos();
    }
}
void displayString(char *s)
{
    for (int i = 0; i < strlen (s); i++)
    {
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
    }
}
void setOrthographicProjection()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,ancho,alto,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //gluOrtho2D(0, alto, ancho, 0);


}
void resetPerspectiveProjection()
{

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void HUD(){
    if(HUDO==1)
    {

        ancho = glutGet(GLUT_WINDOW_WIDTH);
        alto  = glutGet(GLUT_WINDOW_HEIGHT);
        char cadena[]="Salida";
        setOrthographicProjection();


        glEnd();
        glColor4f(0.0,0.0,0.0,0.7);
        glBegin(GL_QUADS); //Cuadro de infirmacion
        glVertex2f(10,10);
        glVertex2f(220.0,10);
        glVertex2f(220.0,150.0);
        glVertex2f(10.0,150.0);
        glEnd();
        glColor4f(1.0,1.0,1.0,1.0);
        glRasterPos2i(30,30);
        displayString("TECLAS:\n");
        glRasterPos2i(30,40);
        displayString("W: avance");
        glRasterPos2i(30,50);
        displayString("S: retorceso");
        glRasterPos2i(30,60);
        displayString("A: movimiento a la izquierda");
        glRasterPos2i(30,70);
        displayString("D: movimiento a la derecha");
        glRasterPos2i(30,80);
        displayString("C: alternar vista entre camaras");
        glRasterPos2i(30,90);
        displayString("F: guardar imagen");
        glRasterPos2i(30,100);
        displayString("R: volver a posicion inicial");
        glRasterPos2i(30,110);
        displayString("Esc: salir");
        glRasterPos2i(30,120);
        displayString("M: deterner musica y reanudar la musica");
        glRasterPos2i(30,130);
        displayString("H: cerrar panel informacion");
        glRasterPos2i(30,140);
        displayString("P: cambiar vista");

        resetPerspectiveProjection();
    }


=======
>>>>>>> origin/rama_teo
}

void keyoperations()
{

    //teclas de entorno
    if (keypressed[27])
    {
        exit(0);
    }
    if (keypressed['f'] || keypressed['F'])
    {
        glutFullScreen();

        reshape(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
    }

    //teclas de movimiento
    if (keypressed['w'] || keypressed['W'])
    {
        if(Ccenital!=1)
        {
                eyex +=paso*cos(alfa);
                eyey +=paso*sin(alfa);

            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['a'] || keypressed['A'])
    {
        if(Ccenital!=1)
        {

                eyex =eyex-(paso*sin(alfa));
                eyey =eyey+(paso*cos(alfa));
            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['s'] || keypressed['S'])
    {
        if(Ccenital!=1)
        {

                eyex -=paso*cos(alfa);
                eyey -=paso*sin(alfa);
           CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['d'] || keypressed['D'])
    {
        if(Ccenital!=1)
        {

                eyex =eyex + (paso*sin(alfa));
                eyey =eyey - (paso*cos(alfa));
            CalcFirstPersonPos();
            andando=1;
        }
    }
    if (keypressed['r'] || keypressed['R'])
    {
        if(Ccenital!=1)
        {
            eyex=0.0;//recolocar en la poscion inicial
            eyey=45.0;
            alfa=-90.0;
            beta =0.0;
        }
    }

    //Teclas de camara
    if (specialpressed[GLUT_KEY_RIGHT])
    {
        alfa-=0.08;//solo actualizamos los valores de alpha y beta
        CalcFirstPersonPos();
    }
    if (specialpressed[GLUT_KEY_LEFT])
    {
        alfa+=0.08;//solo actualizamos los valores de alpha y beta
        CalcFirstPersonPos();
    }
    if (specialpressed[GLUT_KEY_UP])
    {
        if(beta<1.48)
        {
            beta+=0.08;//limitamos a -85º
        }
         CalcFirstPersonPos();
    }

    if (specialpressed[GLUT_KEY_DOWN])
    {
        if(beta<1.48)
        {
            beta-=0.08;//limitar a +85
        }
         CalcFirstPersonPos();
    }
}
void displayString(char *s)
{
    for (int i = 0; i < strlen (s); i++)
    {
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
    }
}
void setOrthographicProjection()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,ancho,alto,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //gluOrtho2D(0, alto, ancho, 0);

}
void resetPerspectiveProjection()
{

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void HUD(){
    if(HUDO==1)
    {

        ancho = glutGet(GLUT_WINDOW_WIDTH);
        alto  = glutGet(GLUT_WINDOW_HEIGHT);
        char cadena[]="Salida";
        setOrthographicProjection();


        glEnd();
        glColor4f(0.0,0.0,0.0,0.7);
        glBegin(GL_QUADS); //Cuadro de infirmacion
        glVertex2f(10,10);
        glVertex2f(220.0,10);
        glVertex2f(220.0,150.0);
        glVertex2f(10.0,150.0);
        glEnd();
        glColor4f(1.0,1.0,1.0,1.0);
        glRasterPos2i(30,30);
        displayString("TECLAS:\n");
        glRasterPos2i(30,40);
        displayString("W: avance");
        glRasterPos2i(30,50);
        displayString("S: retorceso");
        glRasterPos2i(30,60);
        displayString("A: movimiento a la izquierda");
        glRasterPos2i(30,70);
        displayString("D: movimiento a la derecha");
        glRasterPos2i(30,80);
        displayString("C: alternar vista entre camaras");
        glRasterPos2i(30,90);
        displayString("F: guardar imagen");
        glRasterPos2i(30,100);
        displayString("R: volver a posicion inicial");
        glRasterPos2i(30,110);
        displayString("Esc: salir");
        glRasterPos2i(30,120);
        displayString("M: deterner musica y reanudar la musica");
        glRasterPos2i(30,130);
        displayString("H: cerrar panel informacion");
        glRasterPos2i(30,140);
        displayString("P: cambiar vista");

        resetPerspectiveProjection();
    }


}

/* Funcion que se llamara cada vez que se dibuje en pantalla */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)ancho / alto, znear, 10);

   //Iluminacion
    GLfloat difusa[] = { 1.0f, 1.0f, 1.0f, 1.0f}; // luz blanca

    GLfloat posicion0[] = { 50.0f, 25.0f, 10.0f, 0.0f}; // posición en la escena
    GLfloat posicion2[] = {-50.0f, -25.0f, 10.0f, 0.0f};
    GLfloat posicion3[] = {50.0f, -25.0f, 10.0f, 0.0f};


    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa); // Se asignan los parámetros
    glLightfv(GL_LIGHT0, GL_POSITION, posicion0); // a la luz 0
    glEnable(GL_LIGHT0); // Se “enciende” la luz 0

    glLightfv(GL_LIGHT2, GL_DIFFUSE, difusa); // Se asignan los parámetros
    glLightfv(GL_LIGHT2, GL_POSITION, posicion2); // a la luz 2
    glEnable(GL_LIGHT2); // Se “enciende” la luz 2

    glLightfv(GL_LIGHT3, GL_DIFFUSE, difusa); // Se asignan los parámetros
    glLightfv(GL_LIGHT3, GL_POSITION, posicion3); // a la luz 2
    glEnable(GL_LIGHT3); // Se “enciende” la luz 2

    glEnable(GL_LIGHTING); // Se activan los cálculos de la iluminación
    glShadeModel(GL_SMOOTH); // Se define el modelo suave

    //Final iluminacion
    // Aqui pondríamos la funcion gluLookAt si el observador se mueve
    gluLookAt(eyex, eyey, eyez, atx, aty, atz, 0.0, 0.0, upz);
    // Aqui se definen los objetos

    glCallList(fortalezaDL);
    glCallList(casa1DL);
    glCallList(casa2DL);
    glCallList(pozoDL);


    Mix_Chunk *pisada;
    pisada=Mix_LoadWAV("chain.wav");

    //sonido andar
    if(andando==1)
    {

        int canal_2;//reproduccion pisada
        while(Mix_Playing(canal_2));
        canal_2=Mix_PlayChannel(-1,pisada,0);
        andando=0;

    }
/*
    //Camara cenital
    if(Ccenital==1)
    {
        camaraCenital();

        glPushMatrix();
        //glDisable(GL_TEXTURE_2D);
        glColor3f(1,0,0);
        glTranslatef(eyex,eyey,eyez);
        //cube(2.0);

        glColor3f(1,0,0);
        glPopMatrix();
    }
    else
    {
        CalcFirstPersonPos();
    }
*/
//Panel informacion
    HUD();

    glutSwapBuffers();
    keyoperations();

}

/* Funcion que controla los eventos de teclado */
void keyboard(unsigned char key, int x, int y)
{
    keypressed[key] = true;
    switch(key)
    {
        case 27:  exit(0); /* tecla escape */
                  break;
        case 'f':
        case 'F': glutFullScreen();
                  break;
        case 'm':
        case 'M':Mix_HaltMusic();
                    break;
        case 'H':
        case 'h':
            if(HUDO==1){
                HUDO=0;
            }
            else if(HUDO==0)
            {
                HUDO=1;
            }
            break;
      /*  case 'p' :
        case 'P' :
            if(Ccenital==1)
            {
                Ccenital=0;
            }
            else if(Ccenital==0)
            {
                Ccenital=1;
            }
        break;
    */

    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    keypressed[key] = false;
}

void special(int key, int x, int y)
{
    specialpressed[key] = true;
}
void specialUp(int key, int x, int y)
{
    specialpressed[key] = false;

}
void raton(int  button,  int  state,  int  x,  int  y){
    if(button==GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
       beta=0.0f;
        display();
    }
    if(button==GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
       alfa=0.0f;
        display();
    }

}

/* Funcion que se ejecuta cuando no hay eventos */
void idle(void)
{
    glutPostRedisplay();
}

/* Funcion principal */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hola Mundo OpenGL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(raton);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);

    glutMainLoop();

    return 0;
}
