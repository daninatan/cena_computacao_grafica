#include "ConstrutorDePista.h"

#include <GL/gl.h>

void ConstrutorDePista::desenhaReta(float tamanho, float largura,
                                    float altura) {
    glPushMatrix();
        glTranslatef(0, 0, -largura / 2);
        reta.desenha(tamanho, largura, altura);
    glPopMatrix();

    glTranslatef(tamanho, 0, 0);
}

void ConstrutorDePista::desenhaCurva(float raio, float largura,
                                     float altura, char direcao) {
    curva.desenha(raio, largura, altura, direcao);

    if (direcao == 'D') {
        glTranslatef(raio, 0, raio);
        glRotatef(-90, 0, 1, 0);
    } else {
        glTranslatef(raio, 0, -raio);
        glRotatef(90, 0, 1, 0);
    }
}

void ConstrutorDePista::desenha() {
   
    const float raio = 50.0f;
    const float largura = 40.0f;
    const float altura = 12.0f;

    glPushMatrix();
        desenhaReta(160, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(80, largura, altura);
        desenhaCurva(raio, largura, altura, 'E');

        desenhaReta(100, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(140, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(70, largura, altura);
        desenhaCurva(raio, largura, altura, 'E');

        desenhaReta(70, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(140, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(100, largura, altura);
        desenhaCurva(raio, largura, altura, 'E');

        desenhaReta(150, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(100, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');

        desenhaReta(100, largura, altura);
        desenhaCurva(raio, largura, altura, 'E');

        desenhaReta(90, largura, altura);
        desenhaCurva(raio, largura, altura, 'D');
    glPopMatrix();
}
