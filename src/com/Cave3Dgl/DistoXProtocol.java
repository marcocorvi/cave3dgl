/* @file DistoXProtocol.java
 *
 * @author marco corvi
 * @date nov 2011
 *
 * @brief TopoDroid TopoDroid-DistoX communication protocol
 * --------------------------------------------------------
 *  Copyright This sowftare is distributed under GPL-3.0 or later
 *  See the file COPYING.
 * --------------------------------------------------------
 */
package com.Cave3Dgl;

import java.io.IOException;
import java.io.EOFException;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.UUID;
import java.util.Locale;
import java.lang.Math;

import java.net.Socket;

import java.nio.channels.ClosedByInterruptException;
import java.nio.ByteBuffer;

// import android.bluetooth.BluetoothDevice;
// import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;

import android.util.Log;

import android.widget.Toast;

public class DistoXProtocol
{
  static final String TAG = "Cave3Dgl";

  private Socket  mSocket = null;
  private DataInputStream  mIn;
  private DataOutputStream mOut;
  // private DataQueue     mQueue;
  private DistoXComm       mComm;

  // private byte[] mAddress;   // request-reply address
  // private byte[] mReplyBuffer;     // reply data
  private byte[] mAcknowledge;
  private byte   mSeqBit;          // sequence bit: 0x00 or 0x80

  private static final UUID MY_UUID = UUID.fromString( "00001101-0000-1000-8000-00805F9B34FB" );

  static final int DISTOX_ERR_OFF       = -1;
  static final int DISTOX_PACKET_NONE   = 0;
  static final int DISTOX_PACKET_DATA   = 1;
  static final int DISTOX_PACKET_G      = 2;
  static final int DISTOX_PACKET_M      = 3;
  static final int DISTOX_PACKET_VECTOR = 4;
  static final int DISTOX_PACKET_REPLY  = 5;

  double mDistance;
  double mBearing;
  double mClino;
  double e, n, z; // x = east, y = north, z = up
  // double mRoll;
  // double mAcceleration;
  // double mMagnetic;
  // double mDip; // magnetic dip
  // private byte mRollHigh; // high byte of roll
  // long mGX, mGY, mGZ;
  // long mMX, mMY, mMZ;

  final private byte[] mBuffer = new byte[8];
  int mMaxTimeout = 8;
  boolean mHasData;

  // byte[] getAddress() { return mAddress; }
  // byte[] getReply()   { return mReplyBuffer; }

  // FIXME the record of written calibration is not used
  // boolean writtenCalib = false;
  // public void setWrittenCalib( boolean b ) { writtenCalib = b; } 

//------------------------------------------------------

  private int getAvailable( long timeout, int maxtimeout ) throws IOException
  {
    mMaxTimeout = maxtimeout;
    final int[] dataRead = {0};
    final int[] toRead = {8};
    final int[] count = {0};
    final IOException[] maybeException = { null };
    final Thread reader = new Thread() {
      public void run() {
        // Log.v( TAG, "reader run " + dataRead[0] + "/" + toRead[0] );
        try {
          // synchronized( dataRead ) 
          {
            count[0] = mIn.read( mBuffer, dataRead[0], toRead[0] );
            toRead[0]   -= count[0];
            dataRead[0] += count[0];
          }
        } catch ( ClosedByInterruptException e ) {
          Log.v( TAG, "reader closed by interrupt");
        } catch ( IOException e ) {
          maybeException[0] = e;
        }
      }
    };
    reader.start();

    for ( int k=0; k<mMaxTimeout; ++k) {
      // Log.v( TAG, "interrupt loop " + k + " " + dataRead[0] + "/" + toRead[0] );
      try {
        reader.join( timeout );
      } catch ( InterruptedException e ) { Log.v( TAG, "reader join-1 interrupted"); }
      if ( ! reader.isAlive() ) break;
      {
        Thread interruptor = new Thread() { public void run() {
          // Log.v( TAG, "interruptor run " + dataRead[0] );
          for ( ; ; ) {
            // synchronized ( dataRead ) 
            {
              if ( dataRead[0] > 0 && toRead[0] > 0 ) { // FIXME
                try { wait( 100 ); } catch ( InterruptedException e ) { }
              } else {
                if ( reader.isAlive() ) reader.interrupt(); 
                break;
              }
            }
          }
          // Log.v( TAG, "interruptor done " + dataRead[0] );
        } };
        interruptor.start();

        try {
          interruptor.join( 200 );
        } catch ( InterruptedException e ) { Log.v( TAG, "interruptor join interrupted"); }
      }
      try {
        reader.join( 200 );
      } catch ( InterruptedException e ) { Log.v( TAG, "reader join-2 interrupted"); }
      if ( ! reader.isAlive() ) break; 
    }
    if ( maybeException[0] != null ) throw maybeException[0];
    return dataRead[0];
  }

//-----------------------------------------------------

