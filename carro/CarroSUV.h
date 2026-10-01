#pragma once
// =====================================================================================
//  CARRO SUV: alto, quadrado, 4 portas, rodas grandes, bagageiro no teto e estepe
//  pendurado atrás. Segue a interface ModeloCarro. Cada componente tem xxxSRM()
//  (modelagem na origem dele) e xxxSRU() (leva até o lugar no carro).
//
//    x: capô [1.2, 2.3]   porta da frente [0, 1.2]   porta de trás [-1.2, 0]
//       porta-malas [-2.3, -1.2]
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroSUV : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP         = 4.6f;    // x de -2.3 a 2.3
    static constexpr float LARG         = 1.9f;    // z de -0.95 a 0.95
    static constexpr float VAO_LIVRE    = 0.45f;   // bem mais alto que o sedan
    static constexpr float CHASSI_ALT   = 0.20f;
    static constexpr float CHASSI_LARG  = 1.6f;
    static constexpr float CHASSI_TOPO  = VAO_LIVRE + CHASSI_ALT;   // 0.65
    static constexpr float CAPO_COMP    = 1.1f;
    static constexpr float LINHA_JANELA = 0.55f;   // altura da carroceria de baixo (portas, capô)
    static constexpr float PORTA_COMP   = 1.2f;
    static constexpr float PORTA_MALAS  = COMP - CAPO_COMP - 2 * PORTA_COMP;   // 1.1
    static constexpr float TETO         = 1.25f;   // do topo do chassi até o teto
    static constexpr float RODA_ESCALA  = 1.25f;
    static constexpr float EIXO_FRENTE  = 1.6f, EIXO_TRAS = -1.6f;

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        capoSRU();
        portaMalasSRU();
        cabineSRU();
        interiorSRU();
        estepeSRU();
        rodaSRU(EIXO_FRENTE, -0.93f, e.anguloDirecao, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_FRENTE,  0.93f, e.anguloDirecao, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_TRAS,   -0.93f, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
        rodaSRU(EIXO_TRAS,    0.93f, e.anguloDirecaoTras, e.distancia, RODA_ESCALA);
        for (int lado = -1; lado <= 1; lado += 2) {
            portaSRU(lado, true,  e.anguloPortas);            // dianteira
            portaSRU(lado, false, e.anguloPortas);            // traseira
            farolSRU(lado, e.luzesLigadas);
            lanternaSRU(lado, e.luzesLigadas);
        }
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, COMP / 2 + 0.05f, -COMP / 2 - 0.05f, CHASSI_TOPO + 0.37f, 0.7f);
    }

