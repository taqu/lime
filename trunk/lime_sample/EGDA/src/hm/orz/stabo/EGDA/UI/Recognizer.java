package hm.orz.stabo.EGDA.UI;

import android.view.MotionEvent;

public class Recognizer
{
    public static final int MoveNone = 0;
    public static final int MoveLeft = 1;
    public static final int MoveRight = 2;
    public static final int MoveUp = 3;
    public static final int MoveDown = 4;
    
    public static final long InputSplitMax = 1000;
    public static final float InputLengthMin = 15.0f;
    
    public Recognizer()
    {
        reset();
    }
    public int getMoveType()
    {
        return moveType_;
    }
    
    public void reset()
    {
        moveType_ = MoveNone;

        sx_ = sy_ = 0;
    }
    
    public void update(MotionEvent event)
    {
        switch(event.getAction())
        {
        case MotionEvent.ACTION_UP:
        {
            long duration = event.getEventTime() - event.getDownTime();
            if(duration<InputSplitMax){
                float dx = event.getX() - sx_;
                float dy = event.getY() - sy_;
                calc(dx, dy);
            }
        }
            break;
            
        case MotionEvent.ACTION_DOWN:
            sx_ = event.getX();
            sy_ = event.getY();
            moveType_ = MoveNone;
            break;
        }
    }
    
    private void calc(float dx, float dy)
    {
        float ax = Math.abs(dx);
        float ay = Math.abs(dy);
        
        //‚Æ‚è‚ ‚¦‚¸AˆÚ“®—Ê‚Ìâ‘Î’l‚Ì‘å‚«‚³‚¾‚¯‚Åc‰¡Œˆ’è
        if(ax >= ay){
            if(InputLengthMin > ax){
                return;
            }
            moveType_ = (dx>=0.0f)? MoveRight : MoveLeft;
        }else{
            if(InputLengthMin > ay){
                return;
            }
            
            moveType_ = (dy>=0.0f)? MoveDown : MoveUp;
        }
    }
    
    private int moveType_;
    
    private float sx_, sy_; //‰Ÿ‰ºˆÊ’u
}
