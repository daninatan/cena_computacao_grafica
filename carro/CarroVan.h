#pragma once
// =====================================================================================
//  CARRO VAN: cabine curta com capô pequeno na frente e um baú alto de carga atrás.
//  Segue a interface ModeloCarro. Cada componente tem xxxSRM() (modelagem na origem
//  dele) e xxxSRU() (leva até o lugar no carro, encostado nos vizinhos).
//
//    x: capô [1.9, 2.5]   cabine [0.7, 1.9]   baú [-2.5, 0.7]
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroVan : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP         = 5.0f;    // x de -2.5 a 2.5
    static constexpr float LARG         = 2.0f;    // z de -1.0 a 1.0
    static constexpr float VAO_LIVRE    = 0.35f;
    static constexpr float CHASSI_ALT   = 0.20f;
    static constexpr float CHASSI_LARG  = 1.7f;
    static constexpr float CHASSI_TOPO  = VAO_LIVRE + CHASSI_ALT;   // 0.55: onde a carroceria senta
    static constexpr float CAPO_COMP    = 0.6f;
    static constexpr float CAPO_ALT     = 0.55f;
    static constexpr float CABINE_COMP  = 1.2f;
    static constexpr float ALTURA       = 1.75f;   // do topo do chassi até o teto
    static constexpr float BAU_COMP     = COMP - CAPO_COMP - CABINE_COMP;   // 3.2
    static constexpr float RODA_ESCALA  = 1.1f;
    static constexpr float EIXO_FRENTE  = 2.0f, EIXO_TRAS = -1.7f;

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        capoSRU();
        cabineSRU();
        bauSRU();
        rodaSRU(EIXO_FRENTE, -0.92f, e.anguloDirecao, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_FRENTE,  0.92f, e.anguloDirecao, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_TRAS,   -0.92f, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_TRAS,    0.92f, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
        portaSRU(-1, e.anguloPortas);
        portaSRU(+1, e.anguloPortas);
        farolSRU(-1, e.luzesLigadas);
        farolSRU(+1, e.luzesLigadas);
        lanternaSRU(-1, e.luzesLigadas);
        lanternaSRU(+1, e.luzesLigadas);
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, COMP / 2 + 0.05f, -COMP / 2 - 0.05f, CHASSI_TOPO + 0.37f, 0.75f);
    }

