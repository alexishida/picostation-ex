# Validação e solução de problemas

## Verificações de build

Use um diretório de build por variante e confira a geração do UF2 correspondente. Para alterações em código compartilhado, as regras do projeto pedem compilação das quatro variantes básicas. Alterações em pinout, temporização, leitura do SD ou protocolo do menu também exigem validação no PlayStation com a placa e a variante afetadas. Registre modelo do console, revisão de placa, variante, cartão, imagem usada e resultado.

A qualificação de release pode abranger os quatro perfis PU-8 e PU-18 e o pacote Pico Booty descritos no [guia de build](build.md). A presença das especificações OpenSpec não demonstra que os testes foram executados.

## Cenários para testar no console

- Navegação com pasta vazia, nomes longos, listagem paginada e retorno ao diretório pai.
- Cartão ausente ou removido, reinserção e nova tentativa com Retry.
- CUE/BIN válido Mode 1 e Mode 2, disco somente de áudio, BIN ausente, arquivo truncado e falha de leitura.
- Boot rápido e normal de jogos reconhecidos, incluindo identificação de Game ID.
- Capas indexadas e RGB555, entradas sem capa e `config.ini` presente ou ausente.
- Para cada perfil relevante, reset, sinais e clock da montagem PU-8 ou PU-18 correspondente.

## Diagnóstico

| Sintoma | Verificação inicial |
| --- | --- |
| CMake não encontra o SDK | Defina `PICO_SDK_PATH` ou instale o SDK em `third_party/pico-sdk`; veja [build](build.md). |
| Menu não compila | Confira CMake 3.25+, toolchain MIPS, Python e `mkpsxiso`; para o pacote, confira também `ps1-packer`. |
| Configuração rejeita o menu precompilado | Confira se `PICOSTATION_MENU_IMAGE` aponta para um arquivo existente, não para um diretório. |
| Cartão ou listagem indisponível | Confira cartão, conexão e formato; tente Retry após reinseri-lo. |
| Jogo não abre | Confira caminhos no CUE, arquivos BIN e setores raw de 2352 bytes. A validação não detecta toda forma de truncamento. |
| Capa ou cores não aparecem | Confira nome e localização do `.cov`, tamanho/formato da capa e `config.ini` na raiz. |

Não há resultados de builds ou testes físicos registrados por esta documentação. As especificações podem ser verificadas com `openspec validate --all --strict` quando a ferramenta estiver instalada.

## Aprendizados do boot na PU-18

Em uma PU-18 sem overclock, o firmware 1.0.0 mostrou o logo do PlayStation e voltou ao navegador de Memory Card. Na mesma montagem e com o mesmo cartão, o rc5 original abriu o menu. Reduzir o executável do menu para o tamanho do antigo não corrigiu o sintoma, e o firmware novo com a imagem de menu do rc5 também voltou ao navegador.

Foi identificado um estouro de pilha no núcleo 1. No build examinado, a entrada de `I2S::start` consumia 5.872 bytes com apenas 2.048 bytes reservados. Uma troca de objetos `DiscImage` gerava um temporário grande na pilha. A transferência da imagem preparada passou a evitar esse temporário. Ao alterar objetos grandes, confira o quadro da função no disassembly e a reserva da pilha no mapa do linker, incluindo as chamadas feitas por ela. A pilha configurada não garante por si só que toda a cadeia de chamadas caiba nela.

A preparação do endereço DMA antes da espera pela borda de LRCK e o disparo imediatamente após a borda também fizeram parte dos firmwares posteriores. O log de diagnóstico v2 registrou a transferência dos setores do executável e um pedido `GET_CFG`, mas gravou o relatório no SD antes de responder ao comando. O menu permaneceu em `Please Wait Loading...`. Na v3, o relatório foi adiado, o menu EX foi compilado com o firmware e o usuário confirmou a abertura do menu e o carregamento de um jogo na PU-18. Esses testes identificam uma combinação funcional, mas não isolam a contribuição de cada alteração para o boot.

Para investigar regressões semelhantes, compare na mesma montagem com um firmware conhecido que funcione. Verifique a pilha, o sincronismo de LRCK e o caminho de leitura, e compile o menu e o firmware a partir de fontes compatíveis. Um contador de disparos DMA mostra o que o Pico tentou enviar. A execução do menu e o carregamento do jogo devem ser confirmados no console. Evite gravações extensas no SD enquanto o núcleo 1 precisa responder ao PlayStation.

Para capas com faixas ou cores incorretas, teste arquivos indexados e RGB555, incluindo alternância entre jogos e entre imagens de capa e disco. Verifique a conclusão do DMA do CD e do comando de pausa antes de consumir cada parte, a atualização da textura e da paleta na VRAM e o cache da GPU. A correção desses pontos foi compilada e testada com dados de capas no computador, mas a exibição do release ainda depende de confirmação no console.
