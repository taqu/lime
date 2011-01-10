package hm.orz.stabo.EGDA;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ListView;

public class OpenFileDialog implements View.OnClickListener, DialogInterface.OnClickListener
{
    public static final int Request_OpenFile = 0;
    public static final int Result_Success = 0;
    public static final int Reuslt_Fail = 1;
    
    public void show(Context context, File root)
    {
        context_ = context;
        setRoot(root);
    }

    public void finish()
    {
        if(dialog_ != null){
            dialog_.dismiss();
            dialog_= null;
        }
        
        if(listener_ != null){
            listener_.onClick(selected_);
        }
    }
    
    public void setRoot(File root)
    {
        if(root == null){
            finish();
            return;
        }
        
        try{
            current_ = root;
            files_ = root.listFiles();
            if(files_ == null){
                finish();
                return;
            }
            
            String[] lists = new String[files_.length + 1];
            
            lists[0] = ".."; //１番目は必ず特殊にする
            String name = "";
            for(int i=1; i<=files_.length; ++i){
                File file = files_[i-1];
                if(file.isDirectory()){
                    name = file.getName() + "/";
                }else{
                    name = file.getName();
                }
                lists[i] = name;
            }
            if(dialog_ != null){
                dialog_.dismiss();
            }
            AlertDialog.Builder builder = new AlertDialog.Builder(context_);
            builder.setTitle(root.getName());
            builder.setItems(lists, this);
            dialog_ = builder.show();
            
        }catch(SecurityException e){
            finish();
            return;
        }catch(Exception e){
            finish();
            return;
        }
    }

    @Override
    public void onClick(View v)
    {
        
    }

    @Override
    public void onClick(DialogInterface dialog, int which)
    {
        selected_ = null;

        //範囲外なら選択解除
        if(files_ == null){
            return;
        }
        
        //0番目は親ディレクトリのためのダミー
        if(which<0 || files_.length<which){
            return;
        }
        
        if(which == 0){
            // 親ディレクトリに戻る。はじめのディレクトリより上にはいかない
            if(level_ > 0){
                --level_;
                setRoot(current_.getParentFile());
            }
            return;
        }
        
        --which;
        
        //ディレクトリなら潜る
        if(files_[which].isDirectory()){
            ++level_;
            setRoot(files_[which]);
            return;
        }

        selected_ = files_[which];

        finish();
    }
 
    public void setListener(OpenFileDialogListener listener)
    {
        listener_ = listener;
    }
    public interface OpenFileDialogListener
    {
        public void onClick(File selected);
    }
    
 
    private Context context_ = null;
    private File selected_ = null;
    private int level_ = 0;
    private File current_ = null;
    private File[] files_ = null;

    private AlertDialog dialog_ = null;
    private OpenFileDialogListener listener_ = null;
}
