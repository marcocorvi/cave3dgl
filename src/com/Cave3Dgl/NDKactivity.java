package com.Cave3Dgl;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import java.io.File;

import android.net.Uri;
import android.widget.Button;
import android.view.View;
import android.view.View.OnClickListener;

public class NDKactivity extends Activity
{
  static {
    System.loadLibrary("Cave3Dgl");
  }

  @Override
  public void onCreate( Bundle saved )
  {
    super.onCreate( saved );
 
    setContentView( R.layout.ndkactivity );
    Button btn = (Button)findViewById( R.id.btn_go );
    btn.setOnClickListener( new OnClickListener() {
      @Override
      public void onClick( View v ) {
        startNativeActivity( null );
      }
    } );
  }

  private void startNativeActivity( String filename )
  {
    Intent intent = new Intent( getBaseContext(), android.app.NativeActivity.class );
    if ( filename != null ) {
      intent.setData( Uri.fromFile( new File(filename) ) );
    }
    startActivity( intent );
  }


}
