#include <ESP8266WiFi.h>        // biblioteca Wi-Fi para ESP8266
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <PubSubClient.h>       // para enviar e receber mensagens MQTT, por Nick O'Leary, versão 2.8.0
#include <Espalexa.h>           // por Christian Schwinne, ver 2.7.0
#include <LittleFS.h>           // permite acessar a memória flash: ler, gravar, fechar e excluir arquivos (pasta data) - para upload do Certificado TLS
#include <CertStoreBearSSL.h>   // uso de certificados CA SSL/TLS
#include <ElegantOTA.h>         // por Ayush Sharma, ver 2.2.9 - OTAs para ESP8266 ou ESP32 elegante
#include <ArduinoJson.h>        // biblioteca JSON para C++, por Benoît Blanchon, versão 6.21.3
#include <ESP8266HTTPClient.h>  // biblioteca http para enviar mensagens para o webhook

#include "secrets.h"

// somente para o ESP8266, uso do CallBack. ESP32 não precisa
#ifdef ESP8266
  #include <coredecls.h>       // necessária para a função settimeofday_cb(): ! callback settimeofday_cb() para verificar no servidor NTP
#endif

// Define modo DEBUG - Diretivas de compilação (compilação condicional)
// #define DEBUG                 // comente esta linha para compilar para o ESP01, senão ocorrerá erro nos sensores que usam as portas TX e RX.

// Variaveis
// Porta do Servidor Web
const byte WEBSERVER_PORT = 80;

// Porta do Servidor WebSockets
const byte WEBSOCKET_PORT = 81;

// Porta do Servidor DNS
const byte DNSSERVER_PORT = 53;

// Porta do Servidor Broker MQTT (selecionar/configurar conforme utilização)
const uint16_t BROKER_PORT = 8883;       // Porta do Broker HiveMQ Cloud (privado)

// WEB server
ESP8266WebServer server(WEBSERVER_PORT);

// WebSockets server
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

// DNS server
DNSServer dnsServer;

// instancia global do objeto Alexa
Espalexa espalexa;

// Instancias cliente para envio de requisições para o Webhook
WiFiClient webhook_client;      // instancia WiFi Client
HTTPClient sender;              // instancia HTTP Client

// Função Alexa
void callbackAlexa_01(uint8_t brilho);

// mDNS server
char myHostname[32] = SEC_HOSTNAME;         // nome do host para mDNS. Para "esp8266", digitar no browse http://esp8266.local

// Broker MQTT server (selecionar/configurar conforme utilização)
char BROKER_MQTT[64] = SEC_BROKER;       // URL do servidor broker MQTT da HiveMQ Cloud (privado)

// Definição nome dispositivo Alexa
String deviceName01 = SEC_ALEXA;      // nome do dispositivo 01 da Alexa

// Um único CertStore global que pode ser usado por todas as conexões.
BearSSL::CertStore certStore;             // Precisa permanecer ativo o tempo todo qualquer um dos WiFiClientBearSSLs

// Criacao dos objetos que serao resposaveis pela conexao ao servidor MQTT
// HiveMQ Cloud - Broker privado com certificado TLS:
BearSSL::WiFiClientSecure esp_client;
PubSubClient * mqtt_client;

// Credenciais de acesso à rede WiFi, SSID e senha de acesso à rede WiFi
struct settings {
  char ssid[32];
  char password[64];
} user_wifi = {};


// Nome do arquivo na memória flash, com os parâmetros de configuração
const char* CONFIG_FILE = "/config.json";

// Definindo a estrutura de dados referente aos parâmetros de configuração
struct ConfigData {
  char        URL[64];      // URL do servidor broker MQTT
  uint16_t    PORT;         // Porta do Broker
  char        USER[32];     // Usuário do Dispositivo. Tem que ser o ID previamente cadastrado no broker.
  char        PASS[32];     // Senha do Dispositivo. Tem que ser o ID previamente cadastrado no broker.
  char        PUB[64];      // Tópico para publish. Informe um topico unico. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
  char        SUB[64];      // Tópico para subscribe
  char        host[32];     // Nome do host para mDNS. Para "esp8266", digitar no browse http://esp8266.local
  String      Alexa01;      // nome do dispositivo 01 da Alexa
  word        bootCount;    // Número de inicializações
} user_param = {};          // <- configuração global do objeto

// Variáveis para a páginas WEB servidor Cativo
String pagePortal;                          // página html do portal
String pageScan;                            // página html com a lista de redes encontradas
boolean modoAP        = false;              // indica se o ESP está em modo AP
String ssidName       = "Setup Portal";     // prefixo nome do Potal Cativo (modo AP). Será composto do sufixo ESP.getChipId(). Até 32 caracteres
const char* ssidSenha = "12345678";         // senha de acesso ao Potal Cativo (modo AP). Mínimo de 8 carecteres e máximo de 64 caracteres

