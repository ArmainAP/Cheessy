//Portul utilizat
var PORT = 22222;

// Adresa IP privata
var HOST = 'ip-172-31-17-170.us-west-2.compute.internal';

//Modul Datagram
var dgram = require('dgram');

//Creeaza un socket prin Datagram
var server = dgram.createSocket('udp4');

// Aceasa este functia de listen a server-ului
server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

//Aceasta functie este apelata dupa un HTTP Request sau dupa functia FUDPPing::UDPEcho din Unreal Engine. Dupa ce primeste un mesaj va porni un server dedicat unei sesiuni de joc.
server.on('message', function (message, remote) {
    console.log('Message received from ' + remote.address + ':' + remote.port +' - ' + message.toString());
    server.send(message, 0, message.length, remote.port, remote.address, function(err, bytes) {
	  if (err) throw err;
	  console.log('UDP message sent to ' + remote.address +':'+ remote.port + '\n');
    var exec = require('child_process').execFile;
	exec('cmd.exe', ['/c', 'StartCheessyServer.bat']);
	console.log('CheessyServer.exe started');
	});
});

//In cazul unei erori, afiseaza eroarea si inchide server-ul
server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});

//Porneste server-ul utilizand port-ul si ip-ul dat mai sus
server.bind(PORT, HOST);