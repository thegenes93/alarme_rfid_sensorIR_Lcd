#include <SPI.h>
#include <LiquidCrystal.h>
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA

#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS


LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
const int pinoSensor = 8; //PINO DIGITAL UTILIZADO PELO SENSOR
const int pinoBuzzer = A0; //PINO DIGITAL PELO SENSOR,
const int pinoRele = A1; //PINO DIGITAL PELO SENSOR,
boolean alarme = false;
boolean ativar = false;
char st[20];

void setup()
{
  Serial.begin(9600);   // Inicia a serial
  pinMode(pinoSensor, INPUT); //DEFINE O PINO COMO ENTRADA
  pinMode(pinoBuzzer, OUTPUT); //DEFINE O PINO COMO saida
  pinMode(pinoRele, OUTPUT); //DEFINE O PINO COMO SAÍDA
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  //Define o número de colunas e linhas do LCD:
  lcd.begin(16, 2);
  lcd.print("Ativar alarme ?");
  analogWrite(pinoRele, 255);
}

void loop()
{
   if (ativar == true ) {
    if (digitalRead(pinoSensor) == LOW) { //SE A LEITURA DO PINO FOR IGUAL A LOW, FAZ
      alarme = true;
      estadoAlerta(alarme);
    }
  }

  leituraRfid();
  if (alarme == false) {
    estadoInicial(ativar);
  }
}

void estadoInicial(boolean ativar)
{
  lcd.clear();
  if (ativar == true) {
    lcd.print("Alarme Ativado");
  } else {
    lcd.print("Alarme Desativado");
  }
  analogWrite(pinoRele, 255);
  noTone(pinoBuzzer);
}

void estadoAlerta(boolean alarme) {
  if (alarme == true) {
    Serial.println("objeto dectado");
    lcd.clear();
    lcd.print("Alarme Acionado");
    delay(5000);
    analogWrite(pinoRele, 0);
    tone(pinoBuzzer, 250);
  }
}

void leituraRfid() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return; //RETORNA PARA LER NOVAMENTE

  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  /***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/

  //O ENDEREÇO "27:41:AA:AB" DEVERÁ SER ALTERADO PARA O ENDEREÇO DA SUA TAG RFID QUE CAPTUROU ANTERIORMENTE
  if (strID.indexOf("BC:E3:60:19") >= 0 || strID.indexOf("97:EA:DF:C8") >= 0 || strID.indexOf("99:F2:B9:B1") >= 0) { //SE O ENDEREÇO DA TAG LIDA FOR IGUAL AO ENDEREÇO INFORMADO, FAZ
    lcd.clear();
    Serial.println("cartão valido");
    lcd.print("Ativando Alarme");
    alarme = false;
    if (ativar == false) {
      tone(pinoBuzzer, 250);
      delay(5000);
      ativar = true;
      noTone(pinoBuzzer);
    } else {
      lcd.clear();
      tone(pinoBuzzer, 250);
      lcd.print("Alarme desativado");
      noTone(pinoBuzzer);
      ativar = false;
    }
  } else { //SENÃO, FAZ (CASO A TAG LIDA NÃO SEJÁ VÁLIDA)
    lcd.clear();
    Serial.println("cartão invalido");
    lcd.print("cartão invalido");
  }

  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
}
