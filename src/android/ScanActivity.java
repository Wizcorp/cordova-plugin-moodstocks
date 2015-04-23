package jp.wizcorp.moodstocks;

import android.util.*;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.SurfaceView;

import org.apache.cordova.*;

import com.moodstocks.android.AutoScannerSession;
import com.moodstocks.android.Scanner;
import com.moodstocks.android.MoodstocksError;
import com.moodstocks.android.Result;

public class ScanActivity extends Activity implements AutoScannerSession.Listener {

	private AutoScannerSession session = null;
	private static final int TYPES = Result.Type.IMAGE;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(this.getResources().getIdentifier("scan","layout", this.getPackageName()));

		SurfaceView preview = (SurfaceView)findViewById(this.getResources().getIdentifier("preview","id", this.getPackageName()));

		try {
			Log.d("moodstocks", "Starting Scanner");
			session = new AutoScannerSession(this, Scanner.get(), this, preview);
			session.setResultTypes(TYPES);
		} catch (MoodstocksError e) {
			Log.e("moodstocks", "Starting Scanner Failed", e);
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		session.start();
	}

	@Override
	protected void onPause() {
		super.onPause();
		session.stop();
	}

	@Override
	public void onCameraOpenFailed(Exception e) {
		Log.e("moodstocks", "Could not open camera", e);
		Intent returnIntent = new Intent();
		returnIntent.putExtra("message", "Could not open camera");
		setResult(RESULT_CANCELED, returnIntent);
		finish();
	}

	@Override
	public void onWarning(String debugMessage) {
		Log.w("moodstocks", "Scan warning: " + debugMessage);
	}

	@Override
	public void onResult(Result result) {
		Log.d("moodstocks", "Found something! " + result.getValue());

		Intent returnIntent = new Intent();
		returnIntent.putExtra("data", result.getValue());
		setResult(RESULT_OK, returnIntent);
		finish();
	}
}
