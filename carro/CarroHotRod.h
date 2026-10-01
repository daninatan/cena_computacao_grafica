#pragma once
// HOT ROD: cabine curta, teto rebaixado, motor V8 exposto e pneus traseiros maiores.
// SRM no chão, frente em +x. Cada eixo usa seu próprio raio em rodaSRU,
// mantendo o contato com o chão e a rolagem correta para a mesma distância.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroHotRod : public ModeloCarro {
public:
    static constexpr float COMP = 4.0f, LARG = 1.36f;
    static constexpr float EIXO_FRENTE = 1.55f, EIXO_TRAS = -1.25f;
    static constexpr float ESCALA_FRENTE = 1.0f, ESCALA_TRAS = 1.6f;
    static constexpr float DOBRADICA = 0.30f, PORTA_COMP = 1.25f;

    void desenhar(const EstadoCarro& e) override {
        carroceriaSRM();
        motorSRM();
        cabineSRM();
        interiorSRM();
        for (int lado = -1; lado <= 1; lado += 2) {
            portaSRU(lado, e.anguloPortas);
            rodaSRU(EIXO_FRENTE, lado * 0.98f, e.anguloDirecao, e.distancia, ESCALA_FRENTE);
            rodaSRU(EIXO_TRAS, lado * 1.15f, e.anguloDirecaoTras, e.distancia, ESCALA_TRAS);
            farolSRU(lado, e.luzesLigadas);
            lampada(e.luzesLigadas, false, -2.04f, -1.98f, 0.48f, 0.62f,
                    lado * 0.49f - 0.08f, lado * 0.49f + 0.08f);
        }
    }
    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 1.99f, -2.07f, 0.83f, 0.61f);
    }