private:
    void corDaPintura() { cor(0.10f, 0.22f, 0.45f); }        // azul escuro
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
    // SRM: origem na base da borda de trás (pé do para-brisa).
    void capoSRM() {
        corDaPintura();
        caixa(0, CAPO_COMP, 0, LINHA_JANELA, -LARG / 2, LARG / 2);
        corPreta();
        caixa(CAPO_COMP, CAPO_COMP + 0.02f, 0.15f, 0.45f, -0.55f, 0.55f);                   // grade
        caixa(CAPO_COMP - 0.05f, CAPO_COMP + 0.08f, -0.15f, 0.12f, -LARG / 2, LARG / 2);    // para-choque
    }
    void capoSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, 0);
        capoSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA-MALAS (parte de baixo da traseira)
    // SRM: origem no centro da base.
    void portaMalasSRM() {
        corDaPintura();
        caixa(-PORTA_MALAS / 2, PORTA_MALAS / 2, 0, LINHA_JANELA, -LARG / 2, LARG / 2);
        corPreta();
        caixa(-PORTA_MALAS / 2 - 0.08f, -PORTA_MALAS / 2 + 0.05f, -0.15f, 0.12f, -LARG / 2, LARG / 2);   // para-choque
    }
    void portaMalasSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 + PORTA_MALAS / 2, CHASSI_TOPO, 0);
        portaMalasSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (colunas, teto, bagageiro, vidros fixos)
    // SRM: origem no centro do carro, no topo do chassi.
    void cabineSRM() {
        corDaPintura();
        for (int lado = -1; lado <= 1; lado += 2) {
            float zf = lado * 0.95f, zd = lado * 0.88f;                           // face de fora / de dentro
            barra(1.22f, 0.55f, 0.62f, 1.28f, lado * 0.92f, 0.08f);               // coluna A (inclinada)
            caixa(-0.04f, 0.04f, LINHA_JANELA, TETO, zd, zf);                     // coluna B (entre as portas)
            caixa(-1.26f, -1.20f, LINHA_JANELA, TETO, zd, zf);                    // coluna C
            barra(-2.28f, 0.55f, -2.20f, 1.28f, lado * 0.92f, 0.10f);             // coluna D (traseira, quase reta)
        }
        caixa(-2.28f, 0.64f, TETO, TETO + 0.08f, -LARG / 2, LARG / 2);           // teto
        corPreta();
        caixa(-2.0f, 0.3f, TETO + 0.08f, TETO + 0.13f,  0.70f,  0.76f);          // bagageiro
        caixa(-2.0f, 0.3f, TETO + 0.08f, TETO + 0.13f, -0.76f, -0.70f);

        float pb1[3] = {1.20f, 0.57f,  0.90f};                                   // para-brisa
        float pb2[3] = {1.20f, 0.57f, -0.90f};
        float pb3[3] = {0.64f, 1.25f, -0.90f};
        float pb4[3] = {0.64f, 1.25f,  0.90f};
        vidro(pb1, pb2, pb3, pb4);

        float vt1[3] = {-2.26f, 0.60f, -0.90f};                                  // vidro traseiro (quase em pé)
        float vt2[3] = {-2.26f, 0.60f,  0.90f};
        float vt3[3] = {-2.21f, 1.24f,  0.90f};
        float vt4[3] = {-2.21f, 1.24f, -0.90f};
        vidro(vt1, vt2, vt3, vt4);

        for (int lado = -1; lado <= 1; lado += 2) {                              // vidros laterais fixos (atrás da coluna C)
            float z = lado * 0.955f;
            float q1[3] = {-1.26f, 0.60f, z}, q2[3] = {-1.26f, 1.22f, z};
            float q3[3] = {-2.20f, 1.22f, z}, q4[3] = {-2.24f, 0.60f, z};
            vidro(q1, q2, q3, q4);
        }
    }
    void cabineSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- INTERIOR (assoalho, painel, bancos)
    // SRM: mesma origem da cabine.
    void interiorSRM() {
        cor(0.25f, 0.25f, 0.27f);
        caixa(-COMP / 2, COMP / 2 - CAPO_COMP, -0.05f, 0.05f, -0.9f, 0.9f);     // assoalho
        corPreta();
        caixa(0.85f, 1.2f, 0.30f, 0.50f, -0.9f, 0.9f);                          // painel
        cor(0.20f, 0.20f, 0.22f);                                               // bancos de tecido cinza
        for (int lado = -1; lado <= 1; lado += 2) {
            float zc = lado * 0.45f;
            caixa(-0.30f, -0.20f, 0.05f, 0.75f, zc - 0.25f, zc + 0.25f);        // encosto
            caixa(-0.20f,  0.30f, 0.05f, 0.25f, zc - 0.25f, zc + 0.25f);        // assento
        }
        caixa(-1.30f, -1.20f, 0.05f, 0.70f, -0.8f, 0.8f);                       // banco de trás: encosto
        caixa(-1.20f, -0.70f, 0.05f, 0.25f, -0.8f, 0.8f);                       //                assento
    }
    void interiorSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        interiorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- ESTEPE
    // Reaproveita o MESMO rodaSRM das rodas; só o SRU é diferente: girado -90° em y
    // (a calota, que no SRM olha pra +z, passa a olhar pra trás, -x) e pendurado na traseira.
    void estepeSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 - 0.12f, CHASSI_TOPO + 0.35f, 0);
        glRotatef(-90, 0, 1, 0);
        glScalef(1.15f, 1.15f, 1.15f);
        rodaSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA
    // SRM: origem na DOBRADIÇA, porta de x = 0 até -1.2. A dianteira tem a borda da
    // frente da janela inclinada (acompanha o para-brisa) e o retrovisor.
    void portaSRM(int lado, bool dianteira) {
        corDaPintura();
        caixa(-PORTA_COMP, 0, 0, LINHA_JANELA, -0.02f, 0.001f);                  // chapa
        corPreta();
        caixa(-1.05f, -0.90f, 0.45f, 0.49f, lado * 0.03f, lado * 0.06f);         // maçaneta
        if (dianteira) {
            caixa(-0.20f, -0.10f, 0.58f, 0.62f, 0, lado * 0.15f);                // haste do retrovisor
            corDaPintura();
            caixa(-0.18f, -0.11f, 0.55f, 0.72f, lado * 0.12f, lado * 0.32f);     // espelho
        }
        float frenteBaixo = dianteira ? -0.02f : -0.06f;
        float frenteCima  = dianteira ? -0.54f : -0.06f;
        float j1[3] = {frenteBaixo,         0.55f, -lado * 0.01f};
        float j2[3] = {frenteCima,          1.23f, -lado * 0.01f};
        float j3[3] = {-PORTA_COMP + 0.04f, 1.23f, -lado * 0.01f};
        float j4[3] = {-PORTA_COMP + 0.04f, 0.55f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    // SRU: a dianteira articula onde o capô começa; a traseira, logo atrás dela.
    void portaSRU(int lado, bool dianteira, float abertura) {
        float dobradica = COMP / 2 - CAPO_COMP - (dianteira ? 0 : PORTA_COMP);
        glPushMatrix();
        glTranslatef(dobradica, CHASSI_TOPO, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado, dianteira);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL / LANTERNA
    void farolSRM(bool aceso)    { lampada(aceso, true,  -0.03f, 0.03f, 0, 0.15f, -0.17f, 0.17f); }
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.03f, 0.03f, 0, 0.20f, -0.08f, 0.08f); }
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(COMP / 2, CHASSI_TOPO + 0.30f, lado * 0.70f);
        farolSRM(aceso);
        glPopMatrix();
    }
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-COMP / 2, CHASSI_TOPO + 0.30f, lado * 0.82f);   // do lado do estepe
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
