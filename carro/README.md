# Modelo de veículo

`CarroSedan` herda de `ModeloCarro`, usa `Primitivas.h` e é passado ao
`Veiculo`. A origem fica no chão, com a frente em **+x**, altura em **y** e
largura em **z**.

## Usar em uma cena

```cpp
#include "carro/CarroSedan.h"
#include "carro/Veiculo.h"

carros::CarroSedan modelo; // deve existir enquanto o Veiculo o utilizar
carros::Veiculo carro(modelo, 0, 0, 0);
```

Após posicionar a câmera, chame `carro.luzes()` antes do cenário e
`carro.desenhar()` depois dos objetos opacos. Encaminhe o teclado para
`carro.tecla(tecla, apertada)` e o tempo em segundos para `carro.atualizar(dt)`.

- `W/S` acelerar/frear/ré, `A/D` direção, `P` portas, `L` faróis.
