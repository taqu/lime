package hm.orz.stabo.EGDA.UI;

import java.io.File;
import java.util.Arrays;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.View;

public class OpenFileDialog implements View.OnClickListener, DialogInterface.OnClickListener
{
    public static final int Request_OpenFile = 0;
    public static final int Result_Success = 0;
    public static final int Reuslt_Fail = 1;
    
    public class FileConparator implements java.util.Comparator<File>
    {

		@Override
		public int compare(File object1, File object2)
		{
			if(object1.isDirectory()){
				if(object2.isFile()){
					return -1;
				}
			}else if(object2.isDirectory()){
				return 1;
			}
			
			String name1 = object1.getName().toUpperCase();
			String name2 = object2.getName().toUpperCase();
			return name1.compareTo(name2);
		}
    }
    public OpenFileDialog()
    {
    	comparator_ = new FileConparator();
    }
    
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
            int size = 1;
            if(files_ != null){
                Arrays.sort(files_, comparator_);
                size = files_.length + 1;
            }
            
            String[] lists = new String[size];
            
            
            lists[0] = ".."; //１番目は必ず特殊にする
            String name = "";
            for(int i=1; i<size; ++i){
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
            // 親ディレクトリに戻る
            File parent = current_.getParentFile();
            if(parent != null){
            	if(current_.isFile()){ //ファイルならもうひとつ上に上がる
            		parent = parent.getParentFile();
            		if(parent == null){
            			return;
            		}
            	}
                setRoot(parent);
                parent = null;
            }
            return;
        }
        
        --which;
        
        //ディレクトリなら潜る
        if(files_[which].isDirectory()){
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
    private File current_ = null;
    private File[] files_ = null;

    FileConparator comparator_ = null;
    private AlertDialog dialog_ = null;
    private OpenFileDialogListener listener_ = null;
}
