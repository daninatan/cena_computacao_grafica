#ifndef CONSTRUTOR_DE_CENARIO_H
#define CONSTRUTOR_DE_CENARIO_H

#include "ConstrutorDePista.h"

class ConstrutorDeCenario{
    public:
        void desenha();
        void tecla(unsigned char tecla, bool apertada);
        void atualiza(float dt);
        void posicionaCamera();
};

#endif
