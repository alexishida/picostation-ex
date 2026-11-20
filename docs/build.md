# Compilação e releases

A compilação padrão usa CMake 3.25 ou superior, Pico SDK 2.1.1, toolchain GNU Arm `arm-none-eabi`, toolchain GNU MIPS (por exemplo, `mipsel-linux-gnu`), Python 3.10 ou superior e `mkpsxiso`. O CMake compila o menu PlayStation e incorpora a imagem gerada ao firmware.

Com as ferramentas no `PATH`, execute na raiz do projeto:

```sh
git clone --branch 2.1.1 --recurse-submodules https://github.com/raspberrypi/pico-sdk.git third_party/pico-sdk
python3 -m pip install -r third_party/ps1-bare-metal/tools/requirements.txt
cmake -S . -B build/picostation_pico2 -DPICOSTATION_VARIANT=picostation_pico2
cmake --build build/picostation_pico2
```

O exemplo gera `build/picostation_pico2/picostation_pico2.uf2`. Para outra montagem, troque a variante e use um diretório de compilação próprio. Se já tiver o SDK, dispense o clone e passe `-DPICO_SDK_PATH=/caminho/para/pico-sdk` ao CMake ou defina a variável de ambiente `PICO_SDK_PATH`. A opção `-DPICO_SDK_FETCH_FROM_GIT=ON` permite baixar o SDK durante a configuração. Se a toolchain MIPS estiver fora do `PATH`, passe `-DPICOSTATION_MIPS_TOOLCHAIN_PATH=/caminho/para/toolchain`.

### Pacote PU-8 e PU-18

Com o `ps1-packer` do [PCSX-Redux](https://github.com/pcsx-redux/pcsx-redux/tree/main/tools/ps1-packer) no `PATH`, execute:

```sh
cmake -P scripts/build-release.cmake
```

O comando reúne os cinco arquivos na raiz de `build/`. Os quatro perfis do ODE usam **RP2040 e o pinout padrão**. O sufixo `_OC` seleciona um clock fixo mais alto para o Mechacon. Nos perfis abaixo, GP3 fornece esse clock e a instalação precisa seguir o esquema RC5 da revisão correspondente, com o clock original isolado quando GP3 for ligado ao Mechacon.

| Arquivo | Montagem | Clock em GP3 |
| --- | --- | --- |
| `picostationex_PU8.uf2` | PU-8 | 3,988 MHz |
| `picostationex_PU8_OC.uf2` | PU-8 com overclock | 6,457 MHz |
| `picostationex_PU18.uf2` | PU-18 | 4,238 MHz |
| `picostationex_PU18_OC.uf2` | PU-18 com overclock | 7,533 MHz |
| `pico_booty.uf2` | Segundo RP2040 ligado à porta de expansão do PlayStation | — |

Os quatro firmwares do ODE incorporam o menu compilado da fonte. O Pico Booty empacota o mesmo executável do menu para a porta de expansão. O código do Pico Booty vem do [fork de Nicolas Noble](https://github.com/nicolasnoble/pico_booty/tree/ea96b7fcd4196d666bc90340799bbacba50d3b31) do projeto de John Baumann.

Para gerar um único firmware do ODE, use o nome correspondente da tabela, sem `.uf2`, em `-DPICOSTATION_VARIANT`.

O menu é compilado a partir dos fontes por padrão. Para reutilizar uma imagem de disco do menu já compilada, configure com `-DPICOSTATION_BUILD_MENU=OFF -DPICOSTATION_MENU_IMAGE=/caminho/para/picostation-menu.bin`. Esse modo requer CMake 3.24.1 ou superior e dispensa as ferramentas de compilação do menu. O arquivo deve existir, e o CMake verifica seu caminho durante a configuração. Caminhos relativos são resolvidos a partir da raiz do projeto.


Consulte também [arquitetura](arquitetura.md) e [uso do menu](menu.md).
