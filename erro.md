# Marvel vs. Capcom trava no Fast Boot do PU-18

**Referência confirmada no console:** na tag `v1.0.1` (`ed8aab0`), o mesmo PU-18 e a mesma imagem de Marvel vs. Capcom chegam ao jogo pelo Regular Boot e ficam em tela preta pelo Fast Boot. O Fast Boot dessa imagem funcionava com o firmware rc5. O teste isolado da correção do atendimento XLAT (`build/v1.0.1-xlat`, SHA-256 `594067df…`) também foi ao console e a tela preta persistiu, descartando a ordenação de interrupção do Mechacon como causa única. O teste isolado da remoção do `+1` do MSF absoluto do Q (`build/v1.0.1-subq`, SHA-256 `e777278c…`) também foi ao console em 21/11/2026 e a tela preta persistiu, descartando o deslocamento do Q como causa única.

## Comparação rc5 × v1.0.1 no binário

Comparação sistemática do firmware rc5 (`referencia/rc5-build/picostation_PU18.uf2`) contra a compilação da tag (`build/base-v1.0.1`, confirmada idêntica à fonte da tag) pelo caminho completo de atendimento ao console:

| Componente | Resultado |
|---|---|
| Programas PIO (`mechacon`, `subq`, `soct`, `i2s_data`) | Idênticos byte a byte |
| Tabelas `zone[]` e `sect_per_track[]` | Idênticas |
| Clocks PWM (Mechacon GP3 período 64, DA15, LRCK, CLK) e passo do sled (29 µs) | Idênticos |
| Agendamento do subcanal Q (`now − lastSectorTime + 3333` via `add_alarm_in_us`) | Idêntico em estrutura |
| Laço do núcleo 0 (LMTSW, velocidade, SOCT, sled, avanço de setor) | Idêntico |
| `moveToNextSector`, `setSector`, `moveSled`, TOC do lead-in, P-time do Q, portão do pregap, `start_subq` | Idênticos |
| **MSF absoluto do subcanal Q na área de programa** | **Diverge: rc5 usa `setor − 4500`; v1.0.1 usa `setor − 4499`** |

A divergência está em `DiscImage::generateSubQ` (`src/storage/disc_image.cpp`): o refactor da v1.0.1 alterou `sector_abs = sector - c_leadIn` para `sector_abs = (sector - c_leadIn) + 1`. No binário compilado isso aparece como a base literal `0xffffee6d` (−4499) na v1.0.1 contra `0xffffee6c` (−4500) no rc5, no mesmo pool da função. O P-time relativo, o portão do pregap (`4649`), a lógica do TOC e o `ctrladdr` continuam idênticos, o que isola o deslocamento apenas no campo de endereço absoluto.

Com o `+1`, todo quadro Q entregue durante um setor declara ao console uma posição um setor à frente do endereço verdadeiro do disco (LBA `N` informa MSF `00:02:(N+1)`). O console trava a posição de leitura pelo subcanal; na via warm-boot o kernel relê a janela de licença e o Q desalinhado pode fazer a captura sair deslocada ou falhar na verificação de posição, produzindo a releitura infinita da janela LBA 12–20 observada no traço — sem timeout de LRCK, sem reset e sem cancelamento, exatamente como registrado. O boot regular passa pelo caminho do boot ROM, que não depende do mesmo travamento por posição. Nenhum dos experimentos anteriores tocou nesse cálculo: EDC, Bootsector Fix, imagem corrigida, VRAM, Init do CD, ordem de marcação do setor, reagendamento do Q e ordenação XLAT operam em outras partes do caminho.

## Teste de 21/11/2026 no PU-18: MSF absoluto do Q sem o +1

O [UF2 para o teste](build/v1.0.1-subq/build/picostationex_PU18/picostationex_PU18.uf2) usa a fonte da tag `v1.0.1` com uma única alteração: a remoção do `+1` do endereço absoluto do subcanal Q, restaurando o cálculo do rc5. O binário inteiro difere da tag em exatamente 1 byte (o literal de `−4499` para `−4500` no pool do `generateSubQ`), o mesmo tipo de teste isolado do XLAT. A imagem de menu é a mesma da base `v1.0.1` usada pelo teste XLAT (SHA-256 do menu `92b37e33…`). O SHA-256 do UF2 é `e777278c2f64a1b6269410b6c517b36d53ef4bed49281eec6910b6623bd2db8a`.

