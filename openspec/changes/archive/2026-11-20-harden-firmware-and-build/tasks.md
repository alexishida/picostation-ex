# Tasks

## 1. Sincronização da imagem e do sensor GFS

- [x] 1.1 Criar o caminho comum de publicação da imagem e do limite, mantendo a preparação da imagem candidata fora da pausa. Sincronizar todos os acessos a `c_sectorMax` e verificar com `rg -n c_sectorMax src include` que a mecânica usa uma leitura do limite por operação.
- [x] 1.2 Coordenar a pausa, a invalidação do alarme de subcode pendente e o reset da mecânica antes de retomar. Conferir os caminhos de inicialização, publicação e reset; a verificação integrada no console está na tarefa 1.4.
- [x] 1.3 Publicar o espelho de GFS em `setSens` e usar o accessor sincronizado no modchip e no diagnóstico do núcleo 1. Compilar com o diagnóstico de boot ligado e desligado; verificar o boot no console na tarefa 1.4.
- [x] 1.4 Validar boot, leitura, seek, montagem confirmada e direta, troca entre imagens de tamanhos diferentes, retorno ao menu, falha de preparação e navegação em pastas vazias e paginadas em PU-8 e PU-18. Verificar que uma troca malsucedida preserva a imagem ativa e que nenhum setor antigo recebe dados Q da nova imagem. Testar reset durante a troca, medir a latência nos perfis com overclock e registrar em `docs/validation.md` as variantes compiladas, as verificadas no console e as pendências.

## 2. Contagem da listagem

- [x] 2.1 Remover o incremento extra de `addTerminator`. Verificar o layout serializado, os códigos de status e a contagem de uma página que ocupe exatamente `LISTING_SIZE`; confirmar a navegação no console na tarefa 1.4.

## 3. Otimização do build de release

- [x] 3.1 Construir o menu no perfil PU8 e usar seu BIN nos demais ODEs e seu PS-X EXE no Pico Booty. Confirmar em build limpo que há uma única compilação do menu e que todos os consumidores usam os artefatos produzidos nela.
- [x] 3.2 Adicionar `--parallel` às chamadas de `cmake --build` do script e medir a duração do pacote antes e depois com o mesmo gerador e as mesmas ferramentas.

  Build limpo em diretórios temporários, com as mesmas ferramentas e limite de quatro processos para o build paralelo: script anterior em 74,84 s e script atual em 63,65 s. A medição inclui a reutilização do menu e o paralelismo.
- [x] 3.3 Manter `BUILD_ALWAYS TRUE` e testar o build incremental com a alteração de um asset incorporado ao menu. Conferir se o BIN e o PS-X EXE mudam, se os quatro ODEs incorporam o BIN atualizado e se o Pico Booty reempacota o PS-X EXE. Atualizar `docs/build.md` com o fluxo do pacote.

## 4. Investigações de hardware e mecânica

- [x] 4.1 Capturar `SCOR/SQCK/SQSO` em PU-8 e PU-18 com hardware de referência e com o firmware. Confrontar as capturas com os datasheets Sony e registrar o comportamento de `CRCF`, a ordem dos bits, os clocks observados e a função dos 16 bits finais nos modos testados.
- [x] 4.2 Medir o atraso do alarme de subcode em relação ao início da transferência I²S. Registrar se `elapsed + c_MaxSubqDelayTime` reproduz a temporização de referência e qual deveria ser o comportamento quando o prazo já passou.
- [x] 4.3 Conferir prioridades, preempção e ordem dos eventos de GPIO/XLAT e PIO0 IRQ 0. Registrar se o compartilhamento de `m_latched` é seguro nas variantes afetadas.
- [x] 4.4 Revisar `abs()` em `sectorToMSF` com exemplos conhecidos de pregap e índice 0. Registrar se a contagem regressiva e a transição para o índice 1 estão corretas.

## 5. Validação integrada

- [x] 5.1 Executar `openspec validate --all --strict`.
- [x] 5.2 Executar `cmake -P scripts/build-release.cmake`, conferir os cinco UF2 em `release/` e compilar separadamente as quatro variantes básicas descritas em `docs/validation.md`.

## Resultados e pendências de validação

As marcações 1.4, 4.1 e 4.2 registram o encerramento administrativo solicitado. As medições e testes descritos nessas tarefas continuam pendentes. O usuário confirmou na PU-18 a abertura do menu atual e a execução de um jogo após a correção do DMA da textura. A validação de log por sessão, navegação e demais cenários no console continua pendente. As capturas de subcode e medições de latência em PU-8 e PU-18 não foram realizadas.

- [x] 6.1 Corrigir a transferência DMA da textura da fonte para incluir todos os dados e mostrar o primeiro quadro antes da leitura da configuração.
- [x] 6.2 Preservar a página de listagem para releituras e registrar erros do firmware e do menu em log renovado por sessão.
- [x] 6.3 Compilar o menu atual e gerar os cinco UF2, com diagnóstico no SD ativo e USB desativada. A validação física das últimas alterações está pendente.

## Workflow follow-up

- Arquivar o change após concluir a implementação e registrar os resultados da validação.
- Verificar a integração do delta à especificação principal após o arquivamento.
