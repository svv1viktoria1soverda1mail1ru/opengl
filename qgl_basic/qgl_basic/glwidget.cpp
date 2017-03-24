#include "glwidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <windows.h>


GLWidget::GLWidget(QOpenGLWidget *parent) :
    QOpenGLWidget(parent)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(redraw()));     // timer bedzie wywolywal cyklicznie metode redraw()
    fps = 1;
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);  //wlaczenie flagi testowania z-bufora;(bufor glebi) lun GL_STENCIL_TEST
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    q = gluNewQuadric();   // jeden obiekt do rysowania kwadratyk

    //prostokat  ...tablica wierzch
    {
        vec3f verts[] = { {3.0f,  0.0f, 1.0f}, { -1.0f, 0.0f, 1.0f}, {-1.0f,  0.0f, -1.0f}, {3.0f,  0.0f, -1.0f} };
        vec3f vcols[] = { { 0.0f, 1.0f, 0.0f}, { 1.0f, 0.0f,  0.0f}, { 0.0f, 1.0f,  0.0f}, { 0.0f, 0.0f,  1.0f} };
        uint idx[]   = { 0,1,2,  0,2,3 };

        memcpy(rect, verts, 12*4);
        memcpy(rect_colors, vcols, 12*4);
        memcpy(rect_idx, idx, 6*4);
    }
    //prostokat ...bufor
    vec3f verts_buf[4] = { {3.0f,  0.0f, 1.0f}, { -1.0f, 0.0f, 1.0f}, {-1.0f,  0.0f, -1.0f}, {3.0f,  0.0f, -1.0f} };
    glGenBuffers(1, &vbo);// tworzenie
    glBindBuffer(GL_ARRAY_BUFFER, vbo);// zwiazanie
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts_buf), verts_buf, GL_STATIC_DRAW);//napelnienie
    glBindBuffer(GL_ARRAY_BUFFER, 0);// odwiazanie

    //osie  ...tablica wierzch
    {
        vec3f verts[] = { {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,1.0f}};
        vec3f vcols[] = { {1.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f,1.0f}};
        memcpy(axes, verts, 6*3*4);
        memcpy(axes_colors, vcols, 6*3*4);
    }

    time.start();                  // start pomiaru czasu
    timer.start(1000.0f / 50.0f);  // start timera wyzwalanego 50 razy na sekunde

}

void GLWidget::redraw()
{
    update();  // updateGL() wywoluje posrednio paintGL()

    // pomiar fps co 100 klatek
    if( (fps % 100) == 0)
    {
        setWindowTitle("FPS: " + QString::number(100 /*klatek*/ * 1000 /*milisekund*/ / time.elapsed()));
        time.restart();
    }
    ++fps;
}
void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0, w,h);    //jest funkcja definiujaca powierzchnie obszaru wyswietlania kontekstu renderowania OpenGL
                             //daje mozliwosc definiowania wielu widokow dla jednej sceny
    
    glMatrixMode(GL_PROJECTION);//macierz rzutowania 3D
    glLoadIdentity();
    gluPerspective(60, w/(GLfloat)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}



