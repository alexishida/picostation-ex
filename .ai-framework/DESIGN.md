# Design do menu do PicoStation EX

O menu 1.0.0 roda no PlayStation e permite navegar pelo cartão SD e iniciar jogos. O firmware fornece a listagem, as capas e a configuração de cores. No Pico, o núcleo 0 trata a mecânica e os sinais do leitor, enquanto o núcleo 1 lê o SD, atende ao menu e envia setores por I²S com PIO e DMA. [Arquitetura e protocolo](../docs/architecture.md)

## Build e apresentação

- O build padrão compila o menu em MIPS, gera o executável `picostation-menu.psexe` e cria a imagem de disco incorporada ao firmware Arm. O Pico Booty empacota o executável do menu gerado no build PU-8 para a porta de expansão. [CMake do menu](../cmake/menu/CMakeLists.txt) · [Guia de build](../docs/build.md)
- O logo preserva a identidade visual do PicoStation e acrescenta **EX** em amarelo. O PNG usado pelo menu mede 128 × 20 pixels e o build o converte em uma textura indexada de 4 bits com paleta. O código de exibição usa essas dimensões. [Logo do menu](../assets/menu/images/picostationlogo.png) · [Código da interface](../src/menu/main.c)
- A tela de créditos exibe `PicoStation EX 1.0.0`, `Alex Ishida` e `Original project by megavolt85`. Ao atualizar a versão, mantenha coerentes os créditos, o CMake do firmware e do menu e o README.

## Navegação e estados

- O navegador começa na raiz do cartão, permite entrar em pastas, subir ao diretório pai e voltar à raiz. A listagem é paginada e inclui pastas e entradas CUE visíveis. O firmware envia os nomes dos jogos sem a extensão `.cue`.
- Cima e baixo mudam a seleção, e L1 e R1 mudam de página. X abre a pasta ou a janela de opções do jogo. Fast Boot e Regular Boot iniciam a montagem do CUE selecionado. Círculo fecha a janela ou sobe ao diretório pai. Select abre os créditos.
- A janela de boot permite alterar a correção EDC e o Bootsector Fix. O firmware usa um clock fixo e o menu ainda não executa cheats.
- O terminador da listagem indica continuação, fim, cartão ausente, falha de leitura ou falha ao abrir uma pasta. O menu exibe o erro e permite tentar novamente com X. Se o cartão estiver indisponível, o firmware tenta montá-lo ao receber uma nova solicitação de listagem.

Referências locais da navegação e da montagem. [Listagem do SD](../src/storage/directory_listing.cpp) · [Formato da listagem](../include/listingBuilder.h) · [Montagem e transferência](../src/hardware/i2s.cpp)

## Cores e capas

| Recurso | Comportamento atual |
| --- | --- |
| Esquema de cores | O arquivo opcional `config.ini` fica na raiz do cartão. O menu interpreta até 2.048 bytes e aceita as chaves `background`, `frame` e `filelist` com cores RGB hexadecimais. |
| Capa do jogo | Use o mesmo nome base e a mesma pasta do CUE. Por exemplo, `Jogos/Jogo.cue` usa `Jogos/Jogo.cov`. |
| Capa de uma pasta | Use um arquivo ao lado da pasta com a extensão `.cov`. Por exemplo, a pasta `Jogos/RPG/` usa `Jogos/RPG.cov`. |
| Formato de capa | O menu lê capas de 128 × 128 pixels em RGB555 de 16 bits little-endian e capas indexadas de 8 bits com paleta de 512 bytes. O firmware envia a capa RGB555 em 16 partes de 2.048 bytes com o marcador `RGB5` no fim do setor. A capa indexada usa oito partes de até 2.114 bytes. |
| Arquivo opcional ausente | O firmware sinaliza ausência de capa ou configuração com `DEAD`. Preserve o uso do menu sem esses arquivos. |

O firmware responde a `COMMAND_GET_COVER` com partes marcadas por `PART` a partir do LBA 200 e a `EXTENDED_GET_CFG` com `CFG1` no LBA 150. Confira a compatibilidade entre firmware e menu ao alterar comandos, tamanhos ou setores reservados. [Protocolo compartilhado](../include/menu_protocol.h) · [Comandos](../include/cmd.h) · [Setores do menu](../src/hardware/i2s.cpp)

## Diretrizes para alterações visuais

- Preserve a navegação pelo controle do PlayStation e mantenha o foco da seleção fácil de identificar durante a rolagem e a troca de página. Diferencie pastas e jogos sem depender só da cor ou da presença de uma capa.
- Priorize a leitura dos nomes, dos itens selecionados e das mensagens de estado. Reserve margem para as bordas cortadas em TVs com overscan e confira o contraste com o esquema padrão e com esquemas personalizados.
- Valide alterações visuais no console com nomes longos, pastas vazias, listagem paginada, cartão ausente e entradas com e sem capa. Use o comportamento da versão embarcada como referência para documentar cores, espaçamentos e ações do controle.

## Preparação, montagem e boot

O menu envia `MENU_PREPARE_COMMAND` (`0xB`) antes de montar o jogo e lê a resposta `MNT1` no LBA 110. A resposta inclui o índice solicitado e o resultado FatFs em dois campos de 16 bits little-endian e indica disco somente de áudio no byte de flags. O firmware mantém a imagem do menu ativa enquanto prepara o CUE e valida os BINs. O menu só confirma a montagem quando a resposta tem o marcador correto, o mesmo índice e resultado de sucesso.

A preparação exige arquivos de faixa acessíveis e não comprimidos, com tamanho positivo e múltiplo de 2352 bytes. O firmware verifica o descritor ISO9660 no setor 16 da primeira faixa quando ela é de dados. Essas verificações podem deixar passar truncamentos alinhados a setores. Depois da montagem, o menu usa a assinatura PlayStation para decidir se o Fast Boot está disponível. Discos de áudio e discos de dados sem essa assinatura seguem a inicialização normal. Falhas de preparação ou leitura exibem uma mensagem por 240 quadros e mantêm a seleção. Se a leitura da assinatura falhar após a montagem, `MENU_RETURN_COMMAND` (`0xC`) restaura a imagem do menu.

Para jogos reconhecidos, o menu procura o caminho de boot em `SYSTEM.CNF` e envia o Game ID às duas portas de cartão de memória quando consegue identificá-lo. Quando o caminho aponta para um executável genérico `PSX.EXE`, o menu consulta a tabela de IDs pela data de criação do volume. [Código do Game ID](../src/menu/menu_game_id.c)

Valide o firmware e o menu juntos no console com imagens válidas Mode 1 e Mode 2, disco somente de áudio, CUE com BIN ausente, BIN truncado e falha de leitura do cartão seguida de nova tentativa. [Cenários de validação](../docs/validation.md)
