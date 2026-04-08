# Projet Capteur Low-Cost 4A Génie Physique - Max Simonnet & Basile Vidalenc

## Sommaire
- [Projet Capteur Low-Cost 4A Génie Physique - Max Simonnet \& Basile Vidalenc](#projet-capteur-low-cost-4a-génie-physique---max-simonnet--basile-vidalenc)
  - [Sommaire](#sommaire)
  - [1. Description du projet ](#1-description-du-projet-)
    - [1.1 Caractéristiques et travaux de recherche préliminaires sur le capteur ](#11-caractéristiques-et-travaux-de-recherche-préliminaires-sur-le-capteur-)
    - [1.2 Cours et formation ](#12-cours-et-formation-)
  - [2. Livrables du projet ](#2-livrables-du-projet-)
  - [3. Carte Arduino UNO et code associé ](#3-carte-arduino-uno-et-code-associé-)
  - [4. KiCad ](#4-kicad-)
    - [4.1. Symboles et empreintes des composants ](#41-symboles-et-empreintes-des-composants-)
    - [4.2. Schématique ](#42-schématique-)
    - [4.3. Placement des composants ](#43-placement-des-composants-)
  - [5. Fabrication du shield ](#5-fabrication-du-shield-)
    - [5.1 Réalisation du PCB ](#51-réalisation-du-pcb-)
    - [5.2 Perçage et soudure ](#52-perçage-et-soudure-)
  - [6. Logiciel de simulation LTSpice ](#6-logiciel-de-simulation-ltspice-)
  - [8. Tests et résultats ](#8-tests-et-résultats-)
    - [8.1 Banc de test ](#81-banc-de-test-)
    - [8.2 Résultats obtenus et analyses ](#82-résultats-obtenus-et-analyses-)
    - [8.3 Regard critique sur les résultats ](#83-regard-critique-sur-les-résultats-)
  - [9. Datasheet ](#9-datasheet-)



## 1. Description du projet <a id="PremiereSection"></a>

Ce projet s'inscrit dans le cadre de l'unité de formation "Du capteur au banc de test en open source hardware". Ce cours est dispensé en quatrième année au sein du département de Génie Physique de l'INSA Toulouse et a pour but de nous sensibiliser aux différentes étapes de conception et d'analyse utiles à la création d'un capteur.


### 1.1 Caractéristiques et travaux de recherche préliminaires sur le capteur <a id="PremiereSection1"></a>

Le KTY2000 est un exemple de technologie low-tech à base de graphite. Ce projet s’inspire du travail mené par plusieurs scientifiques et publié dans [cet article](https://www.nature.com/articles/srep03812) en 2014. En effet, les études ont révélé les nombreux avantages du carbone graphite. L’électronique à base de papier attire de plus en plus les ingénieur.es de par sa facilité d’approvisionnement, de fabrication et son faible coût. Il suffit dans notre cas de déposer une fine couche de graphite sur le substrat naturellement poreux pour former la base du capteur. Les mines de crayon sont constituées de réseaux percolés de fines poudres de graphite liées entre elles par des argiles, permettant d’obtenir après dépôt de fins films conducteurs non fabriqués en laboratoire. 

Le système à l’étude est granulaire, autrement dit il existe une dépendance entre la conductivité électrique (notamment par effet tunnel) et l’espace moyen entre les nanoparticules de graphite. Ainsi, une déformation de la feuille de papier va modifier la conductivité globale de la couche de graphite, induisant des changements de résistances réversibles lors des déformations en compression ou en traction. Ceci constitue en fait le principe d'une jauge de contrainte. 
L’expérience est réalisée avec différentes duretés de mine de crayon (2H, HB, 2B). Les mesures de résistance pour chaque crayon sont réalisées en fonction de différents rayons de courbure (soit la déformation) ou de l’angle de flexion de notre capteur. Cela permet une caractérisation complète de chaque type de crayon et ainsi de notre capteur. 

Dans notre cas, les traces de crayon sont déposées sur du papier comme vu ci-dessous. Une fois colorié, le capteur est connecté à un système de mesures externe via des pinces crocodiles reliées à un PCB et branché sur une carte Arduino Uno.

<p align="center"><img width="329" alt="image" src="https://user-images.githubusercontent.com/124165435/234648698-2a138793-281e-4adf-a231-edec8a0e3931.png">

### 1.2 Cours et formation <a id="PremiereSection2"></a>

Pour mener ce projet, nous avons d'abord suivi différents cours et TPs.
- Introduction à la chaîne de mesure
- TP développement d'un capteur à jauge de contrainte à base de crayon graphite
- TP conception d'un circuit analogique pour interfacer le capteur à jauge de contrainte
- TP programmation de microcontrôleurs et développement d'un matériel open source
- TP LTSpice afin de simuler le fonctionnement du circuit électronique comprenant la jauge de contrainte
- TP shield électronique PCB dédié à l'interface du capteur de contrainte
- TP réalisation du banc de test de la jauge de contrainte
- Cours fiche technique de la jauge de contrainte
- Cours Github

<p align="center"><img width="402" alt="image" src="https://user-images.githubusercontent.com/124165435/232909479-6e11000b-4ce8-4656-aaaf-82b67eb49942.png">


## 2. Livrables du projet <a id="DeuxièmeSection"></a>

* Un shield PCB devra être designé et fabriqué. Il sera ensuite connecté à une plaque Arduino UNO. Le shield doit contenir au minimum un amplificateur transimpédence, un module Bluetooth, un écran OLED et un encodeur rotatoire pour la calibration du capteur.

* La carte Arduino fonctionnera avec un code permettant de mesurer la containte appliquée sur le capteur. La carte devra aussi communiquer avec le module Bluetooth, et contrôler et communiquer avec l'écran OLED grâce à l'encodeur rotatoire.

* Une application Android APK
* Un protocole de test
* La datasheet du capteur de contrainte


## 3. Carte Arduino UNO et code associé <a id="TroisièmeSection"></a>

Le code Arduino est consultable [ici]().
Ce code permet la réception des mesures faites par les deux capteurs, la conversion résistance-angle du flex-sensor et celle tension-résistance pour le KTY2000, la gestion de l'affichage sur l'écran OLED et l'envoi de données via un port série.

 
## 4. KiCad <a id="QuatriemeSection"></a>

Voici la liste des différents composants présents sur notre PCB :

- _x2 résistance 100kΩ_
- _x1 résistance 47kΩ_
- _x1 résistance 10kΩ_
- _x1 résistance 1kΩ_
- _x1 résistance variable MCP41010_
- _x1 capacité 1μF_
- _x2 capacité 100nF_
- _x1 amplificateur opérationnel LTC1050_
- _x1 écran OLED I2C 1306_
- _x1 encodeur rotatoire_
- _x1 servo-moteur_
- _x1 capteur graphite_

### 4.1. Symboles et empreintes des composants <a id="QuatriemeSection1"></a>
 
* Amplificateur LTC1050
* MCP41010
* Ecran OLED
* Flex sensor
* Bluetooth HC-05

### 4.2. Schématique <a id="QuatriemeSection2"></a>
 
<p align="center"><img width="500" alt='image' src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20Sch%C3%A9matique%20globale.png">

* Amplificateur LTC1050
<p> 
<img width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20LTC1050.png">

<img  width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BPCB%5D%20LTC1050.png">
</p>

* MCP41010
 <p> 
<img width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20MCP41010.png">

<img  width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BPCB%5D%20Res%20Variable.png">
</p>

* Ecran OLED
 <p> 
<img width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20OLED%201306.png">

<img  width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BPCB%5D%20OLED%201306.png">
</p>
 

* Flex sensor
 <p> 
<img width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20Flex%20Sensor.png">

<img  width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BPCB%5D%20Flex%20Sensor.png">
</p>
  

* Bluetooth HC-05
 <p> 
<img width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BSCH%5D%20Bluetooth%20HC05.png">

<img  width="300" alt="image" src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/%5BPCB%5D%20Bluetooth%20HC05.png">
</p>
 



### 4.3. Placement des composants <a id="QuatriemeSection3"></a>
 
<p align="center"><img width="450" alt='image' src="https://github.com/MOSH-Insa-Toulouse/2025-2026-4GPa-Simonnet-Vidalenc/blob/main/Images/PCB%20Shield%20Complet.png">
 


## 5. Fabrication du shield <a id="CinquiemeSection"></a>
 
### 5.1 Réalisation du PCB <a id="CinquiemeSection1"></a>

Le PCB a été fabriqué grâce au matériel mis à disposition au Génie Physique et au Génie Électrique et Informatique de l'INSA Toulouse. Les manipulations ont été faites avec l'aide de Catherine Crouzet. La modélisation du PCB fait sur [KiCad]() a été imprimée sur du papier calque tranparent. Ce motif a été ensuite transféré par UVs sur la plaquette d'epoxy finement recouverte d'une couche de cuivre. À l'aide d'un révelateur, la partie non insolée de la résine a été retirée. La plaquette d'expoxy a été ensuite placée dans un bain de perchlorure de fer pour que le cuivre non protégé par la résine soit retiré de la plaquette. Enfin, la plaque a été nettoyée des dernières traces de résine avec de l'acétone.

Par soucis de convention, les pattes "1" de chaque composant sont en forme de carré

### 5.2 Perçage et soudure <a id="CinquiemeSection2"></a>

⌀ 1.0mm : Broches de connexion de la carte Arduino Uno

⌀ 0.8mm : Circuit intérieur du PCB 


## 6. Logiciel de simulation LTSpice <a id="SixiemeSection"></a>
 
L'ensemble de notre circuit électronique a été simulé sur le logiciel LTSpice afin de comprendre et d'analyser son comportement dans des conditions réelles de valeurs de tension. Les différentes simulations ont permis d'une part de vérifier le bon fonctionnement du circuit et de chacun des composants intégrés, et d'autre part de comprendre l'utilité de chacun. Les images ci-dessous représentent la schématique du circuit. Se référer à la [datasheet] pour de plus amples informations.
 



## 8. Tests et résultats <a id="HuitiemeSection"></a>

Setup d'utilisation du KTY2000.


### 8.1 Banc de test <a id="HuitiemeSection1"></a>
 

 
### 8.2 Résultats obtenus et analyses <a id="HuitiemeSection2"></a>


Les deux graphes suivant représentent la variation de la résistance relative (en %) en fonction de la déformation appliquée - compression et traction.

Les mines les plus tendres contiennent une proportion plus élevée de particules de graphite. C’est donc pour cette raison que les traces de ce type de crayon paraissent plus foncées sur le papier. À l’inverse, les mines les plus dures contiennent beaucoup de liants argileux et apparaissent plus claires. Cela explique également que les résistances de mines de crayon les plus tendres sont plus faibles que les mines de crayon les plus dures. En effet, sous la traction, les particules de graphite sont davantage écartées les unes des autres favorisant la déconnexion des voies de conduction : le courant est donc minime, la résistance quant à elle augmente.  Sous compression, les particules de graphite contenues dans la trace de crayon se rapprochent, facilitant la conduction du courant à travers le réseau de percolation : la résistance s’abaisse naturellement.

Cette caractéristique est illustrée à travers la variation relative de résistance en fonction de la déformation du capteur. En effet, un crayon qui contient moins de particules de graphite comme le 2H dépose naturellement moins de graphite sur le substrat. Les variations de résistance sont donc plus significatives comme beaucoup de chemins de percolations sont créés ou rompus contrairement au 2B où les déformations n’induisent que de faibles variations relatives de résistance.
 
Il est également notable que le comportement en compression du flex-sensor se rapproche de celui du KTY2000. Nous pouvons en conclure que notre capteur graphite présente des similarités de fonctionnnement qu'un capteur industriel. **Cela confirme l'idée que le low-tech est une alternative pertinente pour le futur de l'industrie.**

### 8.3 Regard critique sur les résultats <a id="HuitiemeSection3"></a>
 
Il est important de souligner la variabilité des conditions expérimentales. Entre chaque relevé de points et malgré le fait que les pinces crocodiles sans dents étaient bien fixées, il se peut que l’attache du capteur ait bougée. Les résultats sont imprécis et pour certains discutables au vu de la qualité du montage mais suivent tout de même les lois physiques. 

Nous pourrions amener plusieurs pistes d'améliorations à notre projet :

- Améliorer notre banc de test afin de garantir des résultats optimaux mais surtout pour augmenter sa durée de vie et sa résistance. Par exemple, nous pourrions concevoir de le fabriquer par l'intermédiaire d'une imprimante 3D. 

- Améliorer le setup expérimental en commençant par l'ergonomie de notre PCB afin de le rendre plus fonctionnel pour la réalisation des mesures de résistance. Penser à un meilleur emplacement pour les différents composants, à un système d'accroche pour l'écran OLED, mais aussi pour les pinces crocodile dans le but de limiter le plus possible la variabilité des conditions expérimentales. 

 
## 9. Datasheet <a id="NeuvièmeSection"></a>
 
La datasheet du KTY2000 est consultable [ici]. Elle reprend tous les concepts physiques ainsi que les résultats de mesures et leur analyse. D'autres courbes illustrant la variation de résistance en fonction de l'angle de flexion y figurent, venant compléter les affirmations énoncées tout au long de ce rendu. 
