#pragma once
// =====================================================================================
//  CARRO ESPORTIVO: cupê de motor central, rente ao chão. Segue a interface ModeloCarro.
//
//  A carroceria é esculpida com perfilExtrudado (a SILHUETA lateral esticada em z):
//    - paralamas: peças laterais com a caixa de roda RECORTADA na borda de baixo e um
//      "ombro" alto por cima de cada roda
//    - capô e tampa do motor: peças do meio, mais baixas que os paralamas
//  Visto de frente: [paralama][ capô baixo ][paralama] -> o visual musculoso.
//
//  Cada componente tem xxxSRM() (modelagem) e xxxSRU() (posicionamento). As silhuetas
//  são escritas direto em x e y do carro; o SRU dos paralamas leva cada um pro seu lado (z).
//
//    x: frente [0.95, 2.3]   porta [-0.95, 0.95]   traseira [-2.3, -0.95]
//    z: meio [-0.6, 0.6]     paralamas [0.6, 1.0] de cada lado
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroEsportivo : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP        = 4.6f;     // x de -2.3 a 2.3
    static constexpr float LARG        = 2.0f;     // z de -1.0 a 1.0 (por fora dos paralamas)
    static constexpr float MEIO        = 1.2f;     // largura das peças do meio (capô, motor)
    static constexpr float PARALAMA    = 0.4f;     // largura de cada paralama
    static constexpr float Z_PARALAMA  = MEIO / 2 + PARALAMA / 2;   // 0.8: centro do paralama
    static constexpr float BASE        = 0.18f;    // altura da borda de baixo da carroceria
    static constexpr float CINTURA     = 0.68f;    // altura da borda de cima das portas
    static constexpr float DOBRADICA   = 0.95f;    // x da dobradiça (fim do paralama da frente)
    static constexpr float PORTA_COMP  = 1.9f;
    static constexpr float EIXO_FRENTE = 1.4f, EIXO_TRAS = -1.45f;
    static constexpr float Z_RODA      = 0.87f;    // esterçada fica entre 0.61 e 1.13: não encosta no meio
    static constexpr float ARCO        = 0.40f;    // raio da caixa de roda (roda tem 0.35)

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        frenteSRU();
        motorSRU();
        cabineSRU();
        interiorSRU();
        volanteSRU(e.anguloDirecao);
        for (int lado = -1; lado <= 1; lado += 2) {
            paralamaFrenteSRU(lado);
            paralamaTrasSRU(lado);
            rodaSRU(EIXO_FRENTE, lado * Z_RODA, e.anguloDirecao,     e.distancia);
            rodaSRU(EIXO_TRAS,   lado * Z_RODA, e.anguloDirecaoTras, e.distancia);
            portaSRU(lado, e.anguloPortas);
            farolSRU(lado, e.luzesLigadas);
        }
        lanternasSRU(e.luzesLigadas);
    }

    float entreEixos() const override { return EIXO_FRENTE - EIXO_TRAS; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, COMP / 2 + 0.05f, -COMP / 2 - 0.05f, 0.50f, Z_PARALAMA);
    }

