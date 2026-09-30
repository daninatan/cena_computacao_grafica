#pragma once
// PERUA clássica: quatro portas, área de carga envidraçada, madeira e bagageiro.
// SRM no chão, frente em +x. Portas completas giram na dobradiça;
// os painéis laterais de madeira acompanham o recorte das caixas de roda.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroPerua : public ModeloCarro {
public:
    static constexpr float COMP = 4.9f, LARG = 1.84f;
    static constexpr float EIXO_FRENTE = 1.55f, EIXO_TRAS = -1.50f;
    static constexpr float Z_RODA = 0.91f, BASE = 0.26f, CINTURA = 0.95f;
    static constexpr float ARCO = 0.44f;

    void desenhar(const EstadoCarro& e) override {
        carroceriaSRM();
        cabineSRM();
        interiorSRM();
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamasSRU(lado);
            portaSRU(lado, true, e.anguloPortas);
            portaSRU(lado, false, e.anguloPortas);
            rodaSRU(EIXO_FRENTE, lado * Z_RODA, e.anguloDirecao, e.distancia);
            rodaSRU(EIXO_TRAS, lado * Z_RODA, e.anguloDirecaoTras, e.distancia);
            lampada(e.luzesLigadas, true, 2.43f, 2.49f, 0.68f, 0.85f,
                    lado * 0.65f - 0.15f, lado * 0.65f + 0.15f);
            lampada(e.luzesLigadas, false, -2.49f, -2.43f, 0.60f, 0.88f,
                    lado * 0.79f - 0.07f, lado * 0.79f + 0.07f);
        }
    }
    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 2.52f, -2.52f, 0.765f, 0.65f);
    }

