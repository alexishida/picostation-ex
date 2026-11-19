# PicoStation EX

O PicoStation EX é um emulador de unidade óptica (ODE) para o PlayStation original, desenvolvido para Raspberry Pi Pico e Pico 2. O projeto foi baseado no [PicoStation de megavolt85](https://github.com/megavolt85/PicoStation).

O firmware emula o leitor de CD do console a partir de imagens de disco armazenadas em um cartão SD. O menu integrado roda no próprio PlayStation e permite navegar por pastas e iniciar jogos com o controle do console.

## Compatibilidade

| Placa do PlayStation | Série do console |
| --- | --- |
| PU-8 | SCPH-100x |
| PU-18 | SCPH-55xx |

Escolha o firmware que corresponde à sua montagem. As variantes PicoStation e PicoStation Plus têm seus próprios mapas de pinos em [`boards/`](boards/).

| Variante de compilação | Microcontrolador |
| --- | --- |
| `picostation_pico1` | RP2040 (Pico) |
| `picostation_pico2` | RP2350 (Pico 2) |
| `picostation_plus_pico1` | RP2040 (Pico) |
| `picostation_plus_pico2` | RP2350 (Pico 2) |

## Cartão SD e menu

Use um cartão SD em **exFAT**. Organize as imagens de disco em arquivos `.cue` e `.bin` e mantenha os nomes e caminhos dos arquivos `.bin` de acordo com as referências no `.cue`. O menu lista as pastas e os arquivos `.cue` do cartão. As imagens devem usar setores raw de 2352 bytes. Antes de iniciar o jogo, o firmware valida o CUE, os BINs e a assinatura PlayStation. Em caso de falha, o menu exibe um erro e retorna à seleção.

- Use **↑** e **↓** para navegar pela lista, **L1** para voltar uma página e **R1** para avançar.
- Pressione **X** para abrir a pasta selecionada ou iniciar o jogo.
- Pressione **Quadrado** para subir um nível na estrutura de pastas.

### Capas e cores

Para exibir uma capa, coloque o arquivo `.cov` ao lado do jogo ou da pasta, com o mesmo nome base. Por exemplo, `Jogos/Exemplo.cue` usa `Jogos/Exemplo.cov`, e a pasta `Jogos/RPG/` usa `Jogos/RPG.cov`. As capas têm 128 × 128 pixels indexados de 8 bits, precedidos por uma paleta de 512 bytes.

Para personalizar as cores, crie um arquivo opcional `config.ini` na raiz do cartão. As chaves `background`, `frame` e `filelist` aceitam cores RGB em hexadecimal.

```ini
background = 404040
frame = 606060
filelist = 303030
```

## Compilação do firmware

A compilação padrão usa CMake 3.25 ou superior, Pico SDK 2.1.1 (a versão usada na CI), toolchain GNU Arm `arm-none-eabi`, toolchain GNU MIPS (por exemplo, `mipsel-linux-gnu`), Python 3.10 ou superior e `mkpsxiso`. O CMake compila o menu PlayStation e incorpora a imagem gerada ao firmware.

Com as ferramentas no `PATH` e o Pico SDK em `third_party/pico-sdk/`, execute na raiz do projeto:

```sh
git submodule update --init --recursive
python3 -m pip install -r third_party/ps1-bare-metal/tools/requirements.txt
cmake -S . -B build/picostation_pico2 -DPICOSTATION_VARIANT=picostation_pico2
cmake --build build/picostation_pico2
```

O exemplo gera `build/picostation_pico2/picostation_pico2.uf2`. Para outra montagem, troque a variante e use um diretório de compilação próprio. Se o SDK estiver em outro local, passe `-DPICO_SDK_PATH=/caminho/para/pico-sdk` ao CMake. Se a toolchain MIPS estiver fora do `PATH`, passe `-DPICOSTATION_MIPS_TOOLCHAIN_PATH=/caminho/para/toolchain`.

O menu é compilado a partir dos fontes por padrão. Para reutilizar uma imagem de disco do menu já compilada, configure com `-DPICOSTATION_BUILD_MENU=OFF -DPICOSTATION_MENU_IMAGE=/caminho/para/picostation-menu.bin`. Esse modo requer CMake 3.24.1 ou superior e dispensa as ferramentas de compilação do menu. O arquivo deve existir, e o CMake verifica seu caminho durante a configuração. Caminhos relativos são resolvidos a partir da raiz do projeto.

## Organização do projeto

| Diretório | Conteúdo |
| --- | --- |
| `src/` | Inicialização do firmware em `main.cpp` |
| `src/drive/` | Comandos e controle da unidade óptica |
| `src/storage/` | Listagem do cartão SD e montagem de imagens |
| `src/hardware/` | I²S, modchip e controle do SI5351 |
| `src/disc/` | EDC e dados de subcanal Q |
| `include/` | Cabeçalhos do firmware e protocolo compartilhado com o menu |
| `boards/` | Variantes de placa e mapas de GPIO |
| `pio/` | Programas PIO |
| `cmake/` | Integração com o Pico SDK |
| `src/menu/` | Código do menu PlayStation |
| `include/menu/` | Cabeçalhos do menu |
| `assets/menu/` | Imagens, sons e arquivos para montar o disco do menu |
| `scripts/` | Scripts de compilação, ambiente Python e envio ao console |
| `third_party/ps1-bare-metal/` | Runtime e ferramentas usados pelo menu |
| `third_party/` | Dependências do firmware |
| `binary/` | Binários embarcados |
| `tools/` | Ferramentas auxiliares |
| `workspaces/` | Workspaces do VS Code para o firmware e o menu |
| `openspec/` | Especificações e planejamento de mudanças |
| `.ai-framework/` | Regras de desenvolvimento e design do menu |

O ponto de entrada da compilação é o `CMakeLists.txt` da raiz. Uma compilação gera o menu para o processador MIPS do PlayStation, incorpora sua imagem ao firmware Arm e produz o `.uf2`. A etapa MIPS fica em `cmake/menu/` e seus arquivos gerados ficam em `build/<variante>/menu/`.

Execute `sh scripts/makeimage.sh` ou `scripts\make-xiso.bat` no Windows para compilar o projeto completo. Esses scripts usam `picostation_pico2` como padrão e aceitam a variável de ambiente `PICOSTATION_VARIANT`. Os scripts `scripts/setup-python-venv.sh` e `scripts/setup-python-venv.bat` preparam o ambiente Python em `env/`. Ative esse ambiente antes de compilar.

O workspace `workspaces/picostation.code-workspace` abre todo o projeto. A origem e os créditos do menu estão em [docs/menu.md](docs/menu.md).


## Licença
- O projeto é distribuído sob a [licença Apache 2.0](LICENSE).
