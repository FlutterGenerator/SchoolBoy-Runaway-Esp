package uk.lgl;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.widget.Toast;
import android.util.Base64;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;

import uk.lgl.modmenu.FloatingModMenuService;
import uk.lgl.modmenu.Sounds;

public class MainActivity extends Activity {

  private static final String TAG = "Mod Menu";
  public static String cacheDir;

  // Only if you have changed MainActivity to yours and you wanna call game's activity.
  public String GameActivity = "com.unity3d.player.UnityPlayerActivity";
  public boolean hasLaunched = false;

  // Load lib
  static {
    // When you change the lib name, change also on Android.mk file
    // Both must have same name
    System.loadLibrary("MyLibName");
  }

  // To call onCreate, please refer to README.md
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    Start(this);
    if (!hasLaunched) {
      try {
        // Start service
        MainActivity.this.startActivity(
            new Intent(MainActivity.this, Class.forName(MainActivity.this.GameActivity)));
        hasLaunched = true;
      } catch (ClassNotFoundException e) {
        e.printStackTrace();
        return;
      }
    }
  }

  public static void Start(final Context context) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !Settings.canDrawOverlays(context)) {
      context.startActivity(
          new Intent(
              "android.settings.action.MANAGE_OVERLAY_PERMISSION",
              Uri.parse("package:" + context.getPackageName())));
      final Handler handler = new Handler();
      handler.postDelayed(
          new Runnable() {
            @Override
            public void run() {
              System.exit(1);
            }
          },
          5000);
      return;
    } else {
      final Handler handler = new Handler();
      handler.postDelayed(
          new Runnable() {
            @Override
            public void run() {
              context.startService(new Intent(context, FloatingModMenuService.class));
            }
          },
          500);
    }
    cacheDir = context.getCacheDir().getPath() + "/";

    writeToFile("OpenMenu.ogg", Sounds.OpenMenu());
    writeToFile("Back.ogg", Sounds.Back());
    writeToFile("Select.ogg", Sounds.Select());
    writeToFile("SliderIncrease.ogg", Sounds.SliderIncrease());
    writeToFile("SliderDecrease.ogg", Sounds.SliderDecrease());
    writeToFile("On.ogg", Sounds.On());
    writeToFile("Off.ogg", Sounds.Off());
  }

  private static void writeToFile(String name, String base64) {
    File file = new File(cacheDir + name);
    try {
      if (!file.exists()) {
        file.createNewFile();
      }
      FileOutputStream fos = new FileOutputStream(file);
      byte[] decode = Base64.decode(base64, 0);
      fos.write(decode);
      fos.close();
    } catch (Exception e) {
      Log.e(TAG, e.getMessage());
    }
  }
}
