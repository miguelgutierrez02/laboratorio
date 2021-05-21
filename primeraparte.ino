# include <WiFi.h>
# include <PubSubClient.h>
# include <ESP32_MailClient.h>
# include <ESP32Servo.h>
# include <DHT.h>
# define DHTPIN 4
# define DHTTYPE DHT11
# define servoPin 15



// ***
// * CONFIG MQTT **
// ***
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
const  char * root_topic_publish_Puerta = "Uv15cdzHn4yzDcj/Puerta"
