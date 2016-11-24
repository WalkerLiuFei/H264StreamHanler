package walker.person.mediahandlerlib;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

import java.io.File;

import walker.person.mediahandlerlib.example.Demux;
import walker.person.mediahandlerlib.example.DemuxCallBackInterface;
import walker.person.mediahandlerlib.video.core.DecodeCallBack;
import walker.person.mediahandlerlib.video.core.H264StreamDecoder;


public class MainActivity extends Activity implements DemuxCallBackInterface, DecodeCallBack {
    private static final String TAG = "MainActivity";
    private H264StreamDecoder decoder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        File file = new File(Environment.getExternalStorageDirectory().getPath());
        String path = null;
        for (File fif : file.listFiles()){
            if (fif.getName().contains(".mp4")){
                path = fif.getAbsolutePath();
                Log.d(TAG,path);
                fif.setReadable(true);
                break;
            }
        }
        Demux demux  = new Demux();
        decoder = new H264StreamDecoder.Builder()
                .setDecodeListener(this)
                .create();
        demux.setDemuxCallbackInterface(this);
        demux.demux(path);

    }

    @Override
    public void callback(byte[] bytes) {
        if (decoder!=null)
            decoder.decodeData(bytes);
    }

    @Override
    public void decodeSuccess(byte[] decodedStream, int width, int height) {

    }

    @Override
    public void decodeSuccess(byte[] decodedStream) {

    }
}
