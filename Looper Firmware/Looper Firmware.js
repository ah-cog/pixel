
console.log("Pixel Firmware 2014.03.26.20.08.39");

// Request data from a remote server (i.e., HTTP GET request)
/*
var wlan = require("CC3000").connect();
wlan.connect( "Hackerspace", "MakingIsFun!", function (s) { 
  console.log("connected?");
  if (s=="dhcp") {
      console.log("dhcp");
    require("http").get("http://www.pur3.co.uk/hello.txt", function(res) {
        console.log("get...");
      res.on('data', function(data) {
        console.log(">" + data);
      });
    });
  }
});
*/


/*
// http://www.espruino.com/Reference#l_fs_readFile
var fs = require('fs');
fs.appendFile("foo.txt", "boop\n");

var fileData = fs.readFile("foo.txt");
console.log(fileData);
*/

// TODO: Stack for Looper DSL interpretter

// Web server
var wlan = require("CC3000").connect();
wlan.connect( "Hackerspace", "MakingIsFun!", function (s) { 
  if (s == "dhcp") {
    
    console.log("My IP is " + wlan.getIP().ip);
    
    require("http").createServer(function (req, res) {
      if (req.method === "GET") {
        if (req.url === "/") {
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('Pixel Firmware');
          res.end();
        } else if (req.url === "/button/1/state") {
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('button is ' + BTN1.read());
          res.end();
        } else {
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('404');
          res.end();
        }
      } else if (req.method === "POST") {
        if (req.url === "/led/1/on") {
          digitalWrite(LED1, 1);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('on');
          res.end();
        } else if (req.url === "/led/2/on") {
          digitalWrite(LED2, 1);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('on');
          res.end();
        } else if (req.url === "/led/3/on") {
          digitalWrite(LED3, 1);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('on');
          res.end();
        } else if (req.url === "/led/1/off") {
          digitalWrite(LED1, 0);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('off');
          res.end();
        } else if (req.url === "/led/2/off") {
          digitalWrite(LED2, 0);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('off');
          res.end();
        } else if (req.url === "/led/3/off") {
          digitalWrite(LED3, 0);
          res.writeHead(200, {'Content-Type': 'text/plain'});
          res.write('off');
          res.end();
        }
      }
      
    }).listen(80);
  }
});

function onPageRequest(req, res) {
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.write('<html><body>');
  res.write('<p>Button is '+(BTN.read()?'on':'off')+'</p>');
  res.write('<p>Turn LED1 <a href="/on">on</a> or <a href="/off">off</a></p>');
  res.end('</body></html>');
  if (req.url=="/on") digitalWrite(LED1, 1);
  if (req.url=="/off") digitalWrite(LED1, 0);
}

// DSL for controlling the board
// These are called in response to HTTP requests

function turnOn(pin) {
  console.log("on " + pin);
  digitalWrite(LED1, 1);
}

function turnOff(pin) {
  console.log("off " + pin);
  digitalWrite(LED1, 0);
}

function turnOnPulse() {
  // Turn on pulse width modulation
}

/*
function turnOffPulse() {
  // TODO: Consider just having turnOff() and check whether it's analog or digital
}
*/

function getPin() {
  // Get state of pin (high or low)
}

// Interpretter functions:
// - "program": pass a complete program; set, get
// - "settings": set, get