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
	private static final String FileName = "config.xml";
	private static final String XmlEncoding = "UTF-8";
	private static final String Empty = "";

	private static final String TagConfig = "config";
	private static final String TagLastDirectory = "last_directory";
	private static final String TagCameraMode = "camera_mode";
	private static final String TagScreenMode = "screen_mode";

	public static final int Camera_Manual = 0;
	public static final int Camera_Animation = 1;

	public static final int Screen_Rot0 = 0;
	public static final int Screen_Rot90 = 1;
	public static final int Screen_Rot180 = 2;
	public static final int Screen_Rot270 = 3;


	//320×240 //QVGA 4:3
	//400×240 //WQVGA 16:9
	//800×480 //WVGA 15:9
	//854×480 //FWVGA 16:9
	//800×600 //SVGA 4:3
	//1024×576 //WSVGA 16:9

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
		lastDirectory_ = directory;
	}

	public int getCameraMode()
	{
	    return cameraMode_;
	}

	public void setCameraMode(int mode)
	{
	    cameraMode_ = mode;
	}

	public int getScreenMode()
	{
	    return screenMode_;
	}

	public void setScreenMode(int mode)
	{
	    screenMode_ = mode;
	}

	private String createPath(String appName)
	{
		File root = Environment.getExternalStorageDirectory();

		//ディレクトリがなかったら作成
		String appDirPath = root.getAbsolutePath() + "/" + appName + "/";
		File appDirectory = new File(appDirPath);
		if(appDirectory.exists()){
		    return appDirPath + FileName;
		}

		if(appDirectory.mkdir()){
		    return appDirPath + FileName;
		}
		return null;
	}

	/**
	 * 設定ファイルから値ロード。ファイルがない、又は外部ストレージがない場合は、デフォルト値を設定。
	 * @param appName ... アプリケーション名
	 */
	public void load(String appName)
	{
	    setDefault();
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
                            screenMode_ = clamp(screenMode_, 0, 3);
                        }
        				break;
        			}
        		}
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
		if(false == Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            return;
        }

        try{
        	String path = createPath(appName);
        	if(path == null){
        	    return;
        	}
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
	    screenMode_ = Screen_Rot0;
	}

	private Config(){}

	private static Config instance_ = null; /** 実体 */

	private String lastDirectory_; /** 最後に選択したディレクトリ */
	private int cameraMode_ = Camera_Animation;
	private int screenMode_ = Screen_Rot0;
}