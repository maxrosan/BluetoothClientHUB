
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
	}
};

function onDeviceReadyBT() {
	BT.init();
}

function selectDevice() {
	alert("Device = " + document.getElementById("deviceId").value);
}

document.addEventListener("deviceready", onDeviceReadyBT, false);