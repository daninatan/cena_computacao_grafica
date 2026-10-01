#ifndef CONSTRUTOR_DE_PISTA_H
#define CONSTRUTOR_DE_PISTA_H

#include "Pista.h"
#include "PistaCurva.h"

#include <vector>

class ConstrutorDePista {
private:
    Pista reta;
    PistaCurva curva;

    void desenhaReta(float tamanho, float subida, float largura, float altura);
    void desenhaCurva(float raio, float largura, float altura,
                      char direcao);

public:
    void desenha();

    // Pontos do meio da pista (y = altura do piso) a cada 'passo', em
    // coordenadas da pista. A lista da a volta completa; o ultimo emenda no primeiro.
    struct Ponto { float x, y, z; };
    static std::vector<Ponto> linhaCentral(float passo);
    static constexpr float MEIA_LARGURA = 40.0f;
};

#endif
