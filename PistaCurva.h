#ifndef PISTACURVA_H
#define PISTACURVA_H

class PistaCurva {
private:
    float Bx[4], By[4], Bz[4];

    float calculaPonto(char coordenada, double t);

public:
    void desenha(float raio, float largura, float altura, char direcao);
};

#endif
