#pragma once
// =====================================================================================
//  CARRO CAMINHÃO: cabine alta na frente (cara reta), baú de carga atrás e dois eixos
//  traseiros. Segue a interface ModeloCarro. Cada componente tem xxxSRM() (modelagem
//  na origem dele) e xxxSRU() (leva até o lugar no caminhão).
//
//  Chassi bem alto (longarinas): cabine e baú sentam em cima dele, acima das rodas.
//    x: cabine [1.3, 3.5]   baú [-3.5, 1.2]
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroCaminhao : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP         = 7.0f;    // x de -3.5 a 3.5
    static constexpr float LARG         = 2.3f;    // z de -1.15 a 1.15
    static constexpr float VAO_LIVRE    = 0.50f;
    static constexpr float CHASSI_ALT   = 0.60f;
    static constexpr float CHASSI_LARG  = 1.2f;
    static constexpr float CHASSI_TOPO  = VAO_LIVRE + CHASSI_ALT;   // 1.1: acima do topo das rodas
    static constexpr float CABINE_COMP  = 2.2f;
    static constexpr float CABINE_ALT   = 2.0f;
    static constexpr float BAU_COMP     = 4.7f;
    static constexpr float BAU_ALT      = 2.4f;
    static constexpr float RODA_ESCALA  = 1.5f;    // raio 0.525
    static constexpr float EIXO_FRENTE  = 2.3f;
    static constexpr float EIXO_TRAS1   = -1.5f, EIXO_TRAS2 = -2.6f;   // eixo duplo

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        paraChoqueSRU();
        cabineSRU();
        bauSRU();
        for (int lado = -1; lado <= 1; lado += 2) {
            float z = lado * 1.0f;
            rodaSRU(EIXO_FRENTE, z, e.anguloDirecao, e.distancia, RODA_ESCALA);
            rodaSRU(EIXO_TRAS1,  z, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
            rodaSRU(EIXO_TRAS2,  z, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
            portaSRU(lado, e.anguloPortas);
            farolSRU(lado, e.luzesLigadas);
            lanternaSRU(lado, e.luzesLigadas);
        }
    }

    // o eixo de trás "efetivo" de um eixo duplo fica no meio dos dois
    float entreEixos() const override { return EIXO_FRENTE - (EIXO_TRAS1 + EIXO_TRAS2) / 2; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, COMP / 2 + 0.05f, -COMP / 2 - 0.05f, CHASSI_TOPO + 0.30f, 0.9f);
    }

