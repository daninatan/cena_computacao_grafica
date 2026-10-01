#include "ConstrutorDePista.h"

#include <GL/gl.h>
#include <cmath>

void ConstrutorDePista::desenhaReta(float tamanho, float subida,
                                    float largura, float altura) {
    // Rampa: inclina a reta em volta do eixo z, no comeco dela.
    float angulo = atan2f(subida, tamanho) * 180.0f / 3.14159265f;
    glPushMatrix();
        glRotatef(angulo, 0, 0, 1);
        glTranslatef(0, 0, -largura / 2);
        reta.desenha(hypotf(tamanho, subida), largura, altura);
    glPopMatrix();

    glTranslatef(tamanho, subida, 0);
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

// Tracado: cada trecho e uma reta (que pode subir ou descer) seguida de uma
// curva de 90 graus. 'D' vira para +z local, 'E' para -z, 0 emenda outra reta.
static const struct { float reta, subida; char curva; } TRECHOS[] = {
    {60, 0, 0}, {160, 55, 0}, {140, 0, 0}, {160, -55, 'D'},   // fundo: morro
    {140, 30, 0}, {120, 0, 0}, {140, -30, 'D'},               // direita: lombada
    {80, 0, 'D'}, {40, 0, 'E'}, {40, 0, 'E'}, {40, 0, 'D'},   // frente: "U" para dentro
    {80, 0, 'D'},
    {400, 0, 'D'},                                            // esquerda: tunel
};
static const float RAIO = 80.0f;
static const float LARGURA = 2 * ConstrutorDePista::MEIA_LARGURA;
static const float ALTURA = 12.0f;

void ConstrutorDePista::desenha() {
    glPushMatrix();
        for (const auto& t : TRECHOS) {
            desenhaReta(t.reta, t.subida, LARGURA, ALTURA);
            if (t.curva)
                desenhaCurva(RAIO, LARGURA, ALTURA, t.curva);
        }
    glPopMatrix();
}

std::vector<ConstrutorDePista::Ponto> ConstrutorDePista::linhaCentral(float passo) {
    // Percorre o mesmo tracado do desenha(). A curva Bezier e tratada como arco.
    // Nao repete pontos: o fim de cada trecho e o comeco do proximo.
    std::vector<Ponto> pontos;
    float x = 0, y = 0, z = 0, dx = 1, dz = 0;
    for (const auto& t : TRECHOS) {
        for (float s = 0; s < t.reta; s += passo)
            pontos.push_back({x + dx * s, y + t.subida * s / t.reta, z + dz * s});
        x += dx * t.reta;
        y += t.subida;
        z += dz * t.reta;
        if (!t.curva)
            continue;

        float lado = t.curva == 'D' ? 1.0f : -1.0f;
        float rx = -dz * lado, rz = dx * lado;  // para onde a curva vira
        int n = (int)(RAIO * 1.5708f / passo) + 1;
        for (int k = 0; k < n; k++) {
            float a = 1.5708f * k / n;
            pontos.push_back({x + (dx * sinf(a) + rx * (1 - cosf(a))) * RAIO, y,
                              z + (dz * sinf(a) + rz * (1 - cosf(a))) * RAIO});
        }
        x += (dx + rx) * RAIO;
        z += (dz + rz) * RAIO;
        dx = rx;
        dz = rz;
    }
    return pontos;
}
