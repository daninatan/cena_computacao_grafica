#pragma once
// =====================================================================================
//  CARRO SEDAN: um tipo de carro que segue a interface ModeloCarro.
//
//  Dentro dele, cada COMPONENTE tem:
//    xxxSRM()  -> modela a peça em volta da própria origem (só primitivas básicas)
//    xxxSRU()  -> glPushMatrix, translação/rotação até o lugar no carro, SRM, glPopMatrix
//  e desenhar() (o SRM do carro inteiro) só chama os SRUs dos componentes.
//
//  No OpenGL a ÚLTIMA transformação escrita é a PRIMEIRA aplicada no vértice:
//  no SRU a leitura é de baixo pra cima (primeiro gira na origem, depois translada).
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroSedan : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    // O SRU de cada peça é calculado a partir destas medidas: é assim que uma peça
    // fica "do lado" da outra.
    static constexpr float CARRO_COMP     = 4.0f;    // x de -2 a 2
    static constexpr float CARRO_LARG     = 1.8f;    // z de -0.9 a 0.9
    static constexpr float VAO_LIVRE      = 0.30f;   // do chão até a base do chassi
    static constexpr float CHASSI_ALT     = 0.20f;
    static constexpr float CHASSI_LARG    = 1.5f;
    static constexpr float CHASSI_TOPO    = VAO_LIVRE + CHASSI_ALT;   // onde a carroceria senta
    static constexpr float CARROCERIA_ALT = 0.42f;   // altura da frente e da traseira
    static constexpr float FRENTE_COMP    = 1.1f;
    static constexpr float TRASEIRA_COMP  = 0.9f;
    static constexpr float ENTRE_EIXOS    = 2.6f;

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        frenteSRU();
        traseiraSRU();
        cabineSRU();
        interiorSRU();
        bancoSRU(-1);                                         // motorista (esquerda)
        bancoSRU(+1);                                         // passageiro
        bancoTraseiroSRU();
        volanteSRU(e.anguloDirecao);
        rodaSRU( ENTRE_EIXOS / 2, -0.88f, e.anguloDirecao, e.distancia);   // dianteira esquerda
        rodaSRU( ENTRE_EIXOS / 2,  0.88f, e.anguloDirecao, e.distancia);   // dianteira direita
        rodaSRU(-ENTRE_EIXOS / 2, -0.88f, e.anguloDirecaoTras, e.distancia);      // traseira esquerda
        rodaSRU(-ENTRE_EIXOS / 2,  0.88f, e.anguloDirecaoTras, e.distancia);      // traseira direita
        portaSRU(-1, e.anguloPortas);
        portaSRU(+1, e.anguloPortas);
        farolSRU(-1, e.luzesLigadas);
        farolSRU(+1, e.luzesLigadas);
        lanternaSRU(-1, e.luzesLigadas);
        lanternaSRU(+1, e.luzesLigadas);
    }

    float entreEixos() const override { return ENTRE_EIXOS; }

    // Spots nos faróis e na traseira. O Veiculo chama isso já dentro do SRU dele.
    void luzes(const EstadoCarro& e) override { spotsDoCarro(e.luzesLigadas, 2.05f, -2.05f, 0.78f, 0.68f); }