// Variáveis para controles de tempo
unsigned long previousMillis = 0;           // Controle de tempo - função StatusRede
unsigned long intervalo = 250;              // Faz o LED piscar quando desconectado do WiFi (250 ms)
unsigned long previousMillis2 = 0;          // Contorle de tempo - função checkRede
unsigned long intervalo2 = 100000;          // Verifica redes disponíveis em modo AP para conexão em modo ST (após reset)
unsigned long reconecta = 0;                // variavel para a conexão com o servidor MQTT
unsigned long lastTime2 = 0;                // controle de tempo para atualização dos sensores
unsigned long timerDelay2 = 1 * 20 * 1000;  // a cada 20 segundos atualiza status dos sensores

// Variáveis WebSocket
String pageToggle;                    // página html botão liga/desliga
boolean PUMPonoff=false; String JSONtxt;

// Variável pata uso no callback para sincronizar com NTP
time_t        nextNTPSync         = 0;                  // não controla o tempo de callback, somente o tempo para verificação das chamadas.
                                                        // A variável interna SNTP_UPDATE_DELAY é a responsável pelo tempo de callback (padrão 1 hora).
// Constantes para conexão com servidor NTP
const char*   ntpServer           = "pool.ntp.org";     // Pode usar também o br.pool.ntp.org, o pool.ntp.org tem mais opções de servidores
const long    gmtOffset_sec       = -3 * 60 * 60;       // Fuso horário em segundos: -3h*60min*60s = -10800s
const int     daylightOffset_sec  = 0;                  // Fuso em horário de verão, quando necessário

// Variável interna que altera SNTP_UPDATE_DELAY, tempo de callback (Servidor NTP)
// ! opcional: caso você queira outro intervalo de polling NTP (padrão 1h)
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 () {
  return 4 * 60 * 60 * 1000UL;        // atualizada horário com servidor NTP a cada 4 horas
  // return 60 * 1000UL;              // atualizada horário com servidor NTP a cada 1 minuto (o resultado será 2 min porque foi colocado um delay de 60s na função callback: ntpSync_cb)
}

// Variáveis MQTT - HiveMQ Cloud
const char*   DISP_ID                = SEC_ID;                 // ID Unico do Dispositivo. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. Será redefinido este ID quando da conexão com MQTT concatenando caracteres aleatórios.
char          DISP_USER[32]          = SEC_USER;               // Usuário do Dispositivo. Tem que ser o ID previamente cadastrado no HiveMQ Cloud. 
char          DISP_PASSWORD[32]      = SEC_PASSWORD;           // Senha do Dispositivo. Tem que ser o ID previamente cadastrado no HiveMQ Cloud. 
char          TOPIC_PUBLISH[64]      = SEC_TOPIC_PUBLISH;      // criacao do topico para publish. Informe um topico unico. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
char          TOPIC_SENSOR0[64]      = SEC_TOPIC_SENSOR0;      // criacao do topico para publish: Sensor0 
char          TOPIC_SENSOR1[64]      = SEC_TOPIC_SENSOR1;      // criacao do topico para publish: Sensor1
char          TOPIC_SUBSCRIBE[64]    = SEC_TOPIC_SUBSCRIBE;    // topico para subscribe
boolean       PUMPstatusAnt          = false;                  // status anterior da bomba

// Variáveis Webhook - https://trigger.esp8266-server.de/ -  Integração Alexa
const char* webhookUrl1 = SEC_URL1;    //Trigger1 - Bomba Sala, sensor de disparo de nível alto da Bomba Sala
const char* webhookUrl2 = SEC_URL2;    //Trigger2 - Bomba Sala, sensor de disparo de nível baixo da Bomba Sala

// Variáveis e definição dos pinos (GPIOs)
int rele0 = 0;                       // rele0 será conectado à GPIO0
int sensor0 = 3;                     // sensor0 será conectado à GPIO3 (RXD)
int sensor1 = 1;                     // sensor1 e LED AZUL conectados à GPIO1 (TXD)
int led = 2;                         // led conectado à GPIO2

// Valores iniciais da variáveis
int rele0State      = LOW;           // estado inicial do rele0
int sensor0State    = HIGH;          // estado inicial do sensor0
int sensor1State    = HIGH;          // estado inicial do sensor1
int ledState        = HIGH;          // estado inicial do led

// Nome do software
String softwareNom = String(__FILE__);
String jsonBuffer;

#include "html_page.h"
#include "functions.h"

