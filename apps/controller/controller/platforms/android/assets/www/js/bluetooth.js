
var connected = false;

var BT = {

 init : function() {
			cordova.exec(
				function (msg) {
					var i;
					var sel = document.getElementById("deviceId");

					for (i = 0; i < msg.length; i++) {
						var opt = document.createElement("option");
						opt.value = msg[i];
						opt.innerHTML = msg[i];
						sel.appendChild(opt);
					}
				}, 
				function(msg) {
					alert("Error: " + msg);
				},
				"BluetoothClient",
				"init",
			[]);
	},

connect: function(deviceName) {

			cordova.exec(
				function (msg) {
					if (msg == "true") {
						alert("Conectado!");

						connected = true;
					} else {
						alert("Failed to connect");
					}
				}, 
				function(msg) {
					alert("Error: " + msg);
				},
				"BluetoothClient",
				"connect",
			[deviceName]);

	},


sendMsg: function(x, y, z) {

			cordova.exec(
				function (msg) {
					
				}, 
				function(msg) {
					alert("Error: " + msg);
				},
				"BluetoothClient",
				"sendmsg",
			[x, y, z]);

	}

};

var firstAccelerometerValue = true;
var x, y, z;
var noiseValueSummed = 0.1;
var noiseValue = 0.3;

function onAccSuccess(event) {

	if (firstAccelerometerValue) {

		firstAccelerometerValue = false;

	} else {

		var dX, dY, dZ;
		
		dX = event.x - x;
		dY = event.y - y;
		dZ = event.z - z;

		if (Math.abs(dX) < noiseValue) {
			dX = 0;
		}

		if (Math.abs(dZ) < noiseValue) {
			dZ = 0;
		}

		if (Math.abs(dY) < noiseValue) {
			dY = 0;
		}				

		var absValue = Math.abs(dX) + Math.abs(dY) + Math.abs(dZ);

		if (connected && absValue > noiseValueSummed) {
			BT.sendMsg(dX, dY, dZ);
		}

	}

	x = event.x;
	y = event.y;
	z = event.z;
}

function onAccError(msg) {
	alert("Error: " + msg);
}

function onDeviceReadyBT() {
	BT.init();

	var options = { frequency: 100 };
    var watchID = navigator.accelerometer.watchAcceleration(onAccSuccess, onAccError, options);
}

function selectDevice() {
	var deviceName = document.getElementById("deviceId").value;
	if (!connected) {
		BT.connect(deviceName);
	}
}

document.addEventListener("deviceready", onDeviceReadyBT, false);