private:
    void corDaPintura() { cor(0.10f, 0.25f, 0.80f); }        // azul

    // ---------------------------------------------------------------- CHASSI
    // SRM: origem no centro da BASE do chassi.
    void chassiSRM() {
        cor(0.15f, 0.15f, 0.17f);
        caixa(-CARRO_COMP / 2, CARRO_COMP / 2, 0, CHASSI_ALT, -CHASSI_LARG / 2, CHASSI_LARG / 2);
    }
    // SRU: suspenso acima do chão pelo vão livre (as rodas ficam embaixo).
    void chassiSRU() {
        glPushMatrix();
        glTranslatef(0, VAO_LIVRE, 0);
        chassiSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FRENTE (capô arredondado)
    // SRM: origem na BASE da borda de trás (onde encosta no para-brisa).
    // A curva é um quarto de elipse (primitiva quartoElipse).
    void frenteSRM() {
        corDaPintura();
        quartoElipse(FRENTE_COMP, CARROCERIA_ALT, CARRO_LARG);
    }
    // SRU: em cima do chassi, terminando junto com a ponta da frente dele.
    void frenteSRU() {
        glPushMatrix();
        glTranslatef(CARRO_COMP / 2 - FRENTE_COMP, CHASSI_TOPO, 0);
        frenteSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- TRASEIRA (porta-malas)
    // SRM: origem no centro da base.
    void traseiraSRM() {
        corDaPintura();
        caixa(-TRASEIRA_COMP / 2, TRASEIRA_COMP / 2, 0, CARROCERIA_ALT, -CARRO_LARG / 2, CARRO_LARG / 2);
    }
    // SRU: em cima do chassi, encostada na ponta de trás dele.
    void traseiraSRU() {
        glPushMatrix();
        glTranslatef(-CARRO_COMP / 2 + TRASEIRA_COMP / 2, CHASSI_TOPO, 0);
        traseiraSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (colunas, teto, para-brisa, vidro traseiro)
    // SRM: origem no centro do carro, na altura do topo do chassi.
    void cabineSRM() {
        corDaPintura();
        barra( 0.99f, 0.35f,  0.42f, 0.90f,  0.86f, 0.07f);   // colunas A (bem deitadas)
        barra( 0.99f, 0.35f,  0.42f, 0.90f, -0.86f, 0.07f);
        barra(-1.20f, 0.35f, -0.92f, 0.93f,  0.83f, 0.10f);   // colunas C (mais grossas)
        barra(-1.20f, 0.35f, -0.92f, 0.93f, -0.83f, 0.10f);
        caixa(-0.95f, 0.45f, 0.88f, 0.95f, -0.88f, 0.88f);    // teto: do topo da coluna C ao topo da A

        // para-brisa: embaixo no pé da coluna A, em cima na borda do teto
        float pb1[3] = {0.90f, 0.45f,  0.83f};
        float pb2[3] = {0.90f, 0.45f, -0.83f};
        float pb3[3] = {0.42f, 0.89f, -0.83f};
        float pb4[3] = {0.42f, 0.89f,  0.83f};
        vidro(pb1, pb2, pb3, pb4);

        // vidro traseiro: mesma ideia, entre as colunas C
        float vt1[3] = {-1.10f, 0.35f, -0.83f};
        float vt2[3] = {-1.10f, 0.35f,  0.83f};
        float vt3[3] = {-0.93f, 0.89f,  0.83f};
        float vt4[3] = {-0.93f, 0.89f, -0.83f};
        vidro(vt1, vt2, vt3, vt4);
    }
    // SRU: senta em cima do chassi.
    void cabineSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- INTERIOR (assoalho + painel)
    // SRM: mesma origem da cabine (centro do carro, topo do chassi).
    void interiorSRM() {
        cor(0.25f, 0.25f, 0.27f);
        caixa(-1.1f, 0.9f, -0.05f, 0.05f, -0.85f, 0.85f);     // assoalho: da traseira até a frente
        cor(0.12f, 0.12f, 0.13f);
        caixa(0.55f, 0.9f, 0.25f, 0.45f, -0.85f, 0.85f);      // painel: colado na frente
    }
    void interiorSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        interiorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- BANCO DA FRENTE
    // SRM: origem atrás do encosto, no chão do banco, no meio da largura dele.
    // Um modelo só, usado DUAS vezes (motorista e passageiro).
    void bancoSRM() {
        cor(0.45f, 0.30f, 0.20f);                              // couro marrom
        caixa(0.00f, 0.10f, 0, 0.65f, -0.25f, 0.25f);          // encosto: fino no x, alto no y
        caixa(0.10f, 0.55f, 0, 0.17f, -0.25f, 0.25f);          // assento: deitado, baixinho
    }
    // SRU: em cima do assoalho (topo do chassi + 0.05), um de cada lado.
    void bancoSRU(int lado) {
        glPushMatrix();
        glTranslatef(-0.20f, CHASSI_TOPO + 0.05f, lado * 0.40f);
        bancoSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- BANCO DE TRÁS
    // SRM: origem atrás do encosto, no chão do banco, no centro.
    void bancoTraseiroSRM() {
        cor(0.45f, 0.30f, 0.20f);
        caixa(0.00f, 0.10f, 0, 0.30f, -0.75f, 0.75f);          // encosto
        caixa(0.05f, 0.55f, 0, 0.17f, -0.75f, 0.75f);          // assento inteiriço
    }
    // SRU: encostado na traseira, em cima do assoalho.
    void bancoTraseiroSRU() {
        glPushMatrix();
        glTranslatef(-CARRO_COMP / 2 + TRASEIRA_COMP, CHASSI_TOPO + 0.05f, 0);
        bancoTraseiroSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- VOLANTE (+ coluna de direção)
    // SRM: origem no centro do volante. O toro da GLUT nasce "olhando" para +z;
    // aqui ele é modelado olhando para o motorista (-x) e um pouco inclinado.
    void volanteSRM(float anguloDirecao) {
        cor(0.05f, 0.05f, 0.05f);
        barra(0.14f, -0.065f, 0, 0, 0, 0.03f);                 // coluna: sai do painel até o centro
        if (passadaDoVidro) return;                            // glutSolidTorus não passa pela caixa()
        glPushMatrix();
        glRotatef(90, 0, 1, 0);                                // 2) vira de frente para o motorista
        glRotatef(25, 1, 0, 0);                                // 1) inclina o topo pra frente
        glRotatef(-3 * anguloDirecao, 0, 0, 1);                // 0) gira no proprio eixo: 3 graus de volante por grau de roda (+ = esquerda)
        glutSolidTorus(0.025, 0.15, 8, 20);
        caixa(-0.14f, 0.14f, -0.012f, 0.012f, -0.01f, 0.01f);  // raio horizontal: mostra o giro
        caixa(-0.012f, 0.012f, -0.14f, 0, -0.01f, 0.01f);      // raio de baixo
        glPopMatrix();
    }
    // SRU: na frente do banco do motorista.
    void volanteSRU(float anguloDirecao) {
        glPushMatrix();
        glTranslatef(0.50f, 1.00f, -0.40f);
        volanteSRM(anguloDirecao);
        glPopMatrix();
    }

    // RODA: usa rodaSRM/rodaSRU de Primitivas.h (comum a todos os carros).

    // ---------------------------------------------------------------- PORTA (+ maçaneta, retrovisor, janela)
    // SRM: origem na DOBRADIÇA, na altura do topo do chassi. A porta vai de x = 0 para
    // trás até x = -2.0. Tudo desenhado aqui dentro está no SRM da porta, então abre junto.
    // lado = -1 esquerda, +1 direita (maçaneta e retrovisor saem pro lado de fora).
    void portaSRM(int lado) {
        corDaPintura();
        caixa(-2.0f, 0, 0, 0.40f, -0.02f, 0.001f);                             // chapa

        cor(0.10f, 0.10f, 0.10f);
        caixa(-1.75f, -1.55f, 0.30f, 0.34f, lado * 0.03f, lado * 0.06f);       // maçaneta
        caixa(-0.20f, -0.10f, 0.46f, 0.50f, 0, lado * 0.15f);                  // haste do retrovisor
        corDaPintura();
        caixa(-0.18f, -0.11f, 0.44f, 0.58f, lado * 0.12f, lado * 0.32f);       // carcaça do espelho

        // janela: borda da frente acompanha a coluna A, a de trás acompanha a coluna C
        float j1[3] = { 0.02f, 0.36f, -lado * 0.01f};
        float j2[3] = {-0.45f, 0.89f, -lado * 0.01f};
        float j3[3] = {-1.80f, 0.89f, -lado * 0.01f};
        float j4[3] = {-2.00f, 0.36f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    // SRU: dobradiça onde a frente começa, na lateral da carroceria. A rotação vem
    // DEPOIS da translação no código, então é aplicada ANTES: a porta gira em volta
    // da própria dobradiça (origem do SRM) e só depois é levada pro lugar.
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(CARRO_COMP / 2 - FRENTE_COMP, CHASSI_TOPO, lado * CARRO_LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);                   // abre sempre pra fora
        portaSRM(lado);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL
    // A peça BRILHA por EMISSÃO do material: faz parte do MODELO, então fica no SRM.
    // SRM: origem na face da frente do farol, embaixo, no meio da largura.
    void farolSRM(bool aceso) { lampada(aceso, true, -0.97f, -0.02f, 0, 0.17f, -0.135f, 0.135f); }
    // SRU: na ponta da frente do carro, um de cada lado.
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(CARRO_COMP / 2, CHASSI_TOPO + 0.05f, lado * 0.685f);
        farolSRM(aceso);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- LANTERNA
    // SRM: origem na face de trás da carroceria, embaixo, no meio da largura.
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.03f, 0.02f, 0, 0.13f, -0.135f, 0.135f); }
    // SRU: na traseira, um de cada lado.
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-CARRO_COMP / 2, CHASSI_TOPO + 0.22f, lado * 0.685f);
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