void setup() {

  // Definição dos pinos de entrada e saída
  pinMode(rele0,OUTPUT);                     // inicializando o pino 0 como uma saída
  pinMode(led,OUTPUT);                       // inicializando o pino 2 como uma saída
  pinMode(sensor0,INPUT);                    // inicializando o pino 3 como uma entrada
  pinMode(sensor1,INPUT);                    // inicializando o pino 1 como uma entrada
  
  digitalWrite(led, ledState);               // ligando o led (pino 2)

  // Serial
  #ifdef DEBUG                                // Inicializa Serial se modo DEBUG ativado
    #ifdef ESP8266
      Serial.begin(74880);                    // usar esta velocidade para o ESP8266 para ver todos os comandos durante o boot  
    #else
      Serial.begin(115200);                   // usar esta velocidade para o ESP32 para ver todos os comandos durante o boot
    #endif
    Serial.println(F("\n\n--- Modo DEBUG ---\n"));
  #endif

  
  EEPROM.begin(sizeof(struct settings));     // inicializando EEPROM
  EEPROM.get(0, user_wifi);                  // lendo as credenciais de rede na EEPROM

  // Iniciando LittleFS para leitura e gravação da flash
  if (!LittleFS.begin()) {
    #ifdef DEBUG
      Serial.printf("LittleFS ERRO!\n");
    #endif
    while (true);
  }

  // Carrega a configuração padrão se for executado pela primeira vez
  #ifdef DEBUG
    Serial.print(F("Carregando configuração do arquivo config.json ..."));
  #endif
  loadConfiguration(CONFIG_FILE,user_param);          // Lê configuração
  
  user_param.bootCount++;                             // Incrementa contador de inicializações
  #ifdef DEBUG
    Serial.printf("Incrementando contador de inicializações: %d boots\n", user_param.bootCount);
  #endif

  // Cria arquivo de configuração
  #ifdef DEBUG
    Serial.print(F("Salvando configuração no arquivo config.json ..."));
  #endif
  saveConfiguration(CONFIG_FILE,user_param);          // Salva configuração

  #ifdef DEBUG
    Serial.println(F("Iniciando conexão à rede WiFi:"));
    Serial.printf("SSID\t: %s\n", user_wifi.ssid);
    Serial.printf("Senha\t: %s\n", user_wifi.password);
    Serial.print(F("Conectando à rede WiFi"));
  #endif

  ConnectWiFi();                            // conecta à rede WiFi. Não sendo possível, entra em modo AP (Ponto de Acesso)
  ConfigDNS();                              // configura servidor DNS, quando no modo AP (WiFi desconectado)
  ConfigmDNS();                             // configura servidor mDNS, quando no modo ST (WiFi conectado)

  delay(100);                               // atraso de 100 milisegundos
  

  if (WiFi.status() != WL_CONNECTED) {                          // se não está conectado ao WiFi (ESP em modo AP), faz o scan de redes disponíveis
    // WiFi.scanNetworksAsync(onComplete, show_hidden)
    // Parâmetros da função:
    // - onComplete: o manipulador de eventos executado quando a varredura é concluída 
    // - show_hidden (opcional): defina-o para procurar redes ocultas boolean true
    WiFi.scanNetworksAsync(scanResult);
  }
  
  ServerCaptive();                        // configura servidor Portal Cativo, quando no modo AP (WiFi desconectado)
  ServerToggle ();                        // configura servidor Portal Toggle, quando no modo ST (WiFi conectado)

  // Define callback para sincronização de horário com servidor NTP
  #ifdef ESP8266
    settimeofday_cb(ntpSync_cb);                      // ESP8266
  #else
    sntp_set_time_sync_notification_cb(ntpSync_cb);   // ESP32
  #endif

 setDateTime();                           // sincroniza o relógio do ESP com horário do servidor NTP, quando no modo ST (WiFi conectado). Alguns caso, chame isso antes da conexão WiFi!
 configMQTT();                            // configura o servidor MQTT usando certificado para conexão TLS, quando no modo ST (WiFi conectado)
 configAlexa();                           // configura Alexa
 ElegantOTA.begin(&server);               // Inicia ElegantOTA
 
}


void loop() {
  yield();                              // Watchdog

  espalexa.loop();                      // trata as requisiçoes Alexa
  delay(1);                             // comando padrão biblioteca Alexa

  if (modoAP){
    server.handleClient();              // Requisições Web
    dnsServer.processNextRequest();     // Requisições DNS, apenas quando em modo AP
    checkRede();                        // verifica se a rede pré-configurada está disponível para conexão em modo ST, estando disponível reseta o ESP
  }
  
  webSocket.loop();                     // Requisições WebSocket

  MDNS.update();                        // Requisições mDNS

  StatusRede();                         // verifica conexão de rede. Faz o LED piscar quando desconectado, ou deixa o LED ligado quando conectado
  
  if (WiFi.status() == WL_CONNECTED){   // Requisições MQTT - HiveMQ
    mqtt_client->loop();                // mantem a conexao ativa com o servidor HiveMQ
    checkMQTT();                        // verifica conexão com servidor MQTT, fazendo nova conexão caso necessário
    publishTOPIC();                     // publica mensagens no Broker: envia valores/mensagens ao Broker qunado há alteração do estado da variável
  }

  checkSensores();                      // verifica sensores de nível

}
