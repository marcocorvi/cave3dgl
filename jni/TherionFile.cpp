#include "TherionFile.h"

#include <stdio.h>
#include "TherionFlip.h"
#include "Log.h"

// used by ReadFile
int survey_pos[50]; // FIXME max 50 levels
int ks = 0;


// void 
// TherionFile::DumpStations() const
// {
//   for ( std::vector<TherionStation *>::const_iterator it = stations.begin(); it != stations.end(); ++it ) {
//     TherionStation * st = *it;
//     LOGI( "Station %s %.2f %.2f %.2f", st->Name().c_str(), st->E(), st->N(), st->Z() );
//   }
// }
// 
// void 
// TherionFile::DumpShots() const
// {
//   for ( std::vector< TherionShot * >::const_iterator it = shots.begin(); it != shots.end(); ++it ) {
//     TherionShot * sh = *it;
//     LOGI( "Shot %s %s %.2f %.1f %.1f", sh->From().c_str(), sh->To().c_str(), sh->Length(), sh->Bering(), sh->Clino() );
//   }
// }

TherionSurvey * 
TherionFile::GetSurvey( const char * name )
{
  for ( std::vector<TherionSurvey *>::iterator it = surveys.begin(); it != surveys.end(); ++it ) {
    if ( (*it)->Name() == name ) return *it;
  }
  return NULL;
}

/** fill an array of float with the values of the stations coordinates
 *  and set the "vertex" index of the stations
 */
void 
TherionFile::GetStationVertices( float v[] )
{
  int k = 0;
  int k3 = 0;
  for ( std::vector<TherionStation *>::iterator it = stations.begin(); it != stations.end(); ++it ) {
    TherionStation * s = *it;
    s->SetVertex( k++ );
    v[k3++] = s->E(); // X horizontal
    v[k3++] = s->N(); // Y vertical
    v[k3++] = s->Z(); // Z depth
  }
}
 
void 
TherionFile::SetShotSurveys()
{
  for ( std::vector<TherionShot*>::iterator it = shots.begin(); it != shots.end(); ++it ) {
    TherionShot * sh = *it;
    TherionStation * sf = sh->FromStation();
    TherionStation * st = sh->ToStation();
    sh->SetSurvey( NULL );
    if ( sf != NULL && st != NULL ) {
      std::string sv = sh->From();
      size_t pos = sv.find('@', 0);
      if ( pos != std::string::npos ) {
        sv = sv.substr( 1 + pos );
        for ( std::vector<TherionSurvey*>::iterator it2 = surveys.begin(); it2 != surveys.end(); ++it2 ) {
          TherionSurvey * srv = *it2;
          if ( srv->Name() == sv ) {
            sh->SetSurvey( srv );
            srv->AddShotInfo( sh );
            break;
          }
        }
        if ( sh->Survey() == NULL ) {
          // LOGI("New survey <%s>", sv.c_str() );
          TherionSurvey * survey = new TherionSurvey( sv.c_str() );
          surveys.push_back( survey );
          sh->SetSurvey( survey );
          survey->AddShotInfo( sh );
        } 
      }
    }
  }
}

void 
TherionFile::SetSplaySurveys()
{
  for ( std::vector<TherionShot*>::iterator it = splays.begin(); it != splays.end(); ) {
    TherionShot * sh = *it;
    std::string sv;
    TherionStation * sf = sh->FromStation();
    if ( sf == NULL ) {
      sf = sh->ToStation();
      sv = sh->To();
    } else {
      sv = sh->From();
    }
    if ( sv.length() > 0 ) {
      size_t pos = sv.find('@', 0);
      if ( pos != std::string::npos ) {
        sv = sv.substr( 1 + pos );
        for ( std::vector<TherionSurvey *>::iterator it2 = surveys.begin(); it2 != surveys.end(); ++it2 ) {
          TherionSurvey * srv = *it2;
          if ( srv->Name() == sv ) {
            // sh->SetSurvey( srv );
            srv->AddSplayInfo( sh );
            break;
          }
        }
      }
      ++it;
    } else {
      it = splays.erase( it );
      // LOGW("empty splay <%s> <%s> ", sh->From().c_str(), sh->To().c_str() );
    }
  }
}

