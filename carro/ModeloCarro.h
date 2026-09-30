#pragma once
// =====================================================================================
//  INTERFACE de um tipo de carro.
//
//  Um ModeloCarro é o SRM do carro INTEIRO: sabe desenhar o carro em volta da
//  própria origem, mas não sabe onde ele está no mundo nem como se move.
//  Quem coloca ele no universo (SRU) e dá movimento é o Veiculo.
//
//  Convenção do SRM de todo modelo:
//    origem = centro do carro, no chão
//    x = comprimento (frente em +x)   y = altura   z = largura (esquerda em -z, direita em +z)
//
//  Pra criar um tipo novo de carro: herde de ModeloCarro, implemente desenhar() e
//  entreEixos() usando as primitivas de Primitivas.h, e passe pro Veiculo.
// =====================================================================================

namespace carros {

// O que muda no visual conforme o carro funciona. O Veiculo altera, o modelo só lê.
struct EstadoCarro {
    float anguloDirecao = 0;     // graus que as rodas da frente estão viradas (+ = esquerda)
    float anguloDirecaoTras = 0; // graus das rodas de trás (esterço nas 4 rodas)
    float anguloPortas  = 0;     // graus de abertura das portas
    bool  luzesLigadas  = true;
    float distancia     = 0;     // quanto o carro já andou (negativo = ré): faz as rodas rolarem
};

class ModeloCarro {
public:
    virtual ~ModeloCarro() {}

    // SRM do carro: desenha o carro inteiro. É chamado 2 vezes por frame
    // (passadaDoVidro = false e depois true), as primitivas se viram com isso.
    virtual void desenhar(const EstadoCarro& estado) = 0;

    // Fontes de luz do carro (faróis), com posição no SRM. Opcional.
    virtual void luzes(const EstadoCarro&) {}

    // Distância entre o eixo da frente e o de trás: o Veiculo usa pra calcular a curva.
    virtual float entreEixos() const = 0;
};

} // namespace carros
