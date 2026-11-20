# Falha ao iniciar o PicoStation EX 1.0.0 na PU-18

**Estado:** aberto. O firmware 1.0.0 e a compilação de teste com o menu compacto apresentam o mesmo sintoma na PU-18. A causa ainda não foi isolada.

## Sintoma e roteiro testado

Na montagem PU-18 em questão, o usuário utilizava `picostation_PU18.uf2`, que abria o menu do PicoStation. Após gravar `picostationex_PU18.uf2`, o console passou a mostrar o logo do PlayStation e, logo depois, entrar no navegador do BIOS (Memory Card), antes de exibir o menu do PicoStation. O perfil gravado foi o PU-18 sem overclock do Mechacon.

| UF2 | Resultado informado no console | Setores do executável do menu (LBA) |
| --- | --- | --- |
| `picostation_PU18.uf2` antigo | Era usado nessa montagem e abria o menu | 24–49 |
| `picostationex_PU18.uf2` do pacote 1.0.0 | Logo do PlayStation seguido do navegador do BIOS | 24–72 |
| `picostationex_PU18.uf2` com menu compacto | Mesmo sintoma após gravar o UF2 de teste | 24–49 |

O teste com menu compacto mostra que voltar ao setor 49, por si só, não corrige a falha. Ainda não há resultado informado de uma nova gravação do UF2 antigo após a falha nem de uma partida do 1.0.0 sem cartão SD.

### Arquivos comparados no checkout

Os caminhos são relativos à raiz do repositório. Os hashes identificam as cópias locais usadas na análise.

| Caminho | SHA-256 |
| --- | --- |
| `referencia/rc5-build/picostation_PU18.uf2` | `31e0231d8abc243bec6c8a788d3d027f3af97354ae48edacb79aa1acb498b702` |
| `release/picostationex_PU18.uf2` | `d8d170c8b3438be6373071fd7a239c9595dd5853eed9698e22cae0e9cc147eae` |
| `build/pu18-menu-setor49/picostationex_PU18.uf2` | `2cbbc901f1c3238d57223a4da91d11dccb46e0594a8faa77ae5da495671ab748` |

## O que foi verificado

### Imagem de boot

- Os setores 0 a 15 da faixa de dados, que incluem a licença, são idênticos no UF2 antigo e no pacote 1.0.0. O conteúdo de `SYSTEM.CNF` também coincide. Ele aponta para `SCES_313.37;1`, presente nas duas imagens com cabeçalho `PS-X EXE`.
- O executável antigo tem 53.248 bytes. O executável original do 1.0.0 tem 100.352 bytes. O executável compacto tem 53.248 bytes e mantém o menu 1.0.0.
- A compilação de teste usa `-Os` e omite do executável o preenchimento de zeros que o código de inicialização já aplica à BSS. A comparação verificou que os dados inicializados do ELF foram preservados e que a imagem montada pelo build foi incorporada ao UF2 de teste. [Build do menu](cmake/menu/CMakeLists.txt) · [Inicialização em MIPS](third_party/ps1-bare-metal/libc/crt0.c)
- O executável compacto tem o mesmo tamanho em disco que o antigo, mas continua sendo o menu novo. O teste não isolou diferenças no código do menu ou no caminho de leitura do firmware.

### Clocks e GPIOs

- A comparação estática dos dois UF2 sem overclock encontrou a mesma configuração de clock de sistema do Pico, **271,2 MHz**, e o mesmo PWM no **GP3**, com período de 64 ciclos. Isso corresponde a **4,2375 MHz** nominais para o Mechacon. A frequência e a qualidade do sinal no console ainda não foram medidas. [Clock do Pico](src/main.cpp) · [Perfil PU-18](boards/picostationex_release.cmake)
- A tabela de inicialização coincide nos dois UF2 para XLAT (GP0), SQCK (GP1), LMTSW (GP2), SCEX (GP4), tampa (GP6), reset (GP7), CS do SD (GP9), SENS (GP14), DA15 (GP15), DA16 (GP16), LRCK (GP17), SCOR (GP18), SQSO (GP19), CLK (GP21), CMD_DATA (GP26) e CMD_CK (GP27). As configurações de PWM dos GP3, GP15, GP17 e GP21 também coincidem.
- O código atual define MISO do SD em GP8, SCK em GP10 e MOSI em GP11. Esses três sinais ficam fora da tabela comparada e só foram conferidos no código atual. [Pinout base](boards/picostation_pico1.cmake)

## Hipóteses abertas

- **Entrega de setores e subcanal Q.** O fluxo atual espera a borda de LRCK com limite de 100 µs e desiste daquela tentativa se a posição do drive mudar durante a espera. A publicação do setor em envio e o agendamento do subcanal Q também mudaram. A leitura pode falhar no BIOS mesmo com as frequências nominais e o pinout iguais. Ainda falta observar os sinais da PU-18 ou testar os caminhos separadamente. [Transferência I²S/DMA](src/hardware/i2s.cpp#L383) · [Subcanal Q](src/drive/picostation.cpp#L230)
- **Inicialização do cartão SD.** O núcleo que atende à leitura de setores tenta montar o cartão antes de entrar no laço principal. O menu está no firmware, por isso um boot sem SD pode ajudar a isolar a influência desse passo. [Inicialização do leitor](src/hardware/i2s.cpp#L144) · [Montagem do SD](src/storage/directory_listing.cpp#L48)
- **Código do menu novo ou interação com o firmware.** A presença do executável e os setores de licença iguais confirmam a estrutura examinada da imagem. Ainda não demonstram que o BIOS leu o executável até o fim ou que o menu chegou a executar no console.

## Próximos testes de isolamento

1. Regravar o UF2 antigo e testar na mesma montagem, com o mesmo SD e a tampa fechada. Registrar se o menu antigo ainda abre.
2. Testar o UF2 novo sem o SD, com o console desligado antes de retirar o cartão. Registrar se aparece `No SD Card` no menu do PicoStation ou se o console volta ao BIOS.
3. Gerar um UF2 de diagnóstico combinando o firmware atual da PU-18 com a imagem do menu extraída do UF2 antigo. Avaliar apenas se o menu abre, porque a navegação pode usar um protocolo diferente. Se o antigo completo iniciar e o híbrido falhar, priorizar o caminho de boot do firmware. Se o híbrido iniciar, priorizar as diferenças do menu e da imagem nova.
