#pragma once
// =====================================================================================
//  VEÍCULO: o FUNCIONAMENTO do carro.
//
//  Recebe QUALQUER modelo que siga a interface ModeloCarro e dá vida a ele:
//    W/S = acelera / freia e dá ré     A/D = esterça     P = portas     L = faróis
//
//  O Veiculo é o SRU do carro inteiro: posição (x, z) e direção (angulo, em graus,
//  em volta de y). O modelo desenha no SRM dele; aqui ele é levado pro lugar no mundo.
//
//  USO em qualquer projeto (o projeto continua dono da janela, câmera e callbacks):
//    CarroSedan sedan;                       // o visual (um só, pode ser compartilhado)
//    Veiculo carro(sedan, x, z, angulo);     // spawn
//    teclado:       carro.tecla(k, true);    teclado solto: carro.tecla(k, false);
//    idle/timer:    carro.atualizar(dt);     // dt em segundos
//    display:       depois da câmera -> carro.luzes();  depois -> carro.desenhar();
//  Use glutIgnoreKeyRepeat(1), senão P e L ficam alternando enquanto a tecla está apertada.
// =====================================================================================
#include <cctype>
#include "ModeloCarro.h"
#include "Primitivas.h"

namespace carros {

class Veiculo {
public:
    ModeloCarro& modelo;
    float x, z, angulo;              // SRU: onde o carro está e pra onde aponta (0 = frente em +x)
    float velocidade = 0;            // unidades por segundo (negativa = ré)
    EstadoCarro estado;

    // dirigibilidade (ajuste a gosto)
    float aceleracao    = 8;         // unidades/s²
    float atrito        = 0.5f;      // quanto a velocidade cai sozinha por segundo (fração)
    float velocidadeMax = 12;        // ré vai até a metade disso
    float direcaoMax    = 30;        // graus máximos de esterço das rodas
    float fatorTraseira = -0.5f;     // rodas de trás = fator x frente: negativo vira ao contrário
                                     // (curva mais fechada), 0 = fixas como num carro comum

    Veiculo(ModeloCarro& modelo, float x = 0, float z = 0, float angulo = 0)
        : modelo(modelo), x(x), z(z), angulo(angulo) {}

    void tecla(unsigned char k, bool apertada) {
        switch (tolower(k)) {
            case 'w': w = apertada; break;
            case 's': s = apertada; break;
            case 'a': a = apertada; break;
            case 'd': d = apertada; break;
            case 'p': if (apertada) estado.anguloPortas = estado.anguloPortas > 0 ? 0 : 65; break;
            case 'l': if (apertada) estado.luzesLigadas = !estado.luzesLigadas; break;
        }
    }

    void atualizar(float dt) {
        // velocidade: acelera com W/S, perde um pouco sozinha (atrito)
        float pedal = (w ? 1.f : 0.f) - (s ? 1.f : 0.f);
        velocidade += (pedal * aceleracao - velocidade * atrito) * dt;
        velocidade = fmaxf(-velocidadeMax / 2, fminf(velocidadeMax, velocidade));

        // volante: as rodas vão suavemente até o ângulo pedido por A/D
        float alvo = ((a ? 1.f : 0.f) - (d ? 1.f : 0.f)) * direcaoMax;
        estado.anguloDirecao += (alvo - estado.anguloDirecao) * fminf(1, 6 * dt);
        estado.anguloDirecaoTras = estado.anguloDirecao * fatorTraseira;

        // modelo de bicicleta com esterço nas 4 rodas:
        // velocidade de giro = v * (tan(frente) - tan(trás)) / entreEixos
        float rad = PI_CARRO / 180;
        float tanFrente = tanf(estado.anguloDirecao * rad), tanTras = tanf(estado.anguloDirecaoTras * rad);
        angulo += velocidade * (tanFrente - tanTras) / modelo.entreEixos() * dt / rad;

        // anda pra onde a frente aponta: glRotatef(angulo) leva +x para (cos, 0, -sen)
        x += cosf(angulo * rad) * velocidade * dt;
        z -= sinf(angulo * rad) * velocidade * dt;
        estado.distancia += velocidade * dt;               // o modelo usa pra girar as rodas
    }

    // Faróis iluminando a cena: chamar DEPOIS da câmera e ANTES de desenhar o resto,
    // pra luz ficar presa no carro e já iluminar o chão.
    void luzes() {
        glPushMatrix();
        sru();
        modelo.luzes(estado);
        glPopMatrix();
    }

    // Desenha o carro em 2 passadas (opaco, depois vidro com transparência).
    // Salva e restaura o estado do OpenGL, então não bagunça o projeto que usa.
    // ponytail: os vidros misturam só com o que já foi desenhado; desenhe o carro depois do cenário opaco
    void desenhar() {
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_DEPTH_TEST);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);   // vidro com normal pra dentro não fica escuro

        glPushMatrix();
        sru();

        passadaDoVidro = false;                            // 1ª passada: opacos
        modelo.desenhar(estado);

        passadaDoVidro = true;                             // 2ª passada: vidros
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // cor final = alfa*vidro + (1-alfa)*fundo
        glDepthMask(GL_FALSE);                             // vidro não "tampa" nada no z-buffer
        modelo.desenhar(estado);
        passadaDoVidro = false;

        glPopMatrix();
        glPopAttrib();
    }

private:
    bool w = false, a = false, s = false, d = false;

    // SRU do carro inteiro (leitura de baixo pra cima: gira na origem, depois translada)
    void sru() {
        glTranslatef(x, 0, z);
        glRotatef(angulo, 0, 1, 0);
    }
};

} // namespace carros
