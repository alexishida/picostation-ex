# Changelog

## 1.0.0

O PicoStation EX atualiza o PicoStation original de megavolt85 e corrige problemas no firmware e no menu do PlayStation.

### Drive de CD

- O reset do console solicita a troca de imagem ao núcleo que lê o SD. Esse núcleo encerra a transferência antes de fechar o BIN ao retornar ao menu.
- O cache de setores protege o buffer que o DMA ainda está transmitindo.
- Uma falha transitória de leitura permite reabrir o BIN e repetir a leitura. Falhas persistentes têm um intervalo mínimo de 250 ms entre reaberturas.
- O cálculo do fim do disco conta a pausa da última faixa uma única vez.
- O `PREGAP` sintético de uma faixa de áudio retorna silêncio. Um `INDEX 00` gravado no BIN continua sendo lido do arquivo.
- A busca reversa após atingir o fim do disco usa a zona correspondente à posição do laser emulado.
- A regeneração de setores Mode 2 Form 1 recalcula o ECC mesmo quando o EDC já confere.

### Menu e cartão SD

- O menu exibe a versão 1.0.0, o logo com EX em amarelo e os créditos de Alex Ishida e do projeto original de megavolt85.
- Falhas ao abrir ou ler uma pasta exibem uma mensagem de erro no lugar da listagem anterior ou parcial.
- O menu exibe `No SD Card` como estado. Pressionar X em `Retry` solicita outra listagem e permite montar o cartão após a inserção.
- O menu mantém entradas CUE distintas, inclusive quando os nomes exibidos terminam em `.bin` e `.cue`.
- A leitura do Game ID reconhece a chave `BOOT` inteira em `SYSTEM.CNF` e extrai o nome do executável de caminhos com subpastas.

### Compilação e validação

- O comando `cmake -P scripts/build-release.cmake` gera o pacote local em `release/` com cinco UF2. Os arquivos intermediários ficam em `build/`.
- O pacote contém `picostationex_PU8.uf2`, `picostationex_PU8_OC.uf2`, `picostationex_PU18.uf2`, `picostationex_PU18_OC.uf2` e `pico_booty.uf2`. Os quatro perfis do ODE incorporam a imagem de disco do menu compilado da fonte. O Pico Booty empacota o executável do menu para um segundo Pico na porta de expansão.
- O README, os guias em `docs/` e as especificações OpenSpec documentam o build, a arquitetura e o comportamento do menu.
- O CMake aceita o Pico SDK instalado fora do projeto via `PICO_SDK_PATH` e usa a versão 2.1.1 como padrão quando o download pelo CMake é solicitado. O README descreve a instalação do SDK e o comando de recuperação da listagem no menu.
- Os testes locais cobriram pausas CUE, fim de disco, busca reversa, recuperação de leitura do SD, regeneração de ECC, listagem paginada e Game ID, incluindo os 100 casos da tabela legada.
- A validação em PlayStation físico está pendente para reset durante leitura, transferência DMA, recuperação após falha do SD e reprodução de discos com várias faixas.
