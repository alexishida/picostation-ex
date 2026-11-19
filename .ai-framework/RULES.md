# Regras para IA no PicoStation

Este documento é a referência oficial para trabalhar no repositório. O PicoStation é um emulador de unidade óptica (ODE) para o PlayStation original, implementado em Raspberry Pi Pico e Pico 2. O [README](../README.md) lista as placas PU-8 (SCPH-100x) e PU-18 (SCPH-55xx) e recomenda um cartão SD em exFAT. Confira o firmware e a CI para determinar o comportamento e a configuração de build atuais.

## Código e arquitetura

- Preserve a organização em C++20 e C. Mantenha as implementações em [`src/`](../src/), os cabeçalhos em [`include/`](../include/) e os programas PIO em [`pio/main.pio`](../pio/main.pio). Siga o [`.clang-format`](../.clang-format) nos trechos alterados.
- Respeite a divisão entre os núcleos. O núcleo 0 coordena a mecânica, o reset e os sinais do drive. O núcleo 1 processa a leitura do SD, o atendimento ao menu e o envio de setores pelo caminho I²S/DMA. Preserve a sincronização existente e considere a latência de IRQs e funções marcadas com `__time_critical_func`. [Inicialização](../src/main.cpp) · [Controle do drive](../src/drive/picostation.cpp) · [Leitura e transferência](../src/hardware/i2s.cpp)
- Ao adicionar GPIOs, confira as quatro variantes em [`boards/`](../boards/) e mantenha o pinout no arquivo CMake correspondente. O cabeçalho `picostation_pinout.h` é gerado a partir de [`picostation_pinout.h.in`](../boards/picostation_pinout.h.in).
- Preserve a compatibilidade entre o firmware e o menu embarcado ao alterar comandos, estados compartilhados e dados enviados em setores. Use o [design do menu](DESIGN.md) como referência e valide a integração com o binário incluído.
- Reutilize a infraestrutura existente para FatFs, leitura de imagens, interpretação de CUE e sincronização entre núcleos. Mantenha dependências e abstrações novas ligadas a uma necessidade concreta.

## Build e validação

O [CMake do projeto](../CMakeLists.txt) requer CMake 3.24.1 ou superior. O build padrão do menu requer CMake 3.25, uma toolchain GNU MIPS, Python e `mkpsxiso`. A configuração do projeto indica o Pico SDK 2.1.1 e usa a toolchain GNU Arm `arm-none-eabi` para o firmware. Inicialize os submódulos com `git submodule update --init --recursive` ao preparar um checkout para compilar.

| Variante (`PICOSTATION_VARIANT`) | Microcontrolador |
| --- | --- |
| `picostation_pico1` | RP2040 |
| `picostation_pico2` | RP2350 |
| `picostation_plus_pico1` | RP2040 |
| `picostation_plus_pico2` | RP2350 |

Selecione a variante explicitamente e use um diretório de build por variante. Exemplo com o Pico SDK em `third_party/pico-sdk/`.

```sh
cmake -S . -B build/picostation_pico2 -DPICOSTATION_VARIANT=picostation_pico2
cmake --build build/picostation_pico2
```

- Confira o `.uf2` das variantes afetadas. Para alterações em código compartilhado, compile as quatro variantes listadas acima.
- Alterações de pinout, temporização, leitura do SD ou protocolo com o menu também pedem validação no PlayStation com a placa e a variante afetadas. Registre quais cenários foram exercitados e quais ficaram pendentes.

## Segurança das operações

- Nunca reordene os pinos já definidos no PicoStation nem reatribua seus GPIOs. Preserve a ordem e o mapeamento existentes em cada variante.
- Use o ambiente local ou a CI para validação. Quando uma ação em produção for necessária, documente o comando e oriente a execução pelo responsável.
- Solicite confirmação explícita antes de ações destrutivas ou irreversíveis, como formatar um cartão SD, e explique o impacto esperado.
