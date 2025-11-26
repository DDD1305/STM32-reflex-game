Pour compiler ces projets, un cross-compilateur ARM est nécessaire.
Avec Debian ou Ubuntu, il peut être installé avec la commande :
    sudo apt install gcc-arm-none-eabi

Pour charger le programme compilé sur la carte, l'outil `openocd`
est nécessaire. Il est fourni dans le paquet du même nom :
    sudo apt install openocd

Pour que l'utilisateur courant puisse accéder à la carte, il faut :
1. qu'il soit dans le groupe plugdev
   Vous pouvez utiliser la commande suivante (**NE PAS LA LANCER EN ROOT**)
    id | grep -sq '(plugdev)' || sudo adduser $(id -un) plugdev
   Si elle affiche quelque chose, vous **devez** quitter (et rouvrir) votre
   session
2. avoir branché la carte **après** avoir installé `openocd`.
   Sinon, débranchez et rebranchez la carte

Avec tout cela, vous devriez pouvoir faire dans le répertoire `TP1` :
   make
   make load

Pour pouvoir débogguer la carte STM32 depuis votre PC, il faut
installer le paquet `gdb-multiarch` :
    sudo apt install gdb-multiarch

Une fois cela fait, vous pouvez charger et exécuter votre programme dans `gdb`
avec (toujours dans le répertoire `TP1`) :
    make gdb

