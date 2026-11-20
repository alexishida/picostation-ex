# Regras para IA no PicoStation EX

Este documento é a referência oficial para trabalhar no repositório. O PicoStation EX 1.0.0 é um emulador de unidade óptica (ODE) para o PlayStation original, desenvolvido por Alex Ishida a partir do PicoStation de megavolt85 para atualizar o projeto e corrigir bugs. O firmware atende montagens em Raspberry Pi Pico e Pico 2. O [README](../README.md) descreve as placas PU-8 (SCPH-100x) e PU-18 (SCPH-55xx) e recomenda cartão SD em exFAT.

Confira o código e o CMake para determinar o comportamento do checkout. Ao alterar um contrato do projeto, mantenha coerentes a [documentação](../docs/README.md) e as [especificações principais](../openspec/specs/). Os artefatos arquivados no OpenSpec registram o estado da época.

## Código e arquitetura

- Preserve a organização em C++20 e C. Mantenha as implementações em [`src/`](../src/), os cabeçalhos em [`include/`](../include/) e os programas PIO em [`pio/main.pio`](../pio/main.pio). Siga o [`.clang-format`](../.clang-format) nos trechos alterados.
- Respeite a divisão entre os núcleos. O núcleo 0 coordena a mecânica, o reset e os sinais do drive. O núcleo 1 processa a leitura do SD, o atendimento ao menu e o envio de setores pelo caminho I²S/DMA. Preserve a sincronização existente e considere a latência de IRQs e funções marcadas com `__time_critical_func`. [Inicialização](../src/main.cpp) · [Controle do drive](../src/drive/picostation.cpp) · [Leitura e transferência](../src/hardware/i2s.cpp)
- Ao adicionar GPIOs, confira as variantes básicas e os perfis de release em [`boards/`](../boards/) e mantenha o pinout no arquivo CMake correspondente. O cabeçalho `picostation_pinout.h` é gerado a partir de [`picostation_pinout.h.in`](../boards/picostation_pinout.h.in).
- Preserve a compatibilidade entre o firmware e o menu ao alterar comandos, estados compartilhados ou dados em setores. Confira o [protocolo compartilhado](../include/menu_protocol.h) e o [design do menu](DESIGN.md). Compile o par a partir dos fontes e valide separadamente qualquer imagem precompilada que pretenda usar.
- Reutilize a infraestrutura existente para FatFs, leitura de imagens, interpretação de CUE e sincronização entre núcleos. Mantenha dependências e abstrações novas ligadas a uma necessidade concreta.

## Build e validação

Quando o usuário pedir para gerar o release, gere o pacote local na pasta `release/`. Publique uma release no GitHub somente se o usuário pedir explicitamente essa publicação. Um pedido de commit e push junto com a geração do pacote autoriza apenas o envio do código e da documentação ao remoto.

O build padrão do PicoStation EX gera o pacote completo. Execute o comando na raiz do projeto com as ferramentas configuradas.

```sh
cmake -P scripts/build-release.cmake
```

O pacote compila o menu a partir dos fontes e requer CMake 3.25+, Pico SDK 2.1.1, GNU Arm `arm-none-eabi`, GNU MIPS, Python 3.10+ com NumPy e Pillow, `mkpsxiso` e `ps1-packer`. Consulte o [guia de build](../docs/build.md) para configurar as ferramentas.

Os builds intermediários ficam em `build/`. Ao concluir, `release/` deve conter somente estes cinco arquivos.

- `picostationex_PU8.uf2`
- `picostationex_PU8_OC.uf2`
- `picostationex_PU18.uf2`
- `picostationex_PU18_OC.uf2`
- `pico_booty.uf2`

Itens inesperados em `release/` interrompem o empacotamento e são preservados. Os quatro perfis de ODE usam o RP2040, o pinout padrão e o clock do Mechacon em GP3. O Pico Booty usa um segundo RP2040 na porta de expansão.

- Para alterações em código compartilhado ou no empacotamento, execute o script de release e confira os cinco arquivos finais.
- Alterações de pinout, temporização, leitura do SD ou protocolo com o menu também pedem validação no PlayStation com a placa e a variante afetadas. Registre separadamente o que foi compilado, o que foi testado no console e o que ficou pendente. [Cenários de validação](../docs/validacao.md)
- Para mudanças nas especificações, rode `openspec validate --all --strict`.

## Segurança das operações

- Nunca reordene os pinos já definidos no PicoStation nem reatribua seus GPIOs. Preserve a ordem e o mapeamento existentes em cada variante.
- Use o ambiente local ou a CI para validação. Quando uma ação em produção for necessária, documente o comando e oriente a execução pelo responsável.
- Solicite confirmação explícita antes de ações destrutivas ou irreversíveis, como formatar um cartão SD, e explique o impacto esperado.
