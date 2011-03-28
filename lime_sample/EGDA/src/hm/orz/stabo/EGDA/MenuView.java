package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.UI.ScrollView;
import android.view.View;

public class MenuView
{
    public MenuView(ScrollView view)
    {
        impl_ = view;
        if(impl_ != null){
            initialize();
        }
    }
    
    private void initialize()
    {
        
    }
    
    public void startScrollIn(boolean left)
    {
        impl_.startScrollIn(left);
    }
    
    private ScrollView impl_ = null;
}
