package hm.orz.stabo.EGDA.UI;

import hm.orz.stabo.EGDA.R;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

public class MenuBar implements View.OnClickListener
{
	private final int NumButtons = 1;
	
	public MenuBar(View view)
	{
		view_ = view;
		
		menu_ = (Button) view.findViewById(R.id.BtMenu);
		cameraMode_ = (Button) view.findViewById(R.id.BtCamera);
		menuBarGroup_ = (LinearLayout) view.findViewById(R.id.btmenu_group);

		menu_.setOnClickListener(this);
		
		visibleMenuBar_ = false;
		setVisibleMenuBar(false);
	}
	
	@Override
	public void onClick(View v)
	{
		switch(v.getId())
		{
		case R.id.BtMenu:
			visibleMenuBar_ = !visibleMenuBar_;
			setVisibleMenuBar(visibleMenuBar_);
			break;
			
		case R.id.BtCamera:
			break;
			
		default:
			break;
		}
	}
	
	private void setVisibleMenuBar(boolean visible)
	{
		if(visible){
			menuBarGroup_.setVisibility(View.VISIBLE);
			addOnClickListener();
		}else{
			menuBarGroup_.setVisibility(View.GONE);
			removeOnClickListener();
		}
	}
	
	private void addOnClickListener()
	{
		cameraMode_.setOnClickListener(this);
	}
	
	private void removeOnClickListener()
	{
		cameraMode_.setOnClickListener(null);
	}
	
	private boolean visibleMenuBar_;
	private View view_;
	private Button menu_;
	private LinearLayout menuBarGroup_;
	private Button cameraMode_;

	
}
