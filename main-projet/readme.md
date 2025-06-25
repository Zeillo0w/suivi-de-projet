Rapport de Projet : Démonstration d'Attaque par HID (Digispark) pour la Sensibilisation à la Cybersécurité

Date : 05 Mai 2025 (Date de finalisation indicative)
1. Introduction et Objectif du Projet
Ce projet, réalisé dans le cadre du cours de sensibilisation à la cybersécurité, visait à créer une démonstration technique percutante des risques associés aux attaques par périphériques d'interface humaine (HID). L'outil choisi fut une clé USB programmable Digispark, sélectionnée pour sa capacité à simuler un clavier et son faible coût.
L'objectif était de simuler un scénario d'attaque réaliste : l'insertion de cette clé dans un ordinateur Windows non verrouillé devait déclencher une exfiltration automatisée de données sensibles. Cette démonstration avait pour but de sensibiliser un public (le jury, d'autres étudiants) à l'importance cruciale du verrouillage de session, à la méfiance vis-à-vis des périphériques USB d'origine inconnue, et à la vitesse à laquelle des informations peuvent être compromises. Une ambition forte était associée à ce projet : réaliser une démonstration techniquement complète, incluant l'obtention de mots de passe en clair, pour illustrer pleinement l'impact potentiel et viser l'excellence dans l'évaluation.
2. Conception Initiale et Première Implémentation (Digispark)
La conception initiale s'est appuyée sur les fonctionnalités de base de la Digispark et les commandes Windows accessibles à un utilisateur standard. Après une phase de recherche et de planification, le premier script Digispark (.ino), utilisant la librairie DigiKeyboard.h, a été développé pour effectuer les actions suivantes :
Accès Système : Ouvrir l'invite de commandes (CMD) via Win+R, puis taper cmd et Entrée.
Environnement de Travail : Se déplacer dans le dossier temporaire (cd %temp%) où les droits d'écriture sont généralement permis.
Extraction de Données Natives :
WiFi : Utilisation de netsh wlan export profile key=clear pour exporter les profils WiFi connus dans des fichiers XML. Une commande PowerShell (Select-String) était ensuite employée pour extraire les lignes contenant keyMaterial (les clés en clair, si disponibles) de ces fichiers XML et les regrouper dans un fichier wifi-pass.txt.
Fichiers Chrome : Copie des fichiers Login Data (base de données SQLite contenant les identifiants et mots de passe chiffrés) et History (base de données SQLite de l'historique) depuis leur emplacement dans %LOCALAPPDATA% vers le dossier %TEMP%.
Exfiltration : Utilisation de l'outil curl.exe pour envoyer les fichiers wifi-pass.txt, la copie de Login Data, et la copie de History vers un Webhook Discord configuré pour recevoir les données.
Nettoyage : Utilisation de commandes del pour supprimer les fichiers XML et les copies créées dans %TEMP%.
Cette première version fonctionnait, permettant l'exfiltration des clés WiFi et des fichiers Chrome bruts.
3. Problématiques Rencontrées et Raffinement de l'Objectif
Les tests de cette première implémentation ont révélé des limitations importantes :
Limite de Taille Discord : L'envoi du fichier History échouait fréquemment avec une erreur Request entity too large du webhook Discord. La taille potentiellement importante de l'historique dépassait la limite d'upload autorisée. Une tentative de compression préalable via PowerShell a été envisagée mais jugée trop complexe et lente pour être fiable dans le contexte d'une attaque rapide via Digispark. L'extraction de l'historique direct a donc été mise de côté temporairement.
Mots de Passe Chiffrés : L'obstacle majeur était que le fichier Login Data exfiltré contenait les mots de passe sous forme chiffrée, rendant la démonstration moins impactante. L'objectif a donc été affiné : il fallait trouver un moyen de déchiffrer ces mots de passe pour les obtenir en clair.
4. Phase 2 : Développement d'un Outil de Déchiffrement en Python
La recherche a indiqué que le déchiffrement nécessitait l'utilisation de l'API de Protection des Données de Windows (DPAPI) et devait s'exécuter dans le contexte de l'utilisateur sur la machine cible. Une approche post-exfiltration étant impossible, la décision fut de développer un outil dédié qui serait déployé et exécuté par la Digispark.
Choix Technologique : Python a été choisi pour sa flexibilité et la disponibilité de bibliothèques pertinentes (pywin32 pour DPAPI, sqlite3 pour lire la base de données, pycryptodome pour le chiffrement AES-GCM utilisé par les versions récentes de Chrome, json, base64).
Script decryptor.py (Itérations Multiples) :
Logique Initiale : Localiser Local State et Login Data, extraire et déchiffrer la clé maîtresse AES via DPAPI (win32crypt.CryptUnprotectData), lire Login Data, puis tenter de déchiffrer chaque password_value avec AES-GCM en utilisant la clé maîtresse. Une lecture de l'historique a aussi été intégrée. Le script devait écrire les résultats (mots de passe en clair + historique) dans un fichier texte unique dans %TEMP%.
Packaging : PyInstaller a été utilisé pour compiler le script Python en un fichier .exe autonome, incluant les dépendances.
Déploiement via Digispark : Le script .ino a été modifié pour télécharger cet .exe depuis une URL (un dépôt GitHub personnel), l'exécuter, attendre sa complétion, puis envoyer le fichier texte résultant via curl.
Difficultés et Débogage Python/DPAPI : Cette phase fut la plus complexe :
Configuration de l'Environnement : Des problèmes initiaux avec pip, PyInstaller et les variables d'environnement PATH ont été résolus par une installation propre de Python et l'adoption systématique d'environnements virtuels (venv) pour isoler les dépendances. L'activation du venv sous PowerShell a parfois requis un ajustement de la politique d'exécution (Set-ExecutionPolicy Bypass -Scope Process).
Inclusion des Modules : PyInstaller n'incluait pas win32crypt correctement. L'utilisation de venv et de l'option --hidden-import win32crypt a résolu ce point.
Détection Antivirus : L'.exe compilé était systématiquement bloqué par Windows Defender. Une idée astucieuse pour contourner l'UAC et ajouter une exclusion AV via la Digispark (simulation de Tab+Entrée) a été testée mais s'est avérée non fiable en pratique. Les tests de déchiffrement ont donc dû être effectués avec l'AV temporairement désactivé sur la machine de test dédiée.
Échec du Déchiffrement : Malgré les corrections, le déchiffrement échouait. Des étapes de débogage précises (ajout de print dans le Python, analyse des erreurs) ont permis d'établir que :
La clé AES maîtresse était récupérée depuis Local State via DPAPI et avait la bonne longueur (32 bytes).
Les mots de passe dans la base de données commençaient par un préfixe v20, non prévu initialement (corrigé dans le script Python).
L'erreur finale et systématique était AES Echec vérification: MAC check failed. Cela prouvait que la clé AES maîtresse, bien qu'extraite sans erreur Python et de la bonne longueur, était incorrecte pour déchiffrer les mots de passe. L'interaction entre pywin32, DPAPI et le contexte utilisateur spécifique posait un problème insurmontable avec cette approche.
5. Phase 3 : Exploration Brève d'une Alternative C++/Qt
Face à l'impasse du déchiffrement via Python/pywin32, l'idée d'utiliser un outil C++, potentiellement plus proche des API natives Windows, a été explorée. Un projet open source C++/Qt (DjGorillaz/chrome-password-reader) a été identifié et testé.
Mise en place : Installation de Qt et Qt Creator, configuration de l'environnement MinGW.
Compilation : Résolution d'une erreur de linkage (cannot find Crypt32.lib) en adaptant le fichier .pro pour MinGW (LIBS += -lcrypt32).
Obstacle Majeur : Dépendances DLL : L'exécutable C++/Qt compilé dépendait de nombreuses DLL Qt et MinGW qui n'étaient pas présentes sur une machine standard. Le déploiement de toutes ces DLL via la Digispark (probablement via un ZIP à décompresser) a été jugé trop complexe et peu fiable pour une démonstration rapide. Cette piste a donc été abandonnée.
6. Phase 4 : Solution Finale Pragmatique
Reconnaissant les difficultés techniques persistantes et insurmontables pour garantir un déchiffrement fiable en direct dans le cadre de ce projet, et afin d'assurer une démonstration fluide et percutante axée sur l'objectif premier de sensibilisation, une décision stratégique a été prise : revenir à une version modifiée et fiabilisée de l'approche Python, mais sans tenter le déchiffrement final.
Objectif Ajusté : Démontrer l'exfiltration rapide et l'accès aux informations disponibles (WiFi clair, comptes enregistrés, historique), et expliquer clairement le risque que représentent les mots de passe chiffrés volés.
Script Python Final (extractor.py) : La version finale se concentre sur :
L'extraction des clés WiFi en clair.
La lecture de Login Data pour lister les URLs et Usernames, en indiquant explicitement [Données Chiffrées Présentes] là où un mot de passe existe.
La lecture et le formatage de l'historique récent depuis History.
L'écriture de ces informations combinées dans extracted_data.txt.
Script Digispark Final : Le script .ino a été finalisé pour refléter cette approche :
Pas de tentative de bypass UAC/AV.
Téléchargement et exécution de l'extractor.exe compilé.
Envoi de wifi-pass.txt au webhook.
Envoi de extracted_data.txt au webhook.
Nettoyage propre des fichiers temporaires.
7. Démonstration et Flux Opérationnel Final
Le scénario de démonstration présenté est le suivant :
Explication du contexte et des risques liés aux postes non verrouillés et aux clés USB.
Insertion de la Digispark.
Brève apparition d'une fenêtre CMD.
Affichage sur Discord des fichiers wifi_passwords.txt (avec clés lisibles) et extracted_browser_data.txt.
Présentation du contenu de extracted_browser_data.txt, montrant la liste des comptes (URL/User) et l'historique, en soulignant la mention [Données Chiffrées Présentes].
Explication Orale Clé : Mise en exergue de la rapidité du vol d'informations déjà utiles (WiFi, liste de comptes, historique). Explication claire que, bien que les mots de passe Chrome soient chiffrés ici, le vol du fichier Login Data constitue une faille critique, car un attaquant dédié pourrait potentiellement les déchiffrer hors ligne ou avec d'autres techniques s'il obtenait un accès plus persistant ou les clés DPAPI associées. Conclusion sur les mesures de défense essentielles.
8. Défis Rencontrés et Apprentissages
Ce projet a représenté un défi technique stimulant, riche en enseignements :
Interaction Système/Langages : La complexité du chiffrement DPAPI/AES de Chrome et les difficultés d'y accéder de manière fiable et portable via des langages de script comme Python (avec des wrappers comme pywin32) ont été mises en évidence.
Défenses Modernes : Les mécanismes de sécurité comme l'UAC et les antivirus (même basiques comme Defender) sont des obstacles réels, même pour des scripts conçus à des fins éducatives, soulignant leur importance.
Importance de l'Environnement : La nécessité d'un environnement de développement propre et cohérent (gestion des PATH, environnements virtuels) a été cruciale pour résoudre les problèmes de compilation et de dépendances.
Déploiement et Dépendances : La problématique du déploiement d'applications (notamment C++/Qt avec leurs DLLs) est un facteur non négligeable.
Démarche Itérative : Le projet a nécessité de nombreuses itérations, diagnostics précis (analyse des erreurs, ajout de logs/debug), et une capacité à adapter la stratégie face aux blocages techniques. La tentative de contournement de l'UAC, bien qu'infructueuse, témoigne d'une volonté d'explorer des solutions créatives.
9. Conclusion
Bien que l'objectif ambitieux de déchiffrer systématiquement les mots de passe Chrome en clair via cette méthode spécifique n'ait pu être atteint de manière fiable, le projet est une réussite en termes de sensibilisation. La démonstration finale met efficacement en lumière la rapidité et la facilité avec lesquelles un attaquant peut exfiltrer un volume conséquent d'informations sensibles (clés WiFi, liste des comptes, historique) à partir d'un accès physique même bref à une machine non sécurisée.
Le processus de développement a permis d'acquérir une expérience pratique précieuse sur les attaques HID, les mécanismes de protection Windows, les techniques d'extraction de données, les défis du chiffrement et du déchiffrement, ainsi que sur les méthodologies de résolution de problèmes techniques complexes. La solution finale, axée sur une exfiltration fiable et une explication claire des risques résiduels, constitue une base solide et pertinente pour atteindre l'objectif pédagogique initial.