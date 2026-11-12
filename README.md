# Jeu de réflexes sur STM32

Projet universitaire réalisé en binôme à **Polytech Grenoble**, dans le cadre des enseignements de programmation bas niveau.

## Présentation

L'objectif du projet était de développer un jeu de réflexes sur une carte **STM32F446RE**, en utilisant le langage C et les périphériques du microcontrôleur.

Le programme propose trois essais au joueur et mesure son temps de réaction.

## Fonctionnalités

- Communication série UART avec un terminal.
- Gestion des entrées utilisateur.
- Mesure du temps avec SysTick.
- Utilisation d'interruptions matérielles.
- Pilotage d'un buzzer avec PWM.
- Gestion du déroulement du jeu et des scores.

## Aspects techniques

**Langage :** C  
**Microcontrôleur :** STM32F446RE  
**Bibliothèque :** libopencm3  
**Outils :** GCC, Make, GDB

Le projet utilise notamment des accès directs aux registres matériels et une transmission UART asynchrone pilotée par interruptions.

## Organisation

Le code de notre application se trouve dans le répertoire `Projet/`.

Le dépôt contient également les fichiers et exemples fournis dans le cadre des travaux pratiques.

## Compilation

Depuis le répertoire `Projet/` :

```bash
make
```

Pour programmer la carte :

```bash
make flash
```

La compilation nécessite l'environnement de développement ARM adapté et les outils de programmation de la carte.

## Auteurs

Projet réalisé en binôme par :
- Gabriel Deruda
- Antoni Maitre

Polytech Grenoble — Année universitaire 2025-2026.
