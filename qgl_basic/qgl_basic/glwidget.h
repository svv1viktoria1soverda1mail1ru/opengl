#ifndef QGLIDGET_H
#define QGLIDGET_H

#include <QTimer>
#include <QTime>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_1>
#include <GL/glu.h>

typedef struct
{
    float mat[3][3];
}matrix3x3_t;

struct vec3f   //struktura dla wektora
{
    GLfloat x,y,z;
};

#define PRINT_GL_ERROR(msg)  { GLenum err; \
                               while( (err=glGetError())!= GL_NO_ERROR ) \
                               { qDebug() << msg << "in line" << __LINE__ << ": "<< (const char*)gluErrorString(err); } }



class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_2_1
{

    Q_OBJECT

    //prymitywy
    GLUquadric* q;

    //czas
    QTimer timer;   // wyzwalacz dla kolejnych klatek
    QTime time;     // pomiar czasu
    int fps;        // miara ilosci klatek na sekunde

    int ax, ay, ax0, ay0;  // wspolrzedne myszy

    //dane dla tablicy weirzcholkow
    vec3f rect[4],axes[6];         // wierzcholki
    vec3f rect_colors[4],axes_colors[6];  // kolory
    uint rect_idx[6];

    //dane dla bufora
    GLuint vbo;   // uchwyt do bufora wierzcholkow - Vertex Buffer Object

   //operacje na macierzach
    void MatrixAdd(matrix3x3_t* matrixA, matrix3x3_t* matrixB, matrix3x3_t* resultMatrix);
    void ScalarMatrixMult(float scalarValue, matrix3x3_t* matrixA, matrix3x3_t* resultMatrix);
    void MatrixMult(matrix3x3_t* matrixA, matrix3x3_t* matrixB, matrix3x3_t* resultMatrix);
public:
    explicit GLWidget(QOpenGLWidget *parent = 0);
    
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *);

signals:
    
public slots:
    void redraw();
};

#endif // QGLIDGET_H