1. Iniciar o MvC pelo Fast Boot e registrar se chega à tela de título.
2. Repetir pelo Regular Boot para confirmar o comportamento de referência.
3. Medir o tempo até a tela de título do Gran Turismo 2 pelo Fast Boot e comparar com a tag nas mesmas condições: se o deslocamento do Q for a causa, as releituras da janela 13–21 e da região 1060–1068 devem diminuir drasticamente junto com o tempo de boot.

**Resultado (21/11/2026):** a tela preta persistiu no Fast Boot do MvC. O traço de 45 s gravado pelo próprio firmware (a instrumentação de eventos de disco já existe na tag) mostra, porém, que a correção mudou o desfecho de forma mensurável: o boot agora avança além da janela de licença e chega a transmitir a região do executável, o que a tag nunca fez. O `+1` era parte necessária do problema, mas não suficiente — resta uma segunda falha, a jusante. A correção foi mantida no repositório (`5561143`) por restaurar o cálculo do rc5 e por ter efeito comprovado no comportamento do console.

## Traço de 21/11/2026: o boot com o Q corrigido

No teste no PU-18, o Regular Boot do Marvel vs. Capcom chega ao jogo normalmente. No Fast Boot, o menu exibe as mensagens de carregamento e a tela fica preta indefinidamente. O menu exibe `Load and execute` antes de chamar `softFastReboot()`. No caminho do BIOS Sony, essa rotina configura um breakpoint COP0 para pular a abertura e usa o ponto de entrada alternativo do BIOS. O Regular Boot chama `softReset()` e usa o ponto de entrada normal.

Na imagem local, `SYSTEM.CNF` ocupa o LBA 23 e aponta para `SLUS_010.59`, que ocupa os LBAs 24–666. O grupo 302 cobre esse intervalo inteiro em uma sequência de DMA do LBA 19 ao 669. A leitura chega depois ao LBA 719, dentro de `CAPMOV.BIN` (LBAs 667–741). Esses endereços mostram o que o Pico transmitiu. A recepção correta dos setores e a execução do jogo continuam sem confirmação pelo traço.

O traço de 45 s (2839 DMAs, UF2 `e777278c…`) inclui atividade do menu e do jogo. O evento `jogo_montado` aparece aos 30,312 s no log:

- Na tag, a janela LBA 12–20 se repetia ~380 vezes e o último LBA era 16. Agora a janela deslocou para 14–22, houve 4 cancelamentos por posição ou reset e o traço termina no LBA 719, sem timeout de LRCK e sem reset.
- Fases visíveis: passagens repetidas pelo fim do pregap (−9..−1, 15 vezes); trecho omitido de ~20 s dominado pela janela de licença (histograma: LBA 4–12 com 52–59 transmissões cada, LBA 14–22 com 62–71); varredura esparsa ascendente (123→204); releituras em 197–217 e 104–122 (compatíveis com a varredura de diretório); nova rodada de licença (7–26 e depois 14–22 ×24, a cada ~84 ms); transmissão contínua de LBA 19–669 (651 setores ≈ 1,5 MB em ~4,3 s, ≈ 2×, compatível com a carga do PS-X EXE); releitura do trecho 659–667; descida de volta ao pregap; leitura ascendente até 719 no fim da janela.
- Interpretação de trabalho: o Fast Boot chega a solicitar setores na região do executável e de arquivos adjacentes, com releituras antes e depois do trecho contínuo. A comparação com o traço do Regular Boot pode mostrar em que ponto as sequências de leitura passam a divergir.
- Referências de ouro pendentes no mesmo firmware: o traço do cold boot do MvC (reset na tela preta — o jogo abre) e o do Fast Boot do GT2 (o jogo abre). A comparação fase a fase com este traço localiza o estágio doente sem precisar do rc5.
- Observação do console durante a janela: tela preta, **sem som de jogo** — o executável provavelmente não chegou a rodar; os 651 setores transmitidos não resultaram em um jogo em execução.
- Mecânica da captura: o traço é gravado uma vez por sessão de diagnóstico e guarda os 24 grupos iniciais e os 104 finais. A janela de 45 s começa antes da montagem do jogo neste build. Para comparar os dois modos, inicie uma sessão para cada modo e selecione o jogo logo que o menu abrir.