  public DistoXProtocol( DataInputStream in, DataOutputStream out, DistoXComm comm )
  {
    // mSocket = socket;
    // mDistoX = distox;
    mSeqBit = (byte)0xff;
    // mQueue  = queue;
    mComm   = comm;

    // mHeadTailA3 = new byte[3];   // to read head/tail for Protocol A3
    // mHeadTailA3[0] = 0x38;
    // mHeadTailA3[1] = 0x20;       // address 0xC020
    // mHeadTailA3[2] = (byte)0xC0;

    // mAddress = new byte[2];
    // mReplyBuffer   = new byte[4];

    mAcknowledge = new byte[1];
    // mAcknowledge[0] = ( b & 0x80 ) | 0x55;

    // mBuffer = new byte[8];
  
    // try {
    //   if ( mSocket != null ) {
    //     // mIn  = new DataInputStream( extractCoreInputStream( mSocket.getInputStream() ) );
    //     mIn  = new DataInputStream( mSocket.getInputStream() );
    //     mOut = new DataOutputStream( mSocket.getOutputStream() );
    //   }
    // } catch ( IOException e ) {
    //   // NOTE socket is null there is nothing we can do
    //   Log.v( TAG, "Proto cstr conn failed " + e.getMessage() );
    // }
    mIn  = in;
    mOut = out;
    mHasData = false;
  }

  public void closeIOstreams()
  {
    if ( mIn != null ) {
      try { mIn.close(); } catch ( IOException e ) { }
      mIn = null;
    }
    if ( mOut != null ) {
      try { mOut.close(); } catch ( IOException e ) { }
      mOut = null;
    }
  }

