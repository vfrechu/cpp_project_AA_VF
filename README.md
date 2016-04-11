# cpp_project_AA_VF
![Master 203](http://www.antadis.com/images/stories/references/master-203.jpg)
## This is the C++ project of Ahmed Alia and Valentin Frechu for the M203 CPP.
Our goal is to produce an Obj loader and renderer.
The full outline of this project is available [here](https://github.com/kbok/203-finals/blob/master/obj.md "Github")


Nous n'avons pas pu prendre en compte les différents groupe (object, group, material etc...) et nous nous sommes donc restreint à des .obj relativement simple. Nous vous en avons fourni quelques uns dans notre projet.

D'autre part, pour la projection sur le plan, nous avons trouvé une matrice de passage mais qui ne donnait pas de résultats concluant nous avons donc décidé d'utiliser une fonction de projection inclue dans la librairie glm.

Enfin la méthode glm::normalize fourni parfois des erreurs suite à des calculs vectoriels. Les approximations font que le résultat sorte des bornes de définition des fonctions trigonométriques [-1,1] et renvoi donc une erreur nan(ind).

le fichier "Final Project Download Here" contient le projet ainsi que sont zip (au cas où)

N'oubliez pas de bien spécifier le path pour la librairy glm (contenu dans le projet)

Bonne reception, 
Merci pour tout Ahmed et Valentin