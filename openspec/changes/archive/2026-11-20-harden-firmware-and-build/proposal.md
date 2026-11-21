# Proposal

## Why

O núcleo 0 usa o limite de setores e os metadados do disco enquanto o núcleo 1 pode trocar a imagem ativa. O núcleo 1 também lê GFS sem sincronização com quem atualiza o sensor. A listagem contabiliza dois bytes extras no terminador, e o pacote de release invoca quatro builds independentes do mesmo menu.

## What Changes

- Publicar a imagem ativa, seus metadados para geração de subcode Q e o limite de setores de forma coordenada na montagem, na troca de disco e no retorno ao menu.
- Sincronizar a leitura de GFS pelo núcleo 1 e preservar o acesso local do núcleo 0 no caminho crítico.
- Contabilizar os quatro bytes efetivamente gravados pelo terminador de listagem.
- Construir o menu a partir dos fontes uma vez no pacote de release, reutilizar seus artefatos em todos os perfis e habilitar o build paralelo.
- Corrigir a transferência da fonte, a transição inicial, a releitura de listagens e o log por sessão.
- Investigar o formato e o atraso do subcode Q com referência de hardware. Revisar também o latch compartilhado entre IRQs e o tratamento de setores negativos em `sectorToMSF`.

## Capabilities

### New Capabilities

### Modified Capabilities

- `disc-playback` passa a exigir que o limite de setores e o subcode Q correspondam à mesma imagem durante transições de disco.

## Impact

- A sincronização afeta a mecânica, o alarme do subcode, a montagem de imagens, o modchip e o leitor I²S. Validar as transições no console e medir a latência nos perfis com overclock.
- A serialização da listagem e o empacotamento continuam sujeitos aos contratos de `sd-navigation` e `firmware-build`. O pacote contém os quatro UF2 ODE e o UF2 do Pico Booty.
