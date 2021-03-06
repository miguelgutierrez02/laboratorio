# include <WiFi.h>
# include <PubSubClient.h>
# include <ESP32_MailClient.h>
# include <ESP32Servo.h>
# include <DHT.h>
# define DHTPIN 4
# define DHTTYPE DHT11
# define servoPin 15



// *************
// **** CONFIG MQTT *****
// *************
const  char * mqtt_server = "ioticos.org" ;
const  int    mqtt_port = 1883;
const  char * mqtt_user = "jXbGbtv9uRczDJV" ;
const  char * mqtt_pass = "tE3WSWBm65fzYc4" ;

const  char * root_topic_subscribe = "Uv15cdzHn4yzDcj";
const  char * root_topic_subscribe_Temp1 = "Uv15cdzHn4yzDcj/Temp1";
const  char * root_topic_subscribe_Puerta1 = "Uv15cdzHn4yzDcj/Puerta1";

const  char * root_topic_publish = "Uv15cdzHn4yzDcj/Raiz";
const  char * root_topic_publish_Temp = "Uv15cdzHn4yzDcj/Temp";
const  char * root_topic_publish_Hum = "Uv15cdzHn4yzDcj/Hum";
const  char * root_topic_publish_Puerta = "Uv15cdzHn4yzDcj/Puerta";

// *************
// **** WIFICONFIG ******
// *************
//const  char * ssid = "FAMILIA_RS" ;
//const  char * password = "sergio.0822.1118" ;

const  char * ssid = "SIN WIFI" ;
const  char * password = "luanabra2021" ;

//const char* ssid     = "LAURA CAMILA";
//const char* password = "0591laura";

// *************
// **** GLOBALES ******
// *************
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
SMTPData datosSMTP;
Servo miServo;

char msg [ 30 ];
char msgG [ 60 ];
int ModificarTemp = 32;
float h;
float t;
const int p = 21;
const int v = 22;
String EstadoPuerta;
String EstadoVentilador;


// ********
// *** FUNCIONES **
// ********

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
void sensor();
void puerta();
void moditemp();
void CorreoPuertaAbierta();
void CorreoVentiladorOn();

void setup() {
  Serial.begin(115200);

  dht.begin();

  miServo.attach(servoPin); //Conecta la variable miServo a un pin(15).
  miServo.write(0);
  
  pinMode(p, OUTPUT); 
  digitalWrite(p, LOW);

  pinMode(v, OUTPUT); 
  digitalWrite(v, LOW);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {
   reconnect();
  }

  if (client.connected()){
    sensor();
    puerta();
    moditemp();
 
    String General = "Temperatura = " + String(t) + "??C" + " Humedad = " + String(h) + " Puerta = " + String(EstadoPuerta);
    General.toCharArray(msgG,60);
    client.publish(root_topic_publish,msgG);
    //count++;
    delay(200);
    
    String strT = "La temperatura es -> " + String(t) + "??C";
    strT.toCharArray(msg,30);
    client.publish(root_topic_publish_Temp,msg);
    //count++;
    delay(200);

    String strH = "La humedad es -> " + String(h);
    strH.toCharArray(msg,30);
    client.publish(root_topic_publish_Hum,msg);
    //count++;
    delay(200);

    String strP = "La puerta esta -> " + String(EstadoPuerta);
    strP.toCharArray(msg,30);
    client.publish(root_topic_publish_Puerta,msg);
    //count++;
    delay(200);
  }
  client.loop();
}

//***********
//*    CONEXION WIFI      *
//***********

void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Direcci??n IP: ");
  Serial.println(WiFi.localIP());
}



