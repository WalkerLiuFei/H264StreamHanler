package walker.person.mediahandlerlib.video.core;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import walker.person.mediahandlerlib.video.natives.H264Decoder;

/**
 * Created by walker on 2016/11/21 0021.
 */
public class H264StreamDecoder {
    private H264Decoder mH264Decoder;
    private DecodeCallBack mDecodeCallBack;
    private BlockingQueue<byte[]> mSrcH264Data;
    private BlockingQueue<byte[]> mDstData;
    //make the decoder as singleton
    private H264StreamDecoder h264StreamDecoder;
    private volatile boolean workStatus;
    private H264StreamDecoder(H264StreamDecoder.Builder builder){
        //do noting....
    };

    public static class Builder{
        private static final int DFAULT_LENGTH = 5;
        DecodeCallBack decodeCallBack;
        int queueLength = DFAULT_LENGTH;
        public H264StreamDecoder create(){
            H264StreamDecoder decoder = new H264StreamDecoder(this);
            decoder.mH264Decoder = new H264Decoder();
            decoder.mH264Decoder.initDecoder();
            decoder.mDecodeCallBack = this.decodeCallBack;
            decoder.mSrcH264Data = new LinkedBlockingQueue<>(queueLength);
            decoder.mDstData = new LinkedBlockingQueue<>(queueLength);
            //start decode
            new Thread(decoder.new DecoderWorker()).start();
            return decoder;
        }
        public Builder setDecodeListener(DecodeCallBack decodeCallBack){
            this.decodeCallBack = decodeCallBack;
            return this;
        }

        public Builder setWaitingQueueLength(int queueLength){
            if (queueLength < 0)
                throw new RuntimeException("The blocking length must be positive!");
            this.queueLength = queueLength;
            return this;
        }
    }

    /**
     * the decode work thread
     */
    private class DecoderWorker implements Runnable{
        @Override
        public void run() {
            while (workStatus){
                // keep tracking the Blocking queue
                try {
                    byte[]  data = mSrcH264Data.poll(5, TimeUnit.MILLISECONDS);
                    assert(data!=null); //data should not be null.
                    byte[] decodedData = mH264Decoder.consumeStream(data);
                    mDstData.offer(decodedData);
                    if (mDstData.size() > 0){
                        mDecodeCallBack.decodeSuccess(mDstData.poll());
                        mDstData.offer(decodedData);
                    }else{
                        mDecodeCallBack.decodeSuccess(decodedData);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void decodeData(byte[] datas){
        //TODO:check H264  data...
        mSrcH264Data.offer(datas);
        byte[] decodedData = mH264Decoder.consumeStream(datas);
        if (decodedData != null && mDecodeCallBack != null){
            mDecodeCallBack.decodeSuccess(datas);
        }
    }


    public void decodeData(byte[] datas,DecodeCallBack decodeCallBack){
        //TODO:check H264  data...
        byte[] decodedData = mH264Decoder.consumeStream(datas);
        if (decodedData != null && decodeCallBack != null){
            decodeCallBack.decodeSuccess(decodedData);
        }
    }
}
