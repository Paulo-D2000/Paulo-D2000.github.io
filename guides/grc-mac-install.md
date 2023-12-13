---
layout: page
title: "Instalacao GnuRadio MacOS"
permalink: /guides/grc-mac-install/
exclude: true
---

# Instalação GNU macOS 
Criado por: João Polito

### Passo 1:

Baixe a versão do Miniconda para seu sistema operacional no meu caso foi o seguinte:
```
https://repo.anaconda.com/miniconda/Miniconda3-py310_23.3.1-0-MacOSX-arm64.sh
```

### Passo 2:

Execute o arquivo via terminal da seguinte forma:
```
Cd Downloads
bash Miniconda3-py310_23.3.1-0-MacOSX-arm64.sh
```
E siga as Instruções.
No final aparecerá onde o  Miniconda3 será instalado: ```/Users/joao/miniconda3```


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
