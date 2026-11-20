# Spec Delta

## Purpose

Registrar a interface do menu executado no PlayStation e seus recursos de inicialização, identificação de jogos e personalização visual.

## ADDED Requirements

### Requirement: Navegação pelo controle
O menu SHALL oferecer seleção com cima/baixo, páginas com L1/R1, abertura de pasta ou opções de jogo com X, fechamento ou retorno ao pai com Círculo e créditos com Select na navegação.

#### Scenario: Seleção de jogo
- **WHEN** o usuário pressiona X sobre um CUE
- **THEN** o menu apresenta as opções de inicialização desse jogo.

### Requirement: Opções de inicialização
O menu SHALL oferecer Fast Boot, Regular Boot, alternância da correção EDC e Bootsector Fix. SHALL informar `Cheats unavailable` para cheats e `Clock set by firmware` para overclock, sem executar cheats ou alterar o clock por essas opções.

#### Scenario: Inicialização de jogo reconhecido
- **WHEN** um disco é montado e reconhecido como PlayStation
- **THEN** Fast Boot usa reinicialização rápida e Regular Boot usa reinicialização normal.

#### Scenario: Opções indisponíveis
- **WHEN** o usuário seleciona Cheats ou Overclock
- **THEN** o menu mostra o aviso correspondente e permanece nas opções.

### Requirement: Identificação do jogo
O menu SHALL procurar o caminho de boot em `SYSTEM.CNF` e enviar o Game ID identificado às duas portas de cartão de memória antes de reiniciar. Para `PSX.EXE` genérico, SHALL usar a data de criação do volume na tabela existente de identificação.

#### Scenario: Game ID encontrado
- **WHEN** a identificação do disco reconhecido é bem-sucedida
- **THEN** o progresso mostra o caminho e o ID e o menu envia o ID antes do boot.

### Requirement: Capas opcionais
O sistema SHALL aceitar capas `.cov` de 128 × 128 pixels em RGB555 de 16 bits little-endian ou indexadas de 8 bits com paleta de 512 bytes. SHALL procurar a capa com o mesmo nome base do CUE ou ao lado da pasta, usando o nome da pasta acrescido de `.cov`.

#### Scenario: Transporte RGB555
- **WHEN** a capa RGB555 é solicitada
- **THEN** o firmware a transmite em 16 partes de 2048 bytes com marcador `RGB5`.

#### Scenario: Transporte indexado
- **WHEN** a capa indexada é solicitada
- **THEN** o firmware a transmite em oito partes de até 2114 bytes.

#### Scenario: Capa ausente
- **WHEN** não existe capa para a entrada
- **THEN** o firmware sinaliza ausência com `DEAD` e a navegação continua disponível.

### Requirement: Cores opcionais
O menu SHALL interpretar até 2048 bytes de `config.ini` na raiz do cartão com chaves `background`, `frame` e `filelist` em RGB hexadecimal, mantendo seu uso possível sem o arquivo.

#### Scenario: Configuração presente
- **WHEN** o arquivo contém cores válidas nas chaves suportadas
- **THEN** o menu aplica as cores aos elementos correspondentes.

#### Scenario: Configuração ausente
- **WHEN** o arquivo não existe
- **THEN** o firmware sinaliza ausência com `DEAD` e o menu usa suas cores padrão.