private:
    void corDaPintura() { cor(0.92f, 0.92f, 0.95f); }        // branco

    // ---------------------------------------------------------------- CHASSI
    // SRM: origem no centro da base.
    void chassiSRM() {
        cor(0.15f, 0.15f, 0.17f);
        caixa(-COMP / 2, COMP / 2, 0, CHASSI_ALT, -CHASSI_LARG / 2, CHASSI_LARG / 2);
    }
    void chassiSRU() {
        glPushMatrix();
        glTranslatef(0, VAO_LIVRE, 0);
        chassiSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CAPÔ (+ grade e para-choque)
    // SRM: origem na base da borda de trás (onde começa o para-brisa).
    void capoSRM() {
        corDaPintura();
        caixa(0, CAPO_COMP, 0, CAPO_ALT, -LARG / 2, LARG / 2);
        cor(0.12f, 0.12f, 0.13f);
        caixa(CAPO_COMP, CAPO_COMP + 0.02f, 0.15f, 0.45f, -0.5f, 0.5f);          // grade
        caixa(CAPO_COMP - 0.05f, CAPO_COMP + 0.08f, -0.15f, 0.10f, -LARG / 2, LARG / 2);   // para-choque
    }
    // SRU: em cima do chassi, terminando na ponta da frente.
    void capoSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, 0);
        capoSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (colunas A, teto, para-brisa, interior)
    // SRM: origem na base da parede de trás da cabine (onde encosta no baú), x de 0 a 1.2.
    void cabineSRM() {
        corDaPintura();
        barra(1.2f, 0.55f, 0.76f, 1.70f,  0.97f, 0.08f);       // colunas A
        barra(1.2f, 0.55f, 0.76f, 1.70f, -0.97f, 0.08f);
        caixa(0, 0.80f, 1.65f, ALTURA, -LARG / 2, LARG / 2);   // teto

        cor(0.25f, 0.25f, 0.27f);
        caixa(0, CABINE_COMP, -0.05f, 0.05f, -0.95f, 0.95f);   // assoalho
        cor(0.12f, 0.12f, 0.13f);
        caixa(0.95f, CABINE_COMP, 0.35f, 0.55f, -0.95f, 0.95f);// painel
        cor(0.45f, 0.30f, 0.20f);
        caixa(0.02f, 0.12f, 0, 0.80f, -0.9f, 0.9f);            // banco inteiriço: encosto
        caixa(0.12f, 0.60f, 0, 0.20f, -0.9f, 0.9f);            //                  assento

        float pb1[3] = {1.18f, 0.57f,  0.95f};                 // para-brisa: do capô até o teto
        float pb2[3] = {1.18f, 0.57f, -0.95f};
        float pb3[3] = {0.78f, 1.65f, -0.95f};
        float pb4[3] = {0.78f, 1.65f,  0.95f};
        vidro(pb1, pb2, pb3, pb4);
    }
    // SRU: entre o baú e o capô.
    void cabineSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP - CABINE_COMP, CHASSI_TOPO, 0);
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- BAÚ (compartimento de carga)
    // SRM: origem no centro da base.
    void bauSRM() {
        corDaPintura();
        caixa(-BAU_COMP / 2, BAU_COMP / 2, 0, ALTURA, -LARG / 2, LARG / 2);
        cor(0.3f, 0.3f, 0.32f);
        caixa(-BAU_COMP / 2 - 0.02f, -BAU_COMP / 2, 0.05f, ALTURA - 0.05f, -0.01f, 0.01f);   // fresta da porta traseira
    }
    // SRU: encostado na ponta de trás do chassi.
    void bauSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 + BAU_COMP / 2, CHASSI_TOPO, 0);
        bauSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA (+ maçaneta, retrovisor, janela)
    // SRM: origem na DOBRADIÇA; a porta vai de x = 0 até -1.2 (comprimento da cabine).
    // lado = -1 esquerda, +1 direita.
    void portaSRM(int lado) {
        corDaPintura();
        caixa(-CABINE_COMP, 0, 0, 0.75f, -0.02f, 0.001f);                      // chapa
        cor(0.10f, 0.10f, 0.10f);
        caixa(-1.05f, -0.90f, 0.55f, 0.59f, lado * 0.03f, lado * 0.06f);       // maçaneta
        caixa(-0.15f, -0.05f, 0.80f, 0.84f, 0, lado * 0.15f);                  // haste do retrovisor
        caixa(-0.13f, -0.06f, 0.70f, 0.95f, lado * 0.12f, lado * 0.30f);       // espelho

        // janela: a borda da frente acompanha a inclinação do para-brisa
        float j1[3] = {-0.08f, 0.75f, -lado * 0.01f};
        float j2[3] = {-0.42f, 1.65f, -lado * 0.01f};
        float j3[3] = {-CABINE_COMP, 1.65f, -lado * 0.01f};
        float j4[3] = {-CABINE_COMP, 0.75f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    // SRU: dobradiça onde o capô começa; gira na dobradiça e depois vai pro lugar.
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL / LANTERNA
    // SRM: origem na face do carro, embaixo, no meio da peça.
    void farolSRM(bool aceso)    { lampada(aceso, true,  -0.03f, 0.03f, 0, 0.15f, -0.15f, 0.15f); }
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.03f, 0.03f, 0, 0.30f, -0.08f, 0.08f); }
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(COMP / 2, CHASSI_TOPO + 0.30f, lado * 0.75f);            // na frente do capô
        farolSRM(aceso);
        glPopMatrix();
    }
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-COMP / 2, CHASSI_TOPO + 0.30f, lado * 0.90f);           // atrás do baú
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
