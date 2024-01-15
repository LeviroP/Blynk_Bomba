//Header file "functions.h":

//=====================================================
// Função ConnectWiFi: Conecta à rede WiFi
//=====================================================
void ConnectWiFi () {
  WiFi.mode(WIFI_STA);                                      // definindo ESP em modo estação (STA)
  WiFi.begin(user_wifi.ssid, user_wifi.password);           // fazendo conexão com a rede WiFi
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, !digitalRead(led));   //Faz o LED piscar (inverte o estado).
    #ifdef DEBUG
      Serial.print(F("."));
    #endif
    delay(1000);
    if (tries++ > 30) {
      modoAP = true;
      // ssidName += " - " + String(random(0xffff), HEX);   // usando variáveis aleatórias
      ssidName += " - " + String(ESP.getChipId(), HEX);     // usando id do chip em hexadecimal
      WiFi.mode(WIFI_AP);                                   // não conseguindo conectar à rede WiFi, define o ESP em modo ponto de acesso (AP)
      WiFi.softAP(ssidName.c_str(), ssidSenha);             // definindo SSID como "Setup Portal", senha "12345678" (mínimo 8 caracteres) ou sem senha ("")
      delay(500);
      #ifdef DEBUG
        Serial.println(F("\nNão conseguiu conectar à rede WiFi!"));
        Serial.println(F("Definido ESP em modo ponto de acesso (AP):"));
        Serial.printf("SSID\t: %s\n", ssidName.c_str());
        Serial.printf("Senha\t: %s\n", ssidSenha);
        Serial.printf("IP\t: %s\n", WiFi.softAPIP().toString().c_str());
      #endif
      break;
    }
  }
}   // fim ConnectWiFi


//=====================================================
// Função ConfigDNS: Configura servidor DNS
//=====================================================
void ConfigDNS () {
  if (WiFi.status() != WL_CONNECTED) {                          // se não está conectado ao WiFi (ESP em modo AP), configura servidor DNS
    #ifdef DEBUG
      Serial.print(F("Configurando portal captivo (DNS = *)"));
    #endif
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);         // DNSServer iniciado com "*" para o nome de domínio, ele responderá com qulaquer IP fornecido para todas as solicitações de DNS
    dnsServer.start(DNSSERVER_PORT, "*", WiFi.softAPIP());      // Configura o servidor DNS redirecionando todos os domínios para o endereço de ip do servidor
    #ifdef DEBUG
      Serial.printf("\t: OK!\n");
    #endif
  }
}   // fim ConfigDNS


//=====================================================
// Função ConfigmDNS: Configura servidor mDNS
//=====================================================
void ConfigmDNS () {
  if (WiFi.status() == WL_CONNECTED) {                           // se está conectado ao WiFi (ESP em modo ST), configura servidor mDNS
    #ifdef DEBUG
      Serial.printf("\nConexão com sucesso! ESP em Modo ST.\n");
      Serial.println(F("Configurando mDNS:"));
      Serial.printf("Nome do Host\t: %s\n", myHostname);
      Serial.printf("IP Rede Local\t: %s\n", WiFi.localIP().toString().c_str());
    #endif
    // mDNS
    // Configurar o mDNS:
    // - primeiro argumento é o nome de domínio constante na variável myHostname
    // - segundo argumento é o endereço IP
    // O mDNS é um protocolo DNS Multicast. Um “multicast” repassa a mesma mensagem para vários pontos em uma rede. O mDNS é indicado para redes pequenas sem servidores DNS próprios
    if (MDNS.begin(myHostname)) {
      #ifdef DEBUG
        Serial.println();
        Serial.printf("Iniciando mDNS (DNS Multicast)\t: %s\n", "mDNS iniciado!");
      #endif
      MDNS.addService("http", "tcp", WEBSERVER_PORT);     // Adicionar serviço ao MDNS-SD
      MDNS.addService("ws", "tcp", WEBSOCKET_PORT);
    } else {
      #ifdef DEBUG
        Serial.println(F("Erro ao configurar o MDNS!"));
      #endif
      while (1) {
        delay(1000);
      }
    }
  }
}   // fim ConfigmDNS


//=====================================================
// Função QuickSortDescending: classificação descendente - Método QuickSort
//=====================================================
void QuickSortDescending (int *lista, int left, int right) {
  int i, j, pivot;
  i = left;
  j = right;
  pivot = WiFi.RSSI(lista[(left + right) / 2]);

  while (i <= j) {
    while ( WiFi.RSSI(lista[i]) > pivot ) i++;
    while ( WiFi.RSSI(lista[j]) < pivot ) j--;

    if (i <= j) {
      std::swap(lista[i], lista[j]);
      i++;
      j--;
    }
  }

  // recursivamente na sublista à esquerda (maiores)
  if (j > left) QuickSortDescending(lista, left, j);
  // recursivamente na sublista à direita (menores)
  if (i < right) QuickSortDescending(lista, i, right);

}   // fim QuickSortDescending


//=====================================================
// Função getRSSIasQuality: Converte rssi (qualidade relativa de um sinal recebido em dBm) e qualidade do sinal em %
//=====================================================
String getRSSIasQuality(int rssi) {   // Devolve "quality" em String
  int quality = 0;

  if (rssi <= -100) quality = 0;
  else if (rssi >= -50) quality = 100;
  else quality = 2 * (rssi + 100);
  return String(quality);
}   // fim getRSSIasQuality


//=====================================================
// Função scanResult: Faz varredura das rede WiFi disponíveis
//=====================================================
void scanResult(int networksFound) {
#ifdef DEBUG    // imprime numero de redes encontradas
  Serial.printf ("\nIniciando varredura de redes WiFi disponíveis\n");
  Serial.printf("%d rede(s) encontrada(s)\n", networksFound);
  Serial.printf("%s  %-20s\t %s\t (%s dBm)\t %s\n", "nº", "SSID", "Canal", "RSSI", "Protegida?");
#endif

  // ordenado as redes encontradas
  // criando um vetor com os números das redes encontradas
  int indices[networksFound];
  for (int i = 0; i < networksFound; i++) {
    indices[i] = i;
  }

  // ordenação por rssi (qualidade relativa de um sinal recebido em dBm)
  // classificação descendente - Método QuickSort
  QuickSortDescending(indices, 0, networksFound - 1);

  // montagem da lista ordeanda de redes
  String pageItem;        // página htlm do item, modelo de cada linha da lista
  pageScan = "";
  for (int i = 0; i < networksFound; i++) {
#ifdef DEBUG    // imprime as redes encontradas - com ordenação
    Serial.printf("%2d  %-20s\t Ch:%2d\t (%5ddBm)\t %s\n", i + 1, WiFi.SSID(indices[i]).c_str(), WiFi.channel(indices[i]), WiFi.RSSI(indices[i]), WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE ? "aberta" : "protegida");
#endif

    // modelo do item da lista
    // pageItem = F("<div><a href='#' onclick='c(this)'>{v}</a> <span class='q {i}'>{r}%</span> <span class='s'>({s}dBm)</span> <span class='d'>Ch:{d}</span> </div>");
    // pageItem = F("<div><a href='#' onclick='c(this)'>{v}</a> <span class='q {i}'>{r}%</span></div>");
    pageItem = F("<div><a href='#' onclick='c(this)'>{v}</a> <span class='q {i}'>{r}%</span> <span class='d'>Ch:{d}</span> </div>");

    // {v} = ssid
    pageItem.replace("{v}", WiFi.SSID(indices[i]).c_str());

    // para uso futuro, basta retirar o comentário
    // {s} = rssi
    // pageItem.replace("{s}", String(WiFi.RSSI(indices[i])));
    // {d} = canal
    pageItem.replace("{d}", String(WiFi.channel(indices[i])));

    // {r} = qualidade do sinal em %
    // getRSSIasQuality(rssi): função que converte dBm em % e devolve o resultado como string
    pageItem.replace("{r}", getRSSIasQuality( WiFi.RSSI(indices[i]) ));

    // verificando se a rede é aberta ou protegida por senha
    if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) pageItem.replace("{i}", "b");   //     = b (protegida/bloqueada)
    else pageItem.replace("{i}", "l");                                                    //     = l (aberta)

    pageScan += pageItem;
  }
}   // fim scanResult


