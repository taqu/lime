package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.CommandManager.Command;
import hm.orz.stabo.EGDA.CommandManager.CommandFileLoad;

import java.io.File;

import hm.orz.stabo.EGDA.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

public class Main extends Activity implements OpenFileDialog.OpenFileDialogListener
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        mainView_ = new MainView(getApplication(),
        		false,
        		5,
        		6,
        		5,
        		0,
        		24,
        		8);

        frameLayout_ = new FrameLayout(this);

        frameLayout_.setLayoutParams( new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
        frameLayout_.addView(mainView_);

        setContentView(frameLayout_);
        //setContentView(mainView_);
    }

	@Override
	protected void onPause()
	{
		super.onPause();
		mainView_.onPause();
	}

	@Override
	protected void onResume()
	{
		super.onResume();
		mainView_.onResume();
	}
	
	protected void onStop()
	{
		super.onStop();
		mainView_ = null;
		frameLayout_ = null;
		menu_ = null;
		finish();
	}
	
	protected void onDestroy()
    {
        super.onDestroy();
    }
	
	
	public boolean onCreateOptionsMenu(Menu menu)
    {
        menu_ = menu;
        
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch(item.getItemId())
        {
        case R.id.exit:
            finish();
            return true;

        case R.id.open:
            openFile();
            return true;
        }
        return false;
    }
    
    private void openFile()
    {
        if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            InfoToast.info(this, "Any storages are not mounted.");
            return;
        }
        
        File root = Environment.getExternalStorageDirectory();
        OpenFileDialog dialog = new OpenFileDialog();
        dialog.setListener(this);
        dialog.show(this, root);
        
        //Intent intent = new Intent(this, OpenFileDialog.class);
        //intent.putExtra("root", root);
        //startActivityForResult(intent, OpenFileDialog.Request_OpenFile);
        
        
    }
    
    private void loadFile(File file)
    {
        if(file == null){
            return;
        }
        InfoToast.info(this, file.getName());
        int resourceType = Resource.getTypeFromExtension(file.getName());
        if(resourceType == Resource.Type_None){
            InfoToast.info(this, "Cannot open file type");
            return;
        }
        CommandManager manager = mainView_.getCommandManager();
        Command command = CommandManager.createFileLoad(manager, file.getPath(), resourceType);
        
        manager.push(command);
    }
    
    @Override
    public void onClick(File selected)
    {
        loadFile(selected);
    }
    
    private Menu menu_;
    private FrameLayout frameLayout_;
	private MainView mainView_;
}