void 
TherionFile::GetLegAt( TherionStation * station, std::vector< TherionShot * > & legs )
{
  for ( std::vector< TherionShot *>::iterator it = shots.begin(); it != shots.end(); ++it ) {
    TherionShot * sh = *it;
    if ( sh->FromStation() == station ) {
      legs.push_back( sh );
    } else if ( sh->ToStation() == station ) {
      float b = sh->Bering() + (float)M_PI;
      if ( b > 2*M_PI ) b -= 2*(float)M_PI;
      // FIXME MEMORY LEAK
      legs.push_back( new TherionShot( NULL, NULL, sh->Length(), b, - sh->Clino() ) );
    }
  }
}

void 
TherionFile::GetSplayAt( TherionStation * station, std::vector< TherionShot * > & ret, bool also_legs )
{
  for ( std::vector< TherionShot *>::iterator it = splays.begin(); it != splays.end(); ++it ) {
    TherionShot * sh = *it;
    if ( sh->FromStation() == station ) {
      ret.push_back( sh );
    }
  }
  if ( also_legs ) {
    for ( std::vector< TherionShot *>::iterator it = shots.begin(); it != shots.end(); ++it ) {
      TherionShot * sh = *it;
      if ( sh->FromStation() == station ) {
        ret.push_back( sh );
      } else if ( sh->ToStation() == station ) {
        float b = sh->Bering() + (float)M_PI;
        if ( b > 2*M_PI ) b -= 2*(float)M_PI;
        // FIXME MEMORY LEAK
        ret.push_back( new TherionShot( NULL, NULL, sh->Length(), b, - sh->Clino() ) );
      }
    }
  }
}

void 
TherionFile::SetStationDepths( )
{
  float deltaz = zmax - zmin + 0.001;
  int k = 0;
  for ( std::vector< TherionStation *>::iterator it = stations.begin(); it != stations.end(); ++it ) {
    TherionStation * st = *it;
    st->SetDepth( ( zmax - st->Z() )/deltaz ); // Z depth
  }
}

#define CLEAR_VECTOR( type, vec ) \
  for ( std::vector< type * >::iterator it = vec.begin(); it != vec.end(); ++it ) { \
    delete *it; \
  }


TherionFile::~TherionFile()
{
  // LOGI("TherionFile dstr");
  CLEAR_VECTOR( TherionFix, fixes );
  CLEAR_VECTOR( TherionShot, shots );
  CLEAR_VECTOR( TherionShot, splays );
  CLEAR_VECTOR( TherionSurvey, surveys );
  CLEAR_VECTOR( TherionStation, stations );
  if ( mSurface != NULL ) delete mSurface;
  if ( mBitmap != NULL ) delete mBitmap;
}


TherionFile::TherionFile( const std::string & filename )
  : can_render( false )
  , mSurface( NULL )
  , mBitmap( NULL ) 
{
  ks = 0;
  if ( ReadFile( filename.c_str(), "", false, 0.0, 1.0, 1.0, 1.0 ) ) {
    // LOGI("OK ReadFile survey %d shot %d splay %d fix %d", surveys.size(), shots.size(), splays.size(), fixes.size() );
    ProcessShots();
    SetShotSurveys();
    SetSplaySurveys();
    SetStationDepths();
    // LOGI("survey %d shot %d splay %d fix %d", surveys.size(), shots.size(), splays.size(), fixes.size() );

    if ( stations.size() > 0 ) {
      can_render = true;
    }
  } else {
    LOGW("FAILED ReadFile %s", filename.c_str() );
  }
}

