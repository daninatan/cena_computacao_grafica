#pragma once
// =====================================================================================
//  CARRO CAMINHONETE (picape de cabine dupla): capô, cabine com 4 portas e caçamba
//  aberta atrás. Segue a interface ModeloCarro. Cada componente tem xxxSRM()
//  (modelagem na origem dele) e xxxSRU() (leva até o lugar no carro).
//
//    x: capô [1.5, 2.6]   porta da frente [0.4, 1.5]   porta de trás [-0.6, 0.4]
//       parede da cabine [-0.7, -0.6]   caçamba [-2.6, -0.7]
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroCaminhonete : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP          = 5.2f;    // x de -2.6 a 2.6
    static constexpr float LARG          = 1.9f;
    static constexpr float VAO_LIVRE     = 0.45f;
    static constexpr float CHASSI_ALT    = 0.20f;
    static constexpr float CHASSI_LARG   = 1.6f;
    static constexpr float CHASSI_TOPO   = VAO_LIVRE + CHASSI_ALT;   // 0.65
    static constexpr float LINHA_JANELA  = 0.55f;   // altura do capô, das portas e da caçamba
    static constexpr float TETO          = 1.2f;
    static constexpr float CAPO_COMP     = 1.1f;
    static constexpr float CACAMBA_COMP  = 1.9f;
    static constexpr float PORTA_FRENTE  = 1.1f;    // comprimento das portas
    static constexpr float PORTA_TRAS    = 1.0f;
    static constexpr float RODA_ESCALA   = 1.25f;
    static constexpr float EIXO_FRENTE   = 1.8f, EIXO_TRAS = -1.5f;

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        capoSRU();
        cabineSRU();
        interiorSRU();
        cacambaSRU();
        for (int lado = -1; lado <= 1; lado += 2) {
            rodaSRU(EIXO_FRENTE, lado * 0.93f, e.anguloDirecao,     e.distancia, RODA_ESCALA);
            rodaSRU(EIXO_TRAS,   lado * 0.93f, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
            portaSRU(lado, true,  e.anguloPortas);
            portaSRU(lado, false, e.anguloPortas);
            farolSRU(lado, e.luzesLigadas);
            lanternaSRU(lado, e.luzesLigadas);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, COMP / 2 + 0.05f, -COMP / 2 - 0.05f, CHASSI_TOPO + 0.37f, 0.7f);
    }

