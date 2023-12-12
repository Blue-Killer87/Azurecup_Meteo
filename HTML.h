const char indexHTML[]
PROGMEM = R"rawliteral(
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body{
            font-family: "Helvetica";

        }
        input[type="submit"] {
            background-color: #4CAF50; /* Green */
            color: white;
            width: 200px;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            display: block;

            margin: 0 auto;
        }

        button {
            background-color: #4CAF50; /* Green */
            color: white;
            width: 150px;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            border-radius: 4px;
            font-size: 16px;
            display: block;
            margin: 0 auto;
            transition: 0.3s;
        }
        button:hover {
            background-color: #3d8b40;
        }

        .rssi {
            color: green;
            text-align: center;
        }

        .encrypt1 {
            color: green;
            text-align: center;
        }
    </style>
</head>
<body>
<center>
    <h1>ESP config</h1>
    <button onclick="window.location.href=window.location.href">Scan</button>
    <button onclick="location.href = '/nodes';">Node IP</button>
    <button onclick="location.href = '/jsonConfigure';">Configure</button>
    <button onclick="location.href = '/settings';">Settings</button>
    <button onclick="location.href = '/exit';">Exit</button>
    <p>Skanování může chvíli trvat</p>
    <h1>WIFI sítě</h1>

    <script>
        var getJSON = function (url, callback) {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', url, true);  // Otevre http request
            xhr.responseType = 'json';
            xhr.onload = function () {
                var status = xhr.status; // zkontroluje se status http
                if (status === 200) {
                    callback(null, xhr.response);
                } else {
                    callback(status, xhr.response);
                }
            };
            xhr.send();
        };
        getJSON('scannedWiFi.json', // pracuje s ipAdresa/scannedWiFi.json
            function (err, data) {
                if (err !== null) { // kontrola zda se stal error
                    alert('Something went wrong: ' + err);
                } else { // vezme data z jsonu a ulozi se do pole
                    for (i = 0; i < data.numOfNetworks; i++) {
                        buttons(i, data.networks, data.strengh, data.protection);
                    }
                }
            });

        function buttons(i, networks, rssi, protection) {
            // vytvori tlacitka na zaklade json zpravy
            var button = document.createElement('input');
            button.class = 'button';
            button.type = 'submit';
            button.value = networks[i]; // hodnota se nastavi na SSID site

            var form = document.createElement('form'); // vytvori se formular do ktereho se tlacitko vlozi
            // pokud sit je zabezpecena, odkaze na overeni s heslem
            if (protection[i] == "None") {
                form.method = 'POST';
                form.action = '/wifi/redirect'
            } else {
                form.method = 'GET'
                form.action = '/wifi';
            }


            var inputs = document.createElement('input');
            inputs.type = 'hidden';
            inputs.name = 'ssid';
            inputs.value = networks[i]; // skryty input s ssid
            var rssi1 = document.createElement("p");
            rssi1.innerHTML = rssi[i]; // text s rrsi konkretni site
            rssi1.className = 'rssi';
            var encrypt = document.createElement("p");
            encrypt.innerHTML = protection[i];
            encrypt.className = 'encrypt1';
            // sestaveni html stromu
            document.body.appendChild(form);
            form.appendChild(inputs);
            form.appendChild(button);
            form.appendChild(rssi1);
            form.appendChild(encrypt);

        }
    </script>
</center>
</body>
</html>
)rawliteral";

const char passwordHTML[]
PROGMEM = R"rawliteral(
<html>
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: "Helvetica";

        }

        input[type=password], select {
            width: 50%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }

        button {
            background-color: #0080FF; /* azurová */
            color: white;
            width: 150px;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            border-radius: 4px;
            font-size: 16px;
            display: block;
            margin: 0 auto;
            transition: 0.3s;
        }

        button:hover {
            background-color: #5fc2ed;
        }

    </style>
</head>
<center>
    <h1>Security</h1>
    <form method="POST" action="/wifi/redirect">
        <label for="password">WiFi password</label>
        <br>
        <input type="password" name="password" id="password">
        <button type="submit">submit</button>
        <p>WiFi name: </p>
    </form>
    <body>
    <script>
        const queryString = window.location.search; // slouzi k ziskani argumentu z url
        const urlParams = new URLSearchParams(queryString);

        var form = document.getElementsByTagName('form')[0]; // najdu formular na strance a ulozim do promenne
        // vytvorim skryty vstup a ulozim do nej ssid argument z url
        var wifiname = document.createElement('input');
        wifiname.type = "hidden";
        wifiname.name = "ssid";
        wifiname.value = urlParams.get('ssid');
        form.appendChild(wifiname);
        form.appendChild(document.createTextNode(urlParams.get('ssid')));


    </script>
    <button onClick="location.href = '/';">Return</button>
    </body>
</center>
</html>
)rawliteral";

const char DeviceHTML[]
PROGMEM = R"rawliteral(
<html lang="cs">
<head>
    <style>
        body {
            font-family: "Helvetica";

        }

        input[type=input] {
            width: 50%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }

        button {
            background-color: #0080FF; 
            color: white;
            width: 150px;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            border-radius: 4px;
            font-size: 16px;
            display: block;
            margin: 0 auto;
            transition: 0.3s;
        }

        button:hover {
            background-color: #5fc2ed;
        }

    </style>
    </head>
<body>
<center>
    <meta charset="UTF-8">
    <h1>IP adresses of Nodes</h1>
    <form method="POST" action="/nodes/check">
        <label for="ip1">Node #1</label><br>
        <input type="input" name="ip1" id="ip1">
        <button type="submit">submit</button>
    </form>
    <form method="POST" action="/nodes/check">
        <label for="ip2">Node #2</label><br>
        <input type="input" name="ip2" id="ip2">
        <button type="submit">submit</button>
    </form>
    <button onClick="location.href = '/';">Return</button>
    <h6>If you want to use one socket with multiple outputs, put same IP Address to both fields.</h6>
</center>
<script>
    const queryString = window.location.search;
    const urlParams = new URLSearchParams(queryString);

    var input1 = document.getElementsByTagName('input')[0];
    wifiname.value = urlParams.get('ssid');
    form.appendChild(wifiname);
    form.appendChild(document.createTextNode(urlParams.get('ssid')));
</script>
</body>
</html>
)rawliteral";


const char configHTML[]
PROGMEM = R"rawliteral(
<html>
<head>
  <meta charset="UTF-8">
  <style>
    body {
      font-family: "Helvetica";

    }

    input[type=input] {
      width: 50%;
      padding: 12px 20px;
      margin: 8px 0;
      display: inline-block;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
    }

    button {
      background-color: #0080FF; 
      color: white;
      width: 150px;
      padding: 15px 32px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      border-radius: 4px;
      font-size: 16px;
      display: block;
      margin: 0 auto;
      transition: 0.3s;
    }

    button:hover {
      background-color: #5fc2ed;
    }
  </style>
</head>
<body>
<center>
  <h1>Configuration for JSON API</h1>
  <form method ="POST" action ="/jsonConfigure/check">
    <label for = "button1">JSON node#1</label><br>
    <input type="input" name="button1" id="button1">
    <button type="submit">Submit</button>
  </form>
  <form method ="POST" action ="/jsonConfigure/check">
    <label for = "button2">JSON node#2</label><br>
    <input type="input" name="button2" id="button2">
    <button type="submit">Submit</button>
  </form>
  <button onClick="location.href = '/';">Return</button>
</center>
</body>
</html>
)rawliteral";