  private int handlePacket( ) 
  {
    byte type = (byte)(mBuffer[0] & 0x3f);
    // if ( TDLog.LOG_PROTO ) {
    //   Log.v( TAG,
    //     "packet type " + type + " " + 
    //     String.format("%02x %02x %02x %02x %02x %02x %02x %02x", mBuffer[0], mBuffer[1], mBuffer[2],
    //     mBuffer[3], mBuffer[4], mBuffer[5], mBuffer[6], mBuffer[7] ) );
    // }

    int high, low;
    switch ( type ) {
      case 0x01: // data
        mHasData = true;

        int dhh = (int)( mBuffer[0] & 0x40 );
        double d =  dhh * 1024.0 + MemoryOctet.toInt( mBuffer[2], mBuffer[1] );
        double b = MemoryOctet.toInt( mBuffer[4], mBuffer[3] );
        double c = MemoryOctet.toInt( mBuffer[6], mBuffer[5] );
        // X31--ready
        // mRollHigh = mBuffer[7];

        // int r7 = (int)(mBuffer[7] & 0xff); if ( r7 < 0 ) r7 += 256;
        // // double r = (mBuffer[7] & 0xff);
        // double r = r7;

        if ( d < 99999 ) {
          mDistance = d / 1000.0;
        } else {
          mDistance = 100 + (d-100000) / 100.0;
        }

        mBearing  = b * 180.0 / 32768.0; // 180/0x8000;
        mClino    = c * 90.0  / 16384.0; // 90/0x4000;
        if ( c >= 32768 ) { mClino = (65536 - c) * (-90.0) / 16384.0; }
        // mRoll = r * 180.0 / 128.0;

        // if ( TDLog.LOG_PROTO ) {
        //   Log.v( TAG, "Proto packet data " + String.format(Locale.US, " %7.2f %6.1f %6.1f", mDistance, mBearing, mClino ) );
        // }
        double h = mDistance * Math.cos( mClino * Math.PI/180.0 );
        z = mDistance * Math.sin( mClino * Math.PI/180.0 );
        n = mDistance * Math.cos( mBearing * Math.PI/180.0 );
        e = mDistance * Math.sin( mBearing * Math.PI/180.0 );

        mComm.addPoint( e, n, z );

        mHasData = false;
        return DISTOX_PACKET_DATA;
      case 0x02: // g
        // mGX = MemoryOctet.toInt( mBuffer[2], mBuffer[1] );
        // mGY = MemoryOctet.toInt( mBuffer[4], mBuffer[3] );
        // mGZ = MemoryOctet.toInt( mBuffer[6], mBuffer[5] );

        // if ( mGX > TopoDroidUtil.ZERO ) mGX = mGX - TopoDroidUtil.NEG;
        // if ( mGY > TopoDroidUtil.ZERO ) mGY = mGY - TopoDroidUtil.NEG;
        // if ( mGZ > TopoDroidUtil.ZERO ) mGZ = mGZ - TopoDroidUtil.NEG;
        // Log.v( TAG, "handle Packet G " + String.format(" %x %x %x", mGX, mGY, mGZ ) );
        return DISTOX_PACKET_G;
      case 0x03: // m
        // mMX = MemoryOctet.toInt( mBuffer[2], mBuffer[1] );
        // mMY = MemoryOctet.toInt( mBuffer[4], mBuffer[3] );
        // mMZ = MemoryOctet.toInt( mBuffer[6], mBuffer[5] );

        // if ( mMX > TopoDroidUtil.ZERO ) mMX = mMX - TopoDroidUtil.NEG;
        // if ( mMY > TopoDroidUtil.ZERO ) mMY = mMY - TopoDroidUtil.NEG;
        // if ( mMZ > TopoDroidUtil.ZERO ) mMZ = mMZ - TopoDroidUtil.NEG;
        // Log.v( TAG, "handle Packet M " + String.format(" %x %x %x", mMX, mMY, mMZ ) );
        return DISTOX_PACKET_M;
      case 0x04: // vector data packet
        // if ( mDevice.mType == Device.DISTO_X310 ) {
        //   double acc = MemoryOctet.toInt( mBuffer[2], mBuffer[1] );
        //   double mag = MemoryOctet.toInt( mBuffer[4], mBuffer[3] );
        //   double dip = MemoryOctet.toInt( mBuffer[6], mBuffer[5] );
        //   double rh = MemoryOctet.toInt( mRollHigh, mBuffer[7] );
        //   mAcceleration = acc;
        //   mMagnetic = mag;
        //   mDip = dip * 90.0  / 16384.0; // 90/0x4000;
        //   if ( dip >= 32768 ) { mDip = (65536 - dip) * (-90.0) / 16384.0; }
        //   mRoll  = rh * 180.0 / 32768.0; // 180/0x8000;
        // }
        return DISTOX_PACKET_VECTOR;
      case 0x38: 
        // mAddress[0] = mBuffer[1];
        // mAddress[1] = mBuffer[2];
        // mReplyBuffer[0] = mBuffer[3];
        // mReplyBuffer[1] = mBuffer[4];
        // mReplyBuffer[2] = mBuffer[5];
        // mReplyBuffer[3] = mBuffer[6];
        return DISTOX_PACKET_REPLY;
      default:
        Log.v( TAG,
          "packet error. type " + type + " " + 
          String.format("%02x %02x %02x %02x %02x %02x %02x %02x", mBuffer[0], mBuffer[1], mBuffer[2],
          mBuffer[3], mBuffer[4], mBuffer[5], mBuffer[6], mBuffer[7] ) );
      //   return DISTOX_PACKET_NONE;
    }
    return DISTOX_PACKET_NONE;
  } 

  // read 1 packet ( 8 bytes )
  public int readPacket( )
  {
    try {
      // Log.v( TAG, "VD Proto read packet " );
      mIn.readFully( mBuffer, 0, 8 );
      byte seq  = (byte)(mBuffer[0] & 0x80); // sequence bit
      // Log.v( TAG, "VD read packet seq bit " + String.format("%02x %02x %02x", mBuffer[0], seq, mSeqBit ) );
      boolean ok = ( seq != mSeqBit );
      mSeqBit = seq;
      // if ( (mBuffer[0] & 0x0f) != 0 ) // ack every packet
      { 
        mAcknowledge[0] = (byte)(( mBuffer[0] & 0x80 ) | 0x55);
        mOut.write( mAcknowledge, 0, 1 );
      }
      if ( ok ) return handlePacket();
    } catch ( EOFException e ) {
      Log.v( TAG, "Proto read packet EOFException" + e.toString() );
    } catch (ClosedByInterruptException e ) {
      Log.v( TAG, "Proto read packet ClosedByInterruptException" + e.toString() );
    } catch (IOException e ) {
      // this is OK: the DistoX has been turned off
      Log.v( TAG, "Proto read packet IOException " + e.toString() + " OK distox turned off" );
      return DISTOX_ERR_OFF;
    }
    return DISTOX_PACKET_NONE;
  }
};