void 
TherionFile::ProcessShots()
{
  if ( shots.size() == 0 ) return;
  if ( fixes.size() == 0 ) {
    TherionShot * sh = shots[ 0 ];
    fixes.push_back( new TherionFix( sh->From(), 0.0f, 0.0f, 0.0f ) );
  }

  int mLoopCnt = 0;
  TherionFix * f0 = fixes[0];
  mCaveLength = 0.0;

  for ( size_t kf = 0; kf < fixes.size(); ++kf ) {
    TherionFix * f = fixes[kf];
    // LOGI("Fix %d %s", kf, f->Name().c_str() );

    bool found = false;
    for ( std::vector<TherionStation*>::iterator it = stations.begin(); it != stations.end(); ++it ) {
      if ( f->Name() == (*it)->Name() ) { found = true; break; }
    }
    if ( found ) { 
      // skip fixed stations that are already included in the model
      continue;
    }
    stations.push_back( new TherionStation( f->Name(), f->E(), f->N(), f->Z() ) );
    // sh.from_station = s0;
  
    bool repeat = true;
    while ( repeat ) {
      // LOGI("Stations %d", stations.size() );
      repeat = false;
      for ( std::vector<TherionShot*>::iterator it = shots.begin(); it != shots.end(); ++it ) {
        TherionShot * sh = *it;
        if ( sh->IsUsed() ) continue;
        TherionStation * sf = NULL;
        TherionStation * st = NULL;
        for ( std::vector<TherionStation*>::iterator it2 = stations.begin(); it2 != stations.end(); ++it2 ) {
          TherionStation * s = *it2;
          if ( sh->From() == s->Name() ) {
            sf = s;
            if ( sh->FromStation() == NULL ) {
              sh->SetFromStation( s );
            } else if ( sh->FromStation() != s ) {
              LOGW( "ERROR shot %s %s FROM station mismatch", sh->From().c_str(), sh->To().c_str() );
            }
          } 
          if ( sh->To() == s->Name() )   {
            st = s;
            if ( sh->ToStation() == NULL ) {
              sh->SetToStation( s );
            } else if ( sh->ToStation() != s ) {
              LOGW( "ERROR shot %s %s TO station mismatch", sh->From().c_str(), sh->To().c_str() );
            }
          }
          if ( sf != NULL && st != NULL ) break;
        }
        if ( sf != NULL && st != NULL ) {
          sh->SetUsed( true ); // LOOP
          mCaveLength += sh->Length();
          // make a fake station
          TherionStation * s = sh->GetStationFromStation( sf );
          stations.push_back( s );
          s->AddToName( mLoopCnt );
          ++ mLoopCnt;
          sh->SetToStation( s );
        } else if ( sf != NULL && st == NULL ) {
          TherionStation * s = sh->GetStationFromStation( sf );
          stations.push_back( s );
          sh->SetToStation( s );
          sh->SetUsed( true );
          mCaveLength += sh->Length();
          repeat = true;
        } else if ( sf == NULL && st != NULL ) {
          TherionStation * s = sh->GetStationFromStation( st );
          stations.push_back( s );
          sh->SetFromStation( s );
          sh->SetUsed( true );
          mCaveLength += sh->Length();
          repeat = true;
        } else {
          // LOGI( "WARNING shot %s %s unused", sh->From().c_str(), sh->To().c_str() );
        }
      }
    }
  } // for ( TherionFix f : fixes )

  // 3D splay shots
  for ( std::vector<TherionShot*>::iterator it = splays.begin(); it != splays.end(); ++it ) {
    TherionShot * sh = *it;
    if ( sh->IsUsed() ) continue;
    if (  sh->FromStation() != NULL ) continue;
    for ( std::vector<TherionStation*>::iterator it2 = stations.begin(); it2 != stations.end(); ++it2 ) {
      TherionStation * s = *it2;
      if ( sh->From() == s->Name() ) {
        sh->SetFromStation( s );
        sh->SetUsed( true );
        TherionStation * s2 = sh->GetStationFromStation( s );
        sh->SetToStation( s2 );
        break;
      }
    }
  }

  // bounding box
  emin = emax = stations[0]->E();
  nmin = nmax = stations[0]->N();
  zmin = zmax = stations[0]->Z();
  for ( std::vector<TherionStation*>::iterator it2 = stations.begin(); it2 != stations.end(); ++it2 ) {
    TherionStation * s = *it2;
    if ( nmin > s->N() )      nmin = s->N();
    else if ( nmax < s->N() ) nmax = s->N();
    if ( emin > s->E() )      emin = s->E();
    else if ( emax < s->E() ) emax = s->E();
    if ( zmin > s->Z() )      zmin = s->Z();
    else if ( zmax < s->Z() ) zmax = s->Z();
  }
  // LOGI( "BBox E %.1f %.1f   N %.1f %.1f   Z %.1f %.1f", emin, emax, nmin, nmax, zmin, zmax );
}
      

