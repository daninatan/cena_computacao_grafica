#ifndef PISTACURVA_H
#define PISTACURVA_H

class PistaCurva {
private:
    float Bx[4], By[4], Bz[4];

    float calculaPonto(char coordenada, double t);
    void calculaSecao(float t, float centro[3], float lateral[3]);
    void colocaVertice(float centro[3], float lateral[3],
                       float distancia, float altura);
    void desenhaFaixa(float distanciaA, float alturaA,
                      float distanciaB, float alturaB, char normal);
    void desenhaTampa(float t, float largura, float altura, int sentido);

public:
    void desenha(float raio, float largura, float altura, char direcao);
};

#endif
