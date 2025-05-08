# RSMP_DISP_PROJECT

## 📌 Description

Ce projet permet d’encoder et de décoder des messages RSMP (Road Side Message Protocol) pour un PMV (Panneau à Messages Variables) selon les spécifications SXL.  
Les messages sont convertis entre structures C et objets JSON à l’aide de la bibliothèque `json-c`.

---

## 👤 Auteur

- **Nom** : KITEZE  
- **Prénom** : Donald  
- **Date de création** : 2025-03-27  

---

## 📁 Arborescence du projet

```bash
RSMP_DISP_PROJECT/
│
├── bin/                                # (Optionnel) Fichiers exécutables générés
│
├── include/                            # Fichiers d’en-tête
│   ├── disp_json_generic.h             # Interface générique d'encodage/décodage JSON
│   ├── disp_m31.h                      # Interface message DISP/M31
│   ├── disp_m32.h                      # Interface message DISP/M32
│   ├── disp_m40.h                      # Interface message DISP/M40
│   ├── disp_m41.h                      # Interface message DISP/M41
│   ├── disp_s10.h                      # Interface message DISP/S10
│   ├── disp_s20.h                      # Interface message DISP/S20
│   ├── disp_s30.h                      # Interface message DISP/S30
│   ├── disp_s31.h                      # Interface message DISP/S31
│   └── disp_s40.h                      # Interface message DISP/S40
│
├── src/                                # Fichiers d’implémentation
│   ├── sxl_json.c                      # Implémentation générique JSON
│   ├── sxl_m31.c                       # Implémentation pour le message M31
│   ├── sxl_m32.c                       # Implémentation pour le message M32
│   ├── sxl_m40.c                       # Implémentation pour le message M40
│   ├── sxl_m41.c                       # Implémentation pour le message M41
│   ├── sxl_s10.c                       # Implémentation pour le message S10
│   ├── sxl_s20.c                       # Implémentation pour le message S20
│   ├── sxl_s30.c                       # Implémentation pour le message S30
│   ├── sxl_s31.c                       # Implémentation pour le message S31
│   └── sxl_s40.c                       # Implémentation pour le message S40
│
├── test/                               # Tests unitaires
│   ├── test_disp_m31.c
│   ├── test_disp_m32.c
│   ├── test_disp_m40.c
│   ├── test_disp_m41.c
│   ├── test_disp_s10.c
│   ├── test_disp_s20.c
│   ├── test_disp_s30.c
│   ├── test_disp_s31.c
│   └── test_disp_s40.c
│
├── Makefile                            # Script de compilation
└── README.md                           # Ce fichier
