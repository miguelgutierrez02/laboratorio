void loop() {
  if(!client.connected()) {
   reconnect();
  }

  if (client.connected()){
    sensor();
    puerta();
    moditemp();
 
    String General = "Temperatura = " + String(t) + "°C" + " Humedad = " + String(h) + " Puerta = " + String(EstadoPuerta);
    General.toCharArray(msgG,60);
    client.publish(root_topic_publish,msgG);
    //count++;
    delay(200);
    
    String strT = "La temperatura es -> " + String(t) + "°C";
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

//*****
//*    CONEXION WIFI      *
//*****

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
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****
//*    CONEXION MQTT      *
//*****

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
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
        Serial.println("Fallo Suscripción General");
      }
      if(client.subscribe(root_topic_subscribe_Temp1)){
      Serial.println("Suscripcion a Temp");
      }else{
      Serial.println("Fallo Suscripción a Temp");
      }
      if(client.subscribe(root_topic_subscribe_Puerta1)){
      Serial.println("Suscripcion a Puerta");
      }else{
      Serial.println("Fallo Suscripción a Puerta");
      }
      
    } else {
      Serial.print("Falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

//*****
//*       CALLBACK        *
//*****
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
        Serial.print("La temperatura se modificó a: ");
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
          if(String(incoming) == "a"){
            digitalWrite(p, LOW);
            Serial.print("C = ");
            Serial.println(digitalRead(p));
            }
            else if(String(incoming) == "c"){
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
  // Comprobamos si las lecturas pueden dar algún fallo mediante la función isnan()
  // Esta función devuelve un 1 en caso de que el valor no sea numérico
  // Los caracteres || son como un OR. Si se cumple una de las dos condiciones mostramos error
 if (isnan(h) || isnan(t)) {
    Serial.println(F("¡No se pudo leer el sensor DHT11!"));
  return;
  }
  else{
    Serial.print(F("Humedad: "));
    Serial.print(h);
    Serial.print(F("% Temperatura: "));
    Serial.print(t);
    Serial.println(F("°C "));
  }
  }

void puerta(){
  if(digitalRead(p) == LOW){
    Serial.println("Puerta on");
    EstadoPuerta = "on";
    miServo.write(0);
    }
    else{
      EstadoPuerta = "";
      EstadoPuerta = "off";
      Serial.println("Puerta off");
      miServo.write(90);
      CorreoPuertaAbierta();
      }
}
void moditemp (){
  if(t >= ModificarTemp){
    digitalWrite(v, HIGH);
    EstadoVentilador = "on";
    CorreoVentiladorOn();
    Serial.println("Ventilador on");
    }
    else{
      digitalWrite(v, LOW);
      Serial.println("Ventilador off");
      }
}

void CorreoPuertaAbierta(){
    //Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
  datosSMTP.setLogin("smtp.gmail.com", 465, "iotprueba05@gmail.com", "1118571597");
    // Establecer el nombre del remitente y el correo electrónico
  datosSMTP.setSender("ESP32diego", "iotprueba05@gmail.com");
  // Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
  datosSMTP.setPriority("High");
  // Establecer el asunto
  datosSMTP.setSubject("ESTADO DE LA PUERTA");
  // Establece el mensaje de correo electrónico en formato de texto (sin formato)
  datosSMTP.setMessage(EstadoPuerta, false);
  // Agregar destinatarios, se puede agregar más de un destinatario
  datosSMTP.addRecipient("iotprueba05@gmail.com");
  //Comience a enviar correo electrónico.
  if (!MailClient.sendMail(datosSMTP))
  Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
  //Borrar todos los datos del objeto datosSMTP para liberar memoria
  datosSMTP.empty();
  delay(100);
  Serial.println("¡Correo Puerta Enviado!");
  }

void CorreoVentiladorOn(){
      //Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
  datosSMTP.setLogin("smtp.gmail.com", 465, "iotprueba05@gmail.com", "1118571597");
    // Establecer el nombre del remitente y el correo electrónico
  datosSMTP.setSender("ESP32diego", "iotprueba05@gmail.com");
  // Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
  datosSMTP.setPriority("High");
  // Establecer el asunto
  datosSMTP.setSubject("ESTADO DEL VENTILADOR");
  // Establece el mensaje de correo electrónico en formato de texto (sin formato)
  datosSMTP.setMessage(EstadoVentilador, false);
  // Agregar destinatarios, se puede agregar más de un destinatario
  datosSMTP.addRecipient("iotprueba05@gmail.com");
  //Comience a enviar correo electrónico.
  if (!MailClient.sendMail(datosSMTP))
  Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
  //Borrar todos los datos del objeto datosSMTP para liberar memoria
  datosSMTP.empty();
  delay(100);
  Serial.println("¡Correo Ventilador Enviado!");
  }
