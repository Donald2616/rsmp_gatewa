#!/bin/bash

DATE=$(date +%Y%m%d%H%M%S)
rm auth.db
COMPAGNY="local"
mkdir "$COMPAGNY"
ln -s "$COMPAGNY"/auth.db auth.db

clear;../Build/cache/x86_64/rsmp-gateway --fhs-all=. -f 127.0.0.1:12111 -l CLI3NT_01 -p SUPERVISION_SVMS --disable-challenge 2>&1|tee "$COMPAGNY"/log-"$COMPAGNY"."$DATE".log|grc cat