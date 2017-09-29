package com.Cave3Dgl;

import android.util.Log;

class RfcommThread extends Thread
{
  private DistoXProtocol mProto;
  private volatile boolean doWork = true;

  void cancelWork()
  {
    // if ( mProto != null ) mProto.mMaxTimeout = 0;
    doWork = false;
  }

  /** 
   * @param protocol    communication protocol
   * @param to_read     number of data to read (use -1 to read forever until timeout or an exception)
   */
  public RfcommThread( DistoXProtocol protocol ) // FIXME LISTER
  {
    mProto = protocol;
  }

  public void run()
  {
    boolean hasG = false;
    doWork = true;
    mProto.mHasData = false;

    // Log.v( "CaveGL", "RFcomm thread running ... to_read " + toRead );
    while ( doWork ) {
      // Log.v( "CaveGL", "RFcomm loop: read " + nReadPackets + " to-read " + toRead );
      while ( mProto.mHasData ) {
        try {
          Thread.sleep(100);
        } catch ( InterruptedException e ) { }
      }
      int res = mProto.readPacket( );

      // Log.v( "CaveGL", "RFcomm readPacket returns " + res );
      if ( res == DistoXProtocol.DISTOX_PACKET_NONE ) {
        try {
          // Log.v( "CaveGL", "RFcomm sleeping 1000 " );
          Thread.sleep( 500 );
        } catch (InterruptedException e) { }
      } else if ( res == DistoXProtocol.DISTOX_ERR_OFF ) {
        Log.v( "CaveGL", "RFcomm readPacket returns ERR_OFF " );
        // if ( TDSetting.mCommType == 1 && TDSetting.mAutoReconnect ) { // FIXME ACL_DISCONNECT
        //   mApp.mDataDownloader.setConnected( false );
        //   mApp.notifyStatus();
        //   closeSocket( );
        //   mApp.notifyDisconnected();
        // }
        doWork = false;
      } else if ( res == DistoXProtocol.DISTOX_PACKET_DATA ) {
        // ++nReadPackets;
        // double d = mProto.mDistance;
        // double b = mProto.mBearing;
        // double c = mProto.mClino;
        // double r = mProto.mRoll;
        // Log.v( "CaveGL", "DATA PACKET " + d + " " + b + " " + c );
        // NOTE type=0 shot is DistoX-type
      } else if ( res == DistoXProtocol.DISTOX_PACKET_G ) {
        // Log.v( "CaveGL"OX, "G PACKET" );
        // ++nReadPackets;
        hasG = true;
      } else if ( res == DistoXProtocol.DISTOX_PACKET_M ) {
        // Log.v( "CaveGL", "M PACKET" );
        // ++nReadPackets;
        hasG = false;
      } else if ( res == DistoXProtocol.DISTOX_PACKET_REPLY ) {
      } else if ( res == DistoXProtocol.DISTOX_PACKET_VECTOR ) {
        // ++nReadPackets;  // vector packet do count
        // double acc  = mProto.mAcceleration;
        // double mag  = mProto.mMagnetic;
        // double dip  = mProto.mDip;
        // double roll = mProto.mRoll;
        // Log.v( "CaveGL"OX, "VECTOR PACKET " + mLastShotId + " " + acc + " " + mag + " " + dip + " " + roll );
      }
    }
    // Log.v( "CaveGL", "RFcomm thread run() exiting");
  }
};