Com o Q alinhado e o travamento persistindo, a comparação binária descartou praticamente todo o caminho de clock, PIO, sled e agendamento no código próprio do firmware. As camadas restantes são as que a comparação não cobre: o executável do menu (reconstruído no EX, com protocolo divergente do rc5 — o cruzamento menu-rc5×drive-EX parou antes da seleção), a camada SDK/biblioteca entre as duas compilações e temporizações residuais não visíveis na comparação estrutural. Antes das capturas físicas de SCOR/LRCK/DA16 e do comando e da posição do sled antes de cada releitura, os diferenciais mais baratos são: (1) comparar o caminho de boot do menu rc5 (chamadas finais `FUN_8001614c`, `FUN_800178c8`/`FUN_800178a4`) com o que o menu EX executa antes de `softFastReboot()`; (2) traçar os comandos do Mechacon do warm boot que trava contra o cold boot que funciona no mesmo firmware e na mesma imagem (`build/usbdebug`). O traço de 21/11 reordenou as prioridades: antes de (1) e (2), capturar no mesmo firmware as duas referências de ouro — o cold boot do MvC (o reset na tela preta abre o jogo) e o Fast Boot do GT2 (abre) — e compará-las fase a fase com o traço do warm boot que trava.

## Porte do scrub de cache do rc5 (21/11/2026)

A comparação estática do caminho fast do menu rc5 (`FUN_800178c8`, decompilado em `referencia/menu-decompilado/`) contra o menu EX encontrou **uma única etapa sem equivalente**: `FUN_80019e44`, executada entre a cópia dos stubs e a configuração do breakpoint, via espelho kseg1 (`0xa0019e5c`). O assembly real (`menu_mips.asm`, 10143–10175) mostra a semântica exata: salva SR e BCC, programa `0xfffe0130` com `(BCC & ~0x80) | 0x804` (I-cache em tag test), põe **SR = 0x10000 (bit IsC, D-cache isolado)** e varre `0x000–0xfff` com stores de zero — uma **invalidação das linhas da I-cache**: em tag test os stores atingem o array de tags da I-cache e invalidam cada linha; o bit IsC impede que os stores cheguem à RAM (por isso os endereços kuseg crus são seguros). É a inicialização de cache que o BIOS executa entre `0xbfc00000` e `0xbfc00390`, trecho que ambos os menus pulam ao usar o ponto de entrada alternativo — sem ela, o kernel novo executa sobre instruções stale deixadas em I-cache pelo boot anterior e pelo runtime do menu. Todo o restante do caminho fast é idêntico byte a byte: stubs, `DCIC/BDA/BDAM` e o próprio `0xbfc00390`.

**Primeira tentativa, com semântica errada (commit `4c2ff03`, build `6683d87a…`):** o pseudocódigo decompilado sugeria um "wipe da RAM do kernel em 0x80000100–0x80000fff"; o porte literal assim implementado **regrediu** o sintoma. A tela congelou na mensagem de carregamento e o traço mostrou o drive preso na janela **208–216** (dentro do executável, setores Form 1 ordinários verificados na imagem), re-sequenciando a cada ~84 ms até o fim da janela de captura, sem transmitir o executável — `ultimo_lba=212` contra 719 no build anterior. Zerar estruturas vivas do kernel sob o menu em execução piora o estado; estado stale mas estruturado escapava da janela. O mesmo 208–216 era releitura pesada também no build anterior: a janela é grudenta por estado de leitura, não por conteúdo.

**Porte fiel (build `13ea0efa…`):** `_fastRebootKernelWipe` agora reproduz a sequência instrução a instrução — SR=0x10000, sweep kuseg cru com 16 stores por página e 16 páginas, restore de BCC e SR. Compromisso seguinte sobre o `4c2ff03`. Resultado no console (21/11/2026): o MvC ainda não abriu pelo Fast Boot, mas o traço avançou mais uma vez — tempestade de licença breve (~4 s), e o **stream do executável correndo limpo (LBA 18→647, 630 transferências) quando a janela de 45 s fechou**. O scrub não alterou o desfecho por si.

