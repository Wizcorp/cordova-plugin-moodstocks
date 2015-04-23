package jp.wizcorp.moodstocks;

import java.util.*;
import android.util.*;

import org.apache.cordova.*;

import com.moodstocks.android.Scanner;
import com.moodstocks.android.MoodstocksError;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;

public class SyncProgress implements Scanner.SyncListener {
	public HashMap<String, CallbackContext> events = new HashMap<String, CallbackContext>();

	public void on(String action, CallbackContext callbackContext) {
		Log.d("moodstocks", "Setting sync callback for " + action);
		Log.d("moodstocks", "sync::" + action + " - " + callbackContext.toString());
		this.events.put(action, callbackContext);
	}

	public void onSyncStart() {
		if (this.events.containsKey("start")) {
			Log.d("moodstocks", "Sync started");
			this.events.get("start").success();
		}
	}

	public void onSyncComplete() {
		if (this.events.containsKey("completed")) {
			Log.d("moodstocks", "Sync completed");
			this.events.get("completed").success();
		}
	}

	public void onSyncFailed(MoodstocksError e) {
		if (this.events.containsKey("error")) {
			JSONObject parameter = new JSONObject();
			Log.d("moodstocks", "Sync failed");

			try {
				parameter.put("code", e.getErrorCode());
				parameter.put("message", e.getMessage());
				this.events.get("error").success(parameter);
			} catch (JSONException err) {
				Log.e("moodstocks", "Serializing return data for sync error failed", err);
			}
		}
	}

	public void onSyncProgress(int total, int current) {
		if (this.events.containsKey("progress")) {
			JSONObject parameter = new JSONObject();
			Log.d("moodstocks", "Sync progress");

			try {
				parameter.put("percent", (int)(current / total));
				this.events.get("progress").success(parameter);
			} catch (JSONException err) {
				Log.e("moodstocks", "Serializing return data for sync progress failed", err);
			}
		}
	}
}
