#ifndef CONSTRUTOR_DE_CENARIO_H
#define CONSTRUTOR_DE_CENARIO_H

#include "ConstrutorDePista.h"

class ConstrutorDeCenario{
    public:
        void desenha();
        void tecla(unsigned char tecla, bool apertada);
        void atualiza(float dt);
        void posicionaCamera();
        void posicionaCameraVitrine();
        void cliqueNaVitrine(int x, int y); // x, y da glutMouseFunc
        void luzes(); // chamar depois da camera
};

#endif