## Dano da imagem provado (21/11/2026)

Verificação EDC computacional contra o algoritmo do próprio firmware (`edc.c`: LSB-first, polinômio 0xD8018001, `set32lsb` little-endian), calibrada no GT2 (100% verificado): **a imagem do MvC tem exatamente 4 setores defeituosos — LBA 12, 13, 14 e 15** — os setores de licença Mode 2 Form 2 (submode 0x20, dados zerados) com EDC zerado onde o valor correto é `3F13B0BE`, idêntico ao do GT2 na mesma região. Todo o resto de LBA 0–741 (executável, PVD, diretório) verifica limpo.

O firmware tem um caso especial (`edc.c:168–178`) que **preserva** tail zerado ou `3F13B0BE` em Form 2 submode 0x20 — com EDC correction ON ou OFF, os 12–15 do MvC são servidos com o EDC zerado. As tempestades de releitura de todos os traços pairam sobre essa janela (12–20 na tag, 14–22 depois). GT2 (EDC válido na região) abre mancando; MvC (zerado) morre. O teste anterior da imagem "com EDC corrigido" ocorreu na tag — preso pelo `+1` do Q — e nunca foi repetido no regime atual.

**Imagem corrigida**: `referencia/jogo/mvc-edc-ok/` (CUE + BIN, 16 bytes de patch em LBA 12–15 → `3F13B0BE`; SHA-256 do BIN `effd216cccf4eb4475d0b907f636e4f26f2cca538027e656f07b71671ba89746`). **Resultado (21/11/2026): o par foi copiado por cima dos originais no cartão e o Fast Boot falhou igual** — carregando → tela preta, sem som, traço idêntico ao run não corrigido. **A hipótese do EDC zerado da licença está eliminada** — e a literatura explica por que ela nunca teria efeito: o EDC zerado em 12–15 é o padrão dos **discos originais prensados** (bug de mastering da Sony, documentado pelo EDCRE), não dano; jogos estilo DDR checam o setor 12 e **travam se o EDC for regenerado**, o que explica o caso especial de preservação do `edc.c` — ele está correto e **não deve ser alterado**. O BIN do GT2 com `3F13B0BE` em 12–15 apenas indica cópia re-processada.

Assinatura refinada do travamento, estável nos últimos builds: a carga do executável **trava no rabicho** — o kernel transmite o EXE inteiro, releio os últimos setores (659–667, na fronteira EXE→CAPMOV), não completa, e **reinicia o boot** (descida ao pregap + nova passada), em loop, sem som e sem vídeo. O comparativo pendente no mesmo build: o traço do **GT2 Fast Boot** (EXE de 307 setores, abre) e o do **cold boot do MvC** (o mesmo EXE de 643 setores, carrega com sucesso no mesmo firmware) — o comportamento dos rabichos nas duas cargas bem-sucedidas localiza o estágio. As tempestades em 208–216 permanecem como anomalia secundária: o disco verifica limpo nessa região e elas também aparecem na fase do menu.

## Divergência no motor de entrega (21/11/2026)

O diff instrução a instrução do caminho de entrega de dados (`i2s.cpp`) contra o binário rc5 — a camada que nunca tinha entrado nas comparações — encontrou a divergência funcional sobrevivente:

