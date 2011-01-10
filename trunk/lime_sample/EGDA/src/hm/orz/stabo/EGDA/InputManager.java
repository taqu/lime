package hm.orz.stabo.EGDA;

import java.util.List;

import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;

public class InputManager
{
	public class Info
	{
		public Info()
		{
		}

		public float accelYow_;
		public float accelPitch_;
		public float accelRoll_;
		public float accelX_;
		public float accelY_;
		public float accelZ_;
	}
	
	public static InputManager getInstance()
	{
		return instance_;
	}
	
	public static void initialize(SensorManager sensorManager)
	{
		if(instance_==null){
			instance_ = new InputManager(sensorManager);
		}
	}

	public static void terminate()
	{
		if(instance_ != null){
			instance_.term();
			instance_ = null;
		}
	}
	
	public void onResume()
	{
		// 傾きセンサーの取得と登録
    	List< Sensor >sensors = sensorManager_.getSensorList( Sensor.TYPE_ORIENTATION );
    	
    	accelerometerListener_ = new AccelerometerListener(this);
    	
    	boolean registered = false;
    	
    	if(sensors.size() > 0){
    		registered |= sensorManager_.registerListener(
    				accelerometerListener_,
    				sensors.get(0),
    				SensorManager.SENSOR_DELAY_UI );//UI
			//SensorManager.SENSOR_DELAY_GAME );//rate suitable for games
			//SensorManager.SENSOR_DELAY_FASTEST );//最速
    	}
    	
    	// 加速度センサーの取得と登録
     	sensors = sensorManager_.getSensorList( Sensor.TYPE_ACCELEROMETER );
    	if( sensors.size() > 0 ){
    		registered |= sensorManager_.registerListener(
    				accelerometerListener_,
    				sensors.get(0),
    				SensorManager.SENSOR_DELAY_UI );//UI
			//SensorManager.SENSOR_DELAY_GAME );//rate suitable for games
			//SensorManager.SENSOR_DELAY_FASTEST );//最速
    	}
    	
    	if(false == registered){
    		accelerometerListener_ = null;
    	}
	}
	
	public void onPause()
	{
		if(accelerometerListener_ != null){
			sensorManager_.unregisterListener(accelerometerListener_);
			accelerometerListener_ = null;
		}
	}
	
	private InputManager(SensorManager sensorManager)
	{
		sensorManager_ = sensorManager;
		accelerometerListener_ = null;
		
		info_ = new Info();
	}

	private void term()
	{
		if(accelerometerListener_ != null){
			sensorManager_.unregisterListener(accelerometerListener_);
			accelerometerListener_ = null;
		}
		sensorManager_ = null;
	}
	
	public void onChanged()
	{
		poll();
	}
	
	public void poll()
	{
		// 本当はロックすべきなんだろうが、めんどい
		info_.accelYow_ = accelerometerListener_.getYow();
		info_.accelPitch_ = accelerometerListener_.getPitch();
		info_.accelRoll_ = accelerometerListener_.getRoll();
		
		info_.accelX_ = accelerometerListener_.getX();
		info_.accelY_ = accelerometerListener_.getY();
		info_.accelZ_ = accelerometerListener_.getZ();
		//Log.d("SENSOR", "angle=(" + info_.accelYow_ + ", " + info_.accelPitch_ + ", " + info_.accelRoll_ + ")");
		nativeInputChagned(info_);
		
		accelerometerListener_.clear();
	}
	
	private static InputManager instance_ = null; //インスタンス
	
	private Info info_;
	private SensorManager sensorManager_;
	private AccelerometerListener accelerometerListener_; //センサリスナ。
	
	private static native void nativeInputChagned(Info info);
}
