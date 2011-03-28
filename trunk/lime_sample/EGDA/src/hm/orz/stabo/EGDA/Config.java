package hm.orz.stabo.EGDA;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

import android.os.Environment;
import android.util.Xml;

/**
 * 設定データクラス。シングルトン 
 * @author taqu
 *
 */
public class Config
{
	private final String FileName = "config.xml";
	private final String XmlEncoding = "UTF-8";
	private final String Empty = "";
	
	private final String TagConfig = "config";
	private final String TagLastDirectory = "last_directory";
	private final String TagCameraMode = "camera_mode";
	private final String TagScreenMode = "screen_mode";

	public final int Camera_Animation = 0;
	public final int Camera_Manual = 1;
	
	public final int Screen_Vertical = 0;
	public final int Screen_Horizontal = 1;
	
	private int clamp(int v, int min, int max)
	{
	    if(v<min){
	        return min;
	    }else if(v>max){
	        return max;
	    }
	    return v;
	}
	
	/**
	 * @return 実体
	 */
	static public Config getInstance()
	{
		return instance_;
	}

	/**
	 * 初期化
	 */
	static public void initialize()
	{
		if(instance_ == null){
			instance_ = new Config();
		}
	}
	
	/**
	 * 終了
	 */
	static public void terminate()
	{
		instance_ = null;
	}
	
	public String getLastDirectory()
	{
		if(lastDirectory_.length() == 0){
			if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
				lastDirectory_ = Environment.getExternalStorageDirectory().getAbsolutePath();
			}
        }
		
		return lastDirectory_;
	}
	public void setLastDirectory(String directory)
	{
	    changed_ = true;
		lastDirectory_ = directory;
	}

	public int getCameraMode()
	{
	    return cameraMode_;
	}
	
	public void setCameraMode(int mode)
	{
	    changed_ = true;
	    cameraMode_ = mode;
	}
	
	public int getScreenMode()
	{
	    return screenMode_;
	}
	
	public void setScreenMode(int mode)
	{
	    changed_ = true;
	    screenMode_ = mode;
	}
	
	private String createPath(String appName)
	{
		File root = Environment.getExternalStorageDirectory();
        return root.getAbsolutePath() + "/" + appName + "/" + FileName;
	}
	
	/**
	 * 設定ファイルから値ロード。ファイルがない、又は外部ストレージがない場合は、デフォルト値を設定。
	 * @param appName ... アプリケーション名
	 */
	public void load(String appName)
	{
	    setDefault();
        changed_ = true;
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            return;
        }
 
        File config = new File(createPath(appName));
        if(config.exists()){
        	try{
        		FileInputStream is = new FileInputStream(config);
        		XmlPullParser xmlPullParser = Xml.newPullParser();
        		xmlPullParser.setInput(is, XmlEncoding);
        		
        		for(int event = XmlPullParser.START_DOCUMENT;
        			event != XmlPullParser.END_DOCUMENT;
        			event = xmlPullParser.next())
        		{
        			switch(event)
        			{
        			case XmlPullParser.START_TAG:
        				if(xmlPullParser.getName().equals(TagLastDirectory)){
        					lastDirectory_ = xmlPullParser.nextText();

        				}else if(xmlPullParser.getName().equals(TagCameraMode)){
        				    cameraMode_ = Integer.parseInt( xmlPullParser.nextText() );
        				    cameraMode_ = clamp(cameraMode_, 0, 1);

                        }else if(xmlPullParser.getName().equals(TagScreenMode)){
                            screenMode_ = Integer.parseInt( xmlPullParser.nextText() );
                            screenMode_ = clamp(cameraMode_, 0, 1);
                        }
        				break;
        			}
        		}
        		changed_ = false;
        	}catch(FileNotFoundException e)
        	{
        	} catch (XmlPullParserException e) {
			} catch (IOException e) {
				config.delete(); //失敗したので削除する
			}
        }
	}
	
	/**
	 * 設定ファイルをセーブ。
	 * @param appName ... アプリケーション名
	 */
	public void save(String appName)
	{
	    if(false == changed_){
	        return;
	    }
	    
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            return;
        }
		
        try{
        	String path = createPath(appName);
        	FileOutputStream os = new FileOutputStream(path, false);
        	
        	XmlSerializer xmlSerializer = Xml.newSerializer();
        	xmlSerializer.setOutput(os, XmlEncoding);
        	
        	xmlSerializer.startDocument(XmlEncoding, true);
        	
        	xmlSerializer.startTag(Empty, TagConfig);
        	
        	xmlSerializer.startTag(Empty, TagLastDirectory);
        	xmlSerializer.text(lastDirectory_);
        	xmlSerializer.endTag(Empty, TagLastDirectory);
        	
        	xmlSerializer.startTag(Empty, TagCameraMode);
            xmlSerializer.text( Integer.toString(cameraMode_) );
            xmlSerializer.endTag(Empty, TagCameraMode);
            
            xmlSerializer.startTag(Empty, TagScreenMode);
            xmlSerializer.text( Integer.toString(screenMode_) );
            xmlSerializer.endTag(Empty, TagScreenMode);
        	
            xmlSerializer.endTag(Empty, TagConfig);
            
        	xmlSerializer.endDocument();

        	os.close();
        	changed_ = false;
        	
        }catch(FileNotFoundException e)
        {
        }catch(IOException e)
        {
        }
	}
	
	private void setDefault()
	{
		lastDirectory_ = Empty;
		cameraMode_ = Camera_Animation;
	    screenMode_ = Screen_Vertical;
	}
	
	private Config(){}

	private static Config instance_ = null; /** 実体 */
	
	private boolean changed_ = false;
	
	private String lastDirectory_; /** 最後に選択したディレクトリ */
	private int cameraMode_ = Camera_Animation;
	private int screenMode_ = Screen_Vertical;
}