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
        Config config = Config.getInstance();
        screenMode_ = config.getScreenMode();
        cameraMode_ = config.getCameraMode();
        isAlphaTest_ = config.isAlhpaTest();
        isMipmap_ = config.isMipmap();
        isPhysics_ = config.isPhysics();
        isTextureCompress_ = config.isTextureCompress();

        // スクリーン回転
        Spinner spinner = (Spinner)findViewById(R.id.screen_spinner);
        spinner.setSelection( screenMode_ );

        spinner.setOnItemSelectedListener(this);
        
        // カメラモード
        RadioGroup cameraRadioGroup = (RadioGroup)findViewById(R.id.camera_radioGroup);
        switch(cameraMode_)
        {
        case Config.Camera_Manual:
            cameraRadioGroup.check(R.id.camera_radioM);
            break;
            
        case Config.Camera_Animation:
            cameraRadioGroup.check(R.id.camera_radioA);
            break;
            
        default:
            cameraRadioGroup.check(R.id.camera_radioM);
            break;
        }
        cameraRadioGroup.setOnCheckedChangeListener( this );
        
        // ロード設定
        //----------------------------------------
        // アルファテスト
        RadioGroup group = (RadioGroup)findViewById(R.id.alpha_test_radioGroup);
        int id = (isAlphaTest_)? R.id.alpha_test_on : R.id.alpha_test_off;
        group.check(id);
        group.setOnCheckedChangeListener( this );

        // ミップマップ
        group = (RadioGroup)findViewById(R.id.mipmap_radioGroup);
        id = (isMipmap_)? R.id.mipmap_on : R.id.mipmap_off;
        group.check(id);
        group.setOnCheckedChangeListener( this );
        
        // 物理演算
        group = (RadioGroup)findViewById(R.id.physics_radioGroup);
        id = (isPhysics_)? R.id.physics_on : R.id.physics_off;
        group.check(id);
        group.setOnCheckedChangeListener( this );
        
        // テクスチャ圧縮
        group = (RadioGroup)findViewById(R.id.texcomp_radioGroup);
        id = (isTextureCompress_)? R.id.texcomp_on : R.id.texcomp_off;
        group.check(id);
        group.setOnCheckedChangeListener( this );
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
            
        case R.id.alpha_test_on:
            isAlphaTest_ = true;
            break;
            
        case R.id.alpha_test_off:
            isAlphaTest_ = false;
            break;
            
        case R.id.mipmap_on:
        	isMipmap_ = true;
            break;
            
        case R.id.mipmap_off:
        	isMipmap_ = false;
            break;
            
        case R.id.physics_on:
        	isPhysics_ = true;
            break;
            
        case R.id.physics_off:
        	isPhysics_ = false;
            break;
            
        case R.id.texcomp_on:
            isTextureCompress_ = true;
            break;
            
        case R.id.texcomp_off:
            isTextureCompress_ = false;
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
                boolean changed =
                   (screenMode_ != config.getScreenMode())
                || (cameraMode_ != config.getCameraMode())
                || (isAlphaTest_ != config.isAlhpaTest())
                || (isMipmap_ != config.isMipmap())
                || (isPhysics_ != config.isPhysics())
                || (isTextureCompress_ != config.isTextureCompress());

                config.setCameraMode(cameraMode_);
                config.setScreenMode(screenMode_);
                config.setAlphaTest(isAlphaTest_);
                config.setMipmap(isMipmap_);
                config.setPhysics(isPhysics_);
                config.setTextureCompress(isTextureCompress_);

                if(changed){
                    config.save(app_name_);
                }
            }
            commandManager_.push(CommandManager.createChangeMode(
                    commandManager_, screenMode_, cameraMode_, isAlphaTest_, isMipmap_, isPhysics_, isTextureCompress_));
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
    boolean isAlphaTest_ = false;
    boolean isMipmap_ = false;
    boolean isPhysics_ = false;
    boolean isTextureCompress_ = false;
    private CommandManager commandManager_ = null;
}