//=====================================================
// handle Captive: Função de manipulação, envia página da Web Portal Cativo para o cliente
//=====================================================
void handleRootCaptive() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));      // A função strncpy copia os n primeiros caracteres de uma string para outra. Contudo, isso pode causar overflow, sendo necessário definir onde termina a string destino. Para isso, coloca-se o terminador \0 no no final da string.
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';
    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200, "text/html", F("<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Setup Portal</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Configurar WiFi</h1> <br/> <p>Suas configura&ccedil;&otilde;es foram salvas com sucesso!<br />O m&oacute;dulo ser&aacute; reinicializado em alguns segundos.</p></main></body></html>") );

    ESP.reset();

  } else {
    int n = WiFi.scanComplete();
    scanResult(n);
        
    IPAddress clientIP = server.client().remoteIP();          // obter endereço IP do cliente
    #ifdef DEBUG
      Serial.printf("Cliente conectou ao Servidor Cativo!\n");
      Serial.printf("Endereço IP do cliente: %s\n", clientIP.toString().c_str());
    #endif
    
    pagePortal = FPSTR(webpageCaptive);

    pagePortal += F("<h1 class=''>");
    pagePortal += String(n);
    pagePortal += F(" Redes Encontradas</h1>");

    pagePortal += pageScan;   // usa a última varredura de redes WiFi
    pagePortal += F("<br/><br/><form action='/' method='post'><div class='form-floating'><label>SSID</label><input type='text' class='form-control' id='s' name='ssid' placeholder='ssid'></div><div class='form-floating'><label>Senha</label><input type='password' class='form-control' id='p' name='password' placeholder='Senha'></div><br/><button type='submit'>Salvar</button><p style='text-align: right'></p><button type='reset'>Limpar</button><p style='text-align: right'></p></form><br/><p>Clique aqui para fazer <a href='/'>nova varredura de redes WiFi</a>.</p></main></body></html>");

    server.send(200, "text/html", pagePortal);

    WiFi.scanNetworksAsync(scanResult);   // faz nova varredura de redes WiFi para a próxima atualização da página web
  }
}   // fim handleRootCaptive


//=====================================================
// Função ServerCaptive: Configura servidor Portal Cativo
//=====================================================
void ServerCaptive () {
  if (WiFi.status() != WL_CONNECTED) {  // se não está conectado, configura servidor Portal Cativo (ESP em modo AP)
    #ifdef DEBUG
      Serial.printf("Configurando página web Portal Cativo\t: ");
    #endif
    server.on("/", handleRootCaptive);              // Configurando root Portal Cativo
    server.on("/generate_204", handleRootCaptive);  // Portal cativo Android. Talvez não seja necessário. Pode ser manipulado pelo manipulador notFound
    server.on("/fwlink", handleRootCaptive);        // Portal cativo da Microsoft. Talvez não seja necessário. Pode ser manipulado pelo manipulador notFound
    #ifdef DEBUG
      Serial.printf("OK! Root portal cativo Setup Portal\n");
      Serial.printf("Iniciando servidor portal cativo\t: ");
    #endif
    server.begin();                                 // iniciando o servidor Portal Cativo
    #ifdef DEBUG
      Serial.printf("OK!");
    #endif
  }
}   // fim ServerCaptive


//=====================================================
// handle Toggle: Função de manipulação, envia página da Web para o cliente
//=====================================================
void handleRootToggle() {
  server.send(200, "text/html", pageToggle);
}   // fim handleRootToggle


//=====================================================
// Função webSocketEvent: processa evento, novos dados recebidos do cliente
//=====================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t welength) {
  String payloadString = (const char *)payload;
  IPAddress ip = webSocket.remoteIP(num);

  if (type == WStype_DISCONNECTED) {
#ifdef DEBUG
    Serial.printf("[%u] Desconectado!\n", num);
#endif
  }

  if (type == WStype_CONNECTED) {
#ifdef DEBUG
    Serial.printf("[%u] Cliente Conectado! IP %d.%d.%d.%d", num, ip[0], ip[1], ip[2], ip[3]);
    Serial.println();
#endif
    // send message to client
    // webSocket.sendTXT(num, "Conectado");
  }

  //receber texto do cliente
  if (type == WStype_TEXT) {
    // Serial.printf("[%u] get Text: %s\n", num, payload);
    // send message to client
    // webSocket.sendTXT(num, "message here");
    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    byte separator = payloadString.indexOf('=');
    String var = payloadString.substring(0, separator);
    String val = payloadString.substring(separator + 1);

#ifdef DEBUG
    Serial.printf("[%u] Cliente IP %d.%d.%d.%d payload = ", num, ip[0], ip[1], ip[2], ip[3]);
    Serial.println(payloadString);
    Serial.print(F("var= "));
    Serial.println(var);
    Serial.print(F("val= "));
    Serial.println(val);
    Serial.println();
#endif

    if (var == "PUMPonoff") {
      PUMPonoff = false;
      if (val == "ON") PUMPonoff = true;
    }
  }

  // liga ou desliga a bomba
  int valor;
  if (PUMPonoff == false) valor = LOW;
  else valor = HIGH;

  // só altera o estado do relé se houver água no reservatório
  if(sensor0State == LOW && sensor1State == HIGH ) {
    rele0State = valor;
    digitalWrite(rele0,rele0State);
  }
  // se o nível da água estiver igual ou maior ao máximo, altera a posição do relé sem alterar sua variável de estado para evitar "desliga" e "liga" simultâneos
  else if (sensor1State == LOW){
    digitalWrite(rele0,valor);
  }

  // envia broadcast
  String PUMPstatus = "OFF";
  if (PUMPonoff == true) PUMPstatus = "ON";
  JSONtxt = "{\"PUMPonoff\":\"" + PUMPstatus + "\"}";
  webSocket.broadcastTXT(JSONtxt);

}   // fim webSocketEvent