void GLWidget::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef(0.0f, -2.0f, -5.0f);
   glPushMatrix();

   glShadeModel(GL_SMOOTH);
   glRotatef(ay, 1, 0, 0);
   glRotatef(ax, 0, 1, 0);
   //////////////////////////////////////////////////////////////
   // instant mode    ....wprost
   glBegin(GL_TRIANGLE_FAN);
   {
       glColor3f(1.0f, 1.0f, 1.0f);//0
       glVertex3f(3.0f,  0.0f, 1.0f);

        glColor3f(1.0f, 1.0f, 1.0f);//1
        glVertex3f(-1.0f, 0.0f, 1.0f);
        
        glColor3f(1.0f, 1.0f, 1.0f);//2
        glVertex3f(-1.0f,  0.0f, -1.0f);
        
        glColor3f(1.0f, 1.0f, 1.0f);//3
        glVertex3f(3.0f,  0.0f, -1.0f);

   }
   glEnd();


   glPushMatrix();
   glTranslatef(0.0f,1.0f,0.0f);

   /////////////////////////////////////////////////////////////
    // array mode  ...tablice wierzcholkow

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   {
       glVertexPointer(3, GL_FLOAT, 0, rect);
       glColorPointer( 3, GL_FLOAT, 0, rect_colors);
       glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, rect_idx);
   }
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

   glPushMatrix();
   glTranslatef(0.0f,1.0f,0.0f);

   //////////////////////////////////////////////////////////////
   //buffer mode   ...bufory
   glColor3f(0.5, 1.0, 0.5);
   glEnableClientState(GL_VERTEX_ARRAY);
   {
       glBindBuffer(GL_ARRAY_BUFFER, vbo);   // zwiazanie bufora vbo z aktualnym
       glVertexPointer(3, GL_FLOAT, 0, 0);   // ustawienie wskaznika wierzcholkow (o 3 wspolrzednych kazdy, typu GL_FLOAT) na związany bufor od jego zerowego bajtu
       glBindBuffer(GL_ARRAY_BUFFER, 0);     // odwiazanie aktualnego bufora
       glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
   }
   glDisableClientState(GL_VERTEX_ARRAY);

   glPushMatrix();
   glTranslatef(-1.0f,-2.0f,-1.0f);

  //////////////////////////////////////////////////////////
   //osie ...tabl wierzch
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   {
       glVertexPointer(3, GL_FLOAT, 0, axes);
       glColorPointer(3, GL_FLOAT, 0, axes_colors);
       glDrawArrays(GL_LINES, 0, 6);
   }
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

  //////////////////////////////////////////////////////////

   glTranslatef(1.0f, 0.2f,1.0f);
   // rysowanie kwadratyki sfer
   glColor3f(1,0,0);
   gluSphere(q, 0.2, 16, 16);
   glTranslatef(2.0f, 0.0f,0.5f);
   gluSphere(q, 0.2, 16, 16);


   //rysowanie kwadratyki walca
   glColor3f(0.98f,0.625f,0.12f);
   glTranslatef(0.5f, -0.2f,0.5f);
   glRotatef(-90,1,0,0);
   gluCylinder(q, 0.15, 0.1, 2, 16, 16);
  // glRotatef(90,0,1,0);

   glPopMatrix();
   glPopMatrix();
   glPopMatrix();
   glPopMatrix();


   PRINT_GL_ERROR("ERROR paintGL()");
}

void GLWidget::mousePressEvent(QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)  // test czy klikniety zostal lewy przycisk mychy
    {
        ax0 = e->pos().x();
        ay0 = e->pos().y();
    }

        //e->pos().x(); // pozycja X mychy na ekranie w chwili klikniecia
}

void GLWidget::mouseMoveEvent(QMouseEvent * e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        ax -= ax0-e->pos().x();
        ay -= ay0-e->pos().y();

        ax0 = e->pos().x();
        ay0 = e->pos().y();
    }
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Q) 
		close();
}
void GLWidget::MatrixAdd(matrix3x3_t* matrixA, matrix3x3_t* matrixB, matrix3x3_t* resultMatrix){
    //A+B=result
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            resultMatrix->mat[row][col]=matrixA->mat[row][col]+matrixB->mat[row][col];
        }
    }
}

void GLWidget::ScalarMatrixMult(float scalarValue, matrix3x3_t *matrixA, matrix3x3_t *resultMatrix)
{
    //scalarValue*A=result
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            resultMatrix->mat[row][col]=scalarValue* matrixA->mat[row][col];
        }
    }
}

void GLWidget::MatrixMult(matrix3x3_t* matrixA, matrix3x3_t* matrixB, matrix3x3_t* resultMatrix)
{
    float sum;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            sum=0;
            for(int k=0;k<3;k++){
                sum+=matrixA->mat[row][k]*matrixB->mat[k][col];
            }
            resultMatrix->mat[row][col]=sum;
        }
    }
}
