#include "PistaCurva.h"

#include <GL/gl.h>
#include <cmath>

const float ESPESSURA = 1.0f;

// Mesma função apresentada no PDF da aula.
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

// Calcula o centro da pista e a direção perpendicular à curva.
void PistaCurva::calculaSecao(float t, float centro[3], float lateral[3]) {
    centro[0] = calculaPonto('x', t);
    centro[1] = calculaPonto('y', t);
    centro[2] = calculaPonto('z', t);

    // Derivada exata da curva de Bezier. Nas extremidades, ela produz
    // exatamente a mesma direcao das pistas retas conectadas.
    float u = 1.0f - t;
    float dx = 3 * u * u * (Bx[1] - Bx[0])
             + 6 * u * t * (Bx[2] - Bx[1])
             + 3 * t * t * (Bx[3] - Bx[2]);
    float dz = 3 * u * u * (Bz[1] - Bz[0])
             + 6 * u * t * (Bz[2] - Bz[1])
             + 3 * t * t * (Bz[3] - Bz[2]);
    float tamanho = sqrt(dx * dx + dz * dz);

    lateral[0] = -dz / tamanho;
    lateral[1] = 0;
    lateral[2] = dx / tamanho;
}

void PistaCurva::colocaVertice(float centro[3], float lateral[3],
                               float distancia, float altura) {
    glVertex3f(centro[0] + lateral[0] * distancia,
               centro[1] + altura,
               centro[2] + lateral[2] * distancia);
}

// Desenha uma superfície contínua acompanhando a curva.
void PistaCurva::desenhaFaixa(float distanciaA, float alturaA,
                              float distanciaB, float alturaB, char normal) {
    glBegin(GL_QUAD_STRIP);

    // Mesmo intervalo de 0,02 utilizado no PDF.
    for (int i = 0; i <= 50; i++) {
        float t = i * 0.02f;
        float centro[3], lateral[3];
        calculaSecao(t, centro, lateral);

        if (normal == 'C')
            glNormal3f(0, 1, 0);
        else if (normal == 'B')
            glNormal3f(0, -1, 0);
        else {
            float lado = normal == '+' ? 1.0f : -1.0f;
            glNormal3f(lateral[0] * lado, 0, lateral[2] * lado);
        }

        colocaVertice(centro, lateral, distanciaA, alturaA);
        colocaVertice(centro, lateral, distanciaB, alturaB);
    }

    glEnd();
}

// Fecha as extremidades do piso e das duas proteções laterais.
void PistaCurva::desenhaTampa(float t, float largura,
                              float altura, int sentido) {
    float centro[3], lateral[3];
    calculaSecao(t, centro, lateral);

    float metade = largura / 2;
    glNormal3f(sentido * lateral[2], 0, sentido * -lateral[0]);
    glBegin(GL_QUADS);

        if (sentido < 0) {
            colocaVertice(centro, lateral, -metade, 0);
            colocaVertice(centro, lateral, metade, 0);
            colocaVertice(centro, lateral, metade, ESPESSURA);
            colocaVertice(centro, lateral, -metade, ESPESSURA);

            colocaVertice(centro, lateral, -metade - ESPESSURA, 0);
            colocaVertice(centro, lateral, -metade, 0);
            colocaVertice(centro, lateral, -metade, altura);
            colocaVertice(centro, lateral, -metade - ESPESSURA, altura);

            colocaVertice(centro, lateral, metade, 0);
            colocaVertice(centro, lateral, metade + ESPESSURA, 0);
            colocaVertice(centro, lateral, metade + ESPESSURA, altura);
            colocaVertice(centro, lateral, metade, altura);
        } else {
            // Na extremidade final, a ordem precisa ser invertida para que
            // a normal geometrica aponte para fora da peca.
            colocaVertice(centro, lateral, -metade, ESPESSURA);
            colocaVertice(centro, lateral, metade, ESPESSURA);
            colocaVertice(centro, lateral, metade, 0);
            colocaVertice(centro, lateral, -metade, 0);

            colocaVertice(centro, lateral, -metade - ESPESSURA, altura);
            colocaVertice(centro, lateral, -metade, altura);
            colocaVertice(centro, lateral, -metade, 0);
            colocaVertice(centro, lateral, -metade - ESPESSURA, 0);

            colocaVertice(centro, lateral, metade, altura);
            colocaVertice(centro, lateral, metade + ESPESSURA, altura);
            colocaVertice(centro, lateral, metade + ESPESSURA, 0);
            colocaVertice(centro, lateral, metade, 0);
        }

    glEnd();
}

void PistaCurva::desenha(float raio, float largura,
                         float altura, char direcao) {
    if (direcao != 'D' && direcao != 'd'
        && direcao != 'E' && direcao != 'e')
        return;

    float lado = (direcao == 'D' || direcao == 'd') ? 1.0f : -1.0f;
    float controle = 0.55228475f * raio;

    // Pontos de controle de uma curva de 90 graus.
    Bx[0] = 0;        By[0] = 0; Bz[0] = 0;
    Bx[1] = controle; By[1] = 0; Bz[1] = 0;
    Bx[2] = raio;     By[2] = 0; Bz[2] = lado * (raio - controle);
    Bx[3] = raio;     By[3] = 0; Bz[3] = lado * raio;

    float metade = largura / 2;
    glColor3f(1.0f, 0.35f, 0.0f);

    // Piso com espessura.
    desenhaFaixa(-metade, ESPESSURA, metade, ESPESSURA, 'C');
    desenhaFaixa(metade, 0, -metade, 0, 'B');

    // Proteção esquerda com espessura.
    desenhaFaixa(-metade, altura, -metade, 0, '+');
    desenhaFaixa(-metade - ESPESSURA, 0,
                 -metade - ESPESSURA, altura, '-');
    desenhaFaixa(-metade - ESPESSURA, altura, -metade, altura, 'C');
    desenhaFaixa(-metade, 0, -metade - ESPESSURA, 0, 'B');

    // Proteção direita com espessura.
    desenhaFaixa(metade, 0, metade, altura, '-');
    desenhaFaixa(metade + ESPESSURA, altura,
                 metade + ESPESSURA, 0, '+');
    desenhaFaixa(metade, altura, metade + ESPESSURA, altura, 'C');
    desenhaFaixa(metade + ESPESSURA, 0, metade, 0, 'B');

    desenhaTampa(0, largura, altura, -1);
    desenhaTampa(1, largura, altura, 1);
}
