package com.github.controller;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.LinkedList;
import java.util.UUID;
import java.util.HashMap;

import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.json.JSONArray;
import org.json.JSONException;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import com.github.controller.commands.*;

public class BluetoothClient extends CordovaPlugin {
	
	private static UUID BluetoothSerialUuid = UUID.fromString("b4688270-507d-42b7-8348-5e1e72c4b409");
	
	private BluetoothAdapter btAdapter = null;
	private BluetoothDevice btDevice = null;
	private BluetoothSocket btSocket = null;

	private HashMap<String, Command> commandsMap = new HashMap<String, Command>();
	
	public BluetoothClient() {
		
		btAdapter = BluetoothAdapter.getDefaultAdapter();

		addCommand(new InitCommand(this));
		addCommand(new ConnectCommand(this));
		addCommand(new SendMessageCommand(this));
	}

	private void addCommand(Command command) {
		commandsMap.put(command.getName(), command);
	}

	public UUID getUUID() {
		return BluetoothSerialUuid;
	}

	public BluetoothAdapter getBluetoothAdapter() {
		return btAdapter;
	}

	public BluetoothDevice getBluetoothDevice() {
		return btDevice;
	}

	public void setBluetoothDevice(BluetoothDevice bluetoothDevice) {
		btDevice = bluetoothDevice;
	}

	public BluetoothSocket getBluetoothSocket() {
		return btSocket;
	}

	public void setBluetoothSocket(BluetoothSocket socket) {
		btSocket = socket;
	}
	
	public boolean bluetoothConnect() {
		
		boolean result = true;
		
		BluetoothDevice device = getBluetoothDevice();
		
		if (device != null) {
			Method m;
			try {
				
				if (btSocket != null) {
					btSocket.close();
				}
				
				m = device.getClass().getMethod("createRfcommSocket", new Class[] {int.class});
				btSocket = (BluetoothSocket) m.invoke(device, 1);
				
				result = true;
				
			} catch (NoSuchMethodException e) {
				// TODO Bloco catch gerado automaticamente
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				// TODO Bloco catch gerado automaticamente
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				// TODO Bloco catch gerado automaticamente
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				// TODO Bloco catch gerado automaticamente
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		return result;
	}
	
	@Override
	public boolean execute(String action, JSONArray args, CallbackContext cc) throws JSONException {
		
		boolean result = true;
		
		Log.i("BluetoothClient", "Execute called");
		
		if (commandsMap.get(action) != null) {
			
			Command command = (Command) commandsMap.get(action);
			result = command.execute(args, cc);

		} else {
			cc.error("Action " + action + " not found");
			result = false;
		}
		
		return result;
		
	}
	
}
