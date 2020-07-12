/*################## Servidor web en SP01 ########################
* Filename: ESP-01_Ej2.ino
* Descripción: Enciende/apaga LED desde navegador
* Autor: Jose Mª Morales
* Revisión: 11-06-2018
* Probado: ARDUINO UNO r3 - IDE 1.8.2 (Windows7)
* Web: www.playbyte.es/electronica/
* Licencia: Creative Commons Share-Alike 3.0
* http://creativecommons.org/licenses/by-sa/3.0/deed.es_ES
* ##############################################################
*/
 
#include <SoftwareSerial.h>
const int Rx = 3;   // Pin3 -> RX, conectar con Tx del modulo
const int Tx = 2;   // Pin2 -> TX, conectar con Rx del modulo
#define SSID  "Pardos"    // Nombre de la red
#define PASS  "53707246"  // Password
#define PORT_SERVER  "80"     // Puerto
SoftwareSerial ESP01(Rx,Tx); // Establece comunicación serie en pines Rx y Tx
 
 
void setup() {
 
    Serial.begin(9600);  // monitor serial del arduino
    ESP01.begin(9600);   // baud rate del ESP8266
 
    Serial.println("lED del pin 13 de Arduino OFF");
    pinMode(13,OUTPUT);
    digitalWrite(13,LOW); 
 
    init_mod();         // Configura el modulo
}
//========================================================
 
 
 
void loop() {
/* 
  if (Serial.available()) {
    char c = Serial.read();// manda al modulo lo escrito en el serial monitor
    ESP01.print(c);
  }
*/ 
  if(ESP01.available()) {    // revisar si hay mensaje del ESP01
 
      char c = ESP01.read();
      Serial.print(c);      // escribe lo que recibe del modulo
   
      if(ESP01.find("+IPD,"))   data_server();  // revisar si el servidor recibio datos
  }
 
}
//#######################################################
 
 
 
void init_mod(){
 
    Serial.println("=========================");
    Serial.println("Configurando modulo ESP01");
    Serial.println("=========================");
    Serial.println("Reseteando modulo");
    sendData("AT+RST\r\n",2000);
    Serial.println("_________________________");
    Serial.println("Ajustando velocidad del modulo");
    sendData("AT+CIOBAUD=9600\r\n",1000);
    Serial.println("_________________________");
    sendData ("AT+CIOBAUD?\r\n",1000);           //AÑADIDO PARA Comprobando la velocidad
    Serial.println("_________________________");
    Serial.println("Configurando como cliente");
    sendData("AT+CWMODE=1\r\n",1000);
    Serial.println("_________________________");
    Serial.println("SSID y password para conectarse a red");
    sendData("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n",8000);
    Serial.println("_________________________");
    Serial.println("forzar dirección IP");
    sendData("AT+CIPSTA=\"192.168.0.150\"\r\n",2000); //Añadido para forzar IP
    Serial.println("_________________________");
    Serial.println("comprueba la IP asignada");
    sendData("AT+CIPSTA?\r\n",1000);                  //Añadido para ver IP asgnada     
    Serial.println("_________________________");
    /*hokla*/
    /*Serial.println("obtener direccion IP"); 
    sendData("AT+CIFSR\r\n",2000);                    //suprimido para evitar IP al hazar   
    Serial.println("_________________________");*/
    Serial.println("configuramos el servidor en modo multiconexion");
    sendData("AT+CIPMUX=1\r\n",1000);
    Serial.println("_________________________");
    Serial.println("servidor 1 en el puerto 80"); 
    sendData("AT+CIPSERVER=1,"PORT_SERVER"\r\n",1000);
    Serial.println("=========================");
}
//========================================================
 
 
void sendData(String comando, const int timeout)  {
// Envia comando al ESP01 y verifica la respuesta dentro del tiempo timeout
 
 long int time = millis(); // medir el tiempo actual para verificar timeout
 ESP01.print(comando);     // enviar el comando al ESP01
 
  while( (time+timeout) > millis()) { //mientras no haya timeout
 
      while(ESP01.available()) {// mientras haya datos por leer, lee los datos disponibles
        char c = ESP01.read();  // leer el siguiente caracter
        Serial.print(c);
      }
  } 
 return;
}
 
 
void data_server(){
 
    delay(2000);   // espera que lleguen los datos hacia el buffer AUMENTADO DE 1000 A 2000
    int conexionID = ESP01.read()-48; // obtener el ID de la conexión para poder responder
    ESP01.find("led=");               // buscar el texto "led="
    int state = (ESP01.read()-48);    // Obtener el estado del pin a mostrar
    digitalWrite(13, state);          // Cambia estado del pin
    
    while(ESP01.available())  {
        char c = ESP01.read();
        Serial.print(c)
      }
 
 //responder y cerrar la conexión para que el navegador no se quede cargando 

 /*Lo que queremos en nuestra página web:
  * 1 boton que sea "prender"
  * 1 boton que sea "apagar"
  * 1 aviso que diga cual es el estado actual (o un boton que cambia de color segun el estado)
  */
    String html = ""; // página web a enviar
    if (state==1) html += "<h1>LED_13 = encendido!</h1><h2>Probando un h2</h2><button type="button">Prender LED</button>";
    else {html += "<h1>LED_13 = apagado!</h1>";}
      
    String cmd_Webpage = "AT+CIPSEND="; // comando para enviar página web
    cmd_Webpage += conexionID;
    cmd_Webpage += ",";
    cmd_Webpage += html.length();
    cmd_Webpage += "\r\n";
    sendData(cmd_Webpage,1000);
    sendData(html,1000);
   
    String cmd_close = "AT+CIPCLOSE=";  // comando para terminar conexión
    cmd_close += conexionID;
    cmd_close += "\r\n";
    sendData(cmd_close,3000);
 
}