//=====================================================
// Função ServerToggle: Configura servidor Portal Toggle
//=====================================================
void ServerToggle () {
  if (WiFi.status() == WL_CONNECTED) {  // se está conectado, configura servidor Portal Toggle (ESP em modo ST)
    #ifdef DEBUG
      Serial.printf("Configurando página web Portal Toggle\t: ");
    #endif
        
    String chipID = "ESP-" + String(ESP.getChipId(), HEX);
    chipID.toUpperCase();
    
    pageToggle = FPSTR(webpageCode);                                                     // carrega o código da página Toggle - Botão liga/desliga
    pageToggle.replace(F("{WEBSOCKET_PORT}"), String(WEBSOCKET_PORT));                   // atualiza a porta websocket da página
    pageToggle.replace(F("{Titulo}"),chipID.c_str());                                    // atualiza o títuto com id do chip em hexadecimal
    pageToggle.replace(F("{EspId}"), ("ESP-"+ String(ESP.getChipId(), HEX)).c_str());    // atualiza o títuto com id do chip em hexadecimal
    pageToggle.replace(F("{DeviceName}"), deviceName01.c_str());                         // atualiza o subtítuto com id do disposivo Alexa
    pageToggle.replace(F("{IP}"), WiFi.localIP().toString().c_str());                    // atualiza o IP exibido na página
    pageToggle.replace(F("{ChipId}"), String(ESP.getChipId()));                          // atualiza o Chip ID exibido na página
    pageToggle.replace(F("{FlashId}"), String(ESP.getFlashChipId()));                    // atualiza o Flash ID exibido na página
    pageToggle.replace(F("{MAC}"), String(WiFi.macAddress()));                           // atualiza o endereço MAC do ESP exibido na página

    server.on("/", handleRootToggle);              // Configurando root Portal Toggle
    server.on("/generate_204", handleRootToggle);  // Portal Toggle Android. Talvez não seja necessário. Pode ser manipulado pelo manipulador notFound
    server.on("/fwlink", handleRootToggle);        // Portal Toggle da Microsoft. Talvez não seja necessário. Pode ser manipulado pelo manipulador notFound
    #ifdef DEBUG
      Serial.printf("OK! Root portal Toggle\n");
      Serial.printf("Iniciando servidor portal Toggle\t: ");
    #endif

    // server.begin();              //omita isso pois será feito por espalexa.begin(&server)
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
    #ifdef DEBUG
      Serial.printf("OK!\n");
    #endif
  }
}   // fim ServerToggle


//=====================================================
// Função StatusRede: monitora se a rede WiFi está conectada
//=====================================================
void StatusRede() {
  // Faz o LED piscar quando desconectado do WiFi
  unsigned long currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= intervalo)) {
    previousMillis = currentMillis;
    digitalWrite(led, !digitalRead(led));   //Faz o LED piscar (inverte o estado).
  }

  // Liga o LED quando conectado ao WiFi
  if ((WiFi.status() == WL_CONNECTED) && (digitalRead(led) != HIGH)  ) {
    digitalWrite(led, HIGH);   // liga o LED interno (conectado à rede WiFi)
  }
}   // fim StatusRede


//=====================================================
// Função ConnectMQTT: conecta ao servidor MQTT
//=====================================================
void ConnectMQTT() {
  //Se nao estiver conectado ao servidor MQTT
  while ((!mqtt_client->connected()) && (reconecta < millis())) {
    #ifdef DEBUG
      Serial.printf("Conectando ao Broker MQTT\t: %s\n", BROKER_MQTT);
    #endif

    // Criando um ID único. DISP_ID com sufixo id do chip + string aleatória
    const char* ID;
    String varHex;
    // varHex = String(DISP_ID) + "-" + String(random(0xffff), HEX);      // usando variáveis aleatórias
    varHex = String(DISP_ID) + "-" + String(ESP.getChipId(), HEX);        // usando id do chip em hexadecimal
    
    ID = varHex.c_str();
    
    #ifdef DEBUG
      Serial.printf("%-32s: %s\n", "DISP_ID",ID );
      Serial.printf("%-32s: %s\n", "DISP_USER", DISP_USER);
      Serial.printf("%-32s: %s\n", "DISP_PASSWORD", DISP_PASSWORD);
    #endif    

    // Solicita a conexao com o servidor MQTT utilizando o parametros "DISP_ID"
    if (mqtt_client->connect(ID, DISP_USER, DISP_PASSWORD)) {
      #ifdef DEBUG
        Serial.printf("%-33s: %s\n", "Status conexão ao Broker MQTT", "conectado com sucesso!");
        Serial.printf("%-33s: %s\n", "Publicando no tópico", TOPIC_PUBLISH);
        Serial.printf("%-33s: %s\n", "Inscrevendo-se no tópico", TOPIC_SUBSCRIBE);
      #endif
      // Uma vez conectado, publique um anúncio no tópico "TOPIC_PUBLISH" ...
      // mqtt_client->publish(TOPIC_PUBLISH, "mensagem"), mensagem conforme status da bomba
      if (PUMPonoff == true) mqtt_client->publish(TOPIC_PUBLISH, "ON");
      else mqtt_client->publish(TOPIC_PUBLISH, "OFF");

      if (sensor0State == LOW) mqtt_client->publish(TOPIC_SENSOR0, "ON");
      else mqtt_client->publish(TOPIC_SENSOR0, "OFF");
      if (sensor1State == LOW) mqtt_client->publish(TOPIC_SENSOR1, "ON");
      else mqtt_client->publish(TOPIC_SENSOR1, "OFF");
     
      // ... e inscreva-se novamente no tópico
      mqtt_client->subscribe(TOPIC_SUBSCRIBE);
    } else {
      #ifdef DEBUG
        Serial.printf("%-33s: %s\n", "Status conexão ao Broker MQTT", "não foi possivel se conectar ao Broker MQTT!");
        Serial.print(F("Falhou, rc="));
        Serial.print(mqtt_client->state());
        Serial.println(F(". Nova tentatica de conexão em 10s"));
      #endif
      reconecta = millis() + 10000;       // atualiza a contagem de tempo. Próxima tentativa em 10s
    }
  }
}   // fim ConnectMQTT


//=====================================================
// Função checkMQTT: mantem a conexão com o servidor MQTT
//=====================================================
void checkMQTT() {
  if (!mqtt_client->connected()) {
    ConnectMQTT();
  }
  // ConnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}   // fim checkMQTT


//=====================================================
// Função publishTOPIC: envia valores/mensagens ao Broker
//=====================================================
void publishTOPIC () {
  if (!PUMPonoff && PUMPstatusAnt) {  // alterou o estado do bomba, PUMPonoff = OFF
    mqtt_client->publish(TOPIC_PUBLISH, "OFF");

    if (sensor0State == LOW) mqtt_client->publish(TOPIC_SENSOR0, "ON");
    else mqtt_client->publish(TOPIC_SENSOR0, "OFF");
    if (sensor1State == LOW) mqtt_client->publish(TOPIC_SENSOR1, "ON");
    else mqtt_client->publish(TOPIC_SENSOR1, "OFF");
    
    #ifdef DEBUG
      Serial.println(F("Alterou o estado da bomba, PUMPonoff='OFF'. Payload enviado para o Broker MQTT."));
    #endif
    PUMPstatusAnt = PUMPonoff;
  } else if (PUMPonoff && !PUMPstatusAnt) {  // alterou o estado da bomba, PUMPonoff = ON
    mqtt_client->publish(TOPIC_PUBLISH, "ON");

    if (sensor0State == LOW) mqtt_client->publish(TOPIC_SENSOR0, "ON");
    else mqtt_client->publish(TOPIC_SENSOR0, "OFF");
    if (sensor1State == LOW) mqtt_client->publish(TOPIC_SENSOR1, "ON");
    else mqtt_client->publish(TOPIC_SENSOR1, "OFF");
    
    #ifdef DEBUG
      Serial.println(F("Alterou o estado da bomba, PUMPonoff='ON'. Payload enviado para o Broker MQTT."));
    #endif
    PUMPstatusAnt = PUMPonoff;
  }
}   // fim publishTOPIC

//=====================================================
// Função callback: fica "escutando" o servidor MQTT para receber as mensagens
//=====================================================
void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG
    Serial.print(F("Mensagem recebida do Broker MQTT ["));
    Serial.print(topic);
    Serial.print(F("] "));
  #endif
  for (int i = 0; i < length; i++) {
    #ifdef DEBUG
      Serial.print((char)payload[i]);
    #endif
  }
  #ifdef DEBUG
    Serial.println();
  #endif

  // Atualizando variável PUMPonoff. Se um "N" foi recebido como segundo caractere (payload = ON), então PUMPonoff = true
  if ((char)payload[1] == 'N') PUMPonoff = true;
  else PUMPonoff = false;
    #ifdef DEBUG
      Serial.println(F("Atualizada variável PUMPonoff conforme payload recebido."));
    #endif

  // verifica se alterou o estado da bomba com base na mensagem recebida
  if ((!PUMPonoff && PUMPstatusAnt) || (PUMPonoff && !PUMPstatusAnt)) {
    #ifdef DEBUG
      Serial.println(F("Alterou o estado da bomba!"));
      Serial.println(F("Atualizando estado da bomba e enviando boadcast para atualizar os clientes websockets ..."));
    #endif

  // atualiza estado da bomba
  if (PUMPonoff == false){
    rele0State = LOW;
    digitalWrite(rele0,rele0State);
  } else {
    rele0State = HIGH;
    digitalWrite(rele0,rele0State);    
  }

  // envia broadcast
  String PUMPstatus = "OFF";
  if (PUMPonoff == true) PUMPstatus = "ON";
  JSONtxt = "{\"PUMPonoff\":\"" + PUMPstatus + "\"}";
  webSocket.broadcastTXT(JSONtxt);
  #ifdef DEBUG
    Serial.print(F("JSONtxt = {\"PUMPonoff\":\""));
    Serial.print(PUMPstatus);
    Serial.println(F("\"}"));
  #endif
  PUMPstatusAnt = PUMPonoff;
  }
}   // fim callback


