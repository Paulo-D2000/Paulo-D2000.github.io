---
layout: post
title:  "Demodulando o satélite MicroRad (μRad)!"
date:   2023-12-13 17:35:00 -0300
author: Paulo Dutra
categories:
  - pt-BR
tags:
  - lora
  - satellites
  - iq
  - sdr
---
A nossa equipe de competição de pequenos satélites, [NoizOrbita](https://www.instagram.com/noizorbita/?hl=en), parte do Labóratório Integrado de Sitemas Espaciais (LISE) situado no Campus Alto Paraopeba da Universidade Federal de São João Del Rei [(UFSJ-CAP)](https://www.ufsj.edu.br/cap/) participou da 2 edição da Olimpiada Brasileira de Satélites [(OBSAT)](https://www.obsat.org.br) e foi premiada em 1 lugar no nível 3 da fase 3 da competição prática! Ganhamos um lançamento do nosso satélite (MicroRad) num balão extratosférico que ocorrerá no dia 16 de Dezembro na Universidade Federal de São Carlos (UFSCAR) a janela de lançamento é das 08:00 as 10:00 da manhã!
![microrad_patch](/assets/img/patch_urad_hd.png)\
Patch da missão MicroRad (μRad)!

O satélite MicroRad (μRad, μ = Micro, Rad = Radiação) é um cubesat 1U, possuindo as dimensões de 10x10x10cm e foi projetado para transportar 2 missões:\
Um experimento de Radiação que usa fotodiodos de silício para detectar e contar partículas ionizantes alfa, beta e gamma.

Um experimento de Microbiologia que investiga a presença de de micro-organismos na extratosfera com uma camara limpa que será aberta por radiocomando para capturar os micro-organismos e fechada antes do satélite retornar ao solo.\
![microrad_obsat](/assets/img/obsat_microrad.jpg)\
Foto do satélite na Obsat com as medalhas recebidas!

Convidamos os Radioamadores a fazer parte da missão gravando o sinal (IQ) recebido e demodulando os pacotes do nosso beacon de telemetria, assim acompanhando o status do satélite e da carga útil em tempo real! Estaremos enviando certificados para os operadores que fizerem a gravação IQ do sinal ou registro dos pacotes de telemetria e enviarem no nosso e-mail [noizorbita@ufsj.edu.br](noizorbita@ufsj.edu.br).

O μRad possui um beacon de telemetria usando a modulação LoRa e transmite os dados do satélite e da carga útil (experimento de Microbiologia)  a cada 30 segundos na faixa de 70cm radioamadora (UHF), mais especificamente em 434MHz, a estrutura do pacote no formato de texto é a seguinte:

| Call | Temp | Alt | Pres | Acc_x | Acc_y | Acc_z | Gyro_x | Gyro_y | Gyro_z | Bat % | Payload | Mode | Counter |

Call ----> Indicativo de chamada (PU4THZ)
Temp ----> Temperatura em C (Graus Celsius)\
Alt -----> Altitude em metros acima do nível do mar\
Pres ----> Pressão bruta no sensor\
Acc_x ---> Dados do Acelerômetro (Eixo X) rotação em metros por segundo ao quadrado\
Acc_y ---> Dados do Acelerômetro (Eixo Y) rotação em metros por segundo ao quadrado\
Acc_z ---> Dados do Acelerômetro (Eixo Z) rotação em metros por segundo ao quadrado\
Gyro_x --> Dados do Giroscópio (Eixo X) rotação em graus por segundo\
Gyro_y --> Dados do Giroscópio (Eixo Y) rotação em graus por segundo\
Gyro_z --> Dados do Giroscópio (Eixo Z) rotação em graus por segundo \
Bat % ---> Nível da bateria em Porcento {0 a 100} \
Payload -> Estado da carga útil [OPEN = Camara Aberta, CLOSE = Fechada] \
Mode ----> Modo de operação [INIT, NOMINAL, SAFE, PAYLOAD, SHUTDOWN] \
Counter -> Contador de pacotes enviados


Para demodular o beacon de telemetria sugerimos usar o programa [GnuRadio](https://www.gnuradio.org) e os blocos da biblioteca [Gr-Lora-SDR](https://github.com/tapparelj/gr-lora_sdr) vamos disponibilizar alguns tutoriais de instalação para os sitemas operacionais:

Guia de Instalação [Windows 64bits](/guides/grc-win64-install) \
Guia de Instalação [Linux 64bits](/guides/grc-linux-install) \
Guia de Instalação [MacOs](/guides/grc-mac-install)

Basta clicar no nome do sistema e seguir os passos para instalar e configurar o programa, um guia geral (em ingles) pode ser encontrado no link [GnuRadio CondaInstall](https://wiki.gnuradio.org/index.php/CondaInstall).

Também é necessário um SDR, o popular RTL-SDR deve ser usado, pois esta é a configuração padrão do flowgraph.

Depois de instalado o programa baixe o flowgraph seguinte: [demod_lora.grc](/assets/code/grc/demod_lora.grc) em seguida execute o GnuRadio digitando ```gnuradio-companion``` no terminal onde foi instalado o conda ou usando o atalho no Windows.\
![win64_icon](/assets/img/grc_icon_win64.png)\
Atalho do GnuRadio no Windows.

Navegue até o menu File (Arquivo) e clique na opção Open (Abrir), localize onde foi baixado o arquivo demod_lora.grc e carregue o arquivo clicando em Open. Abaixo estão algumas imagens do processo:\
![grc_menu](/assets/img/grc_menu_open.png)\
![grc_menu](/assets/img/grc_file_open.png)

Você verá uma tela similar a esta imagem:\
![grc_loaded](/assets/img/grc_loaded.png)\
Alguns detalhes importantes: O módulo SDR usado é o Soapy-RTLSDR-Source, isto siginifica que você deve usar o SDR RTL-SDR (serve qualquer versão, necesita ter os drivers instalados, siga o tutorial no [blog rtlsdr](https://www.rtl-sdr.com/rtl-sdr-quick-start-guide/)). É possivel usar outros SDR's modificando o arquivo, entre em contato caso necessite por favor. O botão na barra superior que foi marcado com um quadrado vermelho executa o arquivo e inicia a recepção dos pacotes!

Ao executar o arquivo você verá a seguinte tela mostrando o espectrograma (waterfall ou "cachoeira") do sinal recebido:\
![waterfall](/assets/img/grc_waterfall.png)

Ao abrir o terminal em que o gnuradio está rodando (terminal normal no linux, CMD no windows) as mensagens serão similares a imagem abaixo, é neste terminal onde os pacotes demodulados serão exibidos, é possivel exibir o temrinal e a cachoeira 'lado a lado' como demonstra a segunda imagem.
![cmd](/assets/img/grc_console.png)\
Terminal do windows.

![cmd_side_watfll](/assets/img/grc_side_console.jpg)\
Vista lado á lado do terminal e do espectrograma mostrando pacotes demodulados.

Em caso de dúvidas ou problemas, por favor entre em contato pelas nossas redes sociais: [Instagram Noizorbita](https://www.instagram.com/noizorbita/?hl=en), links aqui no site, direto via meu telegram: [Paulo Dutra PU4THZ](https://web.telegram.org/k/#@Dutracgi) ou pelo [Telegram Joao Polito PU4JPP](https://web.telegram.org/k/#@jp_polito). Obrigado!
