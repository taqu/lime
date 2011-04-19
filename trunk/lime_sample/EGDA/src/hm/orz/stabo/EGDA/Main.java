package hm.orz.stabo.EGDA;


import hm.orz.stabo.EGDA.UI.OpenFileDialog.OpenFileDialogListener;

import java.io.File;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.ImageButton;

public class Main extends Activity
    implements OpenFileDialogListener, OnClickListener
{
    static final int State_Stop = 0;
    static final int State_Play = 1;
    
    /**
     * Called when the activity is first created.
     */
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE); //タイトルバー非表示

      //設定初期化
        Config.initialize();
        Config.getInstance().load( getResources().getString(R.string.app_name) ); //設定ファイルロード

        //GL用ビュー
        renderer_ = new MainRenderer();
        mainView_ = new GLSurfaceView(this, renderer_);
        mainView_.setLayoutParams( new FrameLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

        //メインHUD
        View hud = this.getLayoutInflater().inflate(R.layout.hud, null);

        playButton_ = (ImageButton)hud.findViewById(R.id.play_button);
        playButton_.setOnClickListener(this);


        //GL->HUDの順に重ねる
        frameLayout_ = new FrameLayout(this);
        frameLayout_.setLayoutParams( new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));

        frameLayout_.addView(mainView_);
        frameLayout_.addView(hud);

        setContentView(frameLayout_);

        state_ = State_Stop;
    }

	protected void onPause()
	{
		super.onPause();

		mainView_.onPause();
		LogOut.d("onPause");
	}

	protected void onResume()
	{
		super.onResume();

		mainView_.onResume();
		LogOut.d("onResume");
	}

	protected void onStop()
    {
	    super.onStop();
	    LogOut.d("onStop");
        finish();
    }

	protected void onDestroy()
	{
		super.onDestroy();

		LogOut.d("onDestroy");
		frameLayout_ = null;

		Config.getInstance().save(getResources().getString(R.string.app_name));
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

        case R.id.menu_preferences:
            {
                MenuDialog dialog = new MenuDialog(this, renderer_.getCommandManager());
                dialog.setOwnerActivity(this);
                dialog.show();
            }
            return true;
        case R.id.menu_about:
            {
                View aboutView = this.getLayoutInflater().inflate(R.layout.about, null);

                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setView(aboutView)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int which)
                    {
                        dialog.dismiss();
                    }
                });
                
                AlertDialog alert = builder.create();
                alert.show();
            }
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
        changeState(State_Stop); //ロード後は停止状態
    }

    public void onClick(File selected)
    {
        loadFile(selected);
    }

    public void onClick(View v)
    {
        switch(v.getId())
        {
        case R.id.play_button:
            {
            	if(state_ == State_Play){
            		changeState(State_Stop);
            	}else{
            		changeState(State_Play);
            	}
            }
            break;
        }
    }

    public void changeState(int state)
    {
    	CommandManager manager = renderer_.getCommandManager();
        Command command = null;

        manager.push(command);

        switch(state)
        {
        case State_Stop:
        	playButton_.setImageResource(android.R.drawable.ic_media_pause);

        	command = CommandManager.createChangeState(manager, State_Stop);
        	manager.push(command);
        	break;
        case State_Play:
        	playButton_.setImageResource(android.R.drawable.ic_media_play);

        	command = CommandManager.createChangeState(manager, State_Play);
        	manager.push(command);
        	break;
        default:
        	return;
        }
        state_ = state;
    }

    private int state_ = State_Stop;
    private FrameLayout frameLayout_ = null;
    private MainRenderer renderer_ = null;
	private GLSurfaceView mainView_ = null;
	private ImageButton playButton_ = null;
}