#pragma once
// CONVERSÍVEL clássico: capô longo, dois lugares, cabine aberta e capota recolhida.
// Origem no chão, frente em +x. A capota é fixa; EstadoCarro controla portas,
// rodas, direção e luzes, sem acrescentar estados à interface compartilhada.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroConversivel : public ModeloCarro {
public:
    static constexpr float COMP = 4.4f, LARG = 1.8f;
    static constexpr float EIXO_FRENTE = 1.40f, EIXO_TRAS = -1.35f;
    static constexpr float Z_RODA = 0.88f, BASE = 0.22f, CINTURA = 0.80f;
    static constexpr float DOBRADICA = 0.85f, PORTA_COMP = 1.70f, ARCO = 0.44f;

    void desenhar(const EstadoCarro& e) override {
        carroceriaSRM();
        paraBrisaSRM();
        interiorSRM();
        volanteSRU(e.anguloDirecao);
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamasSRU(lado);
            portaSRU(lado, e.anguloPortas);
            rodaSRU(EIXO_FRENTE, lado * Z_RODA, e.anguloDirecao, e.distancia);
            rodaSRU(EIXO_TRAS, lado * Z_RODA, e.anguloDirecaoTras, e.distancia);
            lampada(e.luzesLigadas, true, 2.17f, 2.23f, 0.60f, 0.77f,
                    lado * 0.65f - 0.13f, lado * 0.65f + 0.13f);
            lampada(e.luzesLigadas, false, -2.23f, -2.18f, 0.62f, 0.73f,
                    lado * 0.64f - 0.15f, lado * 0.64f + 0.15f);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 2.26f, -2.26f, 0.685f, 0.65f);
    }