private:
    void corDaPintura() { cor(0.62f, 0.64f, 0.68f); }        // prata
    void corPreta()     { cor(0.10f, 0.10f, 0.11f); }

    // ---------------------------------------------------------------- CHASSI
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

    // ---------------------------------------------------------------- CAPÔ (+ grade, para-choque)
    // SRM: origem na base da borda de trás.
    void capoSRM() {
        corDaPintura();
        caixa(0, CAPO_COMP, 0, LINHA_JANELA, -LARG / 2, LARG / 2);
        corPreta();
        caixa(CAPO_COMP, CAPO_COMP + 0.02f, 0.15f, 0.45f, -0.6f, 0.6f);                    // grade
        caixa(CAPO_COMP - 0.05f, CAPO_COMP + 0.08f, -0.15f, 0.12f, -LARG / 2, LARG / 2);   // para-choque
    }
    void capoSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, 0);
        capoSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (colunas, teto, parede de trás, vidros)
    // SRM: origem no centro do carro, topo do chassi.
    void cabineSRM() {
        corDaPintura();
        for (int lado = -1; lado <= 1; lado += 2) {
            float zf = lado * 0.95f, zd = lado * 0.80f;
            barra(1.52f, 0.55f, 0.95f, 1.22f, lado * 0.92f, 0.08f);             // coluna A
            caixa(0.36f, 0.44f, LINHA_JANELA, TETO, lado * 0.88f, zf);          // coluna B
            caixa(-0.70f, -0.60f, LINHA_JANELA, TETO, zd, zf);                  // coluna de trás
        }
        caixa(-0.70f, -0.60f, 0, LINHA_JANELA, -LARG / 2, LARG / 2);           // parede de trás (embaixo)
        caixa(-0.70f, 0.97f, TETO, TETO + 0.08f, -LARG / 2, LARG / 2);         // teto

        float pb1[3] = {1.50f, 0.57f,  0.90f};                                 // para-brisa
        float pb2[3] = {1.50f, 0.57f, -0.90f};
        float pb3[3] = {0.97f, 1.20f, -0.90f};
        float pb4[3] = {0.97f, 1.20f,  0.90f};
        vidro(pb1, pb2, pb3, pb4);

        float vt1[3] = {-0.65f, 0.57f, -0.80f};                                // vidro de trás (em pé)
        float vt2[3] = {-0.65f, 0.57f,  0.80f};
        float vt3[3] = {-0.65f, 1.18f,  0.80f};
        float vt4[3] = {-0.65f, 1.18f, -0.80f};
        vidro(vt1, vt2, vt3, vt4);
    }
    void cabineSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- INTERIOR
    void interiorSRM() {
        cor(0.25f, 0.25f, 0.27f);
        caixa(-0.6f, 1.5f, -0.05f, 0.05f, -0.9f, 0.9f);                        // assoalho
        corPreta();
        caixa(1.15f, 1.5f, 0.30f, 0.50f, -0.9f, 0.9f);                         // painel
        cor(0.20f, 0.20f, 0.22f);
        for (int lado = -1; lado <= 1; lado += 2) {
            float zc = lado * 0.45f;
            caixa(0.05f, 0.15f, 0.05f, 0.75f, zc - 0.25f, zc + 0.25f);         // banco da frente: encosto
            caixa(0.15f, 0.65f, 0.05f, 0.25f, zc - 0.25f, zc + 0.25f);         //                  assento
        }
        caixa(-0.60f, -0.50f, 0.05f, 0.70f, -0.8f, 0.8f);                      // banco de trás: encosto
        caixa(-0.50f, -0.05f, 0.05f, 0.25f, -0.8f, 0.8f);                      //                assento
    }
    void interiorSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        interiorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CAÇAMBA
    // SRM: origem no centro da base. Assoalho + 2 laterais + frente + tampa (aberta em cima).
    void cacambaSRM() {
        float c = CACAMBA_COMP / 2, z = LARG / 2, h = LINHA_JANELA;
        corPreta();
        caixa(-c, c, 0, 0.10f, -z, z);                                          // assoalho (revestido)
        corDaPintura();
        caixa(-c, c, 0, h,  z - 0.07f,  z);                                     // lateral esquerda
        caixa(-c, c, 0, h, -z, -z + 0.07f);                                     // lateral direita
        caixa(c - 0.08f, c, 0, h, -z, z);                                       // frente
        caixa(-c, -c + 0.08f, 0, h, -z, z);                                     // tampa traseira
        corPreta();
        caixa(-c - 0.08f, -c + 0.05f, -0.15f, 0.12f, -z, z);                   // para-choque traseiro
    }
    // SRU: na ponta de trás, encostada na cabine.
    void cacambaSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 + CACAMBA_COMP / 2, CHASSI_TOPO, 0);
        cacambaSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA
    // SRM: origem na DOBRADIÇA. A dianteira tem a janela inclinada (para-brisa) e o retrovisor.
    void portaSRM(int lado, bool dianteira) {
        float comp = dianteira ? PORTA_FRENTE : PORTA_TRAS;
        corDaPintura();
        caixa(-comp, 0, 0, LINHA_JANELA, -0.02f, 0.001f);                      // chapa
        corPreta();
        caixa(-comp + 0.15f, -comp + 0.30f, 0.45f, 0.49f, lado * 0.03f, lado * 0.06f);   // maçaneta
        if (dianteira) {
            caixa(-0.20f, -0.10f, 0.58f, 0.62f, 0, lado * 0.15f);              // haste do retrovisor
            caixa(-0.18f, -0.11f, 0.55f, 0.72f, lado * 0.12f, lado * 0.32f);   // espelho
        }
        float frenteBaixo = dianteira ? -0.02f : -0.04f;
        float frenteCima  = dianteira ? -0.51f : -0.04f;
        float j1[3] = {frenteBaixo,   0.55f, -lado * 0.01f};
        float j2[3] = {frenteCima,    1.18f, -lado * 0.01f};
        float j3[3] = {-comp + 0.04f, 1.18f, -lado * 0.01f};
        float j4[3] = {-comp + 0.04f, 0.55f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    void portaSRU(int lado, bool dianteira, float abertura) {
        float dobradica = COMP / 2 - CAPO_COMP - (dianteira ? 0 : PORTA_FRENTE);
        glPushMatrix();
        glTranslatef(dobradica, CHASSI_TOPO, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado, dianteira);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL / LANTERNA
    void farolSRM(bool aceso)    { lampada(aceso, true,  -0.03f, 0.03f, 0, 0.15f, -0.17f, 0.17f); }
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.03f, 0.02f, 0, 0.30f, -0.07f, 0.07f); }
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(COMP / 2, CHASSI_TOPO + 0.30f, lado * 0.70f);
        farolSRM(aceso);
        glPopMatrix();
    }
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-COMP / 2, CHASSI_TOPO + 0.20f, lado * 0.85f);           // em pé, nos cantos da tampa
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
