#pragma once
// MONSTER TRUCK: picape elevada, pneus com garras, eixos e molas expostos.
// Origem no chão, frente em +x. A roda especial reutiliza rodaSRM e acrescenta
// garras no mesmo referencial: tudo esterça e rola junto, inclusive em ré.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroMonstro : public ModeloCarro {
public:
    static constexpr float COMP = 5.2f, LARG = 2.0f;
    static constexpr float EIXO_FRENTE = 1.65f, EIXO_TRAS = -1.65f;
    static constexpr float Z_RODA = 1.65f, RAIO_RODA = 1.08f;
    static constexpr float BASE = 1.95f, CINTURA = 2.60f, ARCO = 1.17f;
    static constexpr float DOBRADICA = 0.65f, PORTA_COMP = 1.30f;

    void desenhar(const EstadoCarro& e) override {
        chassiSRM();
        carroceriaSRM();
        cabineSRM();
        interiorSRM();
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamasSRU(lado);
            portaSRU(lado, e.anguloPortas);
            rodaMonstroSRU(EIXO_FRENTE, lado, e.anguloDirecao, e.distancia);
            rodaMonstroSRU(EIXO_TRAS, lado, e.anguloDirecaoTras, e.distancia);
            molaSRU(EIXO_FRENTE, lado);
            molaSRU(EIXO_TRAS, lado);
            lampada(e.luzesLigadas, true, 2.58f, 2.66f, 2.26f, 2.46f,
                    lado * 0.72f - 0.17f, lado * 0.72f + 0.17f);
            lampada(e.luzesLigadas, false, -2.66f, -2.58f, 2.23f, 2.48f,
                    lado * 0.83f - 0.09f, lado * 0.83f + 0.09f);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 2.69f, -2.69f, 2.36f, 0.72f);
    }

