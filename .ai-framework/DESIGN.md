# Design do menu do PicoStation

O menu roda no PlayStation e serve para navegar pelo cartão SD e iniciar jogos. O build padrão compila o código em [`src/menu/`](../src/menu/) e incorpora a imagem de disco gerada ao firmware. O firmware fornece a listagem, as capas e a configuração de cores. [Integração no CMake](../CMakeLists.txt) · [Instruções de build](../README.md#compilação-do-firmware)

## Navegação e estados

- O navegador começa na raiz do cartão, permite entrar em pastas, subir ao diretório pai e voltar à raiz. A listagem é paginada e inclui pastas e entradas CUE visíveis. O firmware envia os nomes dos jogos sem a extensão `.cue`.
- X abre a pasta ou a janela de opções do jogo. Fast Boot e Regular Boot iniciam a montagem do CUE selecionado. Círculo fecha a janela ou sobe ao diretório pai. Select abre os créditos.
- A janela de boot permite alterar a correção EDC e o Bootsector Fix. O firmware usa um clock fixo e o menu ainda não executa cheats.
- Mantenha legíveis e distinguíveis os estados de carregamento, pasta vazia e cartão ausente. O firmware envia a indicação `No SD Card` na listagem quando o cartão está indisponível.

Referências locais da navegação e da montagem. [Listagem do SD](../src/storage/directory_listing.cpp) · [Formato da listagem](../include/listingBuilder.h) · [Montagem e transferência](../src/hardware/i2s.cpp)

## Cores e capas

| Recurso | Comportamento atual |
| --- | --- |
| Esquema de cores | O arquivo opcional `config.ini` fica na raiz do cartão. O menu interpreta até 2.048 bytes e aceita as chaves `background`, `frame` e `filelist` com cores RGB hexadecimais. |
| Capa do jogo | Use o mesmo nome base e a mesma pasta do CUE. Por exemplo, `Jogos/Jogo.cue` usa `Jogos/Jogo.cov`. |
| Capa de uma pasta | Use um arquivo ao lado da pasta com a extensão `.cov`. Por exemplo, a pasta `Jogos/RPG/` usa `Jogos/RPG.cov`. |
| Formato de capa | O menu lê capas de 128 × 128 pixels em RGB555 de 16 bits little-endian e capas indexadas de 8 bits com paleta de 512 bytes. O firmware envia a capa RGB555 em 16 partes de 2.048 bytes com o marcador `RGB5` no fim do setor. A capa indexada usa oito partes de até 2.114 bytes. |
| Arquivo opcional ausente | O firmware sinaliza ausência de capa ou configuração com `DEAD`. Preserve o uso do menu sem esses arquivos. |

O contrato de transporte inclui os comandos `COMMAND_GET_COVER` e `EXTENDED_GET_CFG` e os cabeçalhos `PART` e `CFG1`. Confira a compatibilidade com o binário do menu se alterar comandos, tamanhos ou setores reservados. [Comandos](../include/cmd.h) · [Leitura dos arquivos](../src/storage/directory_listing.cpp) · [Setores do menu](../src/hardware/i2s.cpp)

## Diretrizes para alterações visuais

- Preserve a navegação pelo controle do PlayStation e mantenha o foco da seleção fácil de identificar durante a rolagem e a troca de página. Diferencie pastas e jogos sem depender só da cor ou da presença de uma capa.
- Priorize a leitura dos nomes, dos itens selecionados e das mensagens de estado. Reserve margem para as bordas cortadas em TVs com overscan e confira o contraste com o esquema padrão e com esquemas personalizados.
- Valide alterações visuais no console com nomes longos, pastas vazias, listagem paginada, cartão ausente e entradas com e sem capa. Use o comportamento da versão embarcada como referência para documentar cores, espaçamentos e ações do controle.

## Falha ao abrir um jogo

O menu envia `MENU_PREPARE_COMMAND` antes de montar o jogo e lê a resposta `MNT1` em `MENU_MOUNT_STATUS_LBA`. A resposta inclui o índice solicitado e o resultado FatFs em dois campos de 16 bits little-endian. O firmware mantém a imagem do menu ativa enquanto prepara o CUE e valida os BINs. A montagem confirma a imagem preparada pelo mesmo índice.

A validação exige setores raw de 2352 bytes e verifica o descritor ISO9660 se a primeira faixa for de dados. A resposta `MNT1` indica quando o disco só tem faixas de áudio. Depois de montar o disco, o menu usa a assinatura PlayStation para decidir se o Fast Boot está disponível. Discos de áudio e discos de dados sem essa assinatura seguem a inicialização normal. Falhas exibem uma mensagem por 240 quadros e mantêm a seleção. Se a leitura após a montagem falhar, `MENU_RETURN_COMMAND` restaura a imagem do menu. As esperas de interrupção e de disponibilidade do controlador CD têm limite aproximado de cinco segundos por etapa.

Para jogos, o menu lê o caminho de boot em `SYSTEM.CNF` e envia o Game ID às duas portas de cartão de memória antes de reiniciar. Quando o caminho aponta para um executável genérico `PSX.EXE`, a data de criação do volume localiza o ID na tabela recuperada de `binary/picostation-menu.bin`. [Código do Game ID](../src/menu/menu_game_id.c)

Firmware e menu devem ser compilados juntos para usar esse protocolo. Valide em hardware imagens válidas Mode 1/2, CUE com BIN ausente, BIN truncado e falha de leitura do cartão, incluindo nova tentativa após o erro.
