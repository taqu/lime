package hm.orz.stabo.EGDA.io;

import java.util.List;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class SensorSystem implements SensorEventListener
{
    static final float HALF_PI = (float)Math.PI * 0.5f;
    static final float FULL_PI = (float)Math.PI;
    static final float INV_HALF = (float)Math.PI/180.0f;
    static final float T = 400 * 1000;
    static final float INV_GRAD = (float)1.0f/SensorManager.GRAVITY_EARTH;
    static final float GRAD_TO_ROT = 100.0f * INV_GRAD * FULL_PI;
    
    public SensorSystem(Context context)
    {
        manager_ = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);
    }

    public void onAccuracyChanged(Sensor arg0, int arg1)
    {
    }

    public void onSensorChanged(SensorEvent event)
    {
        switch(event.sensor.getType())
        {
        case Sensor.TYPE_ORIENTATION: //傾きセンサ
            {
                rots_[0] = INV_HALF*(event.values[0] - 180.0f); //(0 - 360 deg) to (-pi - pi)
                rots_[1] = INV_HALF*event.values[1]; //(-180 - 180 deg) to (-pi - pi)
                rots_[2] = INV_HALF*event.values[2]; //(-90 - 90 deg) to (-pi/2 - pi/2)
                
                //Log.d("LIME", "angle=(" + rots_[0] + ", " + rots_[1] + ", " + rots_[2] + ")");
            }
            break;
            
        case Sensor.TYPE_ACCELEROMETER: //加速度センサ
        {
            long duration = event.timestamp - time_;
            time_ = event.timestamp;
            
            float ratio = T/(T + duration);
            float invRatio = 1.0f - ratio;
            for(int i=0; i<3; ++i){
                gravities_[i] = ratio * gravities_[i] + invRatio * event.values[i];
                rots_[i] -= (event.values[i] - gravities_[i]) * GRAD_TO_ROT;
            }
            rots_[0] = clamp(rots_[0], -HALF_PI, HALF_PI);
            rots_[1] = clamp(rots_[1], -HALF_PI, HALF_PI);
            rots_[2] = clamp(rots_[2], -FULL_PI, FULL_PI);
            
            //Log.d("LIME", "accel=(" + rots_[0] + ", " + rots_[1] + ", " + rots_[2] + ")");
        }
        break;
        }
    }

    public void reset()
    {
        for(int i=0; i<3; ++i){
            rots_[i] = 0.0f;
            gravities_[i] = 0.0f;
        }
    }
    
    public void initialize()
    {
        if(null == manager_){
            return;
        }
        reset();
        List<Sensor> orientations = manager_.getSensorList(Sensor.TYPE_ORIENTATION); //傾きセンサ

        if(orientations.size() > 0){
            manager_.registerListener(this, orientations.get(0), SensorManager.SENSOR_DELAY_NORMAL);
            return;
        }
        
        List<Sensor> accels = manager_.getSensorList(Sensor.TYPE_ACCELEROMETER); //加速度センサ

        if(accels.size() > 0){
            manager_.registerListener(this, accels.get(0), SensorManager.SENSOR_DELAY_NORMAL);
            return;
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

    long time_ = 0;
    public float[] rots_ = new float[3];
    float[] gravities_ = new float[3];
    SensorManager manager_ = null;
}
