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
const int Rx = 3;   // Pin3 -> RX, conectar con Tx del modulo. Asigna al pin 3 el nombre Rx (recepcion)
const int Tx = 2;   // Pin2 -> TX, conectar con Rx del modulo. Asigna al pin 2 el nombre Tx (transmision)
#define SSID  "Pardos"    // Nombre de la red
#define PASS  "53707246"  // Password
#define PORT_SERVER  "80"     // Puerto
SoftwareSerial ESP01(Rx,Tx); // Establece comunicación serie en pines Rx y Tx

boolean estado_led_13=13;
 
 
void setup() {
 
    Serial.begin(9600);  // monitor serial del arduino (velocidad de la comunicacion)
    ESP01.begin(9600);   // baud rate del ESP8266 (velocidad de la comunicacion)
 
    Serial.println("LED del pin 13 de Arduino OFF");
    pinMode(13,OUTPUT); // ajusta pin 13 como salida
    digitalWrite(13,LOW); //pone el pin 13 en cero - apagado
    
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
 
      char c = ESP01.read(); // guarda en la variable c el caracter que esta en el bufer del modulo ESP
      Serial.print(c);      // escribe en el monitor serial lo que recibe del modulo ESP
   
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
    /*Serial.println("obtener direccion IP"); 
    sendData("AT+CIFSR\r\n",2000);                    //suprimido para evitar IP al azar   
    Serial.println("_________________________");*/
    Serial.println("configuramos el servidor en modo multiconexion"); //multiconexion significa que varios clientes se puede conectar
    sendData("AT+CIPMUX=1\r\n",1000);
    Serial.println("_________________________");
    Serial.println("servidor 1 en el puerto 80"); 
    sendData("AT+CIPSERVER=1,"PORT_SERVER"\r\n",1000); // se crea un servidor en el modulo ESP
    Serial.println("=========================");
}
//========================================================


 
void sendData(String comando, const int timeout)  {
// Envia comando al ESP01 y verifica la respuesta dentro del tiempo timeout
 
 long int time = millis(); // medir el tiempo actual para verificar timeout
 ESP01.print(comando);     // enviar el comando al ESP01 (comando = primer parametro de la funcion)
 
  while( (time+timeout) > millis()) { //mientras no haya timeout (timeout = 2o parametro del la funcion)
 
      while(ESP01.available()) {// mientras haya datos por leer, lee los datos disponibles
        char c = ESP01.read();  // leer el siguiente caracter
        Serial.print(c);
      }
  } 
 return;
}
 
 
void data_server(){
 
    delay(2000);   // espera que lleguen los datos hacia el buffer AUMENTADO DE 1000 A 2000
    int conexionID = ESP01.read()-48; /* obtener el ID de la conexión para poder responder y encontrar quien
                                      es el cliente (el -48 tiene que ver con el codigo ascii, alli 48 es 0)*/
    ESP01.find("led=");               // buscar el texto "led="
    int state = (ESP01.read()-48);    // Obtener el estado del pin a mostrar
    digitalWrite(13, state);          // Cambia estado del pin
    
    while(ESP01.available())  {
        char c = ESP01.read();
        Serial.print(c);
      }
 
 //responder y cerrar la conexión para que el navegador no se quede cargando 

 /*Lo que queremos en nuestra página web:
  * 1 boton que sea "prender"
  * 1 boton que sea "apagar"
  * 1 aviso que diga cual es el estado actual (o un boton que cambia de color segun el estado)
  */
    // String paginaweb = ""; //(codigo original) página web a enviar 
    String paginaweb = "<!doctype html><html><head></head><body>";//(cogigo de Bitwise Ar)crea cadena pagina con etiquetas HTML
    //paginaweb += "<head><title>HAGALO USTED</title></head>";
    paginaweb += "<h1>Bienvenido a la pagina de HAGALO USTED</h1>";
    paginaweb += "<button onClick=location.href='./led=1\'>Prender LED 13</button>";
    paginaweb += "<button onClick=location.href='./led=0\'>Apagar LED 13</button>";
    paginaweb += "<h2>Estado actual del LED:</h2>";
    //paginaweb += "<button style='color:green;'>bombillo</button>";
    if (state==1) {
     // paginaweb += "<h1>LED_13 = encendido!</h1></body></html>";
     //paginaweb += "<h1>LED_13 = encendido!</h1>";
      paginaweb += "<button>ENCENDIDO</button></body></html>";
      //estadoled_1 (); // funcion de prueba hecha por fer y lala
    }
    else {
      //paginaweb += "<h1>LED_13 = apagado!</h1></body></html>";
      //paginaweb += "<h1>LED_13 = apagado!";
      paginaweb += "<button>APAGADO</button></body></html>";
      //estadoled_0 (); // // funcion de prueba hecha por fer y lala
    }
      
    String cmd_Webpage = "AT+CIPSEND="; // comando para enviar página web
    cmd_Webpage += conexionID;
    cmd_Webpage += ",";
    cmd_Webpage += paginaweb.length();
    cmd_Webpage += "\r\n";
    sendData(cmd_Webpage,1000);
    sendData(paginaweb,1000);
   
    String cmd_close = "AT+CIPCLOSE=";  // comando para terminar conexión
    cmd_close += conexionID;
    cmd_close += "\r\n";
    sendData(cmd_close,3000);
 
}
//lala y fer crean sus propias funciones

String estadoled_1 (){
  Serial.println("el led se encuentra encendido");
}

String estadoled_0 (){
  Serial.println("el led se encuentra apagado");
}
