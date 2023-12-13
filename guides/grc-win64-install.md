---
layout: page
title: "Instalacao GnuRadio Windows"
permalink: /guides/grc-win64-install/
---

# Instalação GNU Windows x64 
Criado por: Paulo Dutra

### Passo 1:

Baixe e instale a versão do Radioconda para seu sistema operacional mais detalhes no [Github do Miniconda](https://docs.conda.io/projects/miniconda/en/latest/index.html):\
Link de Download [Radioconda-windows-x86_64.exe](https://glare-sable.vercel.app/ryanvolz/radioconda/radioconda-.*-Windows-x86_64.exe)

### Passo 2:

Inicie o shell do conda:\
Na barra de pesquisa do windows procure por Anaconda ou Radioconda o ícone deve ser algo semelhante a imagem abaixo:\
![conda_win64](/assets/img/conda_icon.png)

### Passo 3:

Instalar alguns pacotes no gnuradio (provavelmente já estão instalados...)
```
conda install gnuradio-uhd
conda install gnuradio-soapy
conda install gnuradio-satellites
conda install gnuradio-osmosdr
conda install rtl-sdr
conda install soapysdr-module-rtlsdr
```

### Passo 4 (Opcional, Mas Necessário para o LoRa!!):

Instalar a biblioteca gr-lora-sdr no gnuradio
```
conda install -c tapparelj -c conda-forge gnuradio-lora_sdr
```
### Passo 5:

Executar o Gnuradio

Via atalho do windows:\
Procure o ícone do GnuRadio na Área de Trabalho e clique 2 vezes.\
![gnu_icon](/assets/img/grc_icon_win64.png)

Via linha de comando:\
Execute o Radioconda / Anaconda e digite:
```
conda activate gnuradio
gnuradio-companion
```
