package hm.orz.stabo.EGDA;

import android.content.Context;
import android.widget.Toast;

public class InfoToast
{
    public static void info(Context context, CharSequence text)
    {
        Toast.makeText(context, text, Toast.LENGTH_SHORT).show();
    }
}
