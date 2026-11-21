# disc-playback Specification

## Purpose

Descrever o contrato de carregamento de discos CUE/BIN e a transição entre o menu embarcado e a imagem selecionada no cartão SD.

## Requirements

### Requirement: Validação da imagem candidata
O firmware SHALL exigir faixas com arquivos acessíveis, não comprimidos, não vazios e com tamanho múltiplo de 2352 bytes. Quando a primeira faixa for de dados, SHALL verificar o descritor ISO9660 no setor 16 em layout Mode 1 ou Mode 2 antes de aceitar a imagem.

#### Scenario: Imagem de dados válida
- **WHEN** o CUE é interpretável e os BINs e o descritor satisfazem as verificações
- **THEN** a preparação retorna sucesso sem substituir imediatamente o menu ativo.

#### Scenario: Nome de faixa com acento
- **WHEN** a diretiva `FILE` referencia um BIN existente com nome representável em CP850, em um CUE CP850 ou UTF-8 com ou sem BOM
- **THEN** o firmware tenta o caminho original e, se necessário, converte o argumento UTF-8 para CP850 antes de abrir a faixa.

#### Scenario: BIN ausente ou tamanho incompatível
- **WHEN** um BIN não pode ser aberto ou seu tamanho não é múltiplo de 2352 bytes
- **THEN** a preparação retorna erro e o menu permanece ativo.

### Requirement: Preparação e confirmação pelo mesmo índice
O firmware SHALL responder ao comando de preparação `0xB` no LBA 110 com `MNT1`, índice e resultado FatFs em campos de 16 bits little-endian, e flag de áudio no offset 8. A confirmação SHALL publicar a imagem preparada somente para o mesmo índice. Na montagem, na troca de disco e no retorno ao menu, o limite de setores e os dados do subcode Q SHALL corresponder à imagem usada para a leitura.

#### Scenario: Confirmação válida
- **WHEN** a preparação termina com sucesso e o comando de montagem referencia o mesmo índice
- **THEN** a imagem preparada se torna o disco ativo e seu limite de setores passa a ser usado.

#### Scenario: Disco de áudio
- **WHEN** o CUE aceito não contém faixas de dados
- **THEN** a resposta indica a flag de áudio e o menu segue a inicialização normal do console.

#### Scenario: Troca com subcode pendente
- **WHEN** a imagem ativa muda com uma emissão de subcode Q pendente para um setor da imagem anterior
- **THEN** essa emissão é descartada antes de iniciar a leitura da nova imagem
- **AND** as próximas leituras usam o limite de setores e os dados Q da nova imagem.

#### Scenario: Retorno ao menu
- **WHEN** o menu volta a ser a imagem ativa após a leitura de um disco
- **THEN** as leituras voltam a usar o limite de setores e os dados Q do menu.

### Requirement: Recuperação de falha na abertura
O menu SHALL recusar respostas de preparação sem `MNT1`, com índice diferente ou com resultado de erro, exibindo erro por 240 quadros e preservando a seleção. O comando `0xC` SHALL restaurar a imagem do menu após a montagem.

#### Scenario: Preparação recusada
- **WHEN** a resposta de preparação indica falha
- **THEN** o menu não confirma a montagem e permite outra tentativa a partir da seleção existente.

#### Scenario: Leitura falha após montar
- **WHEN** a verificação de disco após a montagem falha sem indicar o caminho de disco não PlayStation
- **THEN** o menu solicita retorno pelo comando `0xC` e apresenta o erro.
