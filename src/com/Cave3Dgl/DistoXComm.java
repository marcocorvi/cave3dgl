/* @file DistoXComm.java
 *
 * @author marco corvi
 * @date nov 2011
 *
 * @brief DistoX BlueTooth communication (extracted from TopoDroid)
 * --------------------------------------------------------
 *  Copyright This sowftare is distributed under GPL-3.0 or later
 *  See the file COPYING.
 * --------------------------------------------------------
 */
package com.Cave3Dgl;

import java.nio.ByteBuffer;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.EOFException;
import java.io.UnsupportedEncodingException;
import java.util.UUID;
import java.util.List;

import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;

import android.content.Context;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

// import android.os.Parcelable;
import android.os.ParcelUuid;
// import android.os.AsyncTask;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
// import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;

import android.database.DataSetObserver;

// import android.widget.Toast;
import android.util.Log;

public class DistoXComm 
{
  static final String TAG = "Cave3Dgl";

  private Context mContext;

  private BluetoothDevice  mBTDevice;
  private BluetoothSocket  mBTSocket;
  private BluetoothAdapter mBTAdapter;

  static final String mDefaultAddress = "00:13:43:08:3B:3E";
  private String mAddress = null;
  private DistoXProtocol mProtocol;
  // private DataQueue      mQueue;
  private boolean mBTConnected;
  private boolean mConnected;
  public byte[] mCoeff;
  private volatile int nReadPackets;
  private RfcommThread mRfcommThread;

  private BroadcastReceiver mBTReceiver = null;


  // --------------------------------------------------

  DistoXComm( Context context )
  {
    mContext   = (Context)context;
    mAddress   = null;
    mBTSocket  = null;
    mBTAdapter = BluetoothAdapter.getDefaultAdapter();
    // Log.v( TAG, "DistoXComm cstr");
    // mQueue     = new DataQueue();
  }

  // -------------------------------------------------------- 
  // DEVICE

  public boolean disconnect( )
  {
    Log.v( TAG, "DistoXComm disconnect");
    cancelRfcommThread();
    closeProtocol();
    destroySocket( );
    return true;
  }

  public boolean connect( String address )
  {
    // Log.v( TAG, "DistoXComm connect addr <" + address + ">" );
    if ( mRfcommThread != null ) {
      return true;
    }
    if ( ! connectSocket( address ) ) {
      return false;
    }
    startRfcommThread( );
    return true;
  }


  public native void addPoint( double e, double n, double z );

  // public Data getData( ) { return mQueue.get(); }

  // public void resume()
  // {
  //   // if ( mRfcommThread != null ) { mRfcommThread.resume(); }
  // }

  // public void suspend()
  // {
  //   // if ( mRfcommThread != null ) { mRfcommThread.suspend(); }
  // }
  private void resetBTReceiver()
  {
    if ( mBTReceiver == null ) return;
    // Log.v( TAG, "reset BT receiver");
    try {
      mContext.unregisterReceiver( mBTReceiver );
    } catch ( IllegalArgumentException e ) {
      Log.v( TAG, "Error: unregister BT receiver error " + e.getMessage() );
    }
    mBTReceiver = null;
  }

  // -----------------------------------------------------------
  // Bluetooth receiver

