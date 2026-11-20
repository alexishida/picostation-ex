# Cartão SD e menu

## Organização dos jogos

O README do projeto recomenda cartão SD em exFAT. Organize os discos em arquivos `.cue` e `.bin`; os nomes e caminhos dos BINs devem corresponder às referências no CUE. As imagens devem usar setores raw de 2352 bytes. O menu lista diretórios e arquivos `.cue` visíveis, sem exibir a extensão; outros arquivos e entradas ocultas não aparecem.

Antes da montagem, o firmware interpreta o CUE, abre as faixas e verifica se os arquivos são não vazios, não comprimidos e têm tamanho múltiplo de 2352 bytes. Quando a primeira faixa é de dados, verifica também o descritor ISO9660 no setor 16, em Mode 1 ou Mode 2. Essas verificações não garantem a integridade integral de um disco. A identificação PlayStation ocorre no menu após a montagem. Discos somente de áudio e discos sem assinatura PlayStation seguem a inicialização normal do console.

## Controles e boot

| Controle | Ação |
| --- | --- |
| Cima / baixo | Selecionar uma entrada |
| L1 / R1 | Voltar / avançar página |
| X | Abrir pasta, opções do jogo ou tentar novamente após erro |
| Círculo | Fechar opções ou voltar à pasta anterior |
| Select | Abrir créditos durante a navegação |

Na janela de jogo, Fast Boot e Regular Boot iniciam o disco; também é possível alternar EDC correction e Bootsector Fix. Cheats ainda não estão disponíveis e o clock é definido pelo firmware. Se a preparação falhar, o menu exibe um erro e mantém a seleção. Se o cartão estiver indisponível, selecione **Retry** com X para solicitar outra listagem e tentar montá-lo novamente.

Para jogos reconhecidos, o menu procura o executável de boot em `SYSTEM.CNF` e, quando identifica o Game ID, envia-o às duas portas de cartão de memória. Para `PSX.EXE` genérico, usa a data de criação do volume na tabela incorporada ao menu.

## Registro de erros no SD

Nos quatro firmwares ODE do release, o Pico cria ou acrescenta linhas a `/erro.log` apenas ao detectar uma falha. Cada linha informa o tempo desde o boot em milissegundos, a operação, um detalhe numérico e o número de ocorrências agrupadas. Erros repetidos da mesma operação são gravados no máximo uma vez a cada cinco segundos. O arquivo existente é preservado entre partidas.

O registro cobre falhas de montagem do SD, listagem, preparação ou montagem de jogos, leitura de setores, erros de acesso às capas ou à configuração e timeout de LRCK. A ausência de capa ou de `config.ini` é normal. Uma falha enquanto o SD está indisponível só pode ser gravada se o cartão voltar a aceitar escrita durante a mesma partida.

## Capas

Coloque um `.cov` ao lado da entrada usando o mesmo nome base: `Jogos/Exemplo.cue` usa `Jogos/Exemplo.cov`; a pasta `Jogos/RPG/` usa `Jogos/RPG.cov`. São aceitos arquivos de 128 × 128 pixels indexados de 8 bits precedidos por paleta de 512 bytes, ou RGB555 de 16 bits little-endian. Capas ausentes são opcionais e não impedem a navegação.

## Cores

Crie `config.ini` na raiz do cartão com as chaves opcionais `background`, `frame` e `filelist`, usando cores RGB hexadecimais. O menu lê até 2048 bytes; sem arquivo usa as cores padrão.

```ini
background = 404040
frame = 606060
filelist = 303030
```

Consulte [validação e solução de problemas](validacao.md) para cenários de teste e diagnóstico.
