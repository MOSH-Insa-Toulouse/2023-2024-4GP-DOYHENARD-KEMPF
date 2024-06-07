# **4A GP - Projet "Du capteur au banc de test" - I4PMH21**

## **Sommaire**
+ [Objectif du projet](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#objectif-du-projet)
+ [Livrables](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#livrables)
+ [LTSpice](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#ltspice)
+ [KiCad](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#kicad)
+ [Shield](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#conception-du-shield)
+ [Code Arduino](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#code-arduino)
+ [Application Android](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#application-bluetooth-pour-android)
+ [Datasheet](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF?tab=readme-ov-file#datasheet-du-capteur-graphite)

## **Objectif du projet**
Dans le cadre d'un cours dispensé lors du 2<sup>nd</sup> semestre de 4<sup>ème</sup> année de Génie Physique à l'INSA de Toulouse, il nous a été proposé de réaliser un capteur low-tech à base de graphite, puis d'en faire une analyse critique argumentée pour en cerner les potentialités mais également pour évoquer des solutions d’amélioration. Ainsi, ce projet nous a permis de balayer l’ensemble du domaine : du capteur jusqu’à la réalisation d’une datasheet et du banc de test.



## **Livrables**
* Un shield PCB branché à une board Arduino UNO sur lequel nous retrouverons différents composants tels que :
  - Un capteur graphite
  - Un circuit d'amplification transimpédence
  - Un module Bluetooth
  - Un encodeur rotatoire
  - Un potentiomètre digital (_qui vient remplacer la résistance R2 du circuit d'amplifiaction_)
  - Un capteur de contrainte commercial

* Une simulation LTSpice du circuit transimpédence

* Un fichier KiCad du shield avec l'ensemble des composants cités en amont

* Un code Arduino qui gère le fonctionnement et les communications des 2 à 6 avec la board

* Un fichier APK Android (_conçue à l'aide du site MIT APP Inventor_) qui permet, à partir d'un smartphone Android, de gèrer l'interface avec le shield Arduino UNO par le biais d'une communication Bluetooth

* La datasheet du capteur graphite



## **LTSpice**
Nous avons réalisé une étude de notre circuit à l'aide du logiciel _LTSpice_. En effet, le capteur étudié ayant une résistance de l'ordre du GOhm, le courant délivré est alors trop faible (de l'ordre du nA). Pour qu'il puisse être exploitable, ce signal en courant doit donc être **filtré** de tout bruit parasite puis **amplifié**. C'est précisément le rôle du montage suivant :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/LTSpice/Photos/Screen_Circuit_Ampli_LTSpice.png)
_Circuit d'amplification/atténuation_
![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/LTSpice/Photos/Screen_Capteur_LTSpice.png)
_Modélisation du capteur_

Ce montage se compose de 3 filtres passe-bas distincts :
- Un 1<sup>er</sup> formé par R<sub>5</sub>, C<sub>1</sub> et R<sub>1</sub>, filtrant les bruits en courant sur le signal d'entrée causés par l’alimentation 5V symbolisé par le 'SINE' + C<sub>3</sub>
- Un 2<sup>nd</sup> formé par C<sub>4</sub> et R<sub>3</sub>, pour réduire la composante de bruit à 50 Hz induite par réseau électrique ambiant
- Un 3<sup>ème</sup> formé par R<sub>6</sub> et C<sub>2</sub>, placé à la sortie de l'amplificateur qui atténue le bruit intrinsèque du circuit

Grâce à ce montage, nous sommes capable de mesurer la résistance du capteur graphite en suivant la formule suivante : $`R_{Meas} = \frac{V_{CC}}{V_{ADC}}*R_1*(1+\frac{R_3}{R_{potentio}})-R_1-R_5`$

Afin de nous assurer que notre circuit jouait bien le rôle de filtreur/amplificateur, nous avons réalisé 2 simulation : une qui vérifiera la correcte amplification du signal en sortie de circuit, l'autre qui permettra de confirmer la bonne atténuation du signal pour des fréquences non souhaitées :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/LTSpice/Photos/Simu_Ampli_1V.png)

Nous voyons ici que le signal de sortie est de 1V, soit un ordre de grandeur tout à fait mesurable et donc exploitable dans notre cas.

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/LTSpice/Photos/Simu_Atténuation.png)

Ici, nous avons simulé un courant alternatif grâce auquel il nous sera possible d'observer la réduction du bruit : Nous observons une atténuation d'environ 46 dB à 50 Hz. Le comportement global du circuit est bel est bien passe-bas.

--> La simulation LTSpice est donc conforme aux attentes.



## **KiCad**
Afin de construire un shield PCB qui respecte à la fois les consignes mentionnées dans le document _INSTRUCTIONS for the UF “From the sensor to the test bench” – I4PMH21_ ainsi que les contraintes électroniques, mécaniques et physiques des composants qu'il supportera, il nous a fallu élaborer une maquette virtuelle de ce dernier via le logiciel _KiCad_. Nous avons donc dans un 1<sup>er</sup> temps réalisé une 'schematic' de l'ensemble des composants impliqués :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/KiCad/Photos/Screen_SCHEMATIC_Projet_Capteur.png)

Nous avons en suite utilisé la partie _PCB_ du logiciel afin de disposer les différents composants sur le shield, en tenant compte de la contrainte liée aux masses, qui doivent être réliées indirectement entre elles via le plan de masse. Nous sommes arrivés au résultat suivant :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/KiCad/Photos/Screen_PCB__Projet_Capteur.png)

Nous avons également effectué un rendu 3D de notre shield afin d'en avoir un 1<sup>er</sup> aperçu 'réaliste', dont voici un cliché pris du dessus :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/KiCad/Photos/Screen_3D_vue_haut_PCB_Projet_Capteur.png)

## **Conception du shield**
Après validation de notre travail sur _KiCad_ par nos responsables, nous avons procédé à la confection réelle de notre shield, fabriqué à partir d'une plaquette d’époxy recouverte d’une fine couche de cuivre.
Les principales étapes de fabrication sont :
- Une insolation aux UVs de la plaquette grâce au calque modélisé dans la partie PCB de _KiCad_.
- Le retrait de la résine non isolée à l'aide d'un révélateur.
- Le placement de la plaquette dans une solution de percholorure de fer.

A la suite de cela, avons pu assembler les composants sur la plaquette. Pour cela, il a fallu respecter les consignes à appliquer que ce soit au niveau du perçage de la plaquette ou bien du soudage des composants :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/KiCad/Photos/Photo_Shield_Face_Avant.jpg)
![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/KiCad/Photos/Photo_Shield_Face_Arriere.jpg)

> [!NOTE]
> Nous avons finalement décidé de ne pas nous servir de Servomoteur du fait d'un manque de temps.



## **Code Arduino**
En parallèle de la partie _KiCad_, nous nous sommes attelés à la programmation du **[code Arduino](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/Code%20Arduino/Code_V_Finale.c)** qui permettra la commuication de chacun des composants entre eux mais aussi avec l'Arduino UNO, ainsi qu'avec un smartphone via l'application Android.



## **Application Bluetooth pour Android**
Nous avons réalisé une **[application Android](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/tree/main/Application%20Android)** à l'aide du site _MIT App Inventor_. Cette application permet de lire les valeurs des capteurs en temps réel, mais aussi de les stocker pour établir des courbes de variation de résistance en fonction de la déformation qui leur est appliquée : 

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/Application%20Android/Screen_App_Android.PNG)



## **Datasheet du capteur graphite**
La rédaction de la datasheet induit plusieurs tests préalables de nos capteurs. Pour ce faire, nous nous sommes servis d'un banc de test imprimé en 3D, composé de 7 demi-disques allant de 1cm à 5cm de diamètre. Les capteurs quant à eux, ont tous les mêmes dimensions, mais sont usinés avec différents type de crayons à papier, allant du H au B en passant par le F et comprenant nombre de leur dérivées (HB, 2H, 4H, 4B et 6B dans notre cas) :

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/Datasheet/Variation_durete_crayon.jpg)

![alt text](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/Datasheet/Photo_Banc_de_test_et_Sensors.jpg)

Nous avons ainsi effectué plusieurs mesures pour chaque type de crayon, ainsi que pour le flex sensor commercial avec lequel nous comparerons nos résultats : 
- Mesure de résistance à plat
- Série de mesure de résistance avec une déformation en **extension** en suivant certains demi-disques sélectionnés.
- Série de mesure de résistance avec une déformation en **compression** avec les mêmes demi-disques.

> [!NOTE]
> Des valeurs incohérentes ont été observées pour certains capteurs. Nous avons donc décidé de ne pas les retenir dans nos résultats finaux.

--> Nous avons pu par la suite établir des graphes montrant la variation relative de résistance des différents capteurs en fonction de la déformation subie (compression ou extension, à divers degré d'intensité) que l'on retrouve dans la **[datasheet](https://github.com/MOSH-Insa-Toulouse/2023-2024-4GP-DOYHENARD-KEMPF/blob/main/Datasheet/%5BDataheet%5D%20-%20Pencil-Drawn%20Strain%20Gauge.pdf)** de nos capteurs graphite.

> [!WARNING]
> Les mesures ayant été difficiles, du fait d'une faible reproductibilité induite par des paramètres variants (densité de graphite au cours du temps, faux-contact électrique détecté en fin de prise de mesures), les résultats en valeurs absolues n'ont pas répondu à nos exigeances. Il nous a donc fallu nous appuyer sur l'aide de [collègues INSAïens](https://github.com/MOSH-Insa-Toulouse/2022-2023-Conan-Pistre) de la promo 2024, sans qui les courbes de mesures ne figureraient pas.
