package com.github.controller;

import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.json.JSONArray;
import org.json.JSONException;

public class BluetoothClient extends CordovaPlugin {

	@Override
	public boolean execute(String action, JSONArray args, CallbackContext cc) throws JSONException {
		
		cc.success("Hello world!");
		
		return true;
		
	}
	
}
