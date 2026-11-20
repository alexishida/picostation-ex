# Proposal

## Why

O PicoStation EX já implementa firmware ODE, menu PlayStation e empacotamento de releases, mas ainda não possui especificações OpenSpec. Esta mudança registra a situação observada no commit `c379b53`, em 20/11/2026, para orientar futuras mudanças sem confundir código existente com funcionalidades planejadas ou validação em hardware.

## What Changes

- Documentar arquitetura, build, protocolo, dependências e lacunas no design desta mudança.
- Introduzir especificações de baseline para comportamentos já presentes no código, com cenários verificáveis.
- Registrar tarefas de revisão e incorporação da baseline às especificações principais.
- Manter explícita a diferença entre inspeção estática e execução de builds ou testes no console.

`ADDED` significa inclusão no acervo OpenSpec, não implementação de funcionalidades novas. Não há alteração de firmware, pinout, temporização, menu ou dependências nesta mudança.

## Capabilities

### New Capabilities

- `firmware-build`: variantes, menu embarcado e pacote de release com Pico Booty.
- `disc-playback`: leitura CUE/BIN, preparação, montagem e retorno ao menu.
- `sd-navigation`: navegação, paginação e recuperação de erros de listagem.
- `menu-experience`: controle, opções de boot, Game ID, capas e configuração visual.

### Modified Capabilities

Nenhuma. `openspec list --specs` não encontrou especificações anteriores.

## Impact

Somente documentação em `openspec/changes/document-project-baseline/`. Evidências: `CMakeLists.txt`, `boards/`, `scripts/build-release.cmake`, `cmake/`, `src/`, `include/menu_protocol.h`, `README.md` e `.ai-framework/DESIGN.md`.

As especificações principais continuam pendentes de incorporação pelo fluxo OpenSpec. Compilação e testes em hardware não são alegados como realizados; as lacunas encontradas não autorizam correções de código nesta mudança.
