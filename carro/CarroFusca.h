#pragma once
// =====================================================================================
//  CARRO FUSCA: tudo arredondado. Capô e traseira são o MESMO quarto de elipse
//  (a traseira é ele girado 180° no SRU), teto em arco, paralamas redondos soltos
//  por cima das rodas, estribo e para-choques cromados. Segue a interface ModeloCarro.
//
//    x: capô [0.95, 2.05]   porta [-0.15, 0.95]   lateral de trás [-0.85, -0.15]
//       tampa do motor [-2.05, -0.85]
// =====================================================================================
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class CarroFusca : public ModeloCarro {
public:
    // --------------------------------------------------------------- MEDIDAS
    static constexpr float COMP         = 4.1f;    // x de -2.05 a 2.05
    static constexpr float LARG         = 1.4f;    // carroceria (os paralamas saem pra fora disso)
    static constexpr float VAO_LIVRE    = 0.30f;
    static constexpr float CHASSI_ALT   = 0.15f;
    static constexpr float CHASSI_LARG  = 1.2f;
    static constexpr float CHASSI_TOPO  = VAO_LIVRE + CHASSI_ALT;   // 0.45
    static constexpr float CINTURA      = 0.45f;   // altura do capô, da tampa e das portas
    static constexpr float CAPO_COMP    = 1.1f;
    static constexpr float MOTOR_COMP   = 1.2f;
    static constexpr float PORTA_COMP   = 1.1f;
    static constexpr float EIXO         = 1.2f;    // rodas em x = +-1.2
    static constexpr float Z_RODA       = 0.97f;   // bem pra fora: esterçada não encosta na carroceria (0.7)

    // Teto: arco de elipse centrado em x = 0.05, com raio 0.9 em x e 0.6 em y,
    // saindo da cintura. Só a parte de cima (50° a 130°) é chapa; o resto são vidros.
    static constexpr float TETO_XC = 0.05f, TETO_A = 0.9f, TETO_B = 0.6f;

    // --------------------------------------------------------------- INTERFACE
    void desenhar(const EstadoCarro& e) override {
        chassiSRU();
        capoSRU();
        motorSRU();
        cabineSRU();
        interiorSRU();
        paraChoqueSRU(true);
        paraChoqueSRU(false);
        for (int lado = -1; lado <= 1; lado += 2) {
            rodaSRU( EIXO, lado * Z_RODA, e.anguloDirecao,     e.distancia);
            rodaSRU(-EIXO, lado * Z_RODA, e.anguloDirecaoTras, e.distancia);
            paralamaSRU( EIXO, lado);
            paralamaSRU(-EIXO, lado);
            estriboSRU(lado);
            portaSRU(lado, e.anguloPortas);
            farolSRU(lado, e.luzesLigadas);
            lanternaSRU(lado, e.luzesLigadas);
        }
    }

    float entreEixos() const override { return 2 * EIXO; }

    void luzes(const EstadoCarro& e) override {
        spotsDoCarro(e.luzesLigadas, 1.65f, -1.70f, 0.73f, Z_RODA);
    }

private:
    void corDaPintura() { cor(0.35f, 0.65f, 0.85f); }        // azul-bebê
    void corCromo()     { cor(0.80f, 0.80f, 0.83f); }
    void corPreta()     { cor(0.10f, 0.10f, 0.11f); }

    // ponto do arco do teto no ângulo g (graus)
    float tetoX(float g) { return TETO_XC + TETO_A * cosf(g * PI_CARRO / 180); }
    float tetoY(float g) { return CINTURA + TETO_B * sinf(g * PI_CARRO / 180); }

    // ---------------------------------------------------------------- CHASSI (assoalho)
    void chassiSRM() {
        cor(0.15f, 0.15f, 0.17f);
        caixa(-COMP / 2 + 0.05f, COMP / 2 - 0.05f, 0, CHASSI_ALT, -CHASSI_LARG / 2, CHASSI_LARG / 2);
    }
    void chassiSRU() {
        glPushMatrix();
        glTranslatef(0, VAO_LIVRE, 0);
        chassiSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CAPÔ (frente redonda)
    // SRM: origem na base da borda de trás. A curva desce pra frente (+x).
    void capoSRM() {
        corDaPintura();
        quartoElipse(CAPO_COMP, CINTURA, LARG);
    }
    void capoSRU() {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, 0);
        capoSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- TAMPA DO MOTOR (traseira redonda)
    // SRM: o mesmo quarto de elipse do capô, só que mais comprido, com uma grade de ventilação.
    void motorSRM() {
        corDaPintura();
        quartoElipse(MOTOR_COMP, CINTURA, LARG);
        corPreta();
        for (int i = 0; i < 4; i++)                                          // grade na descida da tampa
            caixa(0.75f, 0.85f, 0.26f + i * 0.03f, 0.27f + i * 0.03f, -0.2f, 0.2f);
    }
    // SRU: GIRADO 180° em y -> a curva passa a descer pra TRÁS. Mesma peça, outra orientação.
    void motorSRU() {
        glPushMatrix();
        glTranslatef(-COMP / 2 + MOTOR_COMP, CHASSI_TOPO, 0);
        glRotatef(180, 0, 1, 0);
        motorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- CABINE (teto em arco, colunas, vidros)
    // SRM: origem no centro do carro, topo do chassi.
    void cabineSRM() {
        const int N = 12;
        const float g0 = 50, g1 = 130, z = 0.68f;
        corDaPintura();
        if (!passadaDoVidro) {
            // CHAPA DO TETO: percorre de 130° até 50° (x crescendo) -> face aponta pra fora
            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i <= N; i++) {
                float g = g1 - (g1 - g0) * i / N, r = g * PI_CARRO / 180;
                glNormal3f(cosf(r) / TETO_A, sinf(r) / TETO_B, 0);
                glVertex3f(tetoX(g), tetoY(g), -z);
                glVertex3f(tetoX(g), tetoY(g),  z);
            }
            glEnd();
            // TAMPAS laterais do arco (entre o arco e a linha reta das janelas)
            for (int lado = -1; lado <= 1; lado += 2) {
                glNormal3f(0, 0, lado);
                glBegin(GL_TRIANGLE_FAN);
                glVertex3f(TETO_XC, tetoY(g0), lado * z);
                for (int i = 0; i <= N; i++) {
                    int k = (lado > 0) ? i : N - i;                          // anti-horário visto de fora
                    float g = g0 + (g1 - g0) * k / N;
                    glVertex3f(tetoX(g), tetoY(g), lado * z);
                }
                glEnd();
            }
        }
        // colunas A e C: seguem a corda do arco de 0° a 50° e de 130° a 180°
        barra(tetoX(0),   tetoY(0),   tetoX(g0), tetoY(g0),  z, 0.06f);
        barra(tetoX(0),   tetoY(0),   tetoX(g0), tetoY(g0), -z, 0.06f);
        barra(tetoX(180), tetoY(180), tetoX(g1), tetoY(g1),  z, 0.06f);
        barra(tetoX(180), tetoY(180), tetoX(g1), tetoY(g1), -z, 0.06f);
        for (int lado = -1; lado <= 1; lado += 2) {
            caixa(-0.85f, -0.15f, 0, CINTURA, lado * 0.68f, lado * 0.70f);  // lateral de trás (embaixo)
            caixa(-0.19f, -0.14f, CINTURA, tetoY(g0), lado * 0.64f, lado * 0.70f);   // coluna B
        }

        float yTopo = tetoY(g0) - 0.01f;
        float pb1[3] = {tetoX(0) - 0.01f,  CINTURA + 0.01f,  0.64f};         // para-brisa
        float pb2[3] = {tetoX(0) - 0.01f,  CINTURA + 0.01f, -0.64f};
        float pb3[3] = {tetoX(g0),         yTopo,           -0.64f};
        float pb4[3] = {tetoX(g0),         yTopo,            0.64f};
        vidro(pb1, pb2, pb3, pb4);

        float vt1[3] = {tetoX(180) + 0.01f, CINTURA + 0.01f, -0.64f};        // vidro traseiro
        float vt2[3] = {tetoX(180) + 0.01f, CINTURA + 0.01f,  0.64f};
        float vt3[3] = {tetoX(g1),          yTopo,            0.64f};
        float vt4[3] = {tetoX(g1),          yTopo,           -0.64f};
        vidro(vt1, vt2, vt3, vt4);

        for (int lado = -1; lado <= 1; lado += 2) {                         // janelinha de trás (fixa)
            float zj = lado * 0.69f;
            float q1[3] = {-0.17f, 0.47f, zj}, q2[3] = {-0.17f, yTopo, zj};
            float q3[3] = {tetoX(g1) + 0.02f, yTopo, zj}, q4[3] = {-0.83f, 0.47f, zj};
            vidro(q1, q2, q3, q4);
        }
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
        caixa(-0.85f, 0.95f, -0.05f, 0.05f, -0.68f, 0.68f);                 // assoalho
        corDaPintura();
        caixa(0.75f, 0.95f, 0.20f, CINTURA, -0.68f, 0.68f);                 // painel na cor do carro
        cor(0.55f, 0.45f, 0.35f);                                           // bancos bege
        for (int lado = -1; lado <= 1; lado += 2) {
            float zc = lado * 0.33f;
            caixa(-0.10f,  0.00f, 0.05f, 0.65f, zc - 0.22f, zc + 0.22f);    // banco da frente: encosto
            caixa( 0.00f,  0.40f, 0.05f, 0.22f, zc - 0.22f, zc + 0.22f);    //                  assento
        }
        caixa(-0.85f, -0.75f, 0.05f, 0.60f, -0.62f, 0.62f);                 // banco de trás: encosto
        caixa(-0.75f, -0.35f, 0.05f, 0.22f, -0.62f, 0.62f);                 //                assento
    }
    void interiorSRU() {
        glPushMatrix();
        glTranslatef(0, CHASSI_TOPO, 0);
        interiorSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PARALAMA
    // SRM: meia elipse (arco) sobre a roda = DOIS quartos de elipse de costas um pro
    // outro (um deles girado 180° em y). Origem no centro da roda, na altura do eixo.
    void paralamaSRM() {
        corDaPintura();
        quartoElipse(0.5f, 0.45f, 0.5f, false);             // casca oca: a roda fica embaixo, não dentro
        glPushMatrix();
        glRotatef(180, 0, 1, 0);
        quartoElipse(0.5f, 0.45f, 0.5f, false);
        glPopMatrix();
    }
    // SRU: centrado sobre cada roda. Largo (0.5) pra cobrir a roda mesmo esterçada.
    void paralamaSRU(float x, int lado) {
        glPushMatrix();
        glTranslatef(x, 0.35f, lado * Z_RODA);
        paralamaSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- ESTRIBO (entre os paralamas)
    void estriboSRM() {
        corPreta();
        caixa(-0.75f, 0.75f, 0, 0.05f, -0.13f, 0.13f);
    }
    void estriboSRU(int lado) {
        glPushMatrix();
        glTranslatef(0, 0.33f, lado * 0.85f);
        estriboSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PARA-CHOQUE (cromado)
    // SRM: barra com origem na face de dentro, saindo pra +x.
    void paraChoqueSRM() {
        corCromo();
        caixa(0, 0.10f, 0, 0.08f, -1.05f, 1.05f);
    }
    // SRU: o de trás é o mesmo, girado 180° em y.
    void paraChoqueSRU(bool frente) {
        glPushMatrix();
        glTranslatef(frente ? COMP / 2 - 0.05f : -COMP / 2 + 0.05f, 0.38f, 0);
        if (!frente) glRotatef(180, 0, 1, 0);
        paraChoqueSRM();
        glPopMatrix();
    }

    // ---------------------------------------------------------------- PORTA
    // SRM: origem na DOBRADIÇA, porta de x = 0 até -1.1. Janela: frente acompanha a
    // coluna A, topo reto na altura onde o arco do teto começa.
    void portaSRM(int lado) {
        corDaPintura();
        caixa(-PORTA_COMP, 0, 0, CINTURA, -0.02f, 0.001f);                  // chapa
        corCromo();
        caixa(-0.95f, -0.80f, 0.35f, 0.38f, lado * 0.03f, lado * 0.06f);    // maçaneta
        caixa(-0.15f, -0.10f, 0.50f, 0.53f, 0, lado * 0.12f);               // haste do retrovisor
        caixa(-0.14f, -0.10f, 0.47f, 0.58f, lado * 0.10f, lado * 0.20f);    // espelho redondinho (quase)

        float yTopo = tetoY(50) - 0.01f;
        float dobradica = COMP / 2 - CAPO_COMP;
        float j1[3] = {tetoX(0) - dobradica - 0.02f, 0.47f, -lado * 0.01f};
        float j2[3] = {tetoX(50) - dobradica,        yTopo, -lado * 0.01f};
        float j3[3] = {-PORTA_COMP + 0.04f,          yTopo, -lado * 0.01f};
        float j4[3] = {-PORTA_COMP + 0.04f,          0.47f, -lado * 0.01f};
        vidro(j1, j2, j3, j4);
    }
    void portaSRU(int lado, float abertura) {
        glPushMatrix();
        glTranslatef(COMP / 2 - CAPO_COMP, CHASSI_TOPO, lado * LARG / 2);
        glRotatef(lado * abertura, 0, 1, 0);
        portaSRM(lado);
        glPopMatrix();
    }

    // ---------------------------------------------------------------- FAROL / LANTERNA (nos paralamas)
    void farolSRM(bool aceso)    { lampada(aceso, true,  -0.05f, 0.08f, 0, 0.12f, -0.08f, 0.08f); }
    void lanternaSRM(bool acesa) { lampada(acesa, false, -0.08f, 0.03f, 0, 0.10f, -0.05f, 0.05f); }
    // SRU: na descida da frente do paralama dianteiro / na traseira do paralama de trás.
    void farolSRU(int lado, bool aceso) {
        glPushMatrix();
        glTranslatef(EIXO + 0.35f, 0.63f, lado * Z_RODA);
        farolSRM(aceso);
        glPopMatrix();
    }
    void lanternaSRU(int lado, bool acesa) {
        glPushMatrix();
        glTranslatef(-EIXO - 0.38f, 0.58f, lado * Z_RODA);
        lanternaSRM(acesa);
        glPopMatrix();
    }
};

} // namespace carros