private:
    void pintura() { cor(0.42f, 0.06f, 0.70f); } // roxo
    void destaque() { cor(0.85f, 0.95f, 0.08f); } // amarelo-limão
    void preto() { cor(0.07f, 0.08f, 0.09f); }

    static float recorte(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? fmaxf(BASE, RAIO_RODA + sqrtf(ARCO * ARCO - d * d)) : BASE;
    }
    static float topoCapo(float x) {
        static const float t[][2] = {{0.65f, CINTURA}, {1.8f, 2.60f}, {2.60f, 2.51f}};
        return interpola(x, t, 3);
    }

    void chassiSRM() {
        preto();
        for (int lado = -1; lado <= 1; lado += 2) {
            caixa(-2.48f, 2.48f, 1.73f, BASE, lado * 0.42f, lado * 0.58f);
            barra(-0.40f, 1.82f, EIXO_FRENTE, RAIO_RODA, lado * 0.55f, 0.10f);
            barra(0.40f, 1.82f, EIXO_TRAS, RAIO_RODA, lado * 0.55f, 0.10f);
        }
        caixa(-0.67f, 0.67f, BASE, 2.03f, -0.99f, 0.99f); // piso da cabine
        for (int i = 0; i < 2; ++i) {
            float eixo = i == 0 ? EIXO_FRENTE : EIXO_TRAS;
            cor(0.28f, 0.30f, 0.33f);
            caixa(eixo - 0.10f, eixo + 0.10f, 0.98f, 1.18f, -Z_RODA, Z_RODA);
            caixa(eixo - 0.23f, eixo + 0.23f, 0.85f, 1.29f, -0.25f, 0.25f); // diferencial
        }
        cor(0.40f, 0.42f, 0.45f);
        caixa(EIXO_TRAS, EIXO_FRENTE, 1.10f, 1.20f, -0.07f, 0.07f); // transmissão
    }

    void molaSRM() {
        if (passadaDoVidro) return;
        cor(0.75f, 0.78f, 0.80f);
        caixa(-0.035f, 0.035f, 0, 0.68f, -0.035f, 0.035f); // amortecedor
        destaque();
        for (int i = 0; i < 8; ++i) {
            glPushMatrix();
            glTranslatef(0, 0.04f + i * 0.08f, 0);
            glRotatef(90, 1, 0, 0);
            glutSolidTorus(0.025, 0.11, 8, 16);
            glPopMatrix();
        }
    }
    void molaSRU(float eixo, int lado) {
        glPushMatrix();
        glTranslatef(eixo, 1.20f, lado * 0.68f);
        molaSRM();
        glPopMatrix();
    }

    void rodaMonstroSRM() {
        if (passadaDoVidro) return;
        // Raio do pneu = 3 * 0.35. A escala em z engrossa o pneu sem mudar o raio.
        glPushMatrix();
        glScalef(3, 3, 4.2f);
        rodaSRM();
        preto();
        // Duas fileiras alternadas de garras. A envoltória cabe em RAIO_RODA.
        for (int i = 0; i < 24; ++i) {
            for (int fileira = 0; fileira < 2; ++fileira) {
                glPushMatrix();
                glRotatef(i * 15.0f + fileira * 7.5f, 0, 0, 1);
                float z = fileira == 0 ? -0.07f : 0.07f;
                caixa(-0.04f, 0.04f, 0.322f, 0.356f, z - 0.06f, z + 0.06f);
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
    void rodaMonstroSRU(float eixo, int lado, float direcao, float distancia) {
        glPushMatrix();
        glTranslatef(eixo, RAIO_RODA, lado * Z_RODA);
        glRotatef(direcao, 0, 1, 0);
        glRotatef(-distancia / RAIO_RODA * 180 / PI_CARRO, 0, 0, 1);
        if (lado < 0) glRotatef(180, 0, 1, 0);
        rodaMonstroSRM();
        glPopMatrix();
    }

    void carroceriaSRM() {
        pintura();
        perfilExtrudado(0.65f, 2.60f, 1.20f, topoCapo, [](float) { return BASE; });
        caixa(-2.60f, -0.65f, BASE, 2.04f, -0.60f, 0.60f); // fundo da caçamba
        caixa(-2.60f, -2.51f, BASE, 2.58f, -1.0f, 1.0f);
        caixa(-0.75f, -0.65f, BASE, CINTURA, -1.0f, 1.0f); // parede da cabine
        preto();
        caixa(-2.50f, -0.76f, 2.04f, 2.07f, -0.58f, 0.58f);
        caixa(2.60f, 2.65f, 2.11f, 2.40f, -0.46f, 0.46f);
        caixa(2.52f, 2.77f, 1.99f, 2.12f, -0.96f, 0.96f);
        caixa(-2.77f, -2.52f, 1.99f, 2.12f, -0.96f, 0.96f);
        destaque();
        for (int lado = -1; lado <= 1; lado += 2) {
            glPushMatrix();
            glTranslatef(0, 0, lado * 0.34f);
            perfilExtrudado(0.70f, 2.59f, 0.16f,
                            [](float x) { return topoCapo(x) + 0.012f; },
                            [](float x) { return topoCapo(x) + 0.003f; });
            glPopMatrix();
        }
    }
    void paralamasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.80f);
        pintura();
        perfilExtrudado(0.65f, 2.60f, 0.40f, topoCapo,
                        [](float x) { return recorte(x, EIXO_FRENTE); }, 64);
        perfilExtrudado(-2.60f, -0.65f, 0.40f, [](float) { return 2.58f; },
                        [](float x) { return recorte(x, EIXO_TRAS); }, 64);
        glPopMatrix();
    }

    void cabineSRM() {
        pintura();
        caixa(-0.77f, 0.21f, 3.31f, 3.41f, -0.93f, 0.93f);
        for (int lado = -1; lado <= 1; lado += 2) {
            barra(0.65f, CINTURA, 0.18f, 3.33f, lado * 0.91f, 0.08f);
            caixa(-0.77f, -0.65f, CINTURA, 3.33f, lado * 0.85f, lado * 0.97f);
        }
        float a[] = {0.63f, 2.62f, 0.87f}, b[] = {0.63f, 2.62f, -0.87f};
        float c[] = {0.19f, 3.31f, -0.87f}, d[] = {0.19f, 3.31f, 0.87f};
        vidro(a, b, c, d);
        float r1[] = {-0.71f, 2.63f, -0.83f}, r2[] = {-0.71f, 2.63f, 0.83f};
        float r3[] = {-0.71f, 3.28f, 0.83f}, r4[] = {-0.71f, 3.28f, -0.83f};
        vidro(r1, r2, r3, r4);
    }
    void interiorSRM() {
        preto();
        caixa(0.32f, 0.65f, 2.39f, 2.59f, -0.86f, 0.86f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.43f;
            caixa(-0.43f, 0.09f, 2.04f, 2.22f, z - 0.27f, z + 0.27f);
            caixa(-0.54f, -0.42f, 2.11f, 2.93f, z - 0.27f, z + 0.27f);
            destaque();
            caixa(-0.415f, -0.40f, 2.28f, 2.85f, z - 0.035f, z + 0.035f); // faixa no banco
            preto();
        }
    }

    void portaSRM(int lado) {
        pintura();
        caixa(-PORTA_COMP, 0, 2.03f, CINTURA, -0.035f, 0.035f);
        caixa(-PORTA_COMP, 0, 2.56f, CINTURA, -lado * 0.09f, 0);
        destaque();
        barra(-1.15f, 2.16f, -0.18f, 2.43f, lado * 0.041f, 0.075f);
        preto();
        caixa(-1.10f, -0.89f, 2.48f, 2.53f, lado * 0.035f, lado * 0.065f);
        caixa(-0.18f, -0.10f, 2.63f, 2.71f, 0, lado * 0.25f);
        caixa(-0.24f, -0.08f, 2.67f, 2.86f, lado * 0.18f, lado * 0.34f);
        float z = -lado * 0.09f;
        float a[] = {-0.04f, 2.63f, z}, b[] = {-0.48f, 3.29f, z};
        float c[] = {-1.26f, 3.29f, z}, d[] = {-1.26f, 2.63f, z};
        vidro(a, b, c, d);
    }
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(DOBRADICA, 0, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }
};

} // namespace carros
