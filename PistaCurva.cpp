#include "PistaCurva.h"

#include <GL/gl.h>
#include <cmath>

// Mesma funcao apresentada no PDF da aula.
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
    float controle = 0.55228475f * raio;
    const float espessura = 1.0f;
    const int quantidade = 51;

    // Quatro pontos de controle da curva de 90 graus.
    Bx[0] = 0;        By[0] = 0; Bz[0] = 0;
    Bx[1] = controle; By[1] = 0; Bz[1] = 0;
    Bx[2] = raio;     By[2] = 0; Bz[2] = lado * (raio - controle);
    Bx[3] = raio;     By[3] = 0; Bz[3] = lado * raio;

    float x[quantidade], y[quantidade], z[quantidade];
    float tangenteX[quantidade], tangenteZ[quantidade];
    float lateralX[quantidade], lateralZ[quantidade];

    // Mesmo laco do PDF: t vai de 0 a 1 com passo 0,02.
    for (int i = 0; i < quantidade; i++) {
        float t = i * 0.02f;
        float u = 1.0f - t;

        x[i] = calculaPonto('x', t);
        y[i] = calculaPonto('y', t);
        z[i] = calculaPonto('z', t);

        // A derivada fornece a direcao da curva e, portanto, das normais.
        float dx = 3 * u * u * (Bx[1] - Bx[0])
                 + 6 * u * t * (Bx[2] - Bx[1])
                 + 3 * t * t * (Bx[3] - Bx[2]);
        float dz = 3 * u * u * (Bz[1] - Bz[0])
                 + 6 * u * t * (Bz[2] - Bz[1])
                 + 3 * t * t * (Bz[3] - Bz[2]);
        float tamanho = std::sqrt(dx * dx + dz * dz);

        tangenteX[i] = dx / tamanho;
        tangenteZ[i] = dz / tamanho;
        lateralX[i] = -tangenteZ[i];
        lateralZ[i] = tangenteX[i];
    }

    float metade = largura / 2.0f;

    // Perfil da pista: piso e duas protecoes laterais.
    // As oito arestas percorrem todo o contorno sem repetir faces.
    const float perfil[8][2] = {
        {-metade, espessura},
        {metade, espessura},
        {metade, altura},
        {metade + espessura, altura},
        {metade + espessura, 0},
        {-metade - espessura, 0},
        {-metade - espessura, altura},
        {-metade, altura}
    };

    // 0 = cima, 1 = baixo, 2 = lateral positiva, 3 = lateral negativa.
    const int normais[8] = {0, 3, 0, 2, 1, 3, 0, 2};

    glColor3f(1.0f, 0.35f, 0.0f);

    for (int face = 0; face < 8; face++) {
        int proxima = (face + 1) % 8;
        glBegin(GL_QUAD_STRIP);

        for (int i = 0; i < quantidade; i++) {
            if (normais[face] == 0)
                glNormal3f(0, 1, 0);
            else if (normais[face] == 1)
                glNormal3f(0, -1, 0);
            else {
                float sinal = normais[face] == 2 ? 1.0f : -1.0f;
                glNormal3f(sinal * lateralX[i], 0,
                           sinal * lateralZ[i]);
            }

            glVertex3f(x[i] + lateralX[i] * perfil[face][0],
                       y[i] + perfil[face][1],
                       z[i] + lateralZ[i] * perfil[face][0]);
            glVertex3f(x[i] + lateralX[i] * perfil[proxima][0],
                       y[i] + perfil[proxima][1],
                       z[i] + lateralZ[i] * perfil[proxima][0]);
        }

        glEnd();
    }

    // Retangulos que fecham o piso e as duas laterais nas extremidades.
    const float partes[3][4] = {
        {-metade, 0, metade, espessura},
        {-metade - espessura, 0, -metade, altura},
        {metade, 0, metade + espessura, altura}
    };
    const int verticesTampa[2][4][2] = {
        {{0, 1}, {2, 1}, {2, 3}, {0, 3}},
        {{0, 3}, {2, 3}, {2, 1}, {0, 1}}
    };

    for (int ponta = 0; ponta < 2; ponta++) {
        int i = ponta == 0 ? 0 : quantidade - 1;
        float sinal = ponta == 0 ? -1.0f : 1.0f;
        glNormal3f(sinal * tangenteX[i], 0, sinal * tangenteZ[i]);
        glBegin(GL_QUADS);

        for (int parte = 0; parte < 3; parte++) {
            for (int vertice = 0; vertice < 4; vertice++) {
                float distancia =
                    partes[parte][verticesTampa[ponta][vertice][0]];
                float alturaVertice =
                    partes[parte][verticesTampa[ponta][vertice][1]];

                glVertex3f(x[i] + lateralX[i] * distancia,
                           y[i] + alturaVertice,
                           z[i] + lateralZ[i] * distancia);
            }
        }

        glEnd();
    }
}