//=====================================================
// Função setDateTime: sincroniza o relógio do ESP com horário do NTP
//=====================================================
void setDateTime() {
  // Atualiza o horário do ESP com o servidor NTP
  // No caso do MQTT você pode usar seu próprio fuso horário, mas a hora exata não é usada. Apenas a data é necessária para validar os certificados.
  // Exemplo de conexão/uso dos parâmetros:
  // const char *NTP_SERVER = "pool.ntp.org";
  // seu fuso horário pode ser pesquisado em : https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // Central and South America Brazil,Sao Paulo  BRST+3BRDT+2,M10.3.0,M2.3.0
  // const char *TZ_INFO    = "BRST+3BRDT+2,M10.3.0,M2.3.0";
  //
  // #ifdef ESP8266
  //    configTime(TZ_INFO, NTP_SERVER);
  // #elseconfigTime(0, 0, NTP_SERVER);   // 0, 0 porque usaremos TZ na próxima linha
  //    setenv("TZ", TZ_INFO, 1);         // define variável de ambiente com seu fuso horário
  //    tzset();
  // #endif
  
  if (WiFi.status() == WL_CONNECTED) {    // se está conectado ao WiFi (ESP em modo ST), sincroniza com horário do servidor NTP

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    #ifdef DEBUG
      Serial.print(F("Aguardando sincronização de horário NTP: "));
    #endif
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
      delay(100);
      #ifdef DEBUG
        Serial.print(".");
      #endif
      now = time(nullptr);
    }
    
    #ifdef DEBUG
      // Serial.println();
      // Intervalo de sincronização - definido pela bibioteca lwIP
      Serial.printf("\nNTP sincroniza a cada\t\t: %d segundos\n", SNTP_UPDATE_DELAY / 1000);
    #endif
    
    // Data e hora local, referência https://cplusplus.com/reference/ctime/localtime/
    // Converte time_t para tm como hora local (função)
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    #ifdef DEBUG
      Serial.printf ("Data e hora atual\t\t: %s", asctime(timeinfo));
    #endif
  }
}   // fim setDateTime


//=====================================================
// Função dateTimeStr: Formata time_t como "aaaa-mm-dd hh:mm:ss"
//=====================================================
String dateTimeStr(time_t t, int8_t tz = gmtOffset_sec / 3600) {  //tz=0 (GMT), TZ= -3 (Fuso BRA)
  // Formata time_t como "aaaa-mm-dd hh:mm:ss"
  if (t == 0) {
    return "N/D";
  } else {
    t += tz * 3600;                               // Ajusta fuso horário
    struct tm *ptm;
    ptm = gmtime(&t);
    String s;
    s = ptm->tm_year + 1900;
    s += "-";
    if (ptm->tm_mon < 9) {
      s += "0";
    }
    s += ptm->tm_mon + 1;
    s += "-";
    if (ptm->tm_mday < 10) {
      s += "0";
    }
    s += ptm->tm_mday;
    s += " ";
    if (ptm->tm_hour < 10) {
      s += "0";
    }
    s += ptm->tm_hour;
    s += ":";
    if (ptm->tm_min < 10) {
      s += "0";
    }
    s += ptm->tm_min;
    s += ":";
    if (ptm->tm_sec < 10) {
      s += "0";
    }
    s += ptm->tm_sec;
    return s;
  }
}   // fim dateTimeStr


