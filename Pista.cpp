#include "Pista.h"
#include <GL/gl.h>

void Pista::desenha(float tamanho, float largura, float altura){

    glColor3f(1.0f, 0.35f, 0.0f);

    //face de baixo
    glBegin(GL_POLYGON);
        glNormal3f(0, -1, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(tamanho, 0, 0);
        glVertex3d(tamanho, 0, largura);
        glVertex3d(0, 0, largura);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, 1, 0);
        glVertex3d(0, 1, 0);
        glVertex3d(0, 1, largura);
        glVertex3d(tamanho, 1, largura);
        glVertex3d(tamanho, 1, 0);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(-1, 0, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, largura);
        glVertex3d(0, 1, largura);
        glVertex3d(0, 1, 0);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(1, 0, 0);
        glVertex3d(tamanho, 0, 0);
        glVertex3d(tamanho, 1, 0);
        glVertex3d(tamanho, 1, largura);
        glVertex3d(tamanho, 0, largura);
    glEnd();


    //face lateral 1
    glBegin(GL_POLYGON);
        glNormal3f(0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(tamanho, 0, 0);
        glVertex3d(tamanho, altura, 0);
        glVertex3d(0, altura, 0);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, 0, -1);
        glVertex3d(0, 0, -1);
        glVertex3d(0, altura, -1);
        glVertex3d(tamanho, altura, -1);
        glVertex3d(tamanho, 0, -1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, -1, 0);
        glVertex3d(0, 0, -1);
        glVertex3d(tamanho, 0, -1);
        glVertex3d(tamanho, 0, 0);
        glVertex3d(0, 0, 0);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, 1, 0);
        glVertex3d(0, altura, -1);
        glVertex3d(0, altura, 0);
        glVertex3d(tamanho, altura, 0);
        glVertex3d(tamanho, altura, -1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(-1, 0, 0);
        glVertex3d(0, 0, -1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, altura, 0);
        glVertex3d(0, altura, -1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(1, 0, 0);
        glVertex3d(tamanho, 0, -1);
        glVertex3d(tamanho, altura, -1);
        glVertex3d(tamanho, altura, 0);
        glVertex3d(tamanho, 0, 0);
    glEnd();

    //face lateral 2
    glBegin(GL_POLYGON);
        glNormal3f(0, 0, -1);
        glVertex3d(0, 0, largura);
        glVertex3d(0, altura, largura);
        glVertex3d(tamanho, altura, largura);
        glVertex3d(tamanho, 0, largura);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, 0, 1);
        glVertex3d(0, 0, largura + 1);
        glVertex3d(tamanho, 0, largura + 1);
        glVertex3d(tamanho, altura, largura + 1);
        glVertex3d(0, altura, largura + 1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, -1, 0);
        glVertex3d(0, 0, largura);
        glVertex3d(tamanho, 0, largura);
        glVertex3d(tamanho, 0, largura + 1);
        glVertex3d(0, 0, largura + 1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(0, 1, 0);
        glVertex3d(0, altura, largura);
        glVertex3d(0, altura, largura + 1);
        glVertex3d(tamanho, altura, largura + 1);
        glVertex3d(tamanho, altura, largura);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(1, 0, 0);
        glVertex3d(tamanho, 0, largura);
        glVertex3d(tamanho, altura, largura);
        glVertex3d(tamanho, altura, largura + 1);
        glVertex3d(tamanho, 0, largura + 1);
    glEnd();
    glBegin(GL_POLYGON);
        glNormal3f(-1, 0, 0);
        glVertex3d(0, 0, largura);
        glVertex3d(0, 0, largura + 1);
        glVertex3d(0, altura, largura + 1);
        glVertex3d(0, altura, largura);
    glEnd();

}
