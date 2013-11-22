
window.echo = function(str) {
	cordova.exec(
		function (msg) {
			alert(msg);
		}, 
		function(msg) {
			alert("Error: " + msg);
		},
		"BluetoothClient",
		"echo",
		[str]);
};