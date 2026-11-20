# Spec Delta

## Purpose

Registrar os modos de compilação e os artefatos distribuíveis do PicoStation EX, preservando a seleção de hardware e do menu embarcado.

## ADDED Requirements

### Requirement: Seleção da variante de hardware
O build SHALL aceitar as variantes `picostation_pico1`, `picostation_pico2`, `picostation_plus_pico1` e `picostation_plus_pico2`, preservando seus mapas de GPIO e produzindo um UF2 nomeado pela variante.

#### Scenario: Compilação para Pico ou Pico 2
- **WHEN** uma variante é selecionada com as dependências disponíveis
- **THEN** variantes pico1 usam RP2040 e variantes pico2 usam RP2350
- **AND** a saída é `<variante>.uf2` no diretório de build escolhido.

### Requirement: Menu compilado ou precompilado
O build SHALL compilar e incorporar o menu por padrão e SHALL aceitar uma imagem existente quando `PICOSTATION_BUILD_MENU=OFF` e `PICOSTATION_MENU_IMAGE` indicar seu caminho.

#### Scenario: Menu da fonte
- **WHEN** o build padrão é executado com CMake 3.25+, SDK e toolchains Arm/MIPS, Python 3.10+ e mkpsxiso disponíveis
- **THEN** o executável MIPS é convertido em imagem de disco e incorporado ao firmware Arm.

#### Scenario: Imagem precompilada ausente
- **WHEN** o modo precompilado recebe um caminho inexistente ou um diretório
- **THEN** a configuração falha com indicação de imagem de menu não encontrada.

### Requirement: Pacote de release PU-8 e PU-18
O empacotamento SHALL gerar `picostationex_PU8.uf2`, `picostationex_PU8_OC.uf2`, `picostationex_PU18.uf2`, `picostationex_PU18_OC.uf2` e `pico_booty.uf2` na raiz de `build/` quando todas as ferramentas e fontes necessárias estiverem disponíveis.

#### Scenario: Perfis RC5
- **WHEN** os quatro perfis ODE do pacote são compilados
- **THEN** todos usam RP2040 e o pinout padrão com clock Mechacon em GP3
- **AND** os clocks aproximados são respectivamente 3,988 MHz, 6,457 MHz, 4,238 MHz e 7,533 MHz.

#### Scenario: Menu na porta de expansão
- **WHEN** o pacote gera o Pico Booty com ps1-packer disponível
- **THEN** o executável do menu produzido pelo perfil PU8 é empacotado para um segundo RP2040 na porta de expansão.
