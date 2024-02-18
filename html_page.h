//Header file "html_page.h":

//=====================
// HTML códigos 
//=====================


//=====================================================
// Webpage ServerToggle: servidor Portal Toggle
//=====================================================
const char webpageCode[] PROGMEM =
R"=====(
<!doctype html>
<html lang='pt-BR'>
<head>
  <meta charset='utf-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <title>{Titulo}</title>
</head>
<!-------------------------------C S S------------------------------>
<style>
  #btn
  {
    display: inline-block;
    text-decoration: none;
    background: #8CD460;
    color: rgba(255,255,255, 0.80);
    font-weight: bold;
    font: 60px arial, sans-serif;
    width: 150px;
    height: 150px;
    line-height: 150px;
    border-radius: 50%;
    text-align: center;
    vertical-align: middle;
    overflow: hidden;
    box-shadow: 0px 0px 0px 8px #8CD460;
    border: solid 2px rgba(255,255,255, 0.47);
    transition: 0.5s;
  }
  body {text-align:center; font-family:'Calibri'; background-color:rgba(0, 3, 8, 0.26)}
  h1   {color: rgba(0, 0, 255, 0.87); font-size: 30px;}
  dl   {color: rgba(0, 0, 0, 0.87); font-size: 12px;}
</style>
<!------------------------------H T M L----------------------------->
<body>
   <h1>Controle {EspId}<br>{DeviceName}</h1>
   <br><br><br>
   <a href='#' id='btn' ONCLICK='button()'> </a>
   <br><br><br><br>
   <dl>
     <dt>Informações</dt><br>
     <dt>IPv4: {IP}</dt>
     <dt>Chip ID: {ChipId}</dt>
     <dt>Flash Chip ID: {FlashId}</dt>
     <dt>Station MAC: {MAC}</dt>
   </dl>
   <br><br>
   <p><a href='/info'>Sobre</a></p>     
   <p><a href='/param'>Configurar</a></p>     
<!-----------------------------JavaScript--------------------------->
  <script>
     InitWebSocket()
     function InitWebSocket()
     {
       websock = new WebSocket('ws://'+window.location.hostname+':{WEBSOCKET_PORT}/'); 
       websock.onmessage = function(evt)
       {
          JSONobj = JSON.parse(evt.data);
          document.getElementById('btn').innerHTML = JSONobj.PUMPonoff;
          if(JSONobj.PUMPonoff == 'ON')
          {
            document.getElementById('btn').style.background='#FF0000';
            document.getElementById('btn').style['boxShadow'] = '0px 0px 0px 8px #FF0000';
          }
          else
          {
            document.getElementById('btn').style.background='#111111';
            document.getElementById('btn').style['boxShadow'] = '0px 0px 0px 8px #111111';
          }
       }
     }
     //-------------------------------------------------------------
     function button()
     {
        btn = 'PUMPonoff=ON';
        if(document.getElementById('btn').innerHTML == 'ON')
        {
          btn = 'PUMPonoff=OFF';
        }
        websock.send(btn);
     }
  </script>
</body>
</html>
)=====";


//=====================================================
// Webpage Portal Captive: Portal Cativo para o cliente
//=====================================================
const char webpageCaptive[] PROGMEM =
R"=====(
<!doctype html>
<html lang='pt-BR'><head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Setup Portal</title>
        <style>
            *,
            ::after,
            ::before {box-sizing:border-box;}
            .c {text-align: center;}
            body {margin:0; font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans'; font-size:1rem; font-weight:400; line-height:1.5; color:#212529; background-color:#f5f5f5;}
            .form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}
            .form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}
            h1{text-align: center;}
            .d{float:right ;width: 48px;text-align: left;}
            .s{float:right ;padding-right: 14px ;width: 84px;text-align: right;}
            .q{float:right; width: 70px;text-align: right;}
            .b{background: lightgreen no-repeat left center;background-size: 1.5em;}
            .l{background: lightcoral no-repeat left center;background-size: 1.5em;}
        </style>
        
        <script>
            function c(l) {
                document.getElementById('s').value=l.innerText||l.textContent;
                document.getElementById('p').focus();
            }
        </script>
        </head>
        <body>
            <main class='form-signin'><h1>Configurar Rede WiFi</h1>
)=====";


//=====================================================
// Webpage Reset: Página exibindo reset em 10s
//=====================================================
const char webpageReset[] PROGMEM =
R"=====(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Reinicialização</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f4f4f4;
            text-align: center;
        }
        .message {
            font-size: 20px;
        }
    </style>
</head>
<body>
<div class="message" id="message">
    Aguarde, seu dispositivo será reinicializado em <span id="countdown">10</span> segundos.
</div>
<script>
    var count = 10;
    var countdownElement = document.getElementById("countdown");
    var messageElement = document.getElementById("message");

    var interval = setInterval(function() {
        countdownElement.innerText = count;
        count--;

        if (count < 0) {
            clearInterval(interval);
            messageElement.innerText = "Reinicialização realizada!";
        }
    }, 1000);
</script>
</body>
</html>
)=====";


