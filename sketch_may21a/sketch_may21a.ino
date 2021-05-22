// **
// * FUNCIONES **
// **

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
