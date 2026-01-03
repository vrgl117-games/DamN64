FROM ghcr.io/dragonminded/libdragon
ARG LIBDRAGON_COMMIT 9926cc32e384d58c74b609733c7b50457d811732 #11/22/25

RUN apt-get update -yq

RUN cd /tmp && git clone https://github.com/DragonMinded/libdragon.git && cd libdragon && git reset --hard $LIBDRAGON_COMMIT && make install && make tools-install

WORKDIR /game
