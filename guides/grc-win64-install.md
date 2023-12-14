---
layout: page
title: "Instalacao GnuRadio Windows"
permalink: /guides/grc-win64-install/
exclude: true
---

# Instalação GNU Windows x64 
Criado por: Paulo Dutra

### Passo 1 

#### (Instalação Automática, Recomendada):

Baixe e instale a versão do Radioconda para seu sistema operacional mais detalhes no [Github do Miniconda](https://docs.conda.io/projects/miniconda/en/latest/index.html):\
Link de Download (Versão mais recente): [Radioconda-windows-x86_64.exe](https://glare-sable.vercel.app/ryanvolz/radioconda/radioconda-.*-Windows-x86_64.exe)

Link de Download (Versões antigas): [Releases Radioconda](https://github.com/ryanvolz/radioconda/releases)\
Baixe o arquivo que termina em ``Windows-x86_64.exe``, caso não apareçam os arquivos clique em ```Assets``` 

#### (Instalação Manual, Caso a automática não funcione):

Baixe e instale a versão do Anaconda ou Miniconda (mais compacto) para seu sistema operacional mais detalhes no [Github do Miniconda](https://docs.conda.io/projects/miniconda/en/latest/index.html):\
Link de Download: [Anaconda](https://www.anaconda.com/download) \
Link de Download: [MiniConda](https://docs.conda.io/projects/miniconda/en/latest/index.html)

### Passo 2:

Inicie o shell do conda:\
Na barra de pesquisa do windows procure por Anaconda ou Radioconda o ícone deve ser algo semelhante a imagem abaixo:\
![conda_win64](/assets/img/conda_icon.png)

### Passo 3:

#### Em caso de Instação Manual
Rodar estes comandos primeiro:
```
conda create -n radioconda
conda activate radioconda
conda config --env --add channels conda-forge
conda config --env --set channel_priority strict
conda install gnuradio
```

Ative o ambiente radioconda
```
conda activate radioconda
```

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
conda activate radioconda
gnuradio-companion
```
