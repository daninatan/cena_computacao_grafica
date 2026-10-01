// Confere a linha central da pista: g++ -I. testes/teste_pista.cpp ConstrutorDePista.cpp Pista.cpp PistaCurva.cpp -lopengl32
#include "ConstrutorDePista.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    auto linha = ConstrutorDePista::linhaCentral(2);
    const auto& primeiro = linha.front();
    const auto& ultimo = linha.back();

    // A volta fecha: o ultimo ponto fica a um passo do primeiro, na mesma altura.
    assert(std::hypot(ultimo.x - primeiro.x, ultimo.z - primeiro.z) < 2.5f);
    assert(std::fabs(ultimo.y - primeiro.y) < 0.01f);

    // Sem pontos repetidos nem buracos (os carros automaticos andam de ponto em ponto).
    for (size_t i = 0; i + 1 < linha.size(); i++) {
        float d = std::hypot(linha[i + 1].x - linha[i].x, linha[i + 1].z - linha[i].z);
        assert(d > 1.0f && d < 2.5f);
    }
    std::printf("ok: %zu pontos\n", linha.size());
}
