#include "PistaCurva.h"
#include "Pista.h"
#include <GL/gl.h>
#include <cmath>

float PistaCurva::calculaPonto(char c, double t) {
    switch (c) {
        case 'x':
            return pow(1 - t, 3) * Bx[0]
                 + 3 * t * pow(1 - t, 2) * Bx[1]
                 + 3 * pow(t, 2) * (1 - t) * Bx[2]
                 + pow(t, 3) * Bx[3];
        case 'y':
            return pow(1 - t, 3) * By[0]
                 + 3 * t * pow(1 - t, 2) * By[1]
                 + 3 * pow(t, 2) * (1 - t) * By[2]
                 + pow(t, 3) * By[3];
        case 'z':
            return pow(1 - t, 3) * Bz[0]
                 + 3 * t * pow(1 - t, 2) * Bz[1]
                 + 3 * pow(t, 2) * (1 - t) * Bz[2]
                 + pow(t, 3) * Bz[3];
    }
    return 0;
}

void PistaCurva::desenha(float raio, float largura,
                         float altura, char direcao) {
    bool direcaoValida = direcao == 'D' || direcao == 'd'
                      || direcao == 'E' || direcao == 'e';
    if (!direcaoValida || raio <= 0 || largura <= 0 || altura <= 0)
        return;

    float lado = (direcao == 'D' || direcao == 'd') ? 1.0f : -1.0f;
    float controle = 0.5f * raio;
    const int segmentos = 80;
    const float pi = 3.14159265f;
    const float sobreposicao = largura / segmentos;

    // Quatro pontos de controle da curva de 90 graus.
    Bx[0] = 0;        By[0] = 0; Bz[0] = 0;
    Bx[1] = controle; By[1] = 0; Bz[1] = 0;
    Bx[2] = raio;     By[2] = 0; Bz[2] = lado * (raio - controle);
    Bx[3] = raio;     By[3] = 0; Bz[3] = lado * raio;

    Pista trechoReto;
    float xAnterior = calculaPonto('x', 0);
    float zAnterior = calculaPonto('z', 0);

    // Aproxima a curva usando varios pedacos pequenos da pista reta.
    for (int i = 1; i <= segmentos; i++) {
        float t = (float)(i) / segmentos;
        float xAtual = calculaPonto('x', t);
        float zAtual = calculaPonto('z', t);

        float dx = xAtual - xAnterior;
        float dz = zAtual - zAnterior;
        float tamanho = std::sqrt(dx * dx + dz * dz);
        float angulo = -std::atan2(dz, dx) * 180.0f / pi;

        glPushMatrix();
            glTranslatef(xAnterior, 0, zAnterior);
            glRotatef(angulo, 0, 1, 0);
            glTranslatef(-sobreposicao / 2.0f, 0,
                         -largura / 2.0f);
            trechoReto.desenha(tamanho + sobreposicao,
                               largura, altura);
        glPopMatrix();

        xAnterior = xAtual;
        zAnterior = zAtual;
    }
}
