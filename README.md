# PicoStation EX

O PicoStation EX é um emulador de unidade óptica (ODE) para o PlayStation original, baseado no [PicoStation de megavolt85](https://github.com/megavolt85/PicoStation). O firmware roda em Raspberry Pi Pico ou Pico 2, lê imagens de disco em cartão SD e apresenta um menu no console para navegar e iniciar jogos.

## Compatibilidade

O projeto contempla as placas PU-8 (SCPH-100x) e PU-18 (SCPH-55xx). Escolha o firmware de acordo com a placa e a montagem. As variantes PicoStation e PicoStation Plus possuem mapas de pinos próprios em [boards/](boards/).

| Variante | Microcontrolador |
| --- | --- |
| `picostation_pico1` | RP2040 |
| `picostation_pico2` | RP2350 |
| `picostation_plus_pico1` | RP2040 |
| `picostation_plus_pico2` | RP2350 |

O pacote de release também oferece perfis RC5 para PU-8 e PU-18, com ou sem overclock, e um firmware Pico Booty para a porta de expansão. Consulte os detalhes de instalação e clocks em [compilação e releases](docs/build.md).

## Comece aqui

1. Organize os arquivos `.cue` e `.bin` no cartão SD, recomendado em exFAT; as imagens devem usar setores raw de 2352 bytes.
2. Escolha a variante correspondente ao hardware e compile o firmware seguindo o [guia de build](docs/build.md).
3. Use o [guia do menu](docs/menu.md) para navegar, iniciar discos e configurar capas e cores.

## Documentação

- [Índice da documentação](docs/README.md)
- [Compilação, variantes e pacote de release](docs/build.md)
- [Cartão SD e menu do PlayStation](docs/menu.md)
- [Arquitetura e protocolo](docs/arquitetura.md)
- [Validação e solução de problemas](docs/validacao.md)
- [Especificações OpenSpec](openspec/specs/)
- [Regras de desenvolvimento](.ai-framework/RULES.md) e [design do menu](.ai-framework/DESIGN.md)

## Licença e créditos

O projeto é distribuído sob a [licença Apache 2.0](LICENSE). O firmware deriva do PicoStation de megavolt85. O pacote Pico Booty usa código do [fork de Nicolas Noble](https://github.com/nicolasnoble/pico_booty/tree/ea96b7fcd4196d666bc90340799bbacba50d3b31) do projeto de John Baumann; outras dependências mantêm seus próprios arquivos de licença em `third_party/`.
