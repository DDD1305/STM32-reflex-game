# Généralités
Ce dépôt permet de développer de petits projets utilisant la biliothèque
libopencm3. L'organisation est dérivée du projet libopencm3-template
https://github.com/libopencm3/libopencm3-template.git

# Instructions
 1. cloner ce dépôt Git
 2. aller dans le sous-répertoire contenant votre projet
 3. utiliser une des commandes suivantes :
    - make # contruit le projet (et la bibliothèque libopencm3 initialement)
    - make load # ou make flash : envoie le projet sur la carte STM32
    - make gdb # démarre le projet sous gdb (en remote sur la carte)
    - make clean # efface les fichiers construits pour le projet
    - make distclean # efface en plus la compilation de libopencm3

# Répertoires pour le premier TP
* libopencm3 contient la bibliothèque permettant de programmer la carte STM32
* common/ contient des fichiers pouvant être utilisés dans plusieurs projets
* blink/ est un petit projet montrant comment faire clignoter une LED
  Chaque partie de ce petit programme est écrite avec différentes variantes.
  On va de la version avec les constantes codées en dur dans le code
  jusqu'à la version où on utilise au maximum les petites fonctions de
  libopencm3.
  En cours d'apprentissage, la version intermédiaire est recommandée :
  - accès explicite aux registres mais en utilisant les constantes fournies
    par libopencm3 (la doc ST suffit alors)
  Si vous souhaitez utiliser la version avec les petites fonctions de
  libopencm3, il faudra aussi utiliser la doc de libopencm3 pour découvrir
  l'existance et la syntaxe de ces fonctions.
* TP1/ où vous trouverez un point de départ pour le premier TP