//***********
//*    CONEXION MQTT      *
//***********

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexi??n Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion General");
      }else{
        Serial.println("Fallo Suscripci??n General");
      }
      if(client.subscribe(root_topic_subscribe_Temp1)){
      Serial.println("Suscripcion a Temp");
      }else{
      Serial.println("Fallo Suscripci??n a Temp");
      }
      if(client.subscribe(root_topic_subscribe_Puerta1)){
      Serial.println("Suscripcion a Puerta");
      }else{
      Serial.println("Fallo Suscripci??n a Puerta");
      }
      
    } else {
      Serial.print("Fall?? :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

//***********
//*       CALLBACK        *
//***********
void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  if(String(topic) == "Uv15cdzHn4yzDcj/Temp1"){
    Serial.println("TEMP1");
    for (int i = 0; i < length; i++) {
      incoming += (char)payload[i];
      }
        incoming.trim();
        Serial.println("Mensaje -> " + incoming);
        String cad = incoming.substring(0,2);
        Serial.println(cad);
        ModificarTemp = cad.toInt();
        Serial.print("La temperatura se modific?? a: ");
        Serial.println(ModificarTemp);
        moditemp();
    } 
    if(String(topic) == "Uv15cdzHn4yzDcj/Puerta1"){
    Serial.println("PUERTA1");
    for (int i = 0; i < length; i++) {
      incoming += (char)payload[i];
      }
        incoming.trim();
        Serial.println("Mensaje -> " + incoming);
          if(String(incoming) == "abrir"){
            digitalWrite(p, LOW);
            Serial.print("C = ");
            Serial.println(digitalRead(p));
            }
            else if(String(incoming) == "cerrar"){
              digitalWrite(p, HIGH);
              Serial.print("A = ");
              Serial.println(digitalRead(p));
              }
        puerta();
    }
}

void sensor(){
  //Serial.println(F("DHT11 test!"));
  //dht.begin();
  delay(2000); //Es un sensor lento, por lo que hay que darle tiempo.
  h = dht.readHumidity();
  t = dht.readTemperature();
  // Comprobamos si las lecturas pueden dar alg??n fallo mediante la funci??n isnan()
  // Esta funci??n devuelve un 1 en caso de que el valor no sea num??rico
  // Los caracteres || son como un OR. Si se cumple una de las dos condiciones mostramos error
 if (isnan(h) || isnan(t)) {
    Serial.println(F("??No se pudo leer el sensor DHT11!"));
  return;
  }
  else{
    Serial.print(F("Humedad: "));
    Serial.print(h);
    Serial.print(F("% Temperatura: "));
    Serial.print(t);
    Serial.println(F("??C "));
  }
  }

void puerta(){
  if(digitalRead(p) == LOW){
    Serial.println("Puerta Cerrada");
    EstadoPuerta = "Cerrada";
    miServo.write(0);
    }
    else{
      EstadoPuerta = "";
      EstadoPuerta = "Abierta";
      Serial.println("Puerta Abierta");
      miServo.write(90);
      CorreoPuertaAbierta();
      }
}
void moditemp (){
  if(t >= ModificarTemp){
    digitalWrite(v, HIGH);
    EstadoVentilador = "Encendido";
    CorreoVentiladorOn();
    Serial.println("Ventilador Encendido");
    }
    else{
      digitalWrite(v, LOW);
      Serial.println("Ventilador Apagado");
      }
}

void CorreoPuertaAbierta(){
    //Configuraci??n del servidor de correo electr??nico SMTP, host, puerto, cuenta y contrase??a
  datosSMTP.setLogin("smtp.gmail.com", 465, "iotesp27@gmail.com", "esp32.1234");
    // Establecer el nombre del remitente y el correo electr??nico
  datosSMTP.setSender("ESP32SERGIO", "iotesp27@gmail.com");
  // Establezca la prioridad o importancia del correo electr??nico High, Normal, Low o 1 a 5 (1 es el m??s alto)
  datosSMTP.setPriority("High");
  // Establecer el asunto
  datosSMTP.setSubject("ESTADO DE LA PUERTA");
  // Establece el mensaje de correo electr??nico en formato de texto (sin formato)
  datosSMTP.setMessage(EstadoPuerta, false);
  // Agregar destinatarios, se puede agregar m??s de un destinatario
  datosSMTP.addRecipient("iotesp27@gmail.com");
  //Comience a enviar correo electr??nico.
  if (!MailClient.sendMail(datosSMTP))
  Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
  //Borrar todos los datos del objeto datosSMTP para liberar memoria
  datosSMTP.empty();
  delay(100);
  Serial.println("??Correo Puerta Enviado!");
  }

void CorreoVentiladorOn(){
      //Configuraci??n del servidor de correo electr??nico SMTP, host, puerto, cuenta y contrase??a
  datosSMTP.setLogin("smtp.gmail.com", 465, "iotesp27@gmail.com", "esp32.1234");
    // Establecer el nombre del remitente y el correo electr??nico
  datosSMTP.setSender("ESP32SERGIO", "iotesp27@gmail.com");
  // Establezca la prioridad o importancia del correo electr??nico High, Normal, Low o 1 a 5 (1 es el m??s alto)
  datosSMTP.setPriority("High");
  // Establecer el asunto
  datosSMTP.setSubject("ESTADO DEL VENTILADOR");
  // Establece el mensaje de correo electr??nico en formato de texto (sin formato)
  datosSMTP.setMessage(EstadoVentilador, false);
  // Agregar destinatarios, se puede agregar m??s de un destinatario
  datosSMTP.addRecipient("iotesp27@gmail.com");
  //Comience a enviar correo electr??nico.
  if (!MailClient.sendMail(datosSMTP))
  Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
  //Borrar todos los datos del objeto datosSMTP para liberar memoria
  datosSMTP.empty();
  delay(100);
  Serial.println("??Correo Ventilador Enviado!");
  }