//=====================================================
// Função ntpSync_cb: callback de sincronização para manter o relógio do ESP atualizado com horário do NTP
//=====================================================
#ifdef ESP8266
  void ntpSync_cb() {                     // ESP8266
#else
  void ntpSync_cb(struct timeval *tv) {   // ESP32. Precisa da definição de parâmetros (estrutura dentro da função), mas não será utilizado no código.
#endif
    time_t t;                             // lê a data atual quando for chamada a sincronização
    t = time(NULL);
    // Data/Hora da próxima atualização
    // SNTP_UPDATE_DELAY é a variável interna utilizada na bibioteca lwIP padrão do ESP
    // Por padrão SNTP_UPDATE_DELAY = 1 hora. Ela é em milisegundos, tem que dividir por 1.000 para converter para segundos
    nextNTPSync = t + (SNTP_UPDATE_DELAY / 1000) + 60;      // calcula a partir da atualização a próxima hora de atualização
                                                            // + 60s de delay para ficar um intervalo maior.
                                                            // Caso falhe alguma coisa, não ficará com a data imediatamente desatualizada
    #ifdef DEBUG
      Serial.printf("\nSincronizou com NTP em\t\t: %s\n", dateTimeStr(t).c_str());
      Serial.printf("Próxima sincronização é\t\t: %s\n", dateTimeStr(nextNTPSync).c_str());
    #endif

}   // fim ntpSync_cb


//=====================================================
// Função ntpStatus: obtém o status da sinronização com o NTP
//=====================================================
String ntpStatus() {
  if (nextNTPSync == 0) {
    return "não definida";
  } else if (time(NULL) < nextNTPSync) {
    return "atualizada";
  } else {
    return "atualização pendente";
  }
}   // fim ntpStatus


//=====================================================
// Função configMQTT: configura servidor MQTT usando TLS
//=====================================================
void configMQTT(){
  // Usando os certificados CA para a conexão com o servidor MQTT
  // O certificado da pasta data tem que ser previamente carregado na flash: menu Ferramentas/ESP 8266 LittleFS Data Upload
  // Você pode usar o modo inseguro, quando quiser evitar os certificados: net.setInsecure()
    
  if (WiFi.status() == WL_CONNECTED) {    // se está conectado ao WiFi (ESP em modo ST), configura servidor MQTT

    // Feito a inicialização de LittleFS no setup. Não precisa inicializar aqui novamente.
    // LittleFS.begin();                     // utilizado para carregar o arquivo com certificado CA da pasta data para o ESP. 
    int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    #ifdef DEBUG
      Serial.printf("Número certificados CA lidos\t: %d\n", numCerts);
    #endif
    
    if (numCerts == 0) {                  // se não tem certificados na pasta "data"
      #ifdef DEBUG
        Serial.printf("Nenhum certificado encontrado.\nVocê executou certs-from-mozilla.py e carregou o diretório data LittleFS antes de executar?\n");      
      #endif
      // return;                          // Descomente esta linha para não ser possível se conectar a nada sem certificados!

      #ifdef DEBUG
        Serial.printf("Definindo conexão criptografada sem certificado \t: ");
      #endif

      esp_client.setInsecure();           // Modo "insegura", conexão criptografada sem certificado.
      mqtt_client = new PubSubClient(esp_client);
      
      #ifdef DEBUG
        Serial.printf("OK!\n");
      #endif

    } else {                              // se tem certificados na pasta "data"
      #ifdef DEBUG
        Serial.printf("Carregando o certificado e definindo conexão segura\t: ");
      #endif

      BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
      bear->setCertStore(&certStore);    // integre a loja de certificados com esta conexão
      mqtt_client = new PubSubClient(*bear);
            
      #ifdef DEBUG
        Serial.printf("OK!\n");
      #endif
    }

    // configura servidor MQTT
    // indica para o objeto "MQTT" em que servidor e em que porta iremos nos conectar
    #ifdef DEBUG
      Serial.printf("Servidor MQTT configurado\t: %s:%d\n", BROKER_MQTT, BROKER_PORT);
    #endif

    mqtt_client->setServer(BROKER_MQTT,BROKER_PORT);    // configura servidor MQTT
    mqtt_client->setCallback(callback);                 // habilita callback para receber mensagens do Broker MQTT
    
    #ifdef DEBUG
      Serial.printf("OK!\n");
    #endif

  }
}   // fim configMQTT


//=====================================================
// Função enviarRequisicao: Envia requição para o Webhook
//=====================================================
bool enviarRequisicao(const char *url) {
  sender.begin(webhook_client ,url);
  int httpCode = sender.GET();
  sender.end();
  return httpCode == 200; // Verificar se a requisição foi bem-sucedida
}   // fim enviarRequisicao


//=====================================================
// Função checkSensores: verifica sensores de nível
//=====================================================
void checkSensores() {
  static unsigned long debounceSensor0 = millis();
  static unsigned long debounceSensor1 = millis();

  // Verificando se está sem água (sensor0 = HIGH) e se a bomba está ligada (rele0State = HIGH), 
  // então desliga a bomba para evitar sua queima ou seu funcionamento a vazio (nível baixo - desliga a bomba)
  sensor0State = digitalRead(sensor0);
  if(sensor0State == HIGH && rele0State == HIGH && ((millis() - debounceSensor0) > 500)) {
     rele0State = LOW;
     digitalWrite(rele0,rele0State);
     PUMPonoff = false;
    // realizar a requisição HTTP Webhook
    if (enviarRequisicao(webhookUrl2)) {    //Trigger2 - Bomba Sala, sensor de disparo de nível baixo da Bomba Sala
      #ifdef DEBUG
        Serial.println("Webhook acionado com sucesso!");
      #endif
    } else {
      #ifdef DEBUG
        Serial.println("Erro ao acionar o webhook.");
      #endif
    }     
     debounceSensor0 = millis();
  }
         
  // Verificando se a água atingiu o sensor de nível alto (sensor1State = LOW) e se a bomba está desligada (rele0State = LOW),
  // então liga a bomba (rele0State = HIGH) para iniciar a drenagem (nível alto - liga a bomba)
  sensor1State = digitalRead(sensor1);
  if(sensor0State == LOW && sensor1State == LOW && rele0State == LOW && ((millis() - debounceSensor1) > 500)) {
     rele0State = HIGH;
     digitalWrite(rele0,rele0State);
     delay(500);
     PUMPonoff = true;
    // realizar a requisição HTTP Webhook
    if (enviarRequisicao(webhookUrl1)) {    //Trigger1 - Bomba Sala, sensor de disparo de nível alto da Bomba Sala
      #ifdef DEBUG
        Serial.println("Webhook acionado com sucesso!");
      #endif
    } else {
      #ifdef DEBUG
        Serial.println("Erro ao acionar o webhook.");
      #endif
    }
     debounceSensor1 = millis();
  }

  if (WiFi.status() == WL_CONNECTED){
        // Atualizando status sensores
        if ((millis() - lastTime2) > timerDelay2) {
          if (sensor0State == LOW) mqtt_client->publish(TOPIC_SENSOR0, "ON");
          else mqtt_client->publish(TOPIC_SENSOR0, "OFF");
          if (sensor1State == LOW) mqtt_client->publish(TOPIC_SENSOR1, "ON");
          else mqtt_client->publish(TOPIC_SENSOR1, "OFF");
          lastTime2 = millis();
        }
  }
}   // fim checkSensores

//=====================================================
// Função callbackAlexa_01: Call back Alexa dispositivo 01 (relé)
//=====================================================
void callbackAlexa_01(uint8_t brilho) {
   // rotina para ligar e desligar a bomba
  if (brilho && sensor0State == LOW && rele0State == LOW) {
    rele0State = HIGH;
    digitalWrite(rele0,rele0State);
    delay(500);
  }
  else  {
     rele0State = LOW;
     digitalWrite(rele0,rele0State);
  }

  #ifdef DEBUG
    Serial.printf("Executou callback Alexa: \t%s = %d\n", "Brilho", brilho);
  #endif
  
  // Atualizando variável PUMPonoff.
  if (brilho) PUMPonoff = true;
  else PUMPonoff = false;
    #ifdef DEBUG
      Serial.println(F("Atualizada variável PUMPonoff conforme comando recebido da Alexa."));
    #endif

  // verifica se alterou o estado da bomba com base na mensagem recebida
  if ((!PUMPonoff && PUMPstatusAnt) || (PUMPonoff && !PUMPstatusAnt)) {
    #ifdef DEBUG
      Serial.println(F("Alterou o estado da bomba!"));
      Serial.println(F("Atualizando estado da bomba e enviando boadcast para atualizar os clientes websockets ..."));
    #endif

  /*
  // atualiza estado da bomba
  if (PUMPonoff == false){
    rele0State = LOW;
    digitalWrite(rele0,rele0State);
  } else {
    rele0State = HIGH;
    digitalWrite(rele0,rele0State);    
  }
  */

  // envia broadcast
  String PUMPstatus = "OFF";
  if (PUMPonoff == true) PUMPstatus = "ON";
  JSONtxt = "{\"PUMPonoff\":\"" + PUMPstatus + "\"}";
  webSocket.broadcastTXT(JSONtxt);
  #ifdef DEBUG
    Serial.print(F("JSONtxt = {\"PUMPonoff\":\""));
    Serial.print(PUMPstatus);
    Serial.println(F("\"}"));
  #endif
  PUMPstatusAnt = PUMPonoff;
  }  
  
}   // fim callbackAlexa_01


//=====================================================
// Função hexStr: Converte valor em formato hexadecimal
//=====================================================
String hexStr(const unsigned long &h, const byte &l = 8) {
  // Retorna valor em formato hexadecimal
  String s;
  s= String(h, HEX);
  s.toUpperCase();
  s = ("00000000" + s).substring(s.length() + 8 - l);
  return s;
}   // fim hexStr


//=====================================================
// Função getInfoData: Coleta os dados do dispositivo
//=====================================================
String getInfoData(String id){
  String p;
  if(id==F("esphead")){
    p = FPSTR(HTTP_INFO_esphead);
    p.replace(F("{1}"), hexStr(ESP.getChipId()));
  }
  else if(id==F("wifihead")){
    p = FPSTR(HTTP_INFO_wifihead);
    if (WiFi.getMode()==0){
      p.replace(F("{1}"), "WIFI_OFF");        
    } else if (WiFi.getMode()==1){
      p.replace(F("{1}"), "WIFI_STA");
    } else if (WiFi.getMode()==2){
      p.replace(F("{1}"), "WIFI_AP");
    } else if (WiFi.getMode()==3){
      p.replace(F("{1}"), "WIFI_AP_STA");
    }            
  }
  else if(id==F("uptime")){
    // subject to rollover!
    
    unsigned long elapsedTime = millis(); // tempo atual (prazo decorrido)
    
    // Converta o tempo decorrido em dias, horas, minutos e segundos
    unsigned long seconds = elapsedTime / 1000;  // 1 segundo = 1000 milissegundos
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;
    
    // Calcule o tempo restante após converter para dias, horas, minutos e segundos
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    // Usar a classe String para formatar o valor com 2 posições, preenchendo com zeros à esquerda
    String strSeconds = String(seconds, DEC);
    if (strSeconds.length() == 1) strSeconds = "0" + strSeconds;
    String strMinutes = String(minutes, DEC);
    if (strMinutes.length() == 1) strMinutes = "0" + strMinutes;
    String strHours = String(hours, DEC);
    if (strHours.length() == 1) strHours = "0" + strHours;
    
    p = FPSTR(HTTP_INFO_uptime);
    p.replace(F("{1}"),(String)(days));
    p.replace(F("{2}"),strHours+":"+strMinutes+":"+strSeconds);
  }
  else if(id==F("bootcount")){
    p = FPSTR(HTTP_INFO_bootcount);
    p.replace(F("{1}"),(String)user_param.bootCount);
  }
  else if(id==F("chipid")){
    p = FPSTR(HTTP_INFO_chipid);
    p.replace(F("{1}"),(String)ESP.getChipId());
  }
  else if(id==F("fchipid")){
      p = FPSTR(HTTP_INFO_fchipid);
      p.replace(F("{1}"),(String)ESP.getFlashChipId());
  }
  else if(id==F("idesize")){
    p = FPSTR(HTTP_INFO_idesize);
    p.replace(F("{1}"),(String)ESP.getFlashChipSize());
  }
  else if(id==F("flashsize")){
    p = FPSTR(HTTP_INFO_flashsize);
    p.replace(F("{1}"),(String)ESP.getFlashChipRealSize());
  }
  else if(id==F("corever")){
    p = FPSTR(HTTP_INFO_corever);
    p.replace(F("{1}"),(String)ESP.getCoreVersion());
  }
  else if(id==F("bootver")){
    p = FPSTR(HTTP_INFO_bootver);
    p.replace(F("{1}"),(String)system_get_boot_version());
  }
  else if(id==F("cpufreq")){
    p = FPSTR(HTTP_INFO_cpufreq);
    p.replace(F("{1}"),(String)ESP.getCpuFreqMHz());
  }
  else if(id==F("freeheap")){
    p = FPSTR(HTTP_INFO_freeheap);
    p.replace(F("{1}"),(String)ESP.getFreeHeap());
  }
  else if(id==F("memsketch")){
    p = FPSTR(HTTP_INFO_memsketch);
    p.replace(F("{1}"),(String)(ESP.getSketchSize()));
    p.replace(F("{2}"),(String)(ESP.getSketchSize()+ESP.getFreeSketchSpace()));
  }
  else if(id==F("memsmeter")){
    p = FPSTR(HTTP_INFO_memsmeter);
    p.replace(F("{1}"),(String)(ESP.getSketchSize()));
    p.replace(F("{2}"),(String)(ESP.getSketchSize()+ESP.getFreeSketchSpace()));
  }
  else if(id==F("lastreset")){
    p = FPSTR(HTTP_INFO_lastreset);
    p.replace(F("{1}"),(String)ESP.getResetReason());
  }
  else if(id==F("apip")){
    p = FPSTR(HTTP_INFO_apip);
    p.replace(F("{1}"),WiFi.softAPIP().toString());
  }
  else if(id==F("apmac")){
    p = FPSTR(HTTP_INFO_apmac);
    p.replace(F("{1}"),(String)WiFi.softAPmacAddress());
  }
  else if(id==F("apssid")){
    p = FPSTR(HTTP_INFO_apssid);
    //p.replace(F("{1}"),htmlEntities(WiFi.softAPSSID()));
    p.replace(F("{1}"),ssidName + " - " + String(ESP.getChipId(), HEX));
  }
  else if(id==F("apbssid")){
    p = FPSTR(HTTP_INFO_apbssid);
    p.replace(F("{1}"),(String)WiFi.BSSIDstr());
  }
  else if(id==F("stassid")){
    p = FPSTR(HTTP_INFO_stassid);
//    p.replace(F("{1}"),htmlEntities((String)WiFi_SSID()));
    p.replace(F("{1}"),user_wifi.ssid);
  }
  else if(id==F("staip")){
    p = FPSTR(HTTP_INFO_staip);
    p.replace(F("{1}"),WiFi.localIP().toString());
  }
  else if(id==F("stagw")){
    p = FPSTR(HTTP_INFO_stagw);
    p.replace(F("{1}"),WiFi.gatewayIP().toString());
  }
  else if(id==F("stasub")){
    p = FPSTR(HTTP_INFO_stasub);
    p.replace(F("{1}"),WiFi.subnetMask().toString());
  }
  else if(id==F("dnss")){
    p = FPSTR(HTTP_INFO_dnss);
    p.replace(F("{1}"),WiFi.dnsIP().toString());
  }
  else if(id==F("host")){
    p = FPSTR(HTTP_INFO_host);
    p.replace(F("{1}"),WiFi.hostname());
  }
  else if(id==F("stamac")){
    p = FPSTR(HTTP_INFO_stamac);
    p.replace(F("{1}"),WiFi.macAddress());
  }
  else if(id==F("conx")){
    p = FPSTR(HTTP_INFO_conx);
    p.replace(F("{1}"),WiFi.isConnected() ? "Sim" : "Não");
  }
  else if(id==F("autoconx")){
    p = FPSTR(HTTP_INFO_autoconx);
    p.replace(F("{1}"),WiFi.getAutoConnect() ? "Habilitado" : "Desabilitado");
  }
  else if(id==F("paramhead")){
    p = FPSTR(HTTP_INFO_paramhead);
    p.replace(F("{1}"),(String)CONFIG_FILE);
  }
  else if(id==F("broker")){
    p = FPSTR(HTTP_INFO_broker);
    // p.replace(F("{1}"),(String)user_param.URL);
    p.replace(F("{1}"),String(user_param.URL).substring(0,24));
    p.replace(F("{2}"),String(user_param.URL).substring(24));
  }
  else if(id==F("porta")){
    p = FPSTR(HTTP_INFO_porta);
    p.replace(F("{1}"),(String)user_param.PORT);
  }
  else if(id==F("user")){
    p = FPSTR(HTTP_INFO_user);
    p.replace(F("{1}"),(String)user_param.USER);
  }
  else if(id==F("pass")){
    p = FPSTR(HTTP_INFO_pass);
    p.replace(F("{1}"),(String)user_param.PASS);
  }
  else if(id==F("pub")){
    p = FPSTR(HTTP_INFO_pub);
    p.replace(F("{1}"),(String)user_param.PUB);
  }
  else if(id==F("sub")){
    p = FPSTR(HTTP_INFO_sub);
    p.replace(F("{1}"),(String)user_param.SUB);
  }
  else if(id==F("hostmdns")){
    p = FPSTR(HTTP_INFO_hostmdns);
    p.replace(F("{1}"),(String)user_param.host);
  }
  else if(id==F("alexa01")){
    p = FPSTR(HTTP_INFO_alexa01);
    p.replace(F("{1}"),user_param.Alexa01);
  }

  return p;
}   // fim getInfoData


//=====================================================
// Função formattedDateTime: Formata __DATE__ e __TIME__ como "aaaa-mm-dd hh:mm:ss"
//=====================================================
String formattedDateTime() {
  // Obter a data no formato __DATE__ e a hora no formato __TIME__
  String dateStr = __DATE__;
  String timeStr = __TIME__;
  
  // Extrair o dia, mês e ano da string da data
  String day = dateStr.substring(4, 7);
  String month = dateStr.substring(0, 3);
  String year = dateStr.substring(7, 11);

  // Extrair a hora, minuto e segundo da string da hora
  String hour = timeStr.substring(0, 2);
  String minute = timeStr.substring(3, 5);
  String second = timeStr.substring(6, 8);

  // Mapear o mês de formato abreviado para formato numérico
  if      (month == "Jan") month = "01";
  else if (month == "Feb") month = "02";
  else if (month == "Mar") month = "03";
  else if (month == "Apr") month = "04";
  else if (month == "May") month = "05";
  else if (month == "Jun") month = "06";
  else if (month == "Jul") month = "07";
  else if (month == "Aug") month = "08";
  else if (month == "Sep") month = "09";
  else if (month == "Oct") month = "10";
  else if (month == "Nov") month = "11";
  else if (month == "Dec") month = "12";

  // Remover espaços em branco do dia e preencher com zero à esquerda, se necessário
  day.trim();
  if (day.length() == 1) day = "0" + day;
  
  // Formatar a data e hora no formato "aaaa-mm-dd hh:mm:ss"
  String formattedDateTime = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;

  // Enviar a data e hora formatadas para a porta serial
  //#ifdef DEBUG
  //  Serial.println("Data e hora formatadas: " + formattedDateTime);
  //#endif
  
  // Retorna a data e hora formatadas
  return formattedDateTime;
  
}   // fim formattedDateTime


//=====================================================
// Função handleInfo: Requisições página de informações
//=====================================================
void handleInfo() {
  String pageInfo = FPSTR(HTTP_HEAD_START);
  uint16_t infos = 0;
  infos = 37;
    String infoids[] = {
      F("esphead"),
      F("uptime"),
      F("bootcount"),
      F("chipid"),
      F("fchipid"),
      F("idesize"),
      F("flashsize"),
      F("corever"),
      F("bootver"),
      F("cpufreq"),
      F("freeheap"),
      F("memsketch"),
      F("memsmeter"),
      F("lastreset"),
      F("wifihead"),
      F("conx"),
      F("stassid"),
      F("staip"),
      F("stagw"),
      F("stasub"),
      F("dnss"),
      F("host"),
      F("stamac"),
      F("autoconx"),
      F("apssid"),
      F("apip"),
      F("apbssid"),
      F("apmac"),
      F("paramhead"),
      F("broker"),
      F("porta"),
      F("user"),
      F("pass"),
      F("pub"),
      F("sub"),
      F("hostmdns"),
      F("alexa01")
    };

  for(size_t i=0; i<infos;i++){
    if(infoids[i] != NULL) pageInfo += getInfoData(infoids[i]);
  }
 
  pageInfo += F("</table>");
  pageInfo += F("<hr><h3>Sobre</h3><hr><table>");
  pageInfo += F("<tr><th>Software</th><td>")+ softwareNom + F("</td></tr>");
  pageInfo += F("<tr><th>Compilado em</th><td>")+ formattedDateTime() + F("</td></tr>");
  pageInfo += F("<tr><th>Por</th><td>Léviro Péres Emydio</td></tr></table>");
  
  pageInfo += FPSTR(HTTP_BACKBTN);
 // pageInfo += FPSTR(HTTP_HELP);
  pageInfo += FPSTR(HTTP_END);

  server.send(200, "text/html", pageInfo);
}   // fim handleInfo


//=====================================================
// Função handleParam: Configura servidor/página de parâmetros
//=====================================================
void handleParam() {

 if (server.method() == HTTP_POST) {
    strncpy(user_param.URL, server.arg("url").c_str(), sizeof(user_param.URL));       // A função strncpy copia os n primeiros caracteres de uma string para outra.
    user_param.PORT = server.arg("port").toInt();                                     // Contudo, isso pode causar overflow, sendo necessário definir onde termina a string destino.
    strncpy(user_param.USER, server.arg("user").c_str(), sizeof(user_param.USER));    // Para isso, coloca-se o terminador \0 no no final da string.
    strncpy(user_param.PASS, server.arg("pass").c_str(), sizeof(user_param.PASS));
    strncpy(user_param.PUB, server.arg("pub").c_str(), sizeof(user_param.PUB));
    strncpy(user_param.SUB, server.arg("sub").c_str(), sizeof(user_param.SUB));
    strncpy(user_param.host, server.arg("host").c_str(), sizeof(user_param.host));
    user_param.Alexa01 = server.arg("alexa01");
    user_param.URL[server.arg("url").length()] = '\0';
    user_param.USER[server.arg("user").length()] = '\0';
    user_param.PASS[server.arg("pass").length()] = '\0';
    user_param.PUB[server.arg("pub").length()] = '\0';
    user_param.SUB[server.arg("sub").length()] = '\0';
    user_param.host[server.arg("host").length()] = '\0';

//    EEPROM.put(0, user_wifi);
//    EEPROM.commit();

    Serial.println("SALVOU ........");

    server.send(200, "text/html", F("<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Setup Portal</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Configurar Parâmetros</h1> <br/> <p>Suas configura&ccedil;&otilde;es foram salvas com sucesso!<br />O m&oacute;dulo ser&aacute; reinicializado em alguns segundos.</p></main></body></html>") );
//    ESP.reset();

  } else {  
    String pageParam = FPSTR(webpageParam);                      // carrega o código da página Parâmetros - webpageParam
    pageParam.replace(F("{url}"),String(user_param.URL));        // atualiza o campo url
    pageParam.replace(F("{port}"),String(user_param.PORT));      // atualiza o campo port
    pageParam.replace(F("{user}"),String(user_param.USER));      // atualiza o campo user
    pageParam.replace(F("{pass}"),String(user_param.PASS));      // atualiza o campo pass
    pageParam.replace(F("{pub}"),String(user_param.PUB));        // atualiza o campo pub
    pageParam.replace(F("{sub}"),String(user_param.SUB));        // atualiza o campo sub
    pageParam.replace(F("{host}"),String(user_param.host));      // atualiza o campo host
    pageParam.replace(F("{alexa01}"),user_param.Alexa01);        // atualiza o campo alexa01
    server.send(200, "text/html", pageParam);
  }
  
}   // fim handleParam


//=====================================================
// Função configAlexa: Configura a Alexa
//=====================================================
void configAlexa() {
  if(WiFi.status() == WL_CONNECTED){
    
    server.on("/info", handleInfo);              // Configurando página de informações
    // server.on("/param", handleParam);            // Configurando página de parâmetros
    
    server.onNotFound([](){
      if (!espalexa.handleAlexaApiCall(server.uri(),server.arg(0))){      // se você não conhece o URI, pergunte ao espalexa se é uma solicitação de controle do Alexa
        server.send(404, "text/plain", "Pagina nao encontrada!");         // o que você quiser fazer com 404s
        }
      });
      
      // Defina seus dispositivos Alexa aqui.
      espalexa.addDevice(deviceName01, callbackAlexa_01);                 // definição mais simples, estado padrão desligado
  
      espalexa.begin(&server);                                            // dê ao espalexa um ponteiro para seu objeto de servidor para que ele possa usar seu servidor em vez de criar seu próprio
      // server.begin();                                                  // omita isso pois será feito por espalexa.begin(&server)
  }
}   // fim configAlexa

//=====================================================
// Função checkRede: Verifica redes disponíveis em modo AP para conexão em modo ST (após reset)
//=====================================================
void checkRede() {                                                                   // verifica se a rede pré-configurada está disponível para conexão em modo ST
  unsigned long currentMillis2 = millis();
  if ( (WiFi.SSID()!="") && (currentMillis2 - previousMillis2 >= intervalo2) ) {     // verifica a cada "intervalo2" se há rede pré-configurada: WiFi.SSID()!= ""
    previousMillis2 = currentMillis2;
    #ifdef DEBUG
      // strcmp(user_wifi.ssid,String((char)0xFFFD).c_str())!=02
      Serial.print(F("Tentando reconexão .... "));
      Serial.println(previousMillis2);
      Serial.print(F("SSID da Rede .......... "));
      Serial.println(user_wifi.ssid);
    #endif
    // procurar redes próximas:
    #ifdef DEBUG
      Serial.println(F("** Procurando Redes **"));
    #endif
    byte numSsid = WiFi.scanNetworks();
    // imprimir a lista de redes encontradas:
    #ifdef DEBUG
      Serial.print(F("Lista de SSIDs:"));
      Serial.println(numSsid);
    #endif
    // imprimir o número e o nome para cada rede encontrada:
    for (int thisNet = 0; thisNet<numSsid; thisNet++) {
      #ifdef DEBUG
        Serial.print(thisNet);
        Serial.print(F(") Rede: "));
        Serial.println(WiFi.SSID(thisNet));
      #endif                    
      if (WiFi.SSID(thisNet) == user_wifi.ssid){        // estando disponível, reseta o ESP
        #ifdef DEBUG
          Serial.print(F("Rede "));
          Serial.print(WiFi.SSID(thisNet));
          Serial.println(F(" voltou !!!!!!!!!!"));
          Serial.println(F("<<<<<<<<<< Resetando o ESP em 10s >>>>>>>>>>"));
        #endif                    
        delay(10000);
        ESP.reset();
      }
    }
  }
}   // fim checkRede


//=====================================================
// Função resetConfiguration: Reseta configuração do arquivo JSON, definindo o padrão
//=====================================================
void  resetConfiguration() {
  // Define configuração padrão
  strlcpy(user_param.URL, BROKER_MQTT, sizeof(user_param.URL)); 
  user_param.PORT = BROKER_PORT;
  strlcpy(user_param.USER, DISP_USER, sizeof(user_param.USER)); 
  strlcpy(user_param.PASS, DISP_PASSWORD, sizeof(user_param.PASS)); 
  strlcpy(user_param.PUB, TOPIC_PUBLISH, sizeof(user_param.PUB)); 
  strlcpy(user_param.SUB, TOPIC_SUBSCRIBE, sizeof(user_param.SUB)); 
  strlcpy(user_param.host, myHostname, sizeof(user_param.host)); 
  user_param.Alexa01 = deviceName01;
  user_param.bootCount = 0;
}   // fim resetConfiguration


//=====================================================
// Função loadConfiguration: Carrega a configuração do arquivo JSON
//=====================================================
void loadConfiguration(const char *CONFIG_FILE, ConfigData &user_param) {
  // Lendo o arquivo de configuração
  File configFile = LittleFS.open(CONFIG_FILE, "r");

  // Aloca um JsonDocument temporário
  // Não se esqueça de alterar a capacidade para atender às suas necessidades.
  // Use https://arduinojson.org/assistant para calcular a capacidade.
  StaticJsonDocument<768> jsonDoc;

  // Desserializar o documento JSON
  DeserializationError error = deserializeJson(jsonDoc, configFile);
  
  if (error) {
    // Falha na leitura, assume valores padrão
    resetConfiguration();
    #ifdef DEBUG
      Serial.print(F("\nFalha ao ler o arquivo JSON, usando a configuração padrão. Falha: "));
      Serial.println(error.f_str());
    #endif
    return;
  } else {
    // Sucesso na leitura
    // Copia os valores do JsonDocument para o ConfigData
    strlcpy(user_param.URL, jsonDoc["URL"] | "", sizeof(user_param.URL)); 
    user_param.PORT = jsonDoc["PORT"] | 0;
    strlcpy(user_param.USER, jsonDoc["USER"] | "", sizeof(user_param.USER)); 
    strlcpy(user_param.PASS, jsonDoc["PASS"] | "", sizeof(user_param.PASS)); 
    strlcpy(user_param.PUB, jsonDoc["PUB"] | "", sizeof(user_param.PUB)); 
    strlcpy(user_param.SUB, jsonDoc["SUB"] | "", sizeof(user_param.SUB)); 
    strlcpy(user_param.host, jsonDoc["host"] | "", sizeof(user_param.host)); 
    user_param.Alexa01 = jsonDoc["Alexa01"].as<String>();
    user_param.bootCount = jsonDoc["bootCount"] | 0;

    // Feche o arquivo (curiosamente, o destructor do arquivo não fecha o arquivo)
    configFile.close();

    #ifdef DEBUG
      Serial.println(F("\nLendo configuração arquivo config.json:"));
      serializeJsonPretty(jsonDoc, Serial);
      Serial.println("");
    #endif
  }

}   // fim loadConfiguration


//=====================================================
// Função saveConfiguration: Salva a configuração no arquivo JSON
//=====================================================
void saveConfiguration(const char *CONFIG_FILE, const ConfigData &user_param) {
  // Abre o arquivo de configuração para gravação
  File configFile = LittleFS.open(CONFIG_FILE, "w+");
  
  if (!configFile) {
    #ifdef DEBUG
      Serial.println(F("Falha ao criar ou gravar o arquivo JSON"));
    #endif
    // return;
  } else {
    // Aloca um JsonDocument temporário
    // Não se esqueça de alterar a capacidade para atender às suas necessidades.
    // Use https://arduinojson.org/assistant para calcular a capacidade.
    StaticJsonDocument<768> jsonDoc;
    
    // Atribui valores ao arquivo JSON e grava
    jsonDoc["URL"]        = user_param.URL;
    jsonDoc["PORT"]       = user_param.PORT;
    jsonDoc["USER"]       = user_param.USER;
    jsonDoc["PASS"]       = user_param.PASS;
    jsonDoc["PUB"]        = user_param.PUB;
    jsonDoc["SUB"]        = user_param.SUB;
    jsonDoc["host"]       = user_param.host;
    jsonDoc["Alexa01"]    = user_param.Alexa01;
    jsonDoc["bootCount"]  = user_param.bootCount;

    serializeJsonPretty(jsonDoc, configFile);
    configFile.close();

    #ifdef DEBUG
      Serial.println(F("\nGravando configuração no arquivo config.json:"));
      serializeJsonPretty(jsonDoc, Serial);
      Serial.println();
    #endif
  }
}   // fim saveConfiguration
