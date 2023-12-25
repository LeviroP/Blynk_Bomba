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
   <p>Clique aqui para <a href='/info'>Informações do Dispositivo</a>.</p>     
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
// Webpage Parametros: Portal Configuração dos Parâmetros
//=====================================================
const char webpageParam[] PROGMEM =
R"=====(
<!doctype html>
<html lang='pt-BR'>
    <head>
        <meta charset='utf-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>ESP Parâmetros</title>
        <style>
            *,
            ::after,
            ::before {box-sizing:border-box;}
            body {margin:0; font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans'; font-size:1rem; font-weight:400; line-height:1.5; color:#212529; background-color:#f5f5f5;}
            .form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;color: blueviolet;}
            button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}
            .form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}
            h1{text-align: center;}
        </style>
        <script>
            function c(l) {
                document.getElementById('1').value='';
                document.getElementById('2').value='';
                document.getElementById('3').value='';
                document.getElementById('4').value='';
                document.getElementById('5').value='';
                document.getElementById('6').value='';
                document.getElementById('7').value='';
                document.getElementById('8').value='';
            }
        </script>
    </head>
    <body>
        <main class='form-signin'>
            <h1>Configurar Parâmetros</h1>
            <form action='/param' method='post'>
                <div class='form-floating'>
                        <label>Broker MQTT</label>
                        <input type='text' class='form-control' id='1' name='url' value='{url}'>
                        <label>Porta Broker</label>
                        <input type='text' class='form-control' id='2' name='port' value='{port}'>
                        <label>Usuário</label>
                        <input type='text' class='form-control' id='3' name='user' value='{user}'>
                        <label>Senha</label>
                        <input type='text' class='form-control' id='4' name='user' value='{user}'>
                        <label>Tópico Publish</label>
                        <input type='text' class='form-control' id='5' name='pub' value='{pub}'>
                        <label>Tópico Subscribe</label>
                        <input type='text' class='form-control' id='6' name='sub' value='{sub}'>
                        <label>Host mDNS</label>
                        <input type='text' class='form-control' id='7' name='host' value='{host}'>
                        <label>Dispositivo Alexa</label>
                        <input type='text' class='form-control' id='8' name='alexa01' value='{alexa01}'>
                </div>
                <br/>
                <button type='submit'>Salvar</button><p></p>
                <button type='reset'>Restaurar</button><p></p>
                <button type='button' onclick='c(this)'>Limpar</button><p></p>
            </form>
            <form action='/' method='get'>
                <button type='botton'>Voltar</button><p></p>
            </form><br/>
        </main>
    </body>
</html>
)=====";

const char HTTP_INFO_esphead[]    PROGMEM = "<h3>ESP8266-{1}</h3><hr><dl>";
const char HTTP_INFO_uptime[]     PROGMEM = "<dt>Tempo atividade</dt><dd>{1} Dias {2}</dd>";
const char HTTP_INFO_bootcount[]  PROGMEM = "<dt>Inicializações</dt><dd>{1} Boots</dd>";
const char HTTP_INFO_chipid[]     PROGMEM = "<dt>Chip ID</dt><dd>{1}</dd>";
const char HTTP_INFO_fchipid[]    PROGMEM = "<dt>Flash Chip ID</dt><dd>{1}</dd>";
const char HTTP_INFO_idesize[]    PROGMEM = "<dt>Flash Size</dt><dd>{1} bytes</dd>";
const char HTTP_INFO_flashsize[]  PROGMEM = "<dt>Real Flash Size</dt><dd>{1} bytes</dd>";
const char HTTP_INFO_corever[]    PROGMEM = "<dt>Core Version</dt><dd>{1}</dd>";
const char HTTP_INFO_bootver[]    PROGMEM = "<dt>Boot Version</dt><dd>{1}</dd>";
const char HTTP_INFO_cpufreq[]    PROGMEM = "<dt>CPU Frequency</dt><dd>{1}MHz</dd>";
const char HTTP_INFO_freeheap[]   PROGMEM = "<dt>Memory - Free Heap</dt><dd>{1} bytes available</dd>";
const char HTTP_INFO_memsketch[]  PROGMEM = "<dt>Memory - Sketch Size</dt><dd>Used / Total bytes<br/>{1} / {2}";
const char HTTP_INFO_memsmeter[]  PROGMEM = "<br/><progress value='{1}' max='{2}'></progress></dd>";
const char HTTP_INFO_lastreset[]  PROGMEM = "<dt>Motivo último reset</dt><dd>{1}</dd></dl><hr>";
const char HTTP_INFO_wifihead[]   PROGMEM = "<h3>WiFi - Modo: {1}</h3><hr><dl>";
const char HTTP_INFO_conx[]       PROGMEM = "<dt>Conectado?</dt><dd>{1}</dd>";
const char HTTP_INFO_stassid[]    PROGMEM = "<dt>Station SSID</dt><dd>{1}</dd>";
const char HTTP_INFO_staip[]      PROGMEM = "<dt>Station IP</dt><dd>{1}</dd>";
const char HTTP_INFO_stagw[]      PROGMEM = "<dt>Station Gateway</dt><dd>{1}</dd>";
const char HTTP_INFO_stasub[]     PROGMEM = "<dt>Station Subnet</dt><dd>{1}</dd>";
const char HTTP_INFO_dnss[]       PROGMEM = "<dt>DNS Server</dt><dd>{1}</dd>";
const char HTTP_INFO_host[]       PROGMEM = "<dt>Hostname</dt><dd>{1}</dd>";
const char HTTP_INFO_stamac[]     PROGMEM = "<dt>Station MAC</dt><dd>{1}</dd>";
const char HTTP_INFO_autoconx[]   PROGMEM = "<dt>Autoconnect</dt><dd>{1}</dd>";
const char HTTP_INFO_apssid[]     PROGMEM = "<dt>Access Point SSID</dt><dd>{1}</dd>";
const char HTTP_INFO_apip[]       PROGMEM = "<dt>Access Point IP</dt><dd>{1}</dd>";
const char HTTP_INFO_apbssid[]    PROGMEM = "<dt>BSSID</dt><dd>{1}</dd>";
const char HTTP_INFO_apmac[]      PROGMEM = "<dt>Access Point MAC</dt><dd>{1}</dd>";
const char HTTP_BACKBTN[]         PROGMEM = "<br><br/><form action='/' method='get'><div id='botao'><button>Voltar</button></div></form><br><br></main>";
const char HTTP_END[]             PROGMEM = "</body></html>";

