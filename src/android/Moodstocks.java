package jp.wizcorp.moodstocks;

import java.util.*;
import android.util.Log;
import android.content.Context;
import android.content.Intent;

import org.apache.cordova.*;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;

import com.moodstocks.android.Scanner;
import com.moodstocks.android.MoodstocksError;

public class Moodstocks extends CordovaPlugin {
	public Scanner scanner;
	public SyncProgress syncProgress;

	protected Context context;
	protected CallbackContext scanCallbackContext = null;

	private int SCAN_CODE = 1143; // Could be anything from what I understand

	protected void pluginInitialize() {
		Log.d("moodstocks", "Creating Moodstocks");
		this.context = this.cordova.getActivity().getApplicationContext();
		this.syncProgress = new SyncProgress();
	}

	@Override
	public boolean execute(String action, JSONArray data, CallbackContext callbackContext) throws JSONException {
		// configure
		if (action.equals("configure")) {
			String path = Scanner.pathFromFilesDir(this.context, "scanner.db");
			JSONObject config = data.getJSONObject(0);

			try {
				Log.d("moodstocks", "Configuring");

				if (this.scanner == null) {
					this.scanner = Scanner.get();
					scanner.open(path, config.getString("key"), config.getString("secret"));
				}

				callbackContext.success();
			} catch (MoodstocksError e) {
				Log.e("moodstocks", "Moodstocks API configuration failed", e);
				callbackContext.error(e.getMessage());
				return false;
			}

			return true;
		}

		// on for sync events
		if (action.equals("on")) {
			this.syncProgress.on(data.getString(0), callbackContext);
			return true;
		}

		//
		// sync
		//
		if (action.equals("sync")) {
			Log.d("moodstocks", "Syncing");
			this.scanner.sync();
			this.scanner.setSyncListener(this.syncProgress);
			return true;
		}

		//
		// scan
		// TODO: support all scan modes - button push, QR codes, etc
		//
		if (action.equals("scan")) {
			Log.d("moodstocks", "Scanning");

			Intent intent = new Intent(this.context, ScanActivity.class);
			this.scanCallbackContext = callbackContext;
			this.cordova.startActivityForResult((CordovaPlugin) this, intent, SCAN_CODE);
			return true;
		}

		Log.e("moodstocks", "Received unknown call: " + action);
		return false;
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == SCAN_CODE) {
			if (this.scanCallbackContext != null) {
				JSONObject parameter = new JSONObject();

				try {
					if (data != null) {
						if (data.hasExtra("message")) {
							parameter.put("message", data.getStringExtra("message"));
						}

						if (data.hasExtra("data")) {
							parameter.put("data", data.getStringExtra("data"));
						}
					}
				} catch (JSONException e) {
					Log.e("moodstocks", "Could not serialize scanning result", e);
				}

				if(resultCode == this.cordova.getActivity().RESULT_OK){
					this.scanCallbackContext.success(parameter);
				} else if (resultCode == this.cordova.getActivity().RESULT_CANCELED) {
					this.scanCallbackContext.error(parameter);
				}

				return;
			}

			Log.w("moodstocks", "Received scan callback but no callback context present");
			return;
		}

		Log.e("moodstocks", "Received unknown activity result: " + Integer.toString(requestCode));
		return;
	}
}
