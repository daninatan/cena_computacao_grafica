# Modelos de veículos

Todos os modelos herdam de `ModeloCarro`, usam `Primitivas.h` e podem ser
passados ao `Veiculo`. A origem fica no chão, com a frente em **+x**, altura
em **y** e largura em **z**. As medidas seguem a escala dos modelos existentes.

## Novos modelos

| Modelo | Visual | Entre-eixos | Tecla na cena |
| --- | --- | --- | --- |
| `CarroHatch` | Compacto turquesa, duas portas, teto preto e traseira curta | 2,24 | `8` |
| `CarroConversivel` | Clássico vinho, dois lugares, interior caramelo e capota recolhida | 2,75 | `9` |
| `CarroJipe` | Verde oliva, cabine aberta, gaiola, meia-porta, guincho e estepe | 2,40 | `0` |
| `CarroMonstro` | Monster truck roxo, pneus gigantes com garras, molas e eixos expostos | 3,30 | `F1` |
| `CarroPerua` | Quatro portas, azul-petróleo, painéis de madeira e bagageiro | 3,05 | `F2` |
| `CarroHotRod` | Cupê laranja, teto baixo, V8 exposto e pneus traseiros maiores | 2,80 | `F3` |

Os seis respondem a todos os campos de `EstadoCarro`: esterço dianteiro e
traseiro, distância para girar os pneus, abertura das portas e luzes.
Os vidros respeitam as duas passadas de desenho do `Veiculo`.
A capota do conversível, o guincho e o estepe são detalhes fixos.
No monster truck, as garras giram junto com os pneus e a rolagem considera
o raio maior. No hot rod, cada eixo usa o raio de sua própria roda.
As molas do monster truck são visuais; a física continua sendo a do `Veiculo`.

![Hatch, conversível e jipe: frente, traseira e portas abertas](previas_novos_modelos.png)

![Monster truck, perua e hot rod: frente, traseira e portas abertas](previas_monstro_perua_hotrod.png)

## Executar a cena

Na raiz do projeto, execute `build.bat carro_of` (MSYS2 UCRT64 com freeglut,
conforme o script existente). O projeto usa C++17 ou posterior.

- `1` sedã, `2` SUV, `3` van, `4` caminhão, `5` esportivo, `6` picape, `7` Fusca.
- `8` hatch, `9` conversível, `0` jipe; o título da janela identifica o selecionado.
- `F1` monster truck, `F2` perua, `F3` hot rod; `Tab` percorre os 13 veículos.
- `W/S` acelerar/frear/ré, `A/D` direção, `P` portas, `L` faróis, `N` dia/noite.
- Arrastar com o botão esquerdo ou usar as setas gira a câmera; a roda do mouse controla o zoom.

## Usar em outra cena

```cpp
#include "carro/CarroHatch.h"
#include "carro/Veiculo.h"

carros::CarroHatch modelo; // deve existir enquanto o Veiculo o utilizar
carros::Veiculo carro(modelo, 0, 0, 0);
```

Após posicionar a câmera, chame `carro.luzes()` antes do cenário e
`carro.desenhar()` depois dos objetos opacos. Encaminhe o teclado para
`carro.tecla(tecla, apertada)` e o tempo em segundos para `carro.atualizar(dt)`.
O mesmo uso vale para os demais modelos, incluindo o header correspondente.
