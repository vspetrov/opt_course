package com.example.opt_course_ex;

import java.io.IOException;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.widget.EditText;
import android.widget.ImageView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.*;
import org.opencv.imgproc.Imgproc;
import org.opencv.android.Utils;
import android.graphics.Bitmap;

public class OpCourseEx extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.op_course_ex_layout);
		((EditText)findViewById(R.id.minTime)).setText("minTime: ");
		((EditText)findViewById(R.id.maxTime)).setText("maxTime: ");
		((EditText)findViewById(R.id.avTime)).setText("avTime: ");
	}

	private void do_test(){
		ImageView imgView = (ImageView) findViewById(R.id.sampleImageView);
		Mat img = new Mat();
		try {
			img = Utils.loadResource(OpCourseEx.this, R.drawable.img1);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		Imgproc.threshold(img,img,128,255,Imgproc.THRESH_BINARY | Imgproc.THRESH_OTSU);
		Mat img_bkp = img.clone();
		
		
		int iterations=10;
		double maxTime=0, minTime=1e10, avTime=0;
		for (int i=0; i<iterations; i++){
			Imgproc.resize(img_bkp, img, new Size(100,100));
			double elapsedTime = thinning(img.getNativeObjAddr(),0);
			if (elapsedTime > maxTime) maxTime = elapsedTime;
			if (elapsedTime < minTime) minTime = elapsedTime;
			avTime += elapsedTime;
		}
		avTime /= iterations;
		
		((EditText)findViewById(R.id.minTime)).setText("minTime: "+String.format("%.6f",minTime));
		((EditText)findViewById(R.id.maxTime)).setText("maxTime: "+String.format("%.6f",maxTime));
		((EditText)findViewById(R.id.avTime)).setText("avTime: "+String.format("%.6f",avTime));
		
		
		Imgproc.resize(img_bkp, img_bkp, new Size(200,200));
		Imgproc.resize(img, img, new Size(200,200));
		
		Mat img_rst = new Mat(200,410,img_bkp.type());
		img_rst.setTo(new Scalar(255));
		img_bkp.copyTo(img_rst.submat(new Rect(0,0,200,200)));
		img.copyTo(img_rst.submat(new Rect(205,0,200,200)));
		Bitmap img_bm = Bitmap.createBitmap(img_rst.cols(),img_rst.rows(),Bitmap.Config.ARGB_8888);
		Utils.matToBitmap(img_rst, img_bm);
		imgView.setImageBitmap(img_bm);
	}
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.op_course_ex, menu);
		return true;
	}
	public native double thinning(long addrMat, long useGHall);
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
	    @Override
	    public void onManagerConnected(int status) {
	        switch (status) {
	            case LoaderCallbackInterface.SUCCESS:
	            {
	              System.loadLibrary("thinning");
	              do_test();
	            } break;
	            default:
	            {
	                super.onManagerConnected(status);
	            } break;
	        }
	    }
	};

	@Override
	public void onResume()
	{
	    super.onResume();
	    OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_6, this, mLoaderCallback);
	}

}