  // called only by connectSocket
  private void setupBTReceiver()
  {
    resetBTReceiver();
    // Log.v( TAG, "setup BT receiver");
    mBTReceiver = new BroadcastReceiver() 
    {
      @Override
      public void onReceive( Context ctx, Intent data )
      {
        String action = data.getAction();
        BluetoothDevice bt_device = data.getParcelableExtra( BluetoothDevice.EXTRA_DEVICE );
        String device_address = ( bt_device != null )? bt_device.getAddress() : "undefined";

        // if ( BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals( action ) ) {
        // } else if ( BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals( action ) ) {
        // } else if ( BluetoothDevice.ACTION_FOUND.equals( action ) ) {
        if ( BluetoothDevice.ACTION_ACL_CONNECTED.equals( action ) ) {
          Log.v( TAG, "[C] ACL_CONNECTED " + device_address + " addr <" + mAddress + ">" );
          if ( device_address.equals(mAddress) ) // FIXME ACL_DISCONNECT
          {
            mConnected = true;
          }
        } else if ( BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED.equals( action ) ) {
          Log.v( TAG, "[C] ACL_DISCONNECT_REQUESTED " + device_address + " addr <" + mAddress + ">" );
          if ( device_address.equals(mAddress) ) // FIXME ACL_DISCONNECT
          {
            mConnected = false;
            closeSocket( );
          }
        } else if ( BluetoothDevice.ACTION_ACL_DISCONNECTED.equals( action ) ) {
          Log.v( TAG, "[C] ACL_DISCONNECTED " + device_address + " addr <" + mAddress + ">" );
          if ( device_address.equals(mAddress) ) // FIXME ACL_DISCONNECT
          {
            mConnected = false;
            closeSocket( );
          }
        } else if ( BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals( action ) ) { // NOT USED
          final int state     = data.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
          final int prevState = data.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE, BluetoothDevice.ERROR);
          if (state == BluetoothDevice.BOND_BONDED && prevState == BluetoothDevice.BOND_BONDING) {
            Log.v( TAG, "BOND STATE CHANGED paired (BONDING --> BONDED) " );
          } else if (state == BluetoothDevice.BOND_NONE && prevState == BluetoothDevice.BOND_BONDED){
            Log.v( TAG, "BOND STATE CHANGED unpaired (BONDED --> NONE) " );
          } else if (state == BluetoothDevice.BOND_BONDING && prevState == BluetoothDevice.BOND_BONDED) {
            Log.v( TAG, "BOND STATE CHANGED unpaired (BONDED --> BONDING) " );
            if ( mBTSocket != null ) {
              Log.v( TAG, "[*] socket is not null: close and retry connect ");
              mConnected = false;
              closeSocket( );
              connectSocket( mAddress ); // returns immediately if mAddress == null
            }
          } else {
            Log.v( TAG, "BOND STATE CHANGED " + prevState + " --> " + state  );
          }

          // DeviceUtil.bind2Device( data );
        // } else if ( BluetoothDevice.ACTION_PAIRING_REQUEST.equals(action) ) {
        //   Log.v("DistoX", "PAIRING REQUEST");
        //   // BluetoothDevice device = getDevice();
        //   // //To avoid the popup notification:
        //   // device.getClass().getMethod("setPairingConfirmation", boolean.class).invoke(device, true);
        //   // device.getClass().getMethod("cancelPairingUserInput", boolean.class).invoke(device, true);
        //   // byte[] pin = ByteBuffer.allocate(4).putInt(0000).array();
        //   // //Entering pin programmatically:  
        //   // Method ms = device.getClass().getMethod("setPin", byte[].class);
        //   // //Method ms = device.getClass().getMethod("setPasskey", int.class);
        //   // ms.invoke(device, pin);
        //     
        //   //Bonding the device:
        //   // Method mm = device.getClass().getMethod("createBond", (Class[]) null);
        //   // mm.invoke(device, (Object[]) null);
        }
      }
    };


    // mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_FOUND ) );
    // mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothAdapter.ACTION_DISCOVERY_STARTED ) );
    // mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothAdapter.ACTION_DISCOVERY_FINISHED ) );
    mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_ACL_CONNECTED ) );
    mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED ) );
    mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_ACL_DISCONNECTED ) );
    // mContext.registerReceiver( mBTReceiver, uuidFilter  = new IntentFilter( myUUIDaction ) );
    mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_BOND_STATE_CHANGED ) );
    // mContext.registerReceiver( mBTReceiver, new IntentFilter( BluetoothDevice.ACTION_PAIRING_REQUEST ) );
  }

  // -------------------------------------------------------- 
  // SOCKET

  /** close the socket (and the RFcomm thread) but don't delete it
   * alwasy called with wait_thread
   */
  private synchronized void closeSocket( )
  {
    if ( mBTSocket == null ) return;

    // Log.v( TAG, "close socket() addr <" + mAddress + ">" );
    for ( int k=0; k<1 && mBTSocket != null; ++k ) {
      // Log.v( TAG, "try close socket nr " + k );
      cancelRfcommThread();
      closeProtocol();
      try {
        mBTSocket.close();
        mBTSocket = null;
      } catch ( IOException e ) {
        Log.v( TAG, "close socket IOexception " + e.getMessage() );
        // LogStackTrace( e );
      } finally {
        mBTConnected = false;
      }
    }
    mBTConnected = false;
  }

  /** close the socket and delete it
   * the connection becomes unusable
   * As a matter of fact this is alwyas called with wait_thread = true
   */
  private void destroySocket( ) // boolean wait_thread )
  {
    if ( mBTSocket == null ) return;
    // Log.v( TAG, "destroy socket() addr <" + mAddress + ">" );
    // closeProtocol(); // already in closeSocket()
    closeSocket();
    // mBTSocket = null;
    resetBTReceiver();
  }


  /** create a socket (not connected)
   *  and a connection protocol on it
   */
  private void createSocket( String address, int port )
  {
    if ( address == null || address.length() == 0 ) {
      address = mDefaultAddress;
      // Log.v( TAG, "create Socket() using default address" );
    }
    // Log.v( TAG, "create Socket() param-addr <" + address + "> addr <" + mAddress + ">" );
    if ( mProtocol == null || ! address.equals( mAddress ) ) {
      if ( mProtocol != null && ! address.equals( mAddress ) ) {
        disconnect();
      }

      if ( mBTSocket != null ) {
        // Log.v( TAG, "create Socket() BTSocket not null ... closing");
        try {
          mBTSocket.close();
        } catch ( IOException e ) { 
          Log.v( TAG, "close Socket IO " + e.getMessage() );
        }
        mBTSocket = null;
      }

      // Log.v( TAG, "get remote BT device for address <" + address + ">" );
      mBTDevice = mBTAdapter.getRemoteDevice( address );
      // assume the device is PAIRED
      {
        try {
          Class[] classes1 = new Class[]{ int.class };
          Class[] classes2 = new Class[]{ UUID.class };
          // if ( TDSetting.mSockType == TDSetting.TD_SOCK_DEFAULT ) {
          //   // Log.v( TAG, "create Socket() createRfcommSocketToServiceRecord " );
          //   mBTSocket = mBTDevice.createRfcommSocketToServiceRecord( UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") );
          // } else if ( TDSetting.mSockType == TDSetting.TD_SOCK_INSEC ) {
            // Log.v( TAG, "create Socket() createInsecureRfcommSocketToServiceRecord " );
            Method m3 = mBTDevice.getClass().getMethod( "createInsecureRfcommSocketToServiceRecord", classes2 );
            mBTSocket = (BluetoothSocket) m3.invoke( mBTDevice, UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") );
          // } else if ( TDSetting.mSockType == TDSetting.TD_SOCK_INSEC_PORT ) {
          //   // Log.v( TAG, "create Socket() invoke createInsecureRfcommSocket " );
          //   Method m1 = mBTDevice.getClass().getMethod( "createInsecureRfcommSocket", classes1 );
          //   mBTSocket = (BluetoothSocket) m1.invoke( mBTDevice, port );
          //   // mBTSocket = mBTDevice.createInsecureRfcommSocket( port );
          //   // mBTSocket = (BluetoothSocket) m1.invoke( mBTDevice, UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") );
          // } else if ( TDSetting.mSockType == TDSetting.TD_SOCK_PORT ) {
          //   // Log.v( TAG, "create Socket() invoke createRfcommSocket " );
          //   Method m2 = mBTDevice.getClass().getMethod( "createRfcommSocket", classes1 );
          //   mBTSocket = (BluetoothSocket) m2.invoke( mBTDevice, port );
          // }

        } catch ( InvocationTargetException e ) {
          Log.v( TAG, "create Socket invoke target " + e.getMessage() );
          if ( mBTSocket != null ) { mBTSocket = null; }
        // } catch ( UnsupportedEncodingException e ) {
        //   Log.v( TAG, "create Socket encoding " + e.getMessage() );
        //   if ( mBTSocket != null ) { mBTSocket = null; }
        } catch ( NoSuchMethodException e ) {
          Log.v( TAG, "create Socket no method " + e.getMessage() );
          if ( mBTSocket != null ) { mBTSocket = null; }
        } catch ( IllegalAccessException e ) {
          Log.v( TAG, "create Socket access " + e.getMessage() );
          if ( mBTSocket != null ) { mBTSocket = null; }
        // } catch ( IOException e ) {
        //   Log.v( TAG, "create Socket IO " + e.getMessage() );
        //   if ( mBTSocket != null ) { mBTSocket = null; }
        }
      }

      mProtocol = null;
      if ( mBTSocket != null ) {
        // Log.v( "CavegL", "create Socket OK");
        // mBTSocket.setSoTimeout( 200 ); // BlueToothSocket does not have timeout 
        try {
          DataInputStream in   = new DataInputStream( mBTSocket.getInputStream() );
          DataOutputStream out = new DataOutputStream( mBTSocket.getOutputStream() );
          mProtocol = new DistoXProtocol( in, out, this );
          mAddress = address;
        } catch ( IOException e ) {
          Log.v( TAG, "socket stream error " + e.getMessage() );
          mAddress = null;
          try {
            mBTSocket.close();
          } catch ( IOException ee ) { 
            Log.v( TAG, "close Socket IO " + ee.getMessage() );
          }
          mBTSocket = null;
        }
      }
      if ( mBTSocket == null ) {
        Log.v( TAG, "create Socket failure");
        if ( mProtocol != null ) mProtocol.closeIOstreams();
        mProtocol = null;
        mAddress = null;
      }
      mBTConnected = false; // socket is created but not connected
    }
  }

  /** get the list of UUIDs supported by the remote device (for the DistoX only SPP uuid)
   */
  private void doGetUuids()
  {
    if ( mBTDevice == null ) {
      Log.v( TAG, "null BT device");
      return;
    }
    try {
      Class cl = Class.forName("android.bluetooth.BluetoothDevice");
      Class[] pars = {};
      Method m0 = cl.getMethod( "getUuids", pars );
      Object[] args = {};
      ParcelUuid[] uuids = (ParcelUuid[]) m0.invoke( mBTDevice, args );
      // if ( uuids != null ) {
      //   for ( ParcelUuid uid : uuids ) {
      //     Log.v( TAG, "uuid " + uid.toString() );
      //   }
      // }
    } catch ( Exception e ) {
      Log.v( TAG, "get uuids error " + e.getMessage() );
    }
  }

  /** connect the socket to the device
   */
  private boolean connectSocket( String address )
  {
    if ( address == null ) return false;
    // Log.v( TAG, "connect socket(): " + address );
    createSocket( address, 1 ); // default port == 1

    // DEBUG
    // doGetUuids();

    if ( mBTSocket != null ) {
      // cancelDiscovery should always be called before connecting
      boolean b = mBTAdapter.cancelDiscovery();

      setupBTReceiver();

      int port = 0;
      while ( ! mBTConnected && port < 3 ) {
        ++ port;
        if ( mBTSocket != null ) {
          // Log.v( TAG, "connect socket() try port " + port );
          try {
            // Log.v( TAG, "[3] device state " + mBTDevice.getBondState() );
            mBTSocket.connect();
            mBTConnected = true;
          } catch ( IOException e ) {
            Log.v( TAG, "connect socket() (port " + port + ") IO error " + e.getMessage() );
            closeSocket();
            // mBTSocket = null;
          }
        }
        if ( mBTSocket == null && port < 3 ) {
          createSocket( address, port );
        }
        // Log.v( TAG, "connect socket() port " + port + " connected " + mBTConnected );
      }
    } else {
      Log.v( TAG, "connect socket() null socket");
    }
    // Log.v( TAG, "connect socket() result " + mBTConnected );
    return mBTConnected;
  }

  // -------------------------------------------------------------------------------------
  // ON-DEMAND DATA DOWNLOAD

  // public int downloadData( String address, Handler /* ILister */ lister ) // FIXME LISTER
  // {
  //   if ( ! checkRfcommThreadNull( "download data: address " + address ) ) {
  //     return DistoXProtocol.DISTOX_ERR_CONNECTED;
  //   }
  //   
  //   int ret = -1; // failure
  //   if ( connectSocket( address ) ) {
  //     if ( TDSetting.mHeadTail ) {
  //       boolean a3 = ( mApp.distoType() == Device.DISTO_A3 );
  //       byte[] command = ( a3 ? DeviceA3Details.HeadTail : DeviceX310Details.HeadTail );
  //       int prev_read = -1;
  //       int to_read = mProtocol.readToRead( command, a3 );
  //       if ( to_read <= 0 ) {
  //         ret = to_read;
  //       } else {
  //         // FIXME asyncTask ?
  //         // nReadPackets = 0; // done in RfcommThread cstr
  //         startRfcommThread( to_read );
  //         while ( mRfcommThread != null && nReadPackets < to_read ) {
  //           if ( nReadPackets != prev_read ) {
  //             prev_read = nReadPackets;
  //           }
  //           try { Thread.sleep( 100 ); } catch ( InterruptedException e ) { }
  //         }
  //         Log.v( TAG, "download done: A3 read " + nReadPackets );
  //         ret = nReadPackets;
  //       }
  //     } else {
  //       startRfcommThread( -1 );
  //       while ( mRfcommThread != null ) {
  //         try { Thread.sleep( 100 ); } catch ( InterruptedException e ) { }
  //       }
  //       // cancelRfcommThread(); // called by closeSocket() which is called by destroySocket()
  //       ret = nReadPackets;
  //     }
  //   } else {
  //     Log.v( TAG, "download data: fail to connect socket");
  //   }
  //   destroySocket( );
  //   
  //   return ret;
  // }

  // -----------------------------------------------------
  // RFCOMM THREAD

  private boolean startRfcommThread( )
  {
    // Log.v( TAG, "start RFcomm thread ");
    if ( mBTSocket != null ) {
      if ( mRfcommThread == null ) {
        mRfcommThread = new RfcommThread( mProtocol );
        mRfcommThread.start();
        // Log.v( TAG, "startRFcommThread started");
      } else {
        Log.v( TAG, "startRFcommThread already running");
      }
      return true;
    } else {
      mRfcommThread = null;
      Log.v( TAG, "startRFcommThread: null socket");
      return false;
    }
  }

  void cancelRfcommThread()
  {
    if ( mRfcommThread != null ) {
      mRfcommThread.cancelWork();
      try {
        mRfcommThread.join();
      } catch ( InterruptedException e ) {
      } finally {
        mRfcommThread = null;
      }
    } else {
      // nothing
    }
  }

  private boolean checkRfcommThreadNull( String msg ) { return ( mRfcommThread == null ); }

  private void closeProtocol() { mProtocol = null; }

};
