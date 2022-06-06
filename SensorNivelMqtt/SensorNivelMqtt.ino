/* =============================================================================================================
    
   
   WR Kits Channel & Usina Info número 061


   Sensor Ultrassônico resistente à água

   
  
   Autor: Eng. Wagner Rambo  Data: Novembro de 2017
   
   www.wrkits.com.br | facebook.com/wrkits | youtube.com/user/canalwrkits
   
   
============================================================================================================= */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "RedeBlumen"; 
const char* password =  "blumenmaker";
const char* mqttServer = "3.131.136.168"; 
const int mqttPort = 1883; 
const char* mqttUser = 0; 
const char* mqttPassword = 0;  
const String MQTT_FLOW_TOPIC = "sensorhall1/volume";
const String MQTT_TOPIC_RESET = "sensorhall1/volume/reset";

const char* clientId = "int";

float mediaAnterior = 0;

float vazao; //variável para armazenar o valor em L/min
float media=0; //variável para tirar a média a cada 1 minuto
String volume;
float somavazao=0; //somar a vazão do minuto
int contaPulso; //variável para a quantidade de pulsos
int i=0; //variável para contagem

void ICACHE_RAM_ATTR incpulso();

WiFiClient espClient;
PubSubClient client(espClient);
/*
-------------------------------------------------
NodeMCU / ESP8266  |  NodeMCU / ESP8266  |
D0 = 16            |  D6 = 12            |
D1 = 5             |  D7 = 13            |
D2 = 4             |  D8 = 15            |
D3 = 0             |  D9 = 3             |
D4 = 2             |  D10 = 1            |
D5 = 14            |                     |
-------------------------------------------------
*/
void mqtt_callback(char* topic, byte* dados_tcp, unsigned int length);

// =============================================================================================================
// --- Mapeamento de Hardware ---
#define trig D7  //Pino 9 do Arduino será a saída de trigger
#define echo D6  //Pino 8 do Arduino será a entrada de echo


void trigPuls();  //Função que gera o pulso de trigger

float pulse;     //Variável que armazena o tempo de duração do echo
float dist_cm;   //Variável que armazena o valor da distância em centímetros

int period = 10000;
unsigned long time_now = 0;
long previousMillis = 0;

void setup() 
{
  pinMode(trig, OUTPUT);   //Pino de trigger será saída digital
  pinMode(echo, INPUT);    //Pino de echo será entrada digital
  
  digitalWrite(trig, LOW); //Saída trigger inicia em nível baixo
  
  Serial.begin(9600);      //Inicia comunicação serial
WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) 
  {   
     delay(100);
    Serial.println("Conectando a WiFi..");
  }
  Serial.println("Conectado!"); 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    
    if (client.connect("Projeto", mqttUser, mqttPassword ))
    {
 
      Serial.println("Conectado ao servidor MQTT!");  
 
    } else {
 
      Serial.print("Falha ao conectar ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 }

  
     void callback(char* topic, byte* payload, unsigned int length) {
      String payloadStr = "";
        for (int i = 0; i < length; i++) {
      payloadStr += (char)payload[i];
      }
      Serial.println(payloadStr);
      String topicStr = String(topic);

     }
void loop() 
{

 client.loop();
  
   trigPulse();                 //Aciona o trigger do módulo ultrassônico
   
   pulse = pulseIn(echo, HIGH, 200000); //Mede o tempo em que o pino de echo fica em nível alto
   
   dist_cm = pulse/58.82;       //Valor da distância em centímetros
   
   // 340m/s
   // 34000cm/s
   
   /*
        100000 us - 17000 cm/s
             x us - 1cm
           1E6
      x = -----
          17E3
   */
   
   Serial.println(dist_cm);     //Imprime o valor na serial
     char message[58];
   String str(dist_cm);
   str.toCharArray(message, 58);
  
   
 time_now = millis();
   

    if(time_now - previousMillis > period){
      previousMillis = time_now;
      
      client.publish(MQTT_FLOW_TOPIC.c_str(), message);
      Serial.println("Menssagem MQTT enviada !");  
    }
  
   delay(200);                  //Taxa de atualização
   
  

}

void trigPulse()
{
  digitalWrite(trig, HIGH);  //Pulso de trigger em nível alto
  delayMicroseconds(10);     //duração de 10 micro segundos
  digitalWrite(trig, LOW);   //Pulso de trigge em nível baixo
}
