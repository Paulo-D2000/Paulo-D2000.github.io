---
layout: page
title: "Instalacao GnuRadio Linux"
permalink: /guides/grc-linux-install/
exclude: true
---

# Instalação GNU Linux x64 
Criado por: Paulo Dutra

### Passo 1:

Baixe e instale a versão do Miniconda para seu sistema operacional mais detalhes no [Site do Miniconda](https://docs.conda.io/projects/miniconda/en/latest/index.html):
```
mkdir -p ~/miniconda3
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda3/miniconda.sh
bash ~/miniconda3/miniconda.sh -b -u -p ~/miniconda3
rm -rf ~/miniconda3/miniconda.sh
```

### Passo 2:

Inicie o shell do conda:
```
~/miniconda3/bin/conda init bash
~/miniconda3/bin/conda init zsh
```

### Passo 3:

Agora você deve criar um ambiente para o Gnuradio
```
conda create -n gnuradio
conda activate gnuradio
conda config --env --add channels conda-forge
conda config --env --set channel_priority strict
```

### Passo 4:

Sera iniciado a instalação do GnuRadio
```
conda install gnuradio
conda upgrade --all
```

### Passo 5:

Instalar alguns pacotes no gnuradio
```
conda install gnuradio-uhd
conda install gnuradio-soapy
conda install gnuradio-satellites
conda install gnuradio-osmosdr
conda install rtl-sdr
conda install soapysdr-module-rtlsdr
```

### Passo 6 (Opcional, Mas Necessário para o LoRa!!):

Instalar a biblioteca gr-lora-sdr no gnuradio
```
conda install -c tapparelj -c conda-forge gnuradio-lora_sdr
```
### Passo 7:

Executar o Gnuradio
```
conda activate gnuradio
gnuradio-companion
```