- **O rc5 publica `{m_sectorSending, m_lastSectorTime}` ANTES de tocar o DMA** (0x20001d82 e 0x20001e1e, anteriores à espera de borda do LRCK em 0x20001e52). O EX só publicava **depois de um trigger bem-sucedido** (`i2s.cpp:725–726`).
- O agendador do Q no núcleo 0 exige `getSectorSending() == currentSector` (`picostation.cpp:276`): com o par publicado só pós-trigger, qualquer abort (timeout de LRCK de 100 µs, `Stale` por geração durante transição de comando) congela o par no último setor **entregue** e o subcanal Q para de andar — justamente no rabicho de leituras longas. Toda janela grudenta dos traços tem exatamente 9 setores (12–20, 14–22, 208–216, 659–667, 13–21 e 1060–1068 no GT2, que **abre** após ~91 releituras): a unidade é o rabicho de um comando de leitura, e o orçamento de retry é quem decide entre o GT2 (abre mancando) e o MvC (desiste na fase do executável).
- Demais achados do diff: a ordem endereço→borda→trigger é a mesma do rc5 (a mudança documentada na validação reproduzia o rc5); o rc5 não tem validação de geração nem timeout de LRCK (espera infinita, sempre dispara); o rc5 grava o timestamp em seção crítica (o EX usa `pseudoatomic`, equivalente); o agendamento do Q tem fórmula e literais idênticos (`now − lastSectorTime + 3333`).
- **Correção (commit `65ec89c`):** restaurada a ordem do rc5 — `sectorSending`, timestamp, endereço de leitura, espera de borda, trigger — mantendo `activeDMABuffer` pós-trigger e toda a validação de geração do EX. Efeito colateral aceito (idêntico ao risco que o rc5 sempre correu): o par publicado pode brevemente descrever um setor cujo trigger abortou; o Q avança mesmo assim, como no rc5.

**Build de teste** (Q corrigido + invalidação de I-cache + esta correção): `build/picostationex_PU18/picostationex_PU18.uf2`, SHA-256 `a669817a2b233261fdaadeb375890db0c87a909917dfa80a7bdb294e886e68e9`. **Resultado (21/11/2026): o MvC não abriu pelo Fast Boot — a restauração da publicação pré-trigger, embora fiel ao rc5, não bastou.** Restam como divergências do caminho, por ordem do diff: os abortos exclusivos do EX (`ClockTimeout` de 100 µs e `Stale` por geração, que descartam a transferência em voo onde o rc5 sempre dispara), a checagem de `enabled` por snapshot contra a leitura ao vivo do rc5, e a camada SDK/compilação nunca comparada. A janela de 9 setores persiste sem correspondência em literal de nenhum dos dois firmwares — sua medição no console (capturas físicas de SCOR/LRCK/DA16 ou o traço usbdebug dos comandos do Mechacon warm × cold) continua sendo o caminho para fechá-la.

## Estado da caçada (21/11/2026)

As três correções de código commitadas — MSF absoluto do Q sem o `+1` (`5561143`), invalidação da I-cache do rc5 (`7677213`) e publicação pré-trigger do par de setor (`65ec89c`) — são fiéis ao rc5 e cada uma eliminou uma divergência real comprovada, mas nenhuma fechou o jogo. O diff instrução a instrução do motor de entrega não encontrou mais nada de maior peso.

Caminhos restantes, por custo:

1. **Barato (último candidato de código com evidência forte):** os abortos exclusivos do EX. O rc5 **sempre dispara** o DMA (espera infinita pela borda, sem validação de geração); o EX desiste — `ClockTimeout` aos 100 µs e `Stale` descartam a transferência em voo. Uma variante que replique o "sempre dispara" (remover o timeout de LRCK, espera infinita como no rc5) se apoia na mesma evidência do diff. O risco: perder a serialização de seek/reset que a validação de geração do EX proporciona.
2. **Caro:** medir a janela de 9 setores no console em vez de inferi-la — capturas físicas de SCOR/LRCK/DA16, ou o traço do `build/usbdebug` comparando os comandos do Mechacon do warm boot que trava contra o cold boot que funciona, na mesma imagem.
3. **Circunscrito e utilizável hoje:** Regular Boot e cold boot funcionam; o problema é exclusivo do Fast Boot (warm boot), com o GT2 abrindo mancando e o MvC (executável de 1,3 MB) travando no rabicho da carga.

## Evidências dos experimentos anteriores

Os números abaixo vêm de builds de investigação posteriores à tag. Os contadores registram os setores que o Pico tentou transmitir. Para saber quais bits o console recebeu, é necessária uma captura física.

