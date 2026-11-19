# Decompilação do menu PicoStation

Arquivo analisado [`binary/picostation-menu.bin`](../../binary/picostation-menu.bin), SHA-256 `d10c138286ffbe4a78b36b5d9a750f41d9a110e58f6a7815db841df1891645e5`.

O executável `SCES_313.37` está em formato PS-X EXE para MIPS R3000 little-endian. O cabeçalho indica endereço de carga `0x80010000`, entrada `0x80018D84` e 151.552 bytes de código, dados e preenchimento carregados pelo console.

| Arquivo | Conteúdo |
| --- | --- |
| [`menu_decompilado.c`](menu_decompilado.c) | Pseudocódigo C exportado pelo Ghidra 12.1.4 para as 132 funções identificadas. |
| [`funcoes.tsv`](funcoes.tsv) | Endereço, nome atribuído pelo Ghidra, tamanho do corpo e linha no pseudocódigo. |
| [`intervalos_de_funcoes.tsv`](intervalos_de_funcoes.tsv) | Intervalos de endereços que o Ghidra atribuiu a funções. |
| [`menu_mips.asm`](menu_mips.asm) | Listagem MIPS dos 151.552 bytes, incluindo dados e áreas sem função identificada. As palavras hexadecimais reproduzem o conteúdo carregado byte a byte. |

O laço principal está em `0x80011A28`, a rotina que desenha o texto do rodapé está em `0x80011420` e a busca do Game ID pela data do volume começa em `0x800149C4`. Os endereços permitem conferir os saltos e as referências a dados na listagem MIPS.

O pseudocódigo contém nomes e tipos inferidos, além de avisos de análise. Ele serve para estudo e comparação. O projeto compilável do menu fica em [`src/menu/`](../../src/menu/).
