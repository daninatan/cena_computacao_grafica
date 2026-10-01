#pragma once
// HATCH compacto de duas portas: traseira curta, teto contrastante e vidro inclinado.
// SRM: origem no chão, frente em +x. Peças fixas usam x/y do carro;
// portas têm origem na dobradiça e paralamas são deslocados lateralmente no SRU.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroHatch : public ModeloCarro {
public:
    static constexpr float COMP = 3.6f, LARG = 1.7f;
    static constexpr float EIXO_FRENTE = 1.12f, EIXO_TRAS = -1.12f;
    static constexpr float RODA_ESCALA = 0.9f, Z_RODA = 0.82f;
    static constexpr float BASE = 0.23f, CINTURA = 0.88f;
    static constexpr float DOBRADICA = 0.70f, PORTA_COMP = 1.36f;
    static constexpr float ARCO = 0.40f;

    void desenhar(const EstadoCarro& e) override {
        carroceriaSRM();
        cabineSRM();
        interiorSRM();
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamasSRU(lado);
            portaSRU(lado, e.anguloPortas);
            rodaSRU(EIXO_FRENTE, lado * Z_RODA, e.anguloDirecao, e.distancia, RODA_ESCALA);
            rodaSRU(EIXO_TRAS, lado * Z_RODA, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
            lampada(e.luzesLigadas, true, 1.77f, 1.83f, 0.64f, 0.79f,
                    lado * 0.59f - 0.15f, lado * 0.59f + 0.15f);
            lampada(e.luzesLigadas, false, -1.83f, -1.77f, 0.57f, 0.84f,
                    lado * 0.71f - 0.07f, lado * 0.71f + 0.07f);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 1.86f, -1.86f, 0.715f, 0.59f);
    }

private:
    void pintura() { cor(0.02f, 0.65f, 0.60f); } // turquesa
    void preto() { cor(0.07f, 0.08f, 0.09f); }

    static float recorte(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? 0.35f * RODA_ESCALA + sqrtf(ARCO * ARCO - d * d) : BASE;
    }
    static float topoFrente(float x) {
        static const float t[][2] = {{0.70f, CINTURA}, {1.35f, 0.86f}, {1.80f, 0.80f}};
        return interpola(x, t, 3);
    }

    void carroceriaSRM() {
        preto();
        caixa(-1.73f, 1.73f, 0.17f, BASE, -0.50f, 0.50f); // assoalho estreito junto às rodas
        caixa(-0.68f, 0.70f, BASE, 0.31f, -0.85f, 0.85f); // soleiras
        pintura();
        perfilExtrudado(0.70f, 1.80f, 1.02f, topoFrente, [](float) { return BASE; });
        caixa(-1.80f, -0.66f, BASE, CINTURA, -0.51f, 0.51f);
        preto();
        caixa(1.78f, 1.87f, 0.27f, 0.44f, -0.77f, 0.77f);
        caixa(-1.87f, -1.78f, 0.27f, 0.44f, -0.77f, 0.77f);
        caixa(1.80f, 1.84f, 0.48f, 0.63f, -0.40f, 0.40f); // grade
        cor(0.85f, 0.87f, 0.89f);
        caixa(-1.845f, -1.81f, 0.51f, 0.65f, -0.21f, 0.21f); // placa traseira
    }

    void paralamasSRM() {
        pintura();
        perfilExtrudado(0.70f, 1.80f, 0.34f, topoFrente,
                        [](float x) { return recorte(x, EIXO_FRENTE); }, 64);
        perfilExtrudado(-1.80f, -0.66f, 0.34f, [](float) { return CINTURA; },
                        [](float x) { return recorte(x, EIXO_TRAS); }, 64);
    }
    void paralamasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.68f);
        paralamasSRM();
        glPopMatrix();
    }

    void cabineSRM() {
        preto();
        caixa(-1.18f, 0.09f, 1.48f, 1.55f, -0.79f, 0.79f); // teto preto
        caixa(-1.32f, -1.13f, 1.47f, 1.52f, -0.82f, 0.82f); // spoiler
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.79f;
            barra(0.70f, CINTURA, 0.06f, 1.49f, z, 0.065f);
            caixa(-0.70f, -0.63f, CINTURA, 1.49f, z - 0.03f, z + 0.03f);
            pintura();
            barra(-1.73f, CINTURA, -1.16f, 1.49f, z, 0.10f);
            float a[] = {-1.65f, 0.90f, z}, b[] = {-0.72f, 0.90f, z};
            float c[] = {-0.72f, 1.46f, z}, d[] = {-1.13f, 1.46f, z};
            vidro(a, b, c, d);
            preto();
        }
        float a[] = {0.69f, 0.90f, 0.76f}, b[] = {0.69f, 0.90f, -0.76f};
        float c[] = {0.06f, 1.48f, -0.76f}, d[] = {0.06f, 1.48f, 0.76f};
        vidro(a, b, c, d);
        float r1[] = {-1.73f, 0.90f, -0.74f}, r2[] = {-1.73f, 0.90f, 0.74f};
        float r3[] = {-1.18f, 1.48f, 0.74f}, r4[] = {-1.18f, 1.48f, -0.74f};
        vidro(r1, r2, r3, r4);
    }

    void interiorSRM() {
        preto();
        caixa(0.40f, 0.70f, 0.67f, 0.86f, -0.74f, 0.74f);
        cor(0.27f, 0.29f, 0.31f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.39f;
            caixa(-0.36f, 0.16f, 0.28f, 0.45f, z - 0.26f, z + 0.26f);
            caixa(-0.44f, -0.32f, 0.32f, 1.04f, z - 0.26f, z + 0.26f);
        }
        caixa(-1.20f, -0.75f, 0.31f, 0.48f, -0.69f, 0.69f);
        caixa(-1.29f, -1.18f, 0.40f, 1.01f, -0.69f, 0.69f);
    }

    // Porta completa (chapa, vidro, moldura e retrovisor) gira na mesma dobradiça.
    void portaSRM(int lado) {
        pintura();
        caixa(-PORTA_COMP, 0, 0.31f, CINTURA, -0.025f, 0.025f);
        caixa(-PORTA_COMP, 0, 0.85f, CINTURA, -lado * 0.06f, 0);
        preto();
        caixa(-1.19f, -1.00f, 0.77f, 0.81f, lado * 0.025f, lado * 0.05f);
        caixa(-0.20f, -0.10f, 0.88f, 0.94f, 0, lado * 0.19f);
        caixa(-0.25f, -0.09f, 0.92f, 1.02f, lado * 0.13f, lado * 0.25f);
        float z = -lado * 0.06f;
        barra(-0.04f, 0.90f, -0.66f, 1.46f, z, 0.025f);
        barra(-0.66f, 1.46f, -1.32f, 1.46f, z, 0.025f);
        float a[] = {-0.05f, 0.90f, z}, b[] = {-0.66f, 1.46f, z};
        float c[] = {-1.32f, 1.46f, z}, d[] = {-1.32f, 0.90f, z};
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
