#ifndef CONSTRUTOR_DE_PISTA_H
#define CONSTRUTOR_DE_PISTA_H

#include "Pista.h"
#include "PistaCurva.h"

class ConstrutorDePista {
private:
    Pista reta;
    PistaCurva curva;

    void desenhaReta(float tamanho, float largura, float altura);
    void desenhaCurva(float raio, float largura, float altura,
                      char direcao);

public:
    void desenha();
};

#endif
