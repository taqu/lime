package hm.orz.stabo.EGDA.UI;

import java.util.List;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class ListStringAdapter extends ArrayAdapter<String>
{
    public ListStringAdapter(Context context, List<String> items)
    {
        super(context, 0, items);
        items_ = items;
    }
    
    public View getView(int position, View convertView, ViewGroup parent)
    {
        if(convertView == null){
            convertView = new TextView(this.getContext());
            convertView.setMinimumWidth(parent.getWidth()/2);
        }
        TextView view = (TextView)convertView;
        view.setText(items_.get(position));
        return convertView;
    }

    private List<String> items_;
}
