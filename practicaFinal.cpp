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

using namespace std;

int ancho, alto;
GLfloat angx=0.0f,angy=0.0f,angz=0.0f;
GLfloat alfa=0.0f, beta=0.0f;
GLfloat upz =1.0;
GLdouble xesf1,yesf1,zesf1,eyex,eyey,eyez,atx,aty,atz;
GLfloat paso=0.05f;
GLdouble znear= 0.001f;
GLfloat tamobj= 0.2;

/*definiciones para las listas de visualización*/
GLuint fortalezaDL;
GLuint casa1DL;
GLuint casa2DL;
GLuint pozoDL;


/*variables y estructuras de datos para practica3*/

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
    atx=eyex+znear*cos(beta)*cos(alfa);
    aty=eyey+znear*cos(beta)*sin(alfa);
    atz=eyez+znear*sin(beta);
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



    /*se cargan los objetos en la lista de visualización*/

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




    glutSwapBuffers();
}

/* Funcion que se llamara cada vez que se redimensione la ventana */
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

/* Funcion que controla los eventos de teclado */
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:  exit(0); /* tecla escape */
                  break;
        case 'f':
        case 'F': glutFullScreen();
                  break;
        case 'w':
        case 'W': eyex+=paso*cos(alfa);
                  eyey+=paso*sin(alfa);
                  CalcFirstPersonPos();
                  break;
        case 'S':
        case 's': eyex-=paso*cos(alfa);
                  eyey-=paso*sin(alfa);
                  CalcFirstPersonPos();
                    break;
        case 'D':
        case 'd': eyex+=paso*sin(alfa);
                  eyey-=paso*cos(alfa);
                    CalcFirstPersonPos();
                    break;
        case 'A':
        case 'a': eyex-=paso*sin(alfa);
                  eyey+=paso*cos(alfa);
                  CalcFirstPersonPos();
                  break;

                  /*en vez de las flechas de dirección se utiliza hjkl, ya que
                  tube problemas con reconocer las flechas y voy algo ajustado de tiempo*/
                    //arriba
        case 'h' :          alfa+=0.1;
                            CalcFirstPersonPos();


                          display();
                  break;
                  //izq
        case 'j' :          beta-=0.1;
                            CalcFirstPersonPos();

                          display();
                  break;
                  //der
        case 'k' :          beta+=0.1;
                            CalcFirstPersonPos();

                          display();
                  break;
                  //abajo
        case 'l' :          alfa-=0.1;
                            CalcFirstPersonPos();
                          display();
                  break;

    }

    glutPostRedisplay();
}
void raton(int  button,  int  state,  int  x,  int  y){
    if(button==GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        printf("click");
       beta=0.0f;
        display();
    }
    if(button==GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        printf("click");
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

    glutMainLoop();

    return 0;
}
