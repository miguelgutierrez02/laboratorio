const  char * ssid = "SIN WIFI" ;
const  char * password = "luanabra2021" ;


// ***
// * GLOBALES *
// ***
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
String EstadoVentilador