//=====================================================
// Webpage Parametros: Portal Configuração dos Parâmetros
//=====================================================
const char webpageParam[] PROGMEM =
R"=====(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP-Parametros</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            padding: 20px;
            background-color: #f4f4f4;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
        }
        input[type="text"], 
        input[type="submit"], 
        button {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
            margin-bottom: 10px;
        }
        button {
            background-color: #f44336;
            color: white;
            border: none;
        }
        input[type="submit"] {
            background-color: #4CAF50;
            color: white;
            border: none;
        }
        h2{text-align: center;
        }
    </style>
</head>
<body>

<h2>Configurações</h2>

<form action="/param" method="post">
    <div class="form-group">
        <label for='url'>Broker MQTT</label>
        <input type='text' id='url' name='url' placeholder='Broker' value='{url}'>
    </div>
    <div class="form-group">
        <label for='port'>Porta Broker</label>
        <input type='text' id='port' name='port' placeholder='Porta' value='{port}'>
    </div>
    <div class="form-group">
        <label for='user'>Usuário</label>
        <input type='text' id='user' name='user' placeholder='Usuário' value='{user}'>
    </div>
    <div class="form-group">
        <label for='pass'>Senha</label>
        <input type='text' id='pass' name='pass' placeholder='Senha' value='{pass}'>
    </div>
    <div class="form-group">
        <label for='pub'>Tópico Publish/Subscribe - Bomba</label>
        <input type='text' id='pub' name='pub' placeholder='Publish' value='{pub}'>
    </div>
    <div class="form-group">
        <label for='sub1'>Tópico Subscribe - Sensor0</label>
        <input type='text' id='sub1' name='sub1' placeholder='Subscribe Sensor0' value='{sub1}'>
    </div>
    <div class="form-group">
        <label for='sub2'>Tópico Subscribe - Sensor1</label>
        <input type='text' id='sub2' name='sub2' placeholder='Subscribe Sensor1' value='{sub2}'>
    </div>
    <div class="form-group">
        <label for='host'>Host mDNS</label>
        <input type='text' id='host' name='host' placeholder='Host' value='{host}'>
    </div>
    <div class="form-group">
        <label for='alexa01'>Dispositivo Alexa</label>
        <input type='text' id='alexa01' name='alexa01' placeholder='Alexa' value='{alexa01}'>
    </div>
    <div class="form-group">
        <label for='api'>Chave API - Trigger Alexa</label>
        <input type='text' id='api' name='api' placeholder='Chave' value='{api}'>
    </div>
    <div class="form-group">
        <label for='id1'>id1 Trigger - Nível Alto</label>
        <input type='text' id='id1' name='id1' placeholder='Trigger1' value='{id1}'>
    </div>

    <div class="form-group">
        <label for='id2'>id2 Trigger - Nível Baixo</label>
        <input type='text' id='id2' name='id2' placeholder='Trigger2' value='{id2}'>
    </div>
    <input type="submit" value="Salvar">
    <button type="button" onclick="location.href='/'">Cancelar</button>
</form>
</body>
</html>
)=====";

const char HTTP_INFO_esphead[]    PROGMEM = "<h3>ESP8266-{1}</h3><hr><table>";
const char HTTP_INFO_uptime[]     PROGMEM = "<tr><th>Tempo atividade</th><td>{1} Dias {2}</td></tr>";
const char HTTP_INFO_bootcount[]  PROGMEM = "<tr><th>Inicializações</th><td>{1} Boots</td></tr>";
const char HTTP_INFO_chipid[]     PROGMEM = "<tr><th>Chip ID</th><td>{1}</td></tr>";
const char HTTP_INFO_fchipid[]    PROGMEM = "<tr><th>Flash Chip ID</th><td>{1}</td></tr>";
const char HTTP_INFO_idesize[]    PROGMEM = "<tr><th>Flash Size</th><td>{1} bytes</td></tr>";
const char HTTP_INFO_flashsize[]  PROGMEM = "<tr><th>Real Flash Size</th><td>{1} bytes</td></tr>";
const char HTTP_INFO_corever[]    PROGMEM = "<tr><th>Core Version</th><td>{1}</td></tr>";
const char HTTP_INFO_bootver[]    PROGMEM = "<tr><th>Boot Version</th><td>{1}</td></tr>";
const char HTTP_INFO_cpufreq[]    PROGMEM = "<tr><th>CPU Frequency</th><td>{1}MHz</td></tr>";
const char HTTP_INFO_freeheap[]   PROGMEM = "<tr><th>Memory - Free Heap</th><td>{1} bytes available</td></tr>";
const char HTTP_INFO_memsketch[]  PROGMEM = "<tr><th>Memory - Sketch Size</th><td>Used / Total bytes<br/>{1} / {2}";
const char HTTP_INFO_memsmeter[]  PROGMEM = "<br/><progress value='{1}' max='{2}'></progress></td></tr>";
const char HTTP_INFO_lastreset[]  PROGMEM = "<tr><th>Motivo último reset</th><td>{1}</td></tr></table><hr>";
const char HTTP_INFO_wifihead[]   PROGMEM = "<h3>WiFi - Modo: {1}</h3><hr><table>";
const char HTTP_INFO_conx[]       PROGMEM = "<tr><th>Conectado?</th><td>{1}</td></tr>";
const char HTTP_INFO_stassid[]    PROGMEM = "<tr><th>Station SSID</th><td>{1}</td></tr>";
const char HTTP_INFO_staip[]      PROGMEM = "<tr><th>Station IP</th><td>{1}</td></tr>";
const char HTTP_INFO_stagw[]      PROGMEM = "<tr><th>Station Gateway</th><td>{1}</td></tr>";
const char HTTP_INFO_stasub[]     PROGMEM = "<tr><th>Station Subnet</th><td>{1}</td></tr>";
const char HTTP_INFO_dnss[]       PROGMEM = "<tr><th>DNS Server</th><td>{1}</td></tr>";
const char HTTP_INFO_host[]       PROGMEM = "<tr><th>Hostname</th><td>{1}</td></tr>";
const char HTTP_INFO_stamac[]     PROGMEM = "<tr><th>Station MAC</th><td>{1}</td></tr>";
const char HTTP_INFO_autoconx[]   PROGMEM = "<tr><th>Autoconnect</th><td>{1}</td></tr>";
const char HTTP_INFO_apssid[]     PROGMEM = "<tr><th>Access Point SSID</th><td>{1}</td></tr>";
const char HTTP_INFO_apip[]       PROGMEM = "<tr><th>Access Point IP</th><td>{1}</td></tr>";
const char HTTP_INFO_apbssid[]    PROGMEM = "<tr><th>BSSID</th><td>{1}</td></tr>";
const char HTTP_INFO_apmac[]      PROGMEM = "<tr><th>Access Point MAC</th><td>{1}</td></tr></table>";
const char HTTP_BACKBTN[]         PROGMEM = "<hr><div class='button-container'><form action='/' method='get'><button type='submit'>Voltar</button></form></div></main>";
const char HTTP_END[]             PROGMEM = "</div></body></html>";

