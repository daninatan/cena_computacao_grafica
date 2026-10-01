#pragma once
// JIPE aberto: entre-eixos curto, pneus grandes, portas baixas, gaiola e estepe.
// SRM: centro no chão, frente em +x. Rodas tocam o chão pelo raio de rodaSRU;
// o estepe usa rodaSRM e permanece fixo, independentemente da distância percorrida.
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroJipe : public ModeloCarro {
public:
    static constexpr float COMP = 3.8f, LARG = 1.6f;
    static constexpr float EIXO_FRENTE = 1.20f, EIXO_TRAS = -1.20f;
    static constexpr float RODA_ESCALA = 1.4f, Z_RODA = 1.06f;
    static constexpr float BASE = 0.48f, CINTURA = 1.13f;
    static constexpr float DOBRADICA = 0.60f, PORTA_COMP = 1.20f, ARCO = 0.56f;

    void desenhar(const EstadoCarro& e) override {
        carroceriaSRM();
        cabineSRM();
        interiorSRM();
        estepeSRU();
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamasSRU(lado);
            portaSRU(lado, e.anguloPortas);
            rodaSRU(EIXO_FRENTE, lado * Z_RODA, e.anguloDirecao, e.distancia, RODA_ESCALA);
            rodaSRU(EIXO_TRAS, lado * Z_RODA, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
            lampada(e.luzesLigadas, true, 1.88f, 1.95f, 0.87f, 1.07f,
                    lado * 0.55f - 0.12f, lado * 0.55f + 0.12f);
            lampada(e.luzesLigadas, false, -1.95f, -1.89f, 0.81f, 0.99f,
                    lado * 0.64f - 0.08f, lado * 0.64f + 0.08f);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }
    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 1.98f, -2.22f, 0.97f, 0.55f);
    }

private:
    void pintura() { cor(0.38f, 0.43f, 0.20f); } // verde oliva
    void preto() { cor(0.09f, 0.10f, 0.08f); }

    static float recorte(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? 0.35f * RODA_ESCALA + sqrtf(ARCO * ARCO - d * d) : BASE;
    }

    void carroceriaSRM() {
        preto();
        caixa(-1.92f, 1.94f, 0.35f, 0.48f, -0.46f, 0.46f); // chassi
        caixa(-0.60f, 0.60f, BASE, 0.55f, -0.80f, 0.80f);
        pintura();
        caixa(0.60f, 1.90f, BASE, CINTURA, -0.64f, 0.64f); // capô estreito
        caixa(-1.90f, -0.60f, BASE, 0.59f, -0.64f, 0.64f); // piso traseiro
        caixa(-1.90f, -1.81f, BASE, CINTURA, -0.80f, 0.80f); // tampa
        preto();
        for (int i = -3; i <= 3; ++i)
            caixa(1.90f, 1.925f, 0.72f, 1.08f, i * 0.115f - 0.03f, i * 0.115f + 0.03f);
        caixa(1.84f, 2.08f, 0.48f, 0.65f, -0.99f, 0.99f);
        caixa(-2.08f, -1.84f, 0.48f, 0.65f, -0.99f, 0.99f);
        caixa(1.98f, 2.11f, 0.64f, 0.81f, -0.29f, 0.29f); // guincho
        cor(0.55f, 0.57f, 0.52f);
        caixa(2.11f, 2.14f, 0.68f, 0.75f, -0.18f, 0.18f);
    }

    void paralamasSRM() {
        pintura();
        perfilExtrudado(0.60f, 1.90f, 0.50f, [](float) { return CINTURA; },
                        [](float x) { return recorte(x, EIXO_FRENTE); }, 64);
        perfilExtrudado(-1.90f, -0.60f, 0.50f, [](float) { return CINTURA; },
                        [](float x) { return recorte(x, EIXO_TRAS); }, 64);
        preto();
        caixa(0.65f, 1.87f, 1.13f, 1.18f, -0.25f, 0.25f); // abas sobre os pneus
        caixa(-1.87f, -0.65f, 1.13f, 1.18f, -0.25f, 0.25f);
    }
    void paralamasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * 0.89f);
        paralamasSRM();
        glPopMatrix();
        preto();
        caixa(-0.60f, 0.60f, 0.43f, 0.51f, lado * 0.78f, lado * 1.01f); // estribo
    }

    void cabineSRM() {
        pintura();
        for (int lado = -1; lado <= 1; lado += 2)
            barra(0.59f, CINTURA, 0.39f, 1.91f, lado * 0.76f, 0.07f);
        caixa(0.35f, 0.43f, 1.87f, 1.94f, -0.80f, 0.80f);
        caixa(0.55f, 0.63f, 1.10f, 1.17f, -0.80f, 0.80f);
        float a[] = {0.58f, 1.17f, 0.72f}, b[] = {0.58f, 1.17f, -0.72f};
        float c[] = {0.40f, 1.87f, -0.72f}, d[] = {0.40f, 1.87f, 0.72f};
        vidro(a, b, c, d);
        preto();
        // Gaiola aberta: arco principal atrás dos bancos e dois apoios traseiros.
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.72f;
            barra(-0.65f, 0.59f, -0.65f, 1.97f, z, 0.085f);
            barra(-1.68f, 1.15f, -0.65f, 1.97f, z, 0.085f);
            barra(-0.65f, 1.97f, 0.37f, 1.91f, z, 0.07f);
        }
        caixa(-0.70f, -0.60f, 1.92f, 2.02f, -0.77f, 0.77f);
    }

    void interiorSRM() {
        preto();
        caixa(0.33f, 0.61f, 0.91f, 1.11f, -0.73f, 0.73f);
        cor(0.30f, 0.27f, 0.19f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 0.38f;
            caixa(-0.39f, 0.11f, 0.56f, 0.75f, z - 0.25f, z + 0.25f);
            caixa(-0.49f, -0.37f, 0.63f, 1.36f, z - 0.25f, z + 0.25f);
        }
        caixa(-1.56f, -1.03f, 0.61f, 0.78f, -0.62f, 0.62f);
        caixa(-1.67f, -1.55f, 0.66f, 1.31f, -0.62f, 0.62f);
    }

    void estepeSRU() {
        preto();
        caixa(-2.08f, -1.90f, 0.86f, 1.24f, -0.22f, 0.22f);
        glPushMatrix();
        glTranslatef(-2.09f, 1.14f, 0);
        glRotatef(-90, 0, 1, 0); // face externa da roda aponta para -x
        glScalef(RODA_ESCALA, RODA_ESCALA, RODA_ESCALA);
        rodaSRM();
        glPopMatrix();
    }

    void portaSRM(int lado) {
        pintura();
        // Meia-porta rebaixada no meio, deixando a cabine e os bancos expostos.
        static const float t[][2] = {{-1.20f, 1.10f}, {-1.00f, 0.91f}, {-0.20f, 0.91f}, {0, 1.10f}};
        perfilExtrudado(-PORTA_COMP, 0, 0.06f,
                        [](float x) { return interpola(x, t, 4); },
                        [](float) { return 0.55f; }, 24);
        preto();
        caixa(-1.03f, -0.84f, 0.83f, 0.87f, lado * 0.03f, lado * 0.055f);
        caixa(-0.14f, -0.08f, 1.06f, 1.31f, 0, lado * 0.16f);
        caixa(-0.18f, -0.07f, 1.25f, 1.43f, lado * 0.12f, lado * 0.27f);
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