private:
    void corDaPintura() { cor(0.15f, 0.30f, 0.70f); }        // azul
    void corPreta()     { cor(0.10f, 0.10f, 0.11f); }

    // ---------------------------------------------------------------- CHASSI (longarinas)
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

    // ---------------------------------------------------------------- PARA-CHOQUE
    // SRM: origem na base da face de trás.
    void paraChoqueSRM() {
        corPreta();
        caixa(0, 0.15f, 0, 0.40f, -LARG / 2, LARG / 2);
    }
    // SRU: na ponta da frente, na altura do chassi.
    void paraChoqueSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - 0.05f, VAO_LIVRE + 0.10f, 0);
        paraChoqueSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE
    // SRM: origem na base da parede de trás, x de 0 a 2.2 (frente reta em x = 2.2).
    void cabineSRM() {
        corDaPintura();
        caixa(0, 0.1f, 0, CABINE_ALT, -LARG / 2, LARG / 2);                    // parede de trás
        caixa(2.1f, 2.2f, 0, 1.0f, -LARG / 2, LARG / 2);                       // frente, embaixo do para-brisa
        caixa(2.1f, 2.2f, 1.0f, 1.9f,  1.06f,  LARG / 2);                      // colunas A
        caixa(2.1f, 2.2f, 1.0f, 1.9f, -LARG / 2, -1.06f);
        caixa(0, 2.2f, 1.9f, CABINE_ALT, -LARG / 2, LARG / 2);                 // teto
        corPreta();
        caixa(2.2f, 2.23f, 0.2f, 0.8f, -0.65f, 0.65f);                         // grade

        cor(0.25f, 0.25f, 0.27f);
        caixa(0.1f, 2.1f, -0.05f, 0.05f, -1.1f, 1.1f);                         // assoalho
        corPreta();
        caixa(1.8f, 2.1f, 0.7f, 1.0f, -1.1f, 1.1f);                            // painel
        cor(0.45f, 0.30f, 0.20f);
        caixa(0.1f, 0.2f, 0.05f, 1.0f, -1.0f, 1.0f);                           // banco: encosto
        caixa(0.2f, 0.7f, 0.05f, 0.45f, -1.0f, 1.0f);                          //        assento

        float pb1[3] = {2.19f, 1.0f,  1.06f};                                  // para-brisa em pé
        float pb2[3] = {2.19f, 1.0f, -1.06f};
        float pb3[3] = {2.19f, 1.9f, -1.06f};
        float pb4[3] = {2.19f, 1.9f,  1.06f};
        vidro(pb1, pb2, pb3, pb4);
    }
    // SRU: em cima do chassi, com a frente na ponta do caminhão.
    void cabineSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CABINE_COMP, CHASSI_TOPO, 0);
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- BAÚ
    // SRM: origem no centro da base.
    void bauSRM() {
        cor(0.90f, 0.90f, 0.88f);                                              // baú branco
        caixa(-BAU_COMP / 2, BAU_COMP / 2, 0, BAU_ALT, -LARG / 2, LARG / 2);
        corDaPintura();
        caixa(-BAU_COMP / 2 - 0.01f, BAU_COMP / 2 + 0.01f, 0.3f, 0.6f, -LARG / 2 - 0.01f, LARG / 2 + 0.01f);   // faixa
        cor(0.3f, 0.3f, 0.32f);
        caixa(-BAU_COMP / 2 - 0.02f, -BAU_COMP / 2, 0.05f, BAU_ALT - 0.05f, -0.01f, 0.01f);                   // fresta da porta
    }
    // SRU: em cima do chassi, encostado na ponta de trás (sobra um vão até a cabine).
    void bauSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 + BAU_COMP / 2, CHASSI_TOPO, 0);
        bauSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA (+ maçaneta, retrovisor grande, janela)
    // SRM: origem na DOBRADIÇA, porta de x = 0 até -2.0.
    void portaSRM(int lado) {
        corDaPintura();
        caixa(-2.0f, 0, 0, 1.0f, -0.02f, 0.001f);                              // chapa
        corPreta();
        caixa(-1.80f, -1.60f, 0.80f, 0.84f, lado * 0.03f, lado * 0.06f);       // maçaneta
        caixa(-0.05f, 0.05f, 1.40f, 1.45f, 0, lado * 0.35f);                   // haste do retrovisor
        caixa(-0.05f, 0.03f, 1.00f, 1.60f, lado * 0.32f, lado * 0.42f);        // espelho comprido

        float j1[3] = {-0.02f, 1.00f, -lado * 0.01f};
        float j2[3] = {-0.02f, 1.88f, -lado * 0.01f};
        float j3[3] = {-1.98f, 1.88f, -lado * 0.01f};
        float j4[3] = {-1.98f, 1.00f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    // SRU: dobradiça logo atrás das colunas A.
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(COMP / 2 - 0.1f, CHASSI_TOPO, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL / LANTERNA
    void farolSRM(bool aceso)    { lampada(aceso, true,  -0.03f, 0.04f, 0, 0.20f, -0.15f, 0.15f); }
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.03f, 0.03f, 0, 0.30f, -0.08f, 0.08f); }
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(COMP / 2, CHASSI_TOPO + 0.20f, lado * 0.90f);            // na cara da cabine
        farolSRM(aceso);
        glPopMatrix();
    }
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-COMP / 2, CHASSI_TOPO + 0.20f, lado * 1.00f);           // atrás do baú
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
