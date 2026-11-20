# Spec Delta

## Purpose

Definir a navegação do cartão SD oferecida ao menu PlayStation, incluindo listagem paginada e estados de erro recuperáveis.

## ADDED Requirements

### Requirement: Entradas visíveis e navegação
O sistema SHALL listar diretórios e arquivos `.cue` sem diferenciar maiúsculas na extensão, omitindo entradas ocultas e exibindo jogos sem a extensão. SHALL permitir abrir diretórios, subir ao pai e retornar à raiz.

#### Scenario: Diretório com arquivos variados
- **WHEN** a listagem encontra pastas, CUEs, BINs e arquivos ocultos
- **THEN** somente pastas e CUEs não ocultos aparecem como entradas navegáveis.

#### Scenario: Retorno ao pai
- **WHEN** o usuário sobe de uma subpasta com histórico de seleção
- **THEN** o menu restaura a seleção anterior limitada ao número atual de entradas.

### Requirement: Paginação e terminadores
O firmware SHALL sinalizar término da listagem com `MENU_LIST_DONE=0` ou continuação com `MENU_LIST_MORE=1`, permitindo ao menu solicitar entradas subsequentes.

#### Scenario: Conteúdo maior que uma resposta
- **WHEN** ainda existem entradas elegíveis após preencher a resposta
- **THEN** o terminador indica continuação.

#### Scenario: Pasta vazia
- **WHEN** não existem entradas elegíveis
- **THEN** a listagem termina sem transformar a pasta vazia em falha de cartão.

### Requirement: Erros e nova tentativa
O sistema SHALL distinguir cartão ausente (`2`), falha de leitura (`3`) e falha de abertura de pasta (`4`) e oferecer nova tentativa no menu. Quando o cartão estiver indisponível, uma nova solicitação de listagem SHALL tentar montá-lo novamente.

#### Scenario: Cartão reinserido
- **WHEN** o cartão estava indisponível e o usuário pressiona X em Retry após reinseri-lo
- **THEN** o firmware tenta montar o cartão e obter nova listagem, retornando conteúdo ou o estado de erro correspondente.