/** read input file
 * @param usd
 * @param sd
 * @param ul units of length (as multiple of 1 meter)
 * @param ub units of bearing (as multiple of 1 degree)
 * @param uc units of clino
 */
bool 
TherionFile::ReadFile( const std::string & filename,
               const std::string & basepath,
               bool usd, 
               float sd, float ul, float ub, float uc )
{
  std::string path = basepath;
  bool in_surface = false;
  bool in_centerline = false;
  bool in_survey = false;
  bool in_map = false;
  bool use_centerline_declination = false;
  bool use_survey_declination = usd;
  float centerline_declination = 0.0f;
  float survey_declination = sd;
  float units_len = ul;
  float units_ber = ub;
  float units_cln = uc;
  float units_grid = 1; // default units meter
  int grid_flip = FLIP_NONE;

  std::string dirname = "./";
  size_t i = filename.rfind('/');
  if ( i != std::string::npos ) dirname = filename.substr(0, i+1);

  // LOGI("ReadFile dirname %s", dirname.c_str() );
  // LOGI("file %s ...", filename.c_str() );

  FILE * br = fopen( filename.c_str(), "r" );
  if ( br == NULL ) {
    LOGW("ERROR cannot open file in read mode");
    return false;
  }

  char line[4096];
  int size = 4096;
  std::vector< const char * >vals;
  int cnt = 1;
  // Log.v(TAG, cnt + ":" + line );
  char * ch = fgets( line, size, br );
  while ( ch != NULL ) {
    RemoveBrackets( ch );
    ch = Trim( ch );
    RemoveComment( ch );
    // LOGI("%s", ch );

    if ( *ch != 0 ) {
      size_t vals_length = SplitOnSpaces( ch, vals );
      if ( vals_length > 0 ) {
        int idx = 0;
        const char * cmd = vals[idx];
        if ( EQUALS(cmd, "survey") ) {
          if ( ++idx < vals_length ) {
            survey_pos[ks] = path.length();
            path = path + "." + vals[idx];
            ++ks;
            // LOGI("survey[%d] %s", ks, path.c_str() );
            in_survey = true;
          }
        } else if ( in_map ) {
          if ( EQUALS(cmd, "endmap") ) {
            in_map = false;
          }
        } else if ( in_centerline ) {
          if ( EQUALS(cmd, "endcenterline") ) {
            in_centerline = false;
            use_centerline_declination = false;
            centerline_declination = 0.0f;
          } else if ( EQUALS(cmd, "date") ) {
            if ( ++idx < vals_length ) {
              const char * date = vals[idx];
              // TODO
            }
          } else if ( EQUALS( cmd, "flags") ) { // skip
          } else if ( EQUALS( cmd, "team") ) { // skip
          } else if ( EQUALS( cmd, "extend") ) { // skip
          } else if ( EQUALS( cmd, "declination") ) { 
            if ( ++idx < vals_length ) {
              float decl = TO_FLOAT( vals[idx] );
              use_centerline_declination = true;
              centerline_declination = decl;
            }
          } else if ( EQUALS(cmd, "data") ) {
            // data normal from to length compass clino ...
            // TODO
          } else if ( EQUALS(cmd, "units") ) {
            if ( ++idx < vals_length ) {
              // parse "units" command
            }
          } else if ( EQUALS(cmd, "fix") ) { // ***** fix station east north Z
            if ( ++idx < vals_length ) {
              std::string name = vals[idx];
              size_t index = name.find('@');
              if ( index != std::string::npos ) {
                name = name.substr(0,index) + "@" + path + "." + name.substr(index+1);
              } else {
                name += "@" + path;
              }
              // Log.v( TAG, "command fix " + name );
              if ( ++idx < vals_length ) {
                float x = TO_FLOAT( vals[idx] );
                if ( ++idx < vals_length ) {
                  float y = TO_FLOAT( vals[idx] );
                  if ( ++idx < vals_length ) {
                    float z = TO_FLOAT( vals[idx] );
                    fixes.push_back( new TherionFix( name, x, y, z ) );
                  }
                }
              }
            }
          } else if ( vals_length >= 5 ) {
            const char * from = vals[idx];
            if ( ++idx < vals_length ) {
              const char * to = vals[idx]; 
              if ( ++idx < vals_length ) {
                float len = TO_FLOAT( vals[idx] ) * units_len;
                if ( ++idx < vals_length ) {
                  float ber = TO_FLOAT( vals[idx] ) * units_len;
                  if ( use_centerline_declination ) {
                    ber += centerline_declination;
                  } else if ( use_survey_declination ) {
                    ber += survey_declination;
                  }
                  if ( ++idx < vals_length ) {
                    float cln = TO_FLOAT( vals[idx] ) * units_len;
                    // LOGI("add shot <%s %s>", from, to );
                    if ( EQUALS(to, "-") || EQUALS(to, ".") ) {
                      // TODO splay shot
                      std::string from_str = from;
                      from_str += "@" + path;
                      // to = "";
                      splays.push_back( new TherionShot( from_str.c_str(), "", len, ber, cln ) );
                    } else {
                      std::string from_str = from; from_str += "@" + path;
                      std::string to_str   = to;   to_str += "@" + path;
                      shots.push_back( new TherionShot( from_str.c_str(), to_str.c_str(), len, ber, cln ) );
                    }
                  }
                }
              }
            }
          }            
        } else if ( in_surface ) {
          if ( EQUALS(cmd, "endsurface") ) {
            in_surface = false;
            // LOGI("finished reading surface data");
          } else if ( EQUALS(cmd, "bitmap") ) {
            if ( vals_length >= 10 && mBitmap == NULL ) {
              mBitmap = new TherionBitmap( );
              std::string bitmapfile = dirname + '/' + vals[1];
              // LOGI("BITMAP file %s", bitmapfile.c_str() );
              if ( ! mBitmap->Load( bitmapfile.c_str(),
                          atoi( vals[2] ), atoi( vals[3] ), atof( vals[4] ), atof( vals[5] ),
                          atoi( vals[6] ), atoi( vals[7] ), atof( vals[8] ), atof( vals[9] ) ) ) {
                delete mBitmap;
                mBitmap = NULL;
              }
            }
          } else if ( EQUALS(cmd, "grid") ) {
            grid_flip = FLIP_NONE;
            units_grid = 1;
            if ( mSurface ) {
              delete mSurface;
              mSurface = NULL;
            }
            float e1, n1, e2, n2, d1, d2;
            int c1, c2;
            // parse grid metadata
            if ( ++idx < vals_length ) {
              e1 = TO_FLOAT( vals[idx] );
              if ( ++idx < vals_length ) {
                n1 = TO_FLOAT( vals[idx] );
                if ( ++idx < vals_length ) {
                  d1 = TO_FLOAT( vals[idx] );
                  if ( ++idx < vals_length ) {
                    d2 = TO_FLOAT( vals[idx] );
                    if ( ++idx < vals_length ) {
                      c1 = TO_INT( vals[idx] );
                      e2 = e1 + d1*(c1-1);
                      if ( ++idx < vals_length ) {
                        c2 = TO_INT( vals[idx] );
                        n2 = n1 + d2*(c2-1);
                        mSurface = new TherionSurface( e1, n1, e2, n2, c1, c2 );
                        // LOGI( "Surface %.2f %.2f -- %.2f %.2f (%d x %d)", e1, n1, e2, n2, c1, c2);
                      }
                    }
                  }
                }
              }
            }
            // and read grid data
            if ( mSurface != NULL ) {
              mSurface->ReadGridData( units_grid, grid_flip, br );
            }
          } else if ( EQUALS(cmd, "grid-flip") ) {
            // parse the flip-value
            if ( ++idx < vals_length ) {
              grid_flip = ParseFlip( vals[idx] );
            }
          } else if ( EQUALS(cmd, "grid-units") ) {
            // parse the grid-units
            if ( ++idx < vals_length ) {
              float value = TO_FLOAT( vals[idx] );
              if ( ++idx < vals_length ) {
                // FIXME TODO
                // units_grid = parseUnits( value, vals[idx] );
              }
            }
          }
        } else if ( EQUALS(cmd, "declination") ) {
          if ( ++idx < vals_length ) {
            use_survey_declination = true;
            survey_declination = TO_FLOAT( vals[idx] );
          }
        } else if ( EQUALS(cmd, "input") ) {
          if ( ++idx < vals_length ) {
            std::string filename = vals[idx];
            size_t len = strlen( vals[idx] );
            if ( len > 3 && EQUALS( vals[idx] + len-3, ".th" ) ) {
              if ( ! ReadFile( dirname + '/' + filename, 
                               path,
                               use_survey_declination, survey_declination,
                               units_len, units_ber, units_cln ) ) {
                return false;
              }
            }
          }
        } else if ( EQUALS(cmd, "equate") ) {
          if ( ++idx < vals_length ) {
            std::string from = vals[idx];
            int index = from.find('@');
            if ( index != std::string::npos ) {
              from = from.substr(0,index) + "@" + path + "." + from.substr(index+1);
            } else {
              from += "@" + path;
            }
            if ( ++idx < vals_length ) {
              std::string to = vals[idx];
              index = to.find('@');
              if ( index != std::string::npos ) {
                to = to.substr(0,index) + "@" + path + "." + to.substr(index+1);
              } else {
                to += "@" + path;
              }
              shots.push_back( new TherionShot( from.c_str(), to.c_str(), 0.0f, 0.0f, 0.0f ) );
            }
          }
        } else if ( EQUALS(cmd, "surface") ) {
          in_surface = true;
          // LOGI("reading surface data ...");
        } else if ( EQUALS(cmd, "centerline") ) {
          in_centerline = true;
        } else if ( EQUALS(cmd, "map") ) {
          in_map = true;
        } else if ( EQUALS(cmd, "endsurvey") ) {
          --ks;
          if ( ks < 0 ) {
            // Log.e( TAG, filename + ":" + cnt + " negative survey level" );
          } else {
            path = path.substr(0, survey_pos[ks]); // return to previous survey_pos in path
            // LOGI("Therion endsurvey PATH %s" + path.c_str() );
            in_survey = ( ks > 0 );
          }
        }
      }
    }
    ch = fgets( line, size, br );
    ++ cnt;
  }
  // LOGI("Done reading file %s: cnt %d", filename.c_str(), cnt );

  fclose( br );
  return ( shots.size() > 0 );
}

