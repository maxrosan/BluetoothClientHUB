package com.github.controller;

import java.util.LinkedList;
import java.util.UUID;

import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.json.JSONArray;
import org.json.JSONException;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.util.Log;

public class BluetoothClient extends CordovaPlugin {

	static final UUID BluetoothSerialUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
	
	private BluetoothAdapter btAdapter = null;
	
	public BluetoothClient() {
		
		
	}
	
	@Override
	public boolean execute(String action, JSONArray args, CallbackContext cc) throws JSONException {
		
		boolean result = true;
		
		Log.i("BluetoothClient", "Execute called");
		
		if (action.equals("init")) {
			
			if (btAdapter == null) {	
				btAdapter = BluetoothAdapter.getDefaultAdapter();
			}
			
			if (btAdapter == null) {
				cc.error("Failed to get default adapter");
				result = false;
			} else {
				if (!btAdapter.isEnabled()) {
					cc.error("Adapter disabled");
					result = false;
				} else {
					
					LinkedList<String> res = new LinkedList<String>();
					
					for (BluetoothDevice bd : btAdapter.getBondedDevices()) {
						res.add(bd.getName());
						
						Log.i("BluetoothClient", bd.getName());
					}
					
					cc.success(new JSONArray(res));
				}
			}
		} else {
			cc.error("Action " + action + " not found");
			result = false;
		}
		
		return result;
		
	}
	
}
