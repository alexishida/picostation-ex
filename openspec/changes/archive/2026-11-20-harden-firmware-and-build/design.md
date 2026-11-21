# Design

## Context

O núcleo 0 controla a mecânica e agenda a emissão de subcode Q. O núcleo 1 lê o SD e substitui a imagem ativa em `I2S::start`. Hoje o alarme do subcode carrega o setor, mas consulta a imagem ativa na execução, que pode já ser outra. O protocolo do menu já distingue a preparação da confirmação e o build já aceita incorporar um BIN de menu gerado externamente.

## Goals / Non-Goals

- Manter setores, limite e metadados Q coerentes durante as transições e preservar os caminhos existentes de preparação, montagem e recuperação.
- Manter o pinout e a temporização configurada dos perfis. O protocolo do menu inclui o comando de registro de erros compartilhado com o firmware. Tratar os pontos pendentes sobre subcode, latch e setores negativos como investigações com resultados registrados.

## Decisions

### Publicação coordenada da imagem e do limite

Preparar e validar a candidata no núcleo 1 com a imagem anterior ativa. Concentrar a publicação das montagens confirmadas, montagens diretas, trocas pela tampa e retornos ao menu em um caminho comum. Garantir que a imagem inicial do menu esteja pronta antes de liberar o consumo no núcleo 0.

Na publicação, o núcleo 1 solicita uma pausa curta ao núcleo 0. Antes de confirmar a pausa, o núcleo 0 deixa de agendar subcode, invalida ou cancela o alarme pendente e garante que nenhum callback ainda acesse os metadados antigos. Qualquer transferência Q em andamento deve terminar ou ser invalidada antes da retomada. Durante a pausa, o núcleo 1 publica a imagem e o limite, e o núcleo 0 aplica o reset da mecânica exigido pelo fluxo antes de retomar. A pausa inclui a coordenação com as IRQs da mecânica para impedir que um comando observe estados de imagens diferentes. O retorno ao menu segue o mesmo protocolo.

Sincronizar o acesso a `c_sectorMax` e carregar o limite uma vez por operação da mecânica. Nenhum núcleo espera confirmação enquanto segura a seção crítica compartilhada ou mantém IRQs desabilitadas. Manter o acesso ao FatFs e a preparação da candidata fora da pausa. Para evitar espera circular, a espera de confirmação do reset no núcleo 0 também atende pedidos de pausa do núcleo 1. O núcleo 1 conclui ou cancela a publicação, libera a pausa e então atende o reset pendente. Uma troca iniciada pelo próprio reset usa o estado já quiescente do núcleo 0 e não solicita uma nova pausa. O tratamento precisa cobrir o reset chegando antes e depois do pedido de pausa e garantir que as IRQs da mecânica sejam restauradas conforme o estado do reset. Se a preparação falhar ou a troca for cancelada antes da publicação, preservar a imagem ativa e liberar qualquer pausa solicitada. Uma variável atômica isolada protege o limite. A pausa também protege a tabela de faixas e invalida o trabalho pendente.

### GFS entre núcleos

Manter `m_sensData` sob controle do núcleo 0 para a porta SENS e o loop da mecânica. Publicar GFS em um espelho `pseudoatomic<bool>` em `setSens` e expor um accessor específico para as leituras do núcleo 1 no modchip e no diagnóstico de boot. O accessor pode ser não `const`, como `getSoct`, pois `pseudoatomic::Load()` exige isso. Medir o custo da escrita no caminho da IRQ e das leituras do limite. O mecanismo usa uma seção crítica compartilhada por todos os `pseudoatomic`.

### Interface do subcode Q

Os datasheets Sony CXD2510Q e CXD2545Q descrevem a leitura em `SQSO` de 80 bits de dados Q após a sinalização de `CRCF`. Em modos de medição de áudio, os 16 bits adicionais têm função de PCM. O pacote bruto do disco usa outro formato, com CRC e paridade invertida segundo a ECMA-130 §22.3.6.

Preservar a sequência atual emitida pelo firmware nesta mudança. Capturar `SCOR`, `SQCK` e `SQSO` no hardware de referência e no firmware para identificar os modos observados, o número de clocks, a ordem de bits e a função dos 16 bits finais. Comparar o momento do sinal com o timestamp do início da transferência do setor. A conclusão da investigação pode fundamentar uma mudança posterior no formato ou na temporização.

### Contagem da listagem

Atualizar `mSize` pelo total de quatro bytes gravados por `addTerminator`. Preservar os bytes serializados, os códigos de status, a paginação e o limite do buffer. Uma página que use exatamente `LISTING_SIZE` deve terminar com a contagem igual à capacidade.

### Menu compartilhado no release

Construir o menu a partir dos fontes no primeiro perfil, PU8. Configurar os outros três perfis com `PICOSTATION_BUILD_MENU=OFF` e o caminho absoluto do BIN produzido pelo PU8. O Pico Booty consome o PS-X EXE do mesmo build. Passar `--parallel` aos builds do script.

Manter `BUILD_ALWAYS TRUE` para que o projeto externo consulte suas dependências em cada execução. As regras internas já declaram saídas e dependências para as conversões e o empacotamento da imagem. O modo incremental precisa atualizar os consumidores do BIN e do PS-X EXE quando os fontes ou assets do menu mudarem.

## Risks / Trade-offs

- Um callback pendente pode carregar dados da imagem seguinte para um setor anterior. Validar cancelamento, retorno ao menu e troca entre imagens com limites diferentes.
- O spinlock compartilhado pode aumentar a latência em rotinas críticas. Medir a temporização nos perfis com overclock e conferir o comportamento de boot, leitura e seek.
- Contar os cinco UF2 pode deixar passar um menu obsoleto. Comparar os artefatos intermediários e o conteúdo incorporado após uma alteração de asset em build incremental.

## Ajustes após teste no console

O DMA de textura usa blocos de tamanho uniforme que cobrem todos os pixels, inclusive a fonte ampliada. O primeiro quadro é apresentado antes da leitura de configuração. A página de listagem continua disponível para releituras. O log no SD é reiniciado por sessão e recebe erros comunicados pelo menu e pelo firmware. O diagnóstico no SD permanece habilitado no pacote e USB desativada. As últimas alterações aguardam validação física.

O usuário confirmou boot do menu atual e carregamento de jogo em PU-18. Capturas de subcode, medição de latência e testes das demais variantes permanecem pendentes.
