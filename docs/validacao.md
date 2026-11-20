# Validação e solução de problemas

## Verificações de build

Use um diretório de build por variante e confira a geração do UF2 correspondente. Para alterações em código compartilhado, as regras do projeto pedem compilação das quatro variantes básicas. Alterações em pinout, temporização, leitura do SD ou protocolo do menu também exigem validação no PlayStation com a placa e a variante afetadas. Registre modelo do console, revisão de placa, variante, cartão, imagem usada e resultado.

A qualificação de release pode abranger os quatro perfis PU-8 e PU-18 e o pacote Pico Booty descritos no [guia de build](build.md). A presença das especificações OpenSpec não demonstra que os testes foram executados.

## Cenários para testar no console

- Navegação com pasta vazia, nomes longos, listagem paginada e retorno ao diretório pai.
- Cartão ausente ou removido, reinserção e nova tentativa com Retry.
- CUE/BIN válido Mode 1 e Mode 2, disco somente de áudio, BIN ausente, arquivo truncado e falha de leitura.
- Boot rápido e normal de jogos reconhecidos, incluindo identificação de Game ID.
- Capas indexadas e RGB555, entradas sem capa e `config.ini` presente ou ausente.
- Para cada perfil relevante, reset, sinais e clock da montagem PU-8 ou PU-18 correspondente.

## Diagnóstico

| Sintoma | Verificação inicial |
| --- | --- |
| CMake não encontra o SDK | Defina `PICO_SDK_PATH` ou instale o SDK em `third_party/pico-sdk`; veja [build](build.md). |
| Menu não compila | Confira CMake 3.25+, toolchain MIPS, Python e `mkpsxiso`; para o pacote, confira também `ps1-packer`. |
| Configuração rejeita o menu precompilado | Confira se `PICOSTATION_MENU_IMAGE` aponta para um arquivo existente, não para um diretório. |
| Cartão ou listagem indisponível | Confira cartão, conexão e formato; tente Retry após reinseri-lo. |
| Jogo não abre | Confira caminhos no CUE, arquivos BIN e setores raw de 2352 bytes. A validação não detecta toda forma de truncamento. |
| Capa ou cores não aparecem | Confira nome e localização do `.cov`, tamanho/formato da capa e `config.ini` na raiz. |

Não há resultados de builds ou testes físicos registrados por esta documentação. As especificações podem ser verificadas com `openspec validate --all --strict` quando a ferramenta estiver instalada.
