# Tasks

Estas tarefas tratam da incorporação documental da baseline. Os artefatos de planejamento já foram produzidos; as caixas abaixo não representam funcionalidades de firmware ainda por implementar. Builds e ensaios em console permanecem fora deste escopo, conforme o design.

## 1. Revisão da baseline

- [x] 1.1 Conferir se o commit de referência ainda representa o código ao aplicar a mudança; verificar `git log` e o diff desde `c379b53`, ajustando somente os artefatos afetados por alterações posteriores.
- [x] 1.2 Revisar os contratos de build e release contra CMake, boards e scripts; confirmar que as quatro variantes básicas, quatro perfis RC5 e Pico Booty estão representados em `firmware-build`.
- [x] 1.3 Revisar reprodução, navegação e experiência do menu contra as evidências do design; confirmar índices/LBAs, estados de erro, formatos de capa e limites da validação de imagens nos cenários correspondentes.

## 2. Incorporação às especificações principais

- [x] 2.1 Sincronizar os quatro deltas com `openspec/specs/` usando o fluxo OpenSpec; verificar que cada capacidade possui Purpose e todos os requisitos e cenários da baseline, sem conteúdo de planejamento misturado ao contrato.
- [x] 2.2 Validar a integração documental com `openspec validate --all --strict`; conferir o diff para garantir que não houve alteração de fontes, GPIOs, binários ou dependências e que nenhuma validação física foi marcada como executada.

## Workflow follow-up

- Arquivar a mudança após concluir a revisão e a incorporação, conferindo a preservação das quatro specs principais.
- Tratar as divergências de README e as lacunas de CI/créditos registradas no design em trabalho documental posterior.
- Quando houver qualificação funcional, registrar builds e ensaios da matriz do design com evidências por placa e variante; não inferir aprovação a partir da validação OpenSpec.
