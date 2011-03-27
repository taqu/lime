package hm.orz.stabo.EGDA;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

public class AccelerometerListener implements SensorEventListener
{
	public AccelerometerListener(InputManager manager)
	{
		manager_ = manager;
		
		clear();
		//orientationSensorAccuracy_ = 1;
		//accelerometerAccuracy_ = 1;
	}
	
	/**
	 * センサーの精度が変更されるときに呼ばれる
	 */
	public void onAccuracyChanged(Sensor sensor, int param)
	{
		/*
		switch(sensor.getType())
		{
		case Sensor.TYPE_ORIENTATION:
			orientationSensorAccuracy_ = param;
			break;
			
		case Sensor.TYPE_ACCELEROMETER:
			accelerometerAccuracy_ = param;
			break;
		}
		*/
	}

	/**
	 * センサーイベントコールバック
	 */
	public void onSensorChanged(SensorEvent event)
	{
		final float INV_HALF = (float)Math.PI/180.0f;

		switch(event.sensor.getType())
		{
		case Sensor.TYPE_ORIENTATION: //傾きセンサ
			{
				//Log.d("SENSOR", "angle=(" + event.values[0] + ", " + event.values[1] + ", " + event.values[2] + ")");
				//Log.d("PROP", "range=" + event.sensor.getMaximumRange() + ", pow=" + event.sensor.getPower() + ", reso=" + event.sensor.getResolution() + ", " + event.sensor.getVendor());
				
				boolean notDevPhone = true;
				if(notDevPhone){ //
					yow_ = INV_HALF*(event.values[0] - 180.0f); //(0 - 360 deg) to (-pi - pi)
					pitch_ = INV_HALF*event.values[1]; //(-180 - 180 deg) to (-pi - pi)
					roll_ = INV_HALF*event.values[2]; //(-90 - 90 deg) to (-pi/2 - pi/2)
					//Log.d("LIME", "angle=(" + yow_ + ", " + pitch_ + ", " + roll_ + ")");
					
				}else{
					// 開発機は、ピッチだけ角度センサで、残りは加速度センサらしい
					pitch_ = INV_HALF*event.values[1]; //(-180 - 180 deg) to (-1 - 1)
					
					float v = INV_HALF*(event.values[0] - 180);
					yow_ += v;
					if(yow_<-1.0f){
						yow_ += 2.0f;
					}else if(yow_>1.0f){
						yow_ -= 2.0f;
					}
					
					v = INV_HALF*event.values[2];
					roll_ += v;
					if(roll_<-1.0f){
						roll_ += 2.0f;
					}else if(roll_>1.0f){
						roll_ -= 2.0f;
					}
				}
			}
			break;
			
		case Sensor.TYPE_ACCELEROMETER: //加速度センサ
			{
				//Log.d("SENSOR", "accel=(" + event.values[0] + ", " + event.values[1] + ", " + event.values[2] + ")");
				//Log.d("SENSOR", "range=" + event.sensor.getMaximumRange() + ", reso=" + event.sensor.getResolution());
				float invMaxRange = 1.0f/event.sensor.getMaximumRange();
				x_ = invMaxRange * event.values[0];
				y_ = invMaxRange * event.values[1];
				z_ = invMaxRange * event.values[2];
			}
			break;
		}
		//manager_.onChanged();
	}
	
	public float getYow(){ return yow_;}
	public float getPitch(){ return pitch_;}
	public float getRoll(){ return roll_;}
	
	public float getX(){ return x_;}
	public float getY(){ return y_;}
	public float getZ(){ return z_;}
	
	public void clear()
	{
		//yow_ = pitch_ = roll_ = 0.0f;
		x_ = y_ = z_ = 0.0f;
	}
	
	private float yow_;
	private float pitch_;
	private float roll_;
	
	
	private float x_;
	private float y_;
	private float z_;
	
	private InputManager manager_;
	//private int orientationSensorAccuracy_; //傾きセンサーの精度
	//private int accelerometerAccuracy_; //加速度センサーの精度
}
