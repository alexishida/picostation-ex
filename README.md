# PicoStation EX

Versão: **1.0.0**

O PicoStation EX é um emulador de unidade óptica (ODE) para o PlayStation original, baseado no [PicoStation de megavolt85](https://github.com/megavolt85/PicoStation). O projeto foi criado para atualizar o PicoStation original e corrigir bugs. O firmware roda em Raspberry Pi Pico ou Pico 2, lê imagens de disco em cartão SD e apresenta um menu no console para navegar e iniciar jogos.

## Compatibilidade

O projeto contempla as placas PU-8 (SCPH-100x) e PU-18 (SCPH-55xx). Escolha o firmware de acordo com a placa e a montagem. Os quatro perfis de release do ODE usam RP2040 e o pinout padrão. Os perfis `_OC` usam um clock mais alto para o Mechacon.

| Arquivo de release | Montagem |
| --- | --- |
| `picostationex_PU8.uf2` | PU-8 |
| `picostationex_PU8_OC.uf2` | PU-8 com overclock |
| `picostationex_PU18.uf2` | PU-18 |
| `picostationex_PU18_OC.uf2` | PU-18 com overclock |
| `pico_booty.uf2` | Segundo RP2040 na porta de expansão |

Os perfis de release fornecem o clock do Mechacon pelo GP3. A instalação deve seguir o esquema da placa PU-8 ou PU-18 correspondente, com o clock original isolado quando GP3 for conectado ao Mechacon. Consulte os clocks e detalhes em [compilação e releases](docs/build.md).

## Gerar o pacote de release

Com as ferramentas descritas no [guia de build](docs/build.md) instaladas, execute na raiz do projeto:

```sh
cmake -P scripts/build-release.cmake
```

Os cinco arquivos finais ficam em `release/`; os arquivos intermediários ficam em `build/`. Se `release/` contiver algum outro arquivo, mova-o antes de gerar o pacote. O script preserva itens inesperados e interrompe a operação.

## Comece aqui

1. Organize os arquivos `.cue` e `.bin` no cartão SD, recomendado em exFAT; as imagens devem usar setores raw de 2352 bytes.
2. Gere o pacote e escolha em `release/` o firmware correspondente à placa e à montagem.
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

PicoStation EX: Alex Ishida. O projeto original PicoStation é de megavolt85. O projeto é distribuído sob a [licença Apache 2.0](LICENSE). O pacote Pico Booty usa código do [fork de Nicolas Noble](https://github.com/nicolasnoble/pico_booty/tree/ea96b7fcd4196d666bc90340799bbacba50d3b31) do projeto de John Baumann; outras dependências mantêm seus próprios arquivos de licença em `third_party/`.