private:
    void corDaPintura() { cor(0.95f, 0.72f, 0.02f); }        // amarelo
    void corPreta()     { cor(0.06f, 0.06f, 0.07f); }
    void corCromo()     { cor(0.75f, 0.75f, 0.78f); }

    // Borda de baixo com a caixa de roda recortada: um arco de raio ARCO em volta do eixo.
    static float caixaDeRoda(float x, float eixo) {
        float d = x - eixo;
        return fabsf(d) < ARCO ? 0.35f + sqrtf(ARCO * ARCO - d * d) : BASE;
    }

    // ---------------------------------------------------------------- CHASSI (assoalho + soleiras)
    void chassiSRM() {
        cor(0.12f, 0.12f, 0.13f);
        caixa(-2.2f, 2.2f, 0, 0.08f, -MEIO / 2, MEIO / 2);                    // assoalho
        caixa(-DOBRADICA, DOBRADICA, 0.03f, 0.12f, -0.98f, 0.98f);            // soleiras (embaixo das portas)
    }
    void chassiSRU() {
        glPushMatrix();
        glTranslatef(0, 0.12f, 0);
        chassiSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FRENTE (capô baixo + bico + entrada de ar)
    // SRM: silhueta do capô, descendo até o bico.
    void frenteSRM() {
        static const float topo[][2] = {{0.95f, 0.67f}, {1.40f, 0.63f}, {1.90f, 0.55f}, {2.15f, 0.45f}, {2.30f, 0.32f}};
        corDaPintura();
        perfilExtrudado(DOBRADICA - 0.05f, COMP / 2, MEIO,
                        [](float x) { return interpola(x, topo, 5); },
                        [](float)   { return BASE; });
        corPreta();
        caixa(2.28f, 2.31f, 0.20f, 0.29f, -0.50f, 0.50f);                    // entrada de ar no bico
        caixa(1.55f, 1.85f, 0.62f, 0.625f, -0.25f, 0.25f);                   // respiro no capô (linha escura)
    }
    void frenteSRU() {
        glPushMatrix();                                                      // silhueta já em x/y do carro, z centrado
        frenteSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PARALAMA DIANTEIRO
    // SRM: silhueta com o ombro alto sobre a roda e a caixa de roda recortada embaixo.
    void paralamaFrenteSRM() {
        static const float topo[][2] = {{0.95f, 0.67f}, {1.40f, 0.80f}, {1.85f, 0.66f},
                                        {2.05f, 0.56f}, {2.20f, 0.45f}, {2.28f, 0.34f}};
        corDaPintura();
        perfilExtrudado(DOBRADICA, 2.28f, PARALAMA,
                        [](float x) { return interpola(x, topo, 6); },
                        [](float x) { return caixaDeRoda(x, EIXO_FRENTE); }, 64);
    }
    // SRU: cada paralama vai pro seu lado, encostado nas peças do meio.
    void paralamaFrenteSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * Z_PARALAMA);
        paralamaFrenteSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PARALAMA TRASEIRO (ombro largo do motor)
    void paralamaTrasSRM() {
        static const float topo[][2] = {{-2.30f, 0.60f}, {-2.20f, 0.72f}, {-1.90f, 0.78f},
                                        {-1.45f, 0.82f}, {-1.10f, 0.76f}, {-0.95f, 0.70f}};
        corDaPintura();
        perfilExtrudado(-COMP / 2, -DOBRADICA, PARALAMA,
                        [](float x) { return interpola(x, topo, 6); },
                        [](float x) { return x < -2.2f ? 0.26f : caixaDeRoda(x, EIXO_TRAS); }, 64);
    }
    void paralamaTrasSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0, lado * Z_PARALAMA);
        paralamaTrasSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- MOTOR (tampa + difusor + escapamentos)
    void motorSRM() {
        static const float topo[][2]  = {{-2.30f, 0.58f}, {-2.20f, 0.74f}, {-1.80f, 0.76f}, {-1.20f, 0.75f}, {-0.95f, 0.72f}};
        static const float baixo[][2] = {{-2.30f, 0.26f}, {-2.00f, BASE}, {-0.95f, BASE}};
        corDaPintura();
        perfilExtrudado(-COMP / 2, -DOBRADICA, MEIO,
                        [](float x) { return interpola(x, topo, 5); },
                        [](float x) { return interpola(x, baixo, 3); });
        corPreta();
        for (int i = 0; i < 5; i++) {                                          // grelha do motor
            float x = -1.40f - i * 0.12f;
            caixa(x, x + 0.05f, 0.745f, 0.775f, -0.40f, 0.40f);
        }
        caixa(-2.33f, -2.00f, 0.15f, 0.25f, -0.80f, 0.80f);                   // difusor
        caixa(-2.25f, -2.18f, 0.72f, 0.80f, -0.95f, 0.95f);                   // lábio do aerofólio (preto)
        corCromo();
        caixa(-2.36f, -2.28f, 0.21f, 0.28f,  0.18f, 0.30f);                   // escapamentos
        caixa(-2.36f, -2.28f, 0.21f, 0.28f, -0.30f, -0.18f);
    }
    void motorSRU() {
        glPushMatrix();
        motorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (teto arqueado, colunas, vidros)
    // SRM: bolha baixa. Para-brisa bem deitado; atrás, vidro caído até a tampa do motor
    // com as colunas C em "arcobotante" até o ombro traseiro.
    void cabineSRM() {
        static const float teto[][2] = {{-0.58f, 1.08f}, {-0.25f, 1.12f}, {0.08f, 1.08f}};
        corDaPintura();
        perfilExtrudado(-0.58f, 0.08f, 1.50f,
                        [](float x) { return interpola(x, teto, 3); },
                        [](float x) { return interpola(x, teto, 3) - 0.04f; }, 12);
        corPreta();
        barra( 0.90f, 0.67f,  0.05f, 1.07f,  0.77f, 0.06f);                   // colunas A (pretas: vidro "envolvente")
        barra( 0.90f, 0.67f,  0.05f, 1.07f, -0.77f, 0.06f);
        corDaPintura();
        barra(-0.56f, 1.07f, -1.25f, 0.73f,  0.78f, 0.08f);                   // colunas C (arcobotante)
        barra(-0.56f, 1.07f, -1.25f, 0.73f, -0.78f, 0.08f);

        float pb1[3] = {0.88f, 0.69f,  0.74f};                                // para-brisa
        float pb2[3] = {0.88f, 0.69f, -0.74f};
        float pb3[3] = {0.08f, 1.06f, -0.72f};
        float pb4[3] = {0.08f, 1.06f,  0.72f};
        vidro(pb1, pb2, pb3, pb4);

        float vt1[3] = {-1.20f, 0.745f, -0.62f};                              // vidro traseiro (em cima do motor)
        float vt2[3] = {-1.20f, 0.745f,  0.62f};
        float vt3[3] = {-0.58f, 1.05f,   0.68f};
        float vt4[3] = {-0.58f, 1.05f,  -0.68f};
        vidro(vt1, vt2, vt3, vt4);

        for (int lado = -1; lado <= 1; lado += 2) {                          // vidrinho entre a porta e a coluna C
            float z = lado * 0.78f;
            float q1[3] = {-1.22f, 0.73f, z}, q2[3] = {-0.97f, 0.71f, z};
            float q3[3] = {-0.59f, 1.04f, z}, q4[3] = {-0.63f, 1.05f, z};
            vidro(q1, q2, q3, q4);
        }
    }
    void cabineSRU() {
        glPushMatrix();
        cabineSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- INTERIOR (bancos concha, painel)
    void interiorSRM() {
        corPreta();
        caixa(0.55f, 0.92f, 0.45f, 0.66f, -0.78f, 0.78f);                     // painel
        cor(0.55f, 0.05f, 0.05f);                                             // bancos de couro vermelho
        for (int lado = -1; lado <= 1; lado += 2) {
            float zc = lado * 0.40f;
            caixa(-0.20f,  0.30f, 0.24f, 0.34f, zc - 0.22f, zc + 0.22f);      // assento
            caixa(-0.30f, -0.18f, 0.24f, 0.86f, zc - 0.22f, zc + 0.22f);      // encosto alto
            caixa(-0.30f,  0.30f, 0.24f, 0.44f, zc - 0.24f, zc - 0.18f);      // abas laterais (concha)
            caixa(-0.30f,  0.30f, 0.24f, 0.44f, zc + 0.18f, zc + 0.24f);
        }
        corPreta();
        caixa(-0.10f, 0.50f, 0.24f, 0.40f, -0.10f, 0.10f);                    // console central
    }
    void interiorSRU() {
        glPushMatrix();
        interiorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- VOLANTE (gira junto com a direção)
    // SRM: origem no centro do volante, "olhando" pro motorista (-x).
    void volanteSRM() {
        if (passadaDoVidro) return;
        corPreta();
        glutSolidTorus(0.022, 0.13, 8, 20);
        caixa(-0.12f, 0.12f, -0.015f, 0.015f, -0.01f, 0.01f);                // raio horizontal
    }
    // SRU: na frente do banco do motorista (esquerda, -z), inclinado e girado pela direção.
    void volanteSRU(float direcao) {
        glPushMatrix();
        glTranslatef(0.42f, 0.68f, -0.40f);                                   // 4) na frente do motorista
        glRotatef(90, 0, 1, 0);                                               // 3) vira pro motorista
        glRotatef(20, 1, 0, 0);                                               // 2) inclina
        glRotatef(direcao * 3, 0, 0, 1);                                      // 1) gira (volante vira mais que a roda)
        volanteSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA
    // SRM: origem na DOBRADIÇA (x = 0), porta até -1.9. A janela fica 0.2 pra DENTRO da
    // chapa (a cabine é mais estreita que a carroceria); o peitoril fecha esse degrau.
    void portaSRM(int lado) {
        float dentro = -lado * 0.20f;                                          // deslocamento pra dentro do carro
        corDaPintura();
        caixa(-PORTA_COMP, 0, 0.26f, CINTURA, -0.025f, 0.025f);               // chapa
        caixa(-PORTA_COMP, 0, CINTURA - 0.03f, CINTURA, dentro, 0);           // peitoril
        corPreta();
        caixa(-1.90f, -0.05f, 0.18f, 0.26f, -0.025f, 0.025f);                 // saia lateral preta
        caixa(-1.55f, -1.35f, 0.60f, 0.62f, lado * 0.02f, lado * 0.04f);      // maçaneta embutida
        caixa(-0.30f, -0.22f, CINTURA, 0.74f, -0.01f, lado * 0.08f);          // haste do retrovisor
        corDaPintura();
        caixa(-0.34f, -0.20f, 0.72f, 0.80f, lado * 0.06f, lado * 0.20f);      // retrovisor

        float j1[3] = {-0.11f, 0.70f, dentro};
        float j2[3] = {-0.84f, 1.04f, dentro};
        float j3[3] = {-1.52f, 1.04f, dentro};
        float j4[3] = {-1.90f, 0.70f, dentro};
        vidro(j1, j2, j3, j4);
    }
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(DOBRADICA, 0, lado * 0.975f);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FARÓIS (finos, deitados na descida do paralama)
    // SRM: lâmina fina centrada na origem.
    void farolSRM(bool aceso) { lampada(aceso, true, -0.10f, 0.10f, -0.02f, 0.02f, -0.13f, 0.13f); }
    // SRU: no meio da descida do paralama e GIRADO pra ficar na mesma inclinação dela
    //      (de (2.05, 0.56) a (2.20, 0.45): uns -36°).
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(2.125f, 0.505f, lado * Z_PARALAMA);
        glRotatef(-36, 0, 0, 1);
        farolSRM(aceso);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- LANTERNAS (faixa de ponta a ponta)
    void lanternasSRM(bool acesas) { lampada(acesas, false, -0.03f, 0.01f, 0, 0.05f, -0.92f, 0.92f); }
    void lanternasSRU(bool acesas) {
        glPushMatrix();
        glTranslatef(-COMP / 2, 0.52f, 0);
        lanternasSRM(acesas);
        glPopMatrix();
    }
};

} // namespace carros