const char HTTP_INFO_paramhead[]  PROGMEM = "<hr><h3>Parâmetros: {1}</h3><hr><table>";
const char HTTP_INFO_broker[]     PROGMEM = "<tr><th>Broker MQTT</th><td>{1}<br/>{2}</td></tr>";
const char HTTP_INFO_porta[]      PROGMEM = "<tr><th>Porta Broker</th><td>{1}</td></tr>";
const char HTTP_INFO_user[]       PROGMEM = "<tr><th>Usuário</th><td>{1}</td></tr>";
const char HTTP_INFO_pass[]       PROGMEM = "<tr><th>Senha</th><td>{1}</td></tr>";
const char HTTP_INFO_pub[]        PROGMEM = "<tr><th>Publish</th><td>{1}</td></tr>";
const char HTTP_INFO_sub1[]       PROGMEM = "<tr><th>Sensor0</th><td>{1}</td></tr>";
const char HTTP_INFO_sub2[]       PROGMEM = "<tr><th>Sensor2</th><td>{1}</td></tr>";
const char HTTP_INFO_hostmdns[]   PROGMEM = "<tr><th>Host mDNS</th><td>http://{1}.local</td></tr>";
const char HTTP_INFO_alexa01[]    PROGMEM = "<tr><th>Alexa</th><td>{1}</td></tr>";
const char HTTP_INFO_api[]        PROGMEM = "<tr><th>Chave API</th><td>{1}<br/>{2}</td></tr>";
const char HTTP_INFO_id1[]        PROGMEM = "<tr><th>Trigger 1</th><td>{1}</td></tr>";
const char HTTP_INFO_id2[]        PROGMEM = "<tr><th>Trigger 2</th><td>{1}</td></tr></table>";

const char HTTP_HEAD_START[]      PROGMEM =
R"=====(
<!DOCTYPE html>
<html lang='pt-BR'>
<head>
    <meta name='format-detection' content='telephone=no'>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>
    <title>ESP-Info</title>
    <style>body {margin: 0; padding: 0; font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans'; font-size: 16px; background-color: #f5f5f5; color: #212529;} .container {width: 100%; max-width: 360px; margin: auto; padding: 20px;} h1, h3 {color: blue; font-style: italic; text-align: center;} hr {margin-top: 10px; margin-bottom: 10px;} table {width: 100%; border-collapse: collapse;} th, td {text-align: left; padding: 8px;} tr:nth-child(odd) {background-color: #f5f5f5;} tr:nth-child(even) {background-color: #FFFFFF;} .button-container {text-align: center; margin-top: 20px;} button {cursor: pointer; border: 1px solid transparent; color: #fff; background-color: #007bff; border-color: #007bff; padding: 10px 20px; font-size: 18px; border-radius: 5px; width: auto;} @media (max-width: 480px) {body {font-size: 14px;} button {padding: 8px 15px; font-size: 16px;}}</style>
 </head>
<body>
    <div class='container'>
        <main class='form-signin'>
            <h1>Informações do Dispositivo</h1>
            <hr>
)=====";                
