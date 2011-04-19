package hm.orz.stabo.EGDA.io;

import java.util.List;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public class SensorSystem implements SensorEventListener
{
    static final float HALF_PI = (float)Math.PI * 0.5f;
    static final float FULL_PI = (float)Math.PI;
    static final float INV_HALF = (float)Math.PI/180.0f;
    static final float T = 400 * 1000;
    static final float INV_GRAD = (float)1.0f/SensorManager.GRAVITY_EARTH;
    static final float GRAD_TO_ROT = 100.0f * INV_GRAD * FULL_PI;
    static final int[] trans = new int[3];

    public SensorSystem(Context context)
    {
        manager_ = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
    }

    public void onResume()
    {
    	initialize();
    }

    public void onPause()
    {
    	terminate();
    }

    public void onAccuracyChanged(Sensor arg0, int arg1)
    {
    }

    public void onSensorChanged(SensorEvent event)
    {
    	if(event.accuracy == SensorManager.SENSOR_STATUS_UNRELIABLE){
    		//return;
    	}

    	boolean beUpdate = false;
        switch(event.sensor.getType())
        {
        case Sensor.TYPE_MAGNETIC_FIELD: //地磁気センサ
            {
            	beUpdate = true;
            	magnetics_[0] = event.values[0];
            	magnetics_[1] = event.values[1];
            	magnetics_[2] = event.values[2];
            }
            break;

        case Sensor.TYPE_ACCELEROMETER: //加速度センサ
            {
            	beUpdate = true;
        	    accels_[0] = event.values[0];
        	    accels_[1] = event.values[1];
        	    accels_[2] = event.values[2];
            }
            break;
        }

        if(beUpdate){
        	SensorManager.getRotationMatrix(tmpRotMat_, null, accels_, magnetics_);

        	SensorManager.remapCoordinateSystem(tmpRotMat_, SensorManager.AXIS_X, SensorManager.AXIS_Y, rotMat_);
        	SensorManager.getOrientation(rotMat_, rots_);

        	//zの絶対値がpi/2を超えていたら裏面が上と判断
        	if(Math.abs(rots_[2]) > HALF_PI){
        	    if(rots_[1]>0.0f){
        	        rots_[1] = FULL_PI - rots_[1];
        	    }else {
        	        rots_[1] = - FULL_PI - rots_[1];
        	    }
        	}

        	//Log.d("LIME", "rot: " + rots_[0] + ", " + rots_[1] + ", " + rots_[2]);
        }
    }

    public void reset()
    {
        for(int i=0; i<3; ++i){
            rots_[i] = 0.0f;

            magnetics_[i] = 0.0f;
            accels_[i] = 0.0f;
        }
    }

    public void initialize()
    {
        if(null == manager_){
            return;
        }
        reset();
        List<Sensor> magnetics = manager_.getSensorList(Sensor.TYPE_MAGNETIC_FIELD); //地磁気センサ

        if(magnetics.size() > 0){
            //manager_.registerListener(this, magnetics.get(0), SensorManager.SENSOR_DELAY_GAME);
            manager_.registerListener(this, magnetics.get(0), SensorManager.SENSOR_DELAY_UI);
        }

        List<Sensor> accels = manager_.getSensorList(Sensor.TYPE_ACCELEROMETER); //加速度センサ

        if(accels.size() > 0){
            //manager_.registerListener(this, accels.get(0), SensorManager.SENSOR_DELAY_GAME);
            manager_.registerListener(this, accels.get(0), SensorManager.SENSOR_DELAY_UI);
        }
    }

    public void terminate()
    {
        if(manager_ != null){
            manager_.unregisterListener(this);
        }
    }

    public float clamp(float v, float min, float max)
    {
        if(v<min){
            return min;
        }
        if(v>max){
            return max;
        }
        return v;
    }

    public float[] rots_ = new float[3];

    private float[] magnetics_ = new float[3];
    private float[] accels_ = new float[3];
    private float[] tmpRotMat_ = new float[3*3];
    private float[] rotMat_ = new float[3*3];
    SensorManager manager_ = null;
}
