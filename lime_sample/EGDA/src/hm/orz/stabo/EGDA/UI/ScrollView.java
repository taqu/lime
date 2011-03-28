package hm.orz.stabo.EGDA.UI;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.LinearLayout;
import android.widget.Scroller;

public class ScrollView extends LinearLayout
{    
    private final int SCROLLING_MSEC = 200;
    
    public ScrollView(Context context)
    {
        super(context);
        recognizer_ = new Recognizer();
        scroller_ = new Scroller(context);
        setVisibility(INVISIBLE);
    }

    public ScrollView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        recognizer_ = new Recognizer();
        scroller_ = new Scroller(context);
        //setVisibility(INVISIBLE);
    }

    public void startScrollIn(boolean left)
    {
        scroller_.abortAnimation();
        int w = getWidth();
        int sx = (left)? -w : w;
        int dx = (left)? w : -w;
        
        scroller_.startScroll(sx, 0, dx, 0, SCROLLING_MSEC);
        setVisibility(VISIBLE);
        visibleAfterScrolling_ = true;
        invalidate();
    }

    public void startScrollOut(boolean left)
    {
        scroller_.abortAnimation();
        
        int w = getWidth();
        int dx = (left)? w : -w;
        
        scroller_.startScroll(0, 0, dx, 0, SCROLLING_MSEC);
        visibleAfterScrolling_ = false;
        invalidate();
    }

    public void computeScroll()
    {
        if(scroller_.computeScrollOffset()){
            scrollTo(scroller_.getCurrX(), scroller_.getCurrY());
            postInvalidate();
        }else{
            if(!visibleAfterScrolling_){
                setVisibility(INVISIBLE);
            }
        }
    }
    
    public boolean onTouchEvent(MotionEvent event)
    {
        recognizer_.update(event);
        
        switch(recognizer_.getMoveType())
        {            
        case Recognizer.MoveLeft:
            startScrollOut(true);
            break;
            
        case Recognizer.MoveRight:
            startScrollOut(false);
            break;
        }
        return true;
    }
    
    private boolean visibleAfterScrolling_ = false;
    private Scroller scroller_ = null;
    private Recognizer recognizer_ = null;
}
