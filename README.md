# Plateforme de Prototypage
## Poke a mole connecté

### Quelques resources
Documentation technique :
- [Documentation Heltec](https://heltec.org/project/wifi-kit-32/)
- [Reference framework Arduino](https://www.arduino.cc/reference/en/)
- [Reference framework ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html)

Librairies utilisées :
- [Reference librairie ArduinoJson](https://arduinojson.org/) : librairie utilisé pour formatter les messages MQTT
- [Reference librairie ESP_WiFiManager](https://github.com/khoih-prog/ESP_WiFiManager) : librairie utilisé pour la configuration du WiFi
- [Reference librairie mDash](https://github.com/cesanta/mDash) : librairie utilisé pour l'OTA
- [Reference librairie Pubsubclient](https://github.com/knolleary/pubsubclient) : librairie utilisé pour MQTT
- [Reference librairie U8g2](https://github.com/olikraus/u8g2/wiki/u8g2reference) : librairie utilisé pour l'ecran OLED
- [Librairies framework Arduino ESP32](https://github.com/espressif/arduino-esp32/tree/master/libraries) : liste des librairies disponible dans le framework Arduino

Tips & tricks :
- [Exemple de Deep Sleep](https://randomnerdtutorials.com/esp32-external-wake-up-deep-sleep/) : endormir l'ESP et le réveiller avec son bouton
- [Exemple d'utilisation d'un serveur NTP](https://gitlab.com/snippets/1961896) : obtenir la date et l'heure via internet
- [Exemple de stockage/lecture en mémoire non volatile](https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/StartCounter/StartCounter.ino) : Pour stocker des informations à conserver même après extinction de la carte.

# Le projet :

### Objectif
J'ai réalisé une application Poke A Mole permettant a un joueur de mesurer son temps de réaction. J'ai ensuite appris à communiquer avec le service Cloud AWS IoT.
Je dois maintenant assembler ces deux fonctionnalités pour réaliser un objet connecté.

**Au démarrage de la carte, mon code doit :**

1. Afficher le message `Welcome` sur l'écran OLED
2. Si le Wifi est configuré, tenter de se connecter au Wifi, afficher le message `Waiting for Wifi` et le SSID du wifi recherché. Sinon un portail captif nommé `PokeAMole-B02` démarre pour configurer le Wifi et le message `Please setup` et `wifi`.
3. Une fois la connexion Wifi établie, tenter de se connecter à AWS IoT, efface l'écran OLED et afficher le message `Waiting for Server`.
4. Une fois la connexion AWS IoT établie, effacer l'écran OLED et s'inscrire au topic `ecepp/PokeAMole/score`

**Une fois l'initialisation terminée, mon code doit :**

1. Allumer la LED et afficher le message `Poke A Mole` et `Long press to start` sur l'écran OLED
- Lorsqu'un utilisateur réalise un simple clic sur le bouton, si un high score existe en mémoire (non volatile), l'écran OLED est effacé, le message `High score`, sa valeur, la date et le numéro du binôme l'ayant réalisé doivent-être affiché sur l'écran OLED durant 5 secondes puis retourner à l'étape 1.
- Lorsqu'un utilisateur appui sur le bouton durant 3 secondes (long press), le jeu demarre :
   - L'écran OLED est effacé, puis durant 5 secondes le message `Game started` est affiché sur l'écran OLED et la LED clignote à 500ms d'interval
   - L'écran OLED est effacé, la LED s'eteind durant 3 secondes
   - Attendre pour une durée aléatoire entre 0 et 10 secondes
   - La LED s'allume et un chronomètre démarre (unité ms)
   - Lorsqu'un utilisateur appui sur le bouton, la LED s'éteind, le chronomètre s'arrête et le temps de réactivité de l'utilisateur est mesuré. Il doit être affiché sur l'ecran OLED avec le message `press to restart`. L'unité du temps de réaction affiché doit-être en millisecondes si la valeur est inférieur à 1000ms et en secondes avec 2 chiffres après la virgule sinon.
   - Le temps de réactivité de l'utilisateur doit-être publié sur le topic `ecepp/PokeAMole/score` avec le formalisme suivant :
   ```
    { 
      "user" : "B02",
      "score" : zzzz, // **Remplacer zzzz par le temps de réaction en millisecondes quelques soit la valeur**
      "date" : "dd/mm/yy hh:mm:ss" // **Remplacer dd/mm/yy hh:mm:ss par la date et l'heure locale** 
    }
    ```

   - Lorsqu'un utilisateur appui sur le bouton, retourner à l'étape 1
2. Lorsqu'un message est publié dans le topic `ecepp/PokeAMole/score` vérifier si le score est inférieur au dernier `high score` enregistré. Si oui, sauvegarder la valeur, la date et le binôme l'ayant réalisé en mémoire (non volatile).
3. Si la connexion wifi est perdue, et ce quelque soit le moment, le fonctionnement de l'application doit-être suspendu, l'écran OLED est effacé et doit afficher le message `Wifi lost` et `try to reconnect` tant que la connexion n'a pas été rétabli.
4. Si la connexion AWS IoT est perdue, et ce quelque soit le moment, le fonctionnement de l'application doit-être suspendu, l'écran OLED est effacé et doit afficher le message `Server lost` et `try to reconnect` tant que la connexion n'a pas été rétabli.
5. Si aucune interaction utilisateur n'a lieu durant plus de 1O minutes, la carte doit passer en mode `deep sleep` et doit en sortir après un appui sur le bouton `PRG`


**Pour permettre à l'utilisateur de réinitialiser la carte, mon code doit :**

- Si en même temps que l'utilisateur branche la carte à une source d'alimentation USB, il maintient appuyé le bouton `PRG` durant plus de 5 secondes, les données de Wifi et de high score doivent-être effacées.