private:
    void pintura() { cor(0.95f, 0.29f, 0.03f); } // laranja
    void preto() { cor(0.06f, 0.06f, 0.07f); }
    void cromo() { cor(0.77f, 0.80f, 0.83f); }

    void carroceriaSRM() {
        preto();
        caixa(-1.94f, 1.92f, 0.24f, 0.36f, -0.40f, 0.40f);
        caixa(-0.95f, 0.30f, 0.30f, 0.39f, -0.68f, 0.68f);
        // Eixos visíveis sob as rodas descobertas.
        cromo();
        caixa(1.50f, 1.60f, 0.30f, 0.40f, -0.98f, 0.98f);
        caixa(-1.31f, -1.19f, 0.50f, 0.62f, -1.15f, 1.15f);
        pintura();
        static const float t[][2] = {{-2.0f, 0.65f}, {-1.75f, 0.88f}, {-0.95f, 0.97f}};
        perfilExtrudado(-2.0f, -0.95f, LARG,
                        [](float x) { return interpola(x, t, 3); },
                        [](float) { return 0.36f; });
        caixa(0.20f, 0.30f, 0.36f, 0.95f, -0.68f, 0.68f); // corta-fogo
        caixa(1.78f, 1.93f, 0.36f, 1.04f, -0.42f, 0.42f); // moldura do radiador
        preto();
        caixa(1.93f, 1.95f, 0.43f, 0.98f, -0.35f, 0.35f);
        cromo();
        for (int i = -4; i <= 4; ++i)
            caixa(1.95f, 1.97f, 0.45f, 0.96f, i * 0.075f - 0.012f, i * 0.075f + 0.012f);
        caixa(1.87f, 2.05f, 0.25f, 0.32f, -0.64f, 0.64f);
        caixa(-2.07f, -1.94f, 0.31f, 0.38f, -0.66f, 0.66f);
    }

    void motorSRM() {
        cor(0.27f, 0.29f, 0.32f);
        caixa(0.40f, 1.45f, 0.38f, 0.71f, -0.30f, 0.30f); // bloco
        // Duas bancadas inclinadas, quatro cabeçotes e quatro coletores por lado.
        for (int lado = -1; lado <= 1; lado += 2) {
            glPushMatrix();
            glTranslatef(0, 0.65f, lado * 0.22f);
            glRotatef(lado * 25.0f, 1, 0, 0);
            cromo();
            caixa(0.41f, 1.44f, 0, 0.20f, -0.14f, 0.14f);
            preto();
            for (int i = 0; i < 4; ++i) {
                float x = 0.48f + i * 0.24f;
                caixa(x, x + 0.07f, 0.20f, 0.23f, -0.12f, 0.12f);
            }
            glPopMatrix();
            cromo();
            for (int i = 0; i < 4; ++i) {
                float x = 0.49f + i * 0.24f;
                caixa(x - 0.035f, x + 0.035f, 0.58f, 0.65f, lado * 0.28f, lado * 0.57f);
                barra(x, 0.61f, x - 0.19f, 0.42f, lado * 0.57f, 0.07f);
            }
            caixa(-0.72f, 1.23f, 0.36f, 0.44f, lado * 0.53f, lado * 0.63f); // escapamento lateral
            preto();
            caixa(-0.74f, -0.72f, 0.375f, 0.425f, lado * 0.55f, lado * 0.61f);
        }
        cromo();
        caixa(0.67f, 1.19f, 0.78f, 0.98f, -0.21f, 0.21f); // compressor
        caixa(0.74f, 1.24f, 0.98f, 1.12f, -0.25f, 0.25f); // tomada de ar
        preto();
        caixa(1.24f, 1.26f, 1.01f, 1.09f, -0.20f, 0.20f);
        caixa(1.44f, 1.48f, 0.44f, 0.88f, -0.035f, 0.035f); // correia frontal
    }

    void cabineSRM() {
        preto();
        caixa(-0.97f, -0.03f, 1.35f, 1.43f, -0.65f, 0.65f); // teto rebaixado
        pintura();
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.63f;
            barra(0.29f, 0.95f, -0.05f, 1.36f, z, 0.055f);
            barra(-1.37f, 0.96f, -0.95f, 1.36f, z, 0.08f);
            caixa(-0.99f, -0.92f, 0.96f, 1.35f, z - 0.025f, z + 0.025f);
            float a[] = {-1.31f, 0.99f, z}, b[] = {-1.01f, 0.99f, z};
            float c[] = {-1.01f, 1.28f, z}, d[] = {-1.03f, 1.28f, z};
            vidro(a, b, c, d);
        }
        float a[] = {0.27f, 0.97f, 0.60f}, b[] = {0.27f, 0.97f, -0.60f};
        float c[] = {-0.05f, 1.35f, -0.60f}, d[] = {-0.05f, 1.35f, 0.60f};
        vidro(a, b, c, d);
        float r1[] = {-1.35f, 0.98f, -0.58f}, r2[] = {-1.35f, 0.98f, 0.58f};
        float r3[] = {-0.96f, 1.35f, 0.58f}, r4[] = {-0.96f, 1.35f, -0.58f};
        vidro(r1, r2, r3, r4);
    }
    void interiorSRM() {
        preto();
        caixa(0.07f, 0.28f, 0.77f, 0.94f, -0.59f, 0.59f);
        cor(0.45f, 0.06f, 0.045f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.32f;
            caixa(-0.74f, -0.23f, 0.39f, 0.53f, z - 0.23f, z + 0.23f);
            caixa(-0.84f, -0.72f, 0.42f, 1.08f, z - 0.23f, z + 0.23f);
        }
    }
    void portaSRM(int lado) {
        pintura();
        caixa(-PORTA_COMP, 0, 0.39f, 0.95f, -0.025f, 0.025f);
        caixa(-PORTA_COMP, 0, 0.92f, 0.95f, -lado * 0.05f, 0);
        // Dois frisos inclinados claros dão identidade ao pequeno cupê.
        cor(1.0f, 0.80f, 0.22f);
        barra(-1.08f, 0.48f, -0.15f, 0.74f, lado * 0.03f, 0.05f);
        barra(-0.90f, 0.45f, -0.14f, 0.61f, lado * 0.03f, 0.035f);
        cromo();
        caixa(-1.09f, -0.91f, 0.83f, 0.87f, lado * 0.025f, lado * 0.05f);
        caixa(-0.18f, -0.12f, 0.96f, 1.03f, 0, lado * 0.15f);
        caixa(-0.21f, -0.10f, 1.0f, 1.10f, lado * 0.12f, lado * 0.22f);
        float z = -lado * 0.05f;
        float a[] = {-0.04f, 0.97f, z}, b[] = {-0.36f, 1.33f, z};
        float c[] = {-1.20f, 1.33f, z}, d[] = {-1.20f, 0.97f, z};
        vidro(a, b, c, d);
    }
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(DOBRADICA, 0, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }
    void farolSRU(int lado, bool aceso) {
        cromo();
        caixa(1.76f, 1.86f, 0.61f, 0.83f, lado * 0.57f, lado * 0.65f);
        caixa(1.70f, 1.94f, 0.71f, 0.95f, lado * 0.61f - 0.13f, lado * 0.61f + 0.13f);
        lampada(aceso, true, 1.94f, 1.96f, 0.74f, 0.92f,
                lado * 0.61f - 0.10f, lado * 0.61f + 0.10f);
    }
};

} // namespace carros