const char HTTP_INFO_paramhead[]  PROGMEM = "<hr><h3>Parâmetros: {1}</h3><hr><dl>";
const char HTTP_INFO_broker[]     PROGMEM = "<dt>Broker MQTT</dt><dd>{1}</dd>";
const char HTTP_INFO_porta[]      PROGMEM = "<dt>Porta Broker</dt><dd>{1}</dd>";
const char HTTP_INFO_user[]       PROGMEM = "<dt>Usuário</dt><dd>{1}</dd>";
const char HTTP_INFO_pass[]       PROGMEM = "<dt>Senha</dt><dd>{1}</dd>";
const char HTTP_INFO_pub[]        PROGMEM = "<dt>Tópico Publish</dt><dd>{1}</dd>";
const char HTTP_INFO_sub[]        PROGMEM = "<dt>Tópico Subscribe</dt><dd>{1}</dd>";
const char HTTP_INFO_hostmdns[]   PROGMEM = "<dt>Host mDNS</dt><dd>http://{1}.local</dd>";
const char HTTP_INFO_alexa01[]    PROGMEM = "<dt>Dispositivo Alexa</dt><dd>{1}</dd>";

const char HTTP_HEAD_START[]      PROGMEM =
R"=====(
<!DOCTYPE html>
<html lang='pt-BR'>
    <head>
        <meta name='format-detection' content='telephone=no'>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'/>
        <title>ESP-Info</title>
    </head>
        
    <style>
        #botao{ text-align: center }
        body {margin:0; font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans'; font-size:1rem; font-weight:400; line-height:1.5; color:#212529; background-color:#f5f5f5}
        button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:30%}
        .form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}
        h1{text-align: center;color:blue;font-style: italic;}
        h3{margin-left: 20px;color:black;font-style: italic;}
        dl{
            display: block;
            margin-top: 1em;
            margin-bottom: 1em;
            margin-left: 20px;
            margin-right: 0px;
            color: black;
            }
    </style>
        
        <body>
            <main class='form-signin'>
                <h1>Informações Dispositivo</h1><hr>
)=====";                

const char HTTP_HELP[]            PROGMEM =
 "<br/><h3>Available Pages</h3><hr>"
 "<table class='table'>"
 "<thead><tr><th>Page</th><th>Function</th></tr></thead><tbody>"
 "<tr><td><a href='/'>/</a></td>"
 "<td>Menu page.</td></tr>"
 "<tr><td><a href='/wifi'>/wifi</a></td>"
 "<td>Show WiFi scan results and enter WiFi configuration.(/0wifi noscan)</td></tr>"
 "<tr><td><a href='/wifisave'>/wifisave</a></td>"
 "<td>Save WiFi configuration information and configure device. Needs variables supplied.</td></tr>"
 "<tr><td><a href='/param'>/param</a></td>"
 "<td>Parameter page</td></tr>"
 "<tr><td><a href='/info'>/info</a></td>"
 "<td>Information page</td></tr>"
 "<tr><td><a href='/u'>/u</a></td>"
 "<td>OTA Update</td></tr>"
 "<tr><td><a href='/close'>/close</a></td>"
 "<td>Close the captiveportal popup,configportal will remain active</td></tr>"
 "<tr><td>/exit</td>"
 "<td>Exit Config Portal, configportal will close</td></tr>"
 "<tr><td>/restart</td>"
 "<td>Reboot the device</td></tr>"
 "<tr><td>/erase</td>"
 "<td>Erase WiFi configuration and reboot Device. Device will not reconnect to a network until new WiFi configuration data is entered.</td></tr>"
 "</table>"
 "<p/>Github <a href='https://github.com/tzapu/WiFiManager'>https://github.com/tzapu/WiFiManager</a>.";
