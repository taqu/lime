package hm.orz.stabo.EGDA.UI;

import hm.orz.stabo.EGDA.CommandManager;
import hm.orz.stabo.EGDA.Config;
import hm.orz.stabo.EGDA.R;

import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.RadioGroup;
import android.widget.Spinner;

public class MenuDialog extends Dialog
implements RadioGroup.OnCheckedChangeListener, AdapterView.OnItemSelectedListener, OnClickListener
{

    public MenuDialog(Context context, CommandManager commandManager)
    {
        super(context);
        app_name_ = context.getString(R.string.app_name);
        
        View menuView = (View)this.getLayoutInflater().inflate(R.layout.menu_view, null);
        setContentView(menuView);
        
        setTitle(R.string.menu_dialog_title);
        
        findViewById(R.id.menu_button_ok).setOnClickListener(this);
        findViewById(R.id.menu_button_cancel).setOnClickListener(this);
        
        commandManager_ = commandManager;
        initialize();
    }

    private void initialize()
    {
        screenMode_ = Config.getInstance().getScreenMode();
        cameraMode_ = Config.getInstance().getCameraMode();

        // スクリーン回転
        Spinner spinner = (Spinner)findViewById(R.id.screen_spinner);
        spinner.setSelection( screenMode_ );

        spinner.setOnItemSelectedListener(this);
        
        // カメラモード
        RadioGroup radioGroup = (RadioGroup)findViewById(R.id.camera_radioGroup);
        switch(cameraMode_)
        {
        case Config.Camera_Manual:
            radioGroup.check(R.id.camera_radioM);
            break;
            
        case Config.Camera_Animation:
            radioGroup.check(R.id.camera_radioA);
            break;
            
        default:
            radioGroup.check(R.id.camera_radioM);
            break;
        }
        radioGroup.setOnCheckedChangeListener( this );
    }
    
    public void onCheckedChanged(RadioGroup group, int checkedId)
    {        
        switch(checkedId)
        {               
        case R.id.camera_radioM:
            cameraMode_ = Config.Camera_Manual;
            break;
            
        case R.id.camera_radioA:
            cameraMode_ = Config.Camera_Animation;
            break;
            
        default:
            return;
        }
//        if(changed){
//            commandManager_.push( CommandManager.createChangeMode(commandManager_) );
//        }
    }

    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id)
    {
        if(Config.Screen_Rot0<=pos && pos<=Config.Screen_Rot270){
            screenMode_ = pos;
        }
    }

    public void onNothingSelected(AdapterView<?> parent)
    {
    }
    
    public void onClick(View v)
    {
        switch(v.getId())
        {
        case R.id.menu_button_ok:
            {
                Config config = Config.getInstance();
                boolean changed = (screenMode_ != config.getScreenMode()) || (cameraMode_ != config.getCameraMode());
                config.setCameraMode(cameraMode_);
                config.setScreenMode(screenMode_);
                if(changed){
                    config.save(app_name_);
                }
            }
            commandManager_.push(CommandManager.createChangeMode(
                    commandManager_, screenMode_, cameraMode_));
            this.dismiss();
            break;
            
        case R.id.menu_button_cancel:
            this.dismiss();
            break;
        }
    }
    String app_name_ = null;
    int cameraMode_ = 0;
    int screenMode_ = 0;
    private CommandManager commandManager_ = null;
}