private:
    void pintura() { cor(0.08f, 0.30f, 0.38f); } // azul-petróleo
    void madeira() { cor(0.48f, 0.27f, 0.12f); }
    void creme() { cor(0.90f, 0.85f, 0.68f); }
    void preto() { cor(0.08f, 0.09f, 0.10f); }
    static float recorte(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? 0.35f + sqrtf(ARCO * ARCO - d * d) : BASE;
    }
    static float topoCapo(float x) {
        static const float t[][2] = {{1.0f, CINTURA}, {1.8f, 0.93f}, {2.45f, 0.88f}};
        return interpola(x, t, 3);
    }
    void carroceriaSRM() {
        preto();
        caixa(-2.38f, 2.38f, 0.19f, BASE, -0.55f, 0.55f);
        caixa(-0.95f, 1.0f, BASE, 0.33f, -0.92f, 0.92f);
        pintura();
        perfilExtrudado(1.0f, 2.45f, 1.10f, topoCapo, [](float) { return BASE; });
        caixa(-2.45f, -0.95f, BASE, 0.42f, -0.55f, 0.55f); // piso do porta-malas
        caixa(-2.45f, -2.36f, BASE, CINTURA, -0.92f, 0.92f);
        preto();
        caixa(2.45f, 2.48f, 0.56f, 0.82f, -0.43f, 0.43f);
        cor(0.76f, 0.79f, 0.80f);
        caixa(2.39f, 2.54f, 0.30f, 0.41f, -0.89f, 0.89f);
        caixa(-2.54f, -2.39f, 0.30f, 0.41f, -0.89f, 0.89f);
        for (int i = 0; i < 3; ++i)
            caixa(2.48f, 2.50f, 0.60f + i * 0.08f, 0.62f + i * 0.08f, -0.41f, 0.41f);
        creme();
        caixa(-2.48f, -2.45f, 0.53f, 0.67f, -0.22f, 0.22f);
    }
    void paralamasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.735f);
        pintura();
        perfilExtrudado(1.0f, 2.45f, 0.37f, topoCapo,
                        [](float x) { return recorte(x, EIXO_FRENTE); }, 64);
        perfilExtrudado(-2.45f, -0.95f, 0.37f, [](float) { return CINTURA; },
                        [](float x) { return recorte(x, EIXO_TRAS); }, 64);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.925f);
        madeira();
        perfilExtrudado(-2.36f, -0.99f, 0.012f, [](float) { return 0.86f; },
                        [](float x) { return fmaxf(0.52f, recorte(x, EIXO_TRAS) + 0.02f); }, 64);
        creme();
        caixa(-2.36f, -0.99f, 0.86f, 0.885f, -0.012f, 0.012f);
        glPopMatrix();
    }
    void cabineSRM() {
        creme();
        caixa(-2.12f, 0.43f, 1.55f, 1.63f, -0.86f, 0.86f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.85f;
            pintura();
            barra(1.0f, CINTURA, 0.41f, 1.56f, z, 0.07f);
            barra(-2.43f, CINTURA, -2.10f, 1.56f, z, 0.09f);
            caixa(-0.04f, 0.03f, CINTURA, 1.55f, z - 0.03f, z + 0.03f);
            caixa(-1.0f, -0.93f, CINTURA, 1.55f, z - 0.03f, z + 0.03f);
            float a[] = {-2.37f, 0.98f, z}, b[] = {-1.02f, 0.98f, z};
            float c[] = {-1.02f, 1.53f, z}, d[] = {-2.08f, 1.53f, z};
            vidro(a, b, c, d);
            preto();
            caixa(-1.93f, 0.20f, 1.69f, 1.75f, lado * 0.72f, lado * 0.78f); // trilhos
            caixa(-1.80f, -1.70f, 1.63f, 1.70f, lado * 0.69f, lado * 0.81f);
            caixa(-0.04f, 0.06f, 1.63f, 1.70f, lado * 0.69f, lado * 0.81f);
        }
        caixa(-1.80f, -1.70f, 1.68f, 1.72f, -0.90f, 0.90f); // travessas do bagageiro
        caixa(-0.04f, 0.06f, 1.68f, 1.72f, -0.90f, 0.90f);
        float a[] = {0.98f, 0.97f, 0.81f}, b[] = {0.98f, 0.97f, -0.81f};
        float c[] = {0.42f, 1.55f, -0.81f}, d[] = {0.42f, 1.55f, 0.81f};
        vidro(a, b, c, d);
        float r1[] = {-2.42f, 0.98f, -0.80f}, r2[] = {-2.42f, 0.98f, 0.80f};
        float r3[] = {-2.11f, 1.55f, 0.80f}, r4[] = {-2.11f, 1.55f, -0.80f};
        vidro(r1, r2, r3, r4);
    }
    void interiorSRM() {
        preto();
        caixa(0.69f, 1.0f, 0.71f, 0.94f, -0.81f, 0.81f);
        caixa(-2.32f, -1.10f, 0.42f, 0.46f, -0.79f, 0.79f);
        creme();
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.44f;
            caixa(0.07f, 0.56f, 0.32f, 0.49f, z - 0.27f, z + 0.27f);
            caixa(-0.04f, 0.08f, 0.36f, 1.08f, z - 0.27f, z + 0.27f);
        }
        caixa(-0.97f, -0.47f, 0.33f, 0.50f, -0.77f, 0.77f);
        caixa(-1.09f, -0.96f, 0.38f, 1.07f, -0.77f, 0.77f);
    }
    void portaSRM(int lado, bool dianteira) {
        float comp = dianteira ? 1.0f : 0.95f;
        pintura();
        caixa(-comp + 0.015f, -0.015f, 0.33f, CINTURA, -0.025f, 0.025f);
        caixa(-comp, 0, 0.92f, CINTURA, -lado * 0.07f, 0);
        madeira();
        caixa(-comp + 0.05f, -0.05f, 0.52f, 0.86f, lado * 0.026f, lado * 0.035f);
        creme();
        caixa(-comp + 0.05f, -0.05f, 0.86f, 0.885f, lado * 0.035f, lado * 0.043f);
        caixa(-comp + 0.05f, -0.05f, 0.50f, 0.525f, lado * 0.035f, lado * 0.043f);
        preto();
        caixa(-comp + 0.12f, -comp + 0.29f, 0.88f, 0.915f, lado * 0.025f, lado * 0.06f);
        if (dianteira) {
            caixa(-0.20f, -0.12f, 0.96f, 1.02f, 0, lado * 0.17f);
            caixa(-0.25f, -0.11f, 1.0f, 1.12f, lado * 0.12f, lado * 0.28f);
        }
        float z = -lado * 0.07f, topo = dianteira ? -0.59f : -0.04f;
        float a[] = {-0.04f, 0.98f, z}, b[] = {topo, 1.53f, z};
        float c[] = {-comp + 0.04f, 1.53f, z}, d[] = {-comp + 0.04f, 0.98f, z};
        vidro(a, b, c, d);
        barra(topo, 1.53f, -comp + 0.04f, 1.53f, z, 0.025f);
    }
    void portaSRU(int lado, bool dianteira, float abertura) {
        glPushMatrix();
        glTranslatef(dianteira ? 1.0f : 0.0f, 0, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado, dianteira);
        glPopMatrix();
    }
};

} // namespace carros
