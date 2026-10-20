## NTRU-HRSS-C

### Description

Ce dépôt est un dépôt personnel d'implémentation de NTRU-HRSS-Encrypt et NTRU-HRSS-KEM en C, voulant répondre aux standards d'implémentation définis par la soumission à la finale du NIST de la compétition de standardisation PQC (2020-2022). Le but est d'avoir, à terme, une implémentation rigoureuse et sécurisée.

### État actuel

Au dernier commit, ceci est une implémentation générique fonctionnelle et internalement cohérente de NTRU-HRSS-Encrypt (la décryption/encryption fonctionnent, de même pour l'encodage/décodage, il n'y a pas le KEM encore). Cependant elle n'est pas encore conforme aux méthodes de la soumission au NIST et ainsi la sortie ne correspond pas aux réponses aux KAT. Le but prochain est de faire en sorte de pouvoir valider les KAT (pour pk et ct qui sont possibles à valider sans le KEM). 
Aucun effort d'optimisation du code n'a été encore effectué.

### Utilisation actuelle

- make
- ./test <c>

produit le fichier PQCkemKAT_1450.ans contenant une réponse au test c des KAT du NIST contenus dans PQCkemKAT_1450.req. Une utilisation répétée écrit la nouvelle réponse dans ce même fichier à la suite.

### Dépendances externes incluses

- rng.c provenant de l'implémentation de référence du NIST.
- aes.c provenant de tiny-AES-c.
- Fichiers pour Keccak provenant de l'implémentation standard FIPS PUB 202 du NIST.