| Experimento | Resultado observado |
|---|---|
| Traço de 45 s iniciado na montagem do MvC | 4.365 DMAs. Após o pregap e os primeiros setores, a janela LBA 12–20 se repetiu cerca de 380 vezes, a cada 84 ms. O último LBA foi 16, sem timeout de LRCK, reset ou cancelamento. |
| Fast Boot do GT2 no mesmo firmware de diagnóstico | O jogo abriu. A janela 13–21 se repetiu cerca de 37 vezes e a região do executável, 1060–1068, cerca de 91 vezes. O traço registrou cerca de 40 s de releituras. |
| Reset do console durante a tela preta do MvC | O boot completo abriu o jogo. |
| Limpeza da VRAM antes de sair do menu | As mensagens deixaram de ficar visíveis na tela preta. O travamento persistiu. |
| EDC correction desligado, Bootsector Fix ligado e imagem do MvC com EDC corrigido nos setores 12–15 | O travamento persistiu em cada um dos três testes. |
| Marcação do setor antes do gatilho de DMA | O travamento persistiu. |
| Canal Q agendado para 3333 µs após o início do DMA | A tela preta persistiu. Em 45 s houve 4.372 DMAs e 379 envios do LBA 16, sem timeout de LRCK ou cancelamento. |
| Marcador imediatamente antes de `softFastReboot()` | O menu chegou ao marcador 1856 ms após a montagem. O LBA 16 apareceu 378 vezes no período. |
| Comando Init do CD antes do warm boot, com espera por INT3 e INT2 | O marcador apareceu aos 1868 ms. Houve 378 envios do LBA 16, dois cancelamentos e nenhum timeout de LRCK ou registro de `menu_fast_boot_init_cd`. |
| Menu extraído do rc5 com drive EX | Parou em `Please Wait Loading`, antes da seleção do jogo. Houve 249 pedidos de listagem e 248 DMAs no LBA 100. O teste terminou antes de exercitar o Fast Boot. |
| Remoção do `+1` do MSF absoluto do Q (`build/v1.0.1-subq`, SHA-256 `e777278c…`, diferença de 1 byte contra a tag) | A tela preta persistiu no Fast Boot do MvC (21/11/2026), mas o traço mudou de estrutura: janela deslocada para 14–22, boot avança até transmitir LBA 19–669 (executável) e termina no LBA 719 com 4 cancelamentos, sem timeout de LRCK. O `+1` era necessário, porém insuficiente. |

## Imagem do disco e diagnóstico

O CUE do MvC tem uma faixa com `INDEX 01 00:00:00`. Os setores 12–15 são Mode 2 Form 2 com dados e EDC zerados. Os setores 16–20, que incluem o descritor primário de volume, têm EDC válido. No GT2, os setores 12–15 também têm dados zerados, com EDC válido (`3F13B0BE`). A comparação com o rc5 identificou substituição pelo `loaderImage` nos setores 0–4, regeneração condicional em Form 1 e preservação do EDC zerado ou padrão no caso Form 2 analisado. A variante do MvC com EDC corrigido está em `referencia/jogo/mvc-edc-ok/`.

O Fast Boot chama `softFastReboot()` e entra no BIOS em `0xBFC00390`. O marcador confirma que o menu chegou à chamada. O traço do MvC registra a repetição na região do descritor de volume durante a captura. A etapa exata em que o BIOS deixa de avançar continua em aberto.

Se o problema persistir no teste isolado, as capturas anteriores permitem investigar o comando e a posição do sled antes de cada releitura, o alinhamento entre o canal Q e os dados, e a sincronização entre os núcleos durante a publicação da imagem e o início do DMA.

- O build USB em `build/usbdebug/picostationex_PU18.uf2` registra comandos e posições de busca pela serial a 115200.
- O diagnóstico de canal Q em `build/diagnostic-pu18-no-usb/picostationex_PU18.uf2` registra no SD os desfechos dos LBAs 12–20 e conserva as últimas 32 amostras. Para um envio, `evento_apos_us` mede o intervalo entre o DMA de origem e SCOR. Os demais eventos medem até a supressão ou o cancelamento. Um `desde_dma_recente_us` negativo indica que outro DMA começou entre o evento e a coleta. `geracao_mudou` compara as gerações na origem e na coleta. O endereço `msf_bcd` é preenchido no envio.
- O firmware usado como referência histórica está em `referencia/rc5-build/picostation_PU18.uf2`.