private:
    void pintura() { cor(0.64f, 0.05f, 0.13f); } // vinho
    void cromo() { cor(0.78f, 0.81f, 0.84f); }
    void preto() { cor(0.07f, 0.06f, 0.06f); }

    static float topoFrente(float x) {
        static const float t[][2] = {{0.85f, 0.84f}, {1.40f, 0.87f}, {1.85f, 0.83f}, {2.20f, 0.79f}};
        return interpola(x, t, 4);
    }
    static float topoTras(float x) {
        static const float t[][2] = {{-2.20f, 0.77f}, {-1.70f, 0.86f}, {-1.35f, 0.88f}, {-0.85f, 0.82f}};
        return interpola(x, t, 4);
    }
    static float recorte(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? 0.35f + sqrtf(ARCO * ARCO - d * d) : BASE;
    }

    void carroceriaSRM() {
        preto();
        caixa(-2.12f, 2.12f, 0.16f, BASE, -0.53f, 0.53f);
        caixa(-0.85f, 0.85f, BASE, 0.29f, -0.90f, 0.90f);
        pintura();
        perfilExtrudado(0.85f, 2.20f, 1.06f, topoFrente, [](float) { return BASE; });
        perfilExtrudado(-2.20f, -0.85f, 1.06f, topoTras, [](float) { return BASE; });
        preto();
        caixa(2.20f, 2.24f, 0.42f, 0.67f, -0.42f, 0.42f); // grade
        cromo();
        for (int i = -3; i <= 3; ++i)
            caixa(2.24f, 2.26f, 0.44f, 0.65f, i * 0.11f - 0.013f, i * 0.11f + 0.013f);
        caixa(2.14f, 2.29f, 0.29f, 0.39f, -0.86f, 0.86f);
        caixa(-2.29f, -2.14f, 0.29f, 0.39f, -0.86f, 0.86f);
        caixa(-2.31f, -2.16f, 0.20f, 0.27f, -0.60f, -0.48f); // escapamento
    }

    void paralamasSRM() {
        pintura();
        perfilExtrudado(0.85f, 2.20f, 0.37f, topoFrente,
                        [](float x) { return recorte(x, EIXO_FRENTE); }, 64);
        perfilExtrudado(-2.20f, -0.85f, 0.37f, topoTras,
                        [](float x) { return recorte(x, EIXO_TRAS); }, 64);
    }
    void paralamasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.715f);
        paralamasSRM();
        glPopMatrix();
    }

    void paraBrisaSRM() {
        cromo();
        for (int lado = -1; lado <= 1; lado += 2)
            barra(0.82f, 0.83f, 0.38f, 1.36f, lado * 0.79f, 0.05f);
        caixa(0.36f, 0.41f, 1.34f, 1.39f, -0.81f, 0.81f);
        caixa(0.79f, 0.85f, 0.81f, 0.86f, -0.81f, 0.81f);
        float a[] = {0.80f, 0.86f, 0.76f}, b[] = {0.80f, 0.86f, -0.76f};
        float c[] = {0.39f, 1.34f, -0.76f}, d[] = {0.39f, 1.34f, 0.76f};
        vidro(a, b, c, d);
        preto();
        caixa(0.41f, 0.47f, 1.20f, 1.28f, -0.14f, 0.14f); // espelho central
    }

    void interiorSRM() {
        cor(0.68f, 0.43f, 0.23f); // couro caramelo, visível com a cabine aberta
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.43f;
            caixa(-0.56f, 0.03f, 0.26f, 0.43f, z - 0.25f, z + 0.25f);
            caixa(-0.65f, -0.53f, 0.30f, 0.95f, z - 0.25f, z + 0.25f);
            caixa(-0.64f, -0.52f, 0.95f, 1.08f, z - 0.15f, z + 0.15f);
        }
        preto();
        caixa(0.47f, 0.83f, 0.65f, 0.82f, -0.78f, 0.78f);
        caixa(-0.58f, 0.46f, 0.24f, 0.43f, -0.12f, 0.12f);
        cromo();
        caixa(0.04f, 0.08f, 0.43f, 0.61f, -0.025f, 0.025f); // câmbio
        preto();
        caixa(0.02f, 0.10f, 0.59f, 0.65f, -0.045f, 0.045f);
        // Capota dobrada atrás dos bancos: três dobras separadas.
        for (int i = 0; i < 3; ++i)
            caixa(-1.18f + i * 0.10f, -1.10f + i * 0.10f, 0.87f, 0.95f, -0.68f, 0.68f);
    }

    void volanteSRM() {
        if (passadaDoVidro) return;
        cor(0.42f, 0.22f, 0.10f);
        glutSolidTorus(0.022, 0.14, 8, 24);
        cromo();
        caixa(-0.13f, 0.13f, -0.014f, 0.014f, -0.01f, 0.01f);
        caixa(-0.014f, 0.014f, -0.13f, 0, -0.01f, 0.01f);
    }
    void volanteSRU(float direcao) {
        glPushMatrix();
        glTranslatef(0.30f, 0.85f, -0.43f);
        glRotatef(75, 0, 1, 0);
        glRotatef(-direcao * 3, 0, 0, 1);
        volanteSRM();
        glPopMatrix();
    }

    void portaSRM(int lado) {
        pintura();
        caixa(-PORTA_COMP, 0, 0.29f, CINTURA, -0.03f, 0.03f);
        cor(0.68f, 0.43f, 0.23f);
        caixa(-1.65f, -0.06f, 0.40f, 0.75f, -lado * 0.045f, -lado * 0.032f);
        cromo();
        caixa(-PORTA_COMP, 0, 0.79f, 0.82f, -0.035f, 0.035f); // friso superior
        caixa(-1.45f, -1.24f, 0.67f, 0.71f, lado * 0.03f, lado * 0.055f);
        caixa(-0.24f, -0.18f, 0.81f, 0.93f, 0, lado * 0.13f);
        caixa(-0.29f, -0.16f, 0.91f, 1.01f, lado * 0.10f, lado * 0.23f);
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
