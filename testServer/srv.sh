#!/bin/bash

DATE=$(date +%Y%m%d%H%M%S)
rm auth.db
COMPAGNY="srv"
mkdir "$COMPAGNY"
ln -s "$COMPAGNY"/auth.db auth.db


kill -9 $(pidof rsmp-gateway);clear;../Build/cache/x86_64/rsmp-gateway --fhs-all=. -S -l SUPERVISION_SVMS -p CLI3NT_01 --disable-challenge 2>&1|tee "$COMPAGNY"/log-"$COMPAGNY"."$DATE".log |grc cat