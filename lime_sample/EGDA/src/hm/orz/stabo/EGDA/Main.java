package hm.orz.stabo.EGDA;

import hm.orz.stabo.EGDA.CommandManager.Command;
import hm.orz.stabo.EGDA.UI.OpenFileDialog;
import hm.orz.stabo.EGDA.UI.Recognizer;
import hm.orz.stabo.EGDA.UI.ScrollView;
import hm.orz.stabo.EGDA.UI.OpenFileDialog.OpenFileDialogListener;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;

public class Main extends Activity implements OpenFileDialogListener
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        renderer_ = new MainRenderer();
        mainView_ = new GLSurfaceView(this);
        mainView_.setRenderer(renderer_);

        ScrollView menuView = (ScrollView)this.getLayoutInflater().inflate(R.layout.menu_view, null);
        menuView_ = new MenuView(menuView);

        frameLayout_ = new FrameLayout(this);

        frameLayout_.setLayoutParams( new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
        frameLayout_.addView(mainView_);
        frameLayout_.addView(menuView);

        setContentView(frameLayout_);
        //setContentView(mainView_);
        
        
        //設定初期化
        Config.initialize();
        Config.getInstance().load( getResources().getString(R.string.app_name) ); //設定ファイルロード
        
        recognizer_ = new Recognizer();
    }

	@Override
	protected void onPause()
	{
	    //mainView_.onPause();
		super.onPause();
	}

	@Override
	protected void onResume()
	{
	    //mainView_.onResume();
		super.onResume();
	}
	
	protected void onStop()
	{
		super.onStop();
		mainView_ = null;
		frameLayout_ = null;
		menuView_ = null;
		recognizer_ = null;
		
		Config.getInstance().save(getResources().getString(R.string.app_name));
		finish();
	}
	
	protected void onDestroy()
    {
        super.onDestroy();
    }
	
	
	public boolean onCreateOptionsMenu(Menu menu)
    {
        MenuInflater inflater = this.getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

    public boolean onOptionsItemSelected(MenuItem item)
    {        
        switch(item.getItemId())
        {
        case R.id.menu_exit:
            finish();
            return true;

        case R.id.menu_open:
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
        
        Config config = Config.getInstance();
        File root = new File(config.getLastDirectory());
        if(false == root.exists()){
        	root = Environment.getExternalStorageDirectory();
        }
        
        OpenFileDialog dialog = new OpenFileDialog();
        dialog.setListener(this);
        dialog.show(this, root);
    }
    
    private void loadFile(File file)
    {
        if(file == null){
            return;
        }
        {
        	String parent = file.getParent();
        	if(parent != null){
        		Config.getInstance().setLastDirectory(parent);
        		parent = null;
        	}
        }
        
        InfoToast.info(this, file.getName());
        int resourceType = ResourceManager.getTypeFromExtension(file.getName());
        if(resourceType == ResourceManager.Type_None){
            InfoToast.info(this, "Cannot open file");
            return;
        }
        CommandManager manager = renderer_.getCommandManager();
        Command command = CommandManager.createFileLoad(manager, file, resourceType);
        
        manager.push(command);
    }
    
    @Override
    public void onClick(File selected)
    {
        loadFile(selected);
    }
    
    public boolean onTouchEvent(MotionEvent event)
    {
        recognizer_.update(event);
        switch(recognizer_.getMoveType())
        {            
        case Recognizer.MoveLeft:
            menuView_.startScrollIn(true);
            break;
            
        case Recognizer.MoveRight:
            menuView_.startScrollIn(false);
            break;
        }
        return true;
    }
    
    private FrameLayout frameLayout_ = null;
    private MainRenderer renderer_ = null;
	private GLSurfaceView mainView_ = null;
	private MenuView menuView_ = null;
	private Recognizer recognizer_ = null;
}