#include "Surface.h"

#include "DrawFlags.h"
#include "Log.h"

#define SUBS 1
// #define TYPE GL_LINES
#define TYPE GL_TRIANGLES

Surface::Surface( const float * coords, int ne, int nn, int ns, Shader * shader )
      : Geometry( TYPE, "surface", FLAG_SURFACE )
      , vertex_data( NULL )
      , index_data( NULL )
      , stride( ns ) 
{
  Vector4 colA(0.1f, 0.1f, 0.1f, 0.2f);
  Vector4 colD(0.5f, 0.5f, 0.5f, 1.0f);

  SetAmbientColor( colA );
  SetDiffuseColor( colD );
  int nx = ne / SUBS - 4;
  int ny = nn / SUBS - 4;
  // LOGI("Surface cstr: %d x %d shader %s", ne, nn, shader->Name() );
  nVertex = nx * ny;
  #if TYPE == GL_TRIANGLES
    nIndex = 2 * 3 * (nx-1) * (ny-1);
  #else
    nIndex = 2 * (nx-1) * ny + 2 * (ny-1) * nx;
  #endif
  // LOGI("Surface %dx%d nr.vertex %d nr.index %d", nx, ny, nVertex, nIndex );

  vertex_data = new float[ stride * nVertex ];
  index_data = new unsigned short[ nIndex ];
  for ( int k2=0; k2 < ny; ++k2 ) {
    for ( int k1=0; k1 < nx; ++k1 ) {
      int kv = stride * (k2*nx+k1);
      int kc = stride * SUBS * (k2 * ne + k1 );
      for ( int s=0; s<stride; ++s ) {
        vertex_data[kv+s] = coords[kc+s];
      }
      // LOGI("%d-%d: %.2f %.2f %.2f", k%nx, k/nx, vertex_data[3*k+0], vertex_data[3*k+1], vertex_data[3*k+2] );
    }
  }
  int j = 0;
  #if TYPE == GL_TRIANGLES
    for ( int n = 0; n < ny-1; ++n ) {
      for ( int e = 0; e < nx-1; ++e ) {
        index_data[ j++ ] = n * nx + e;       // N.B. order important for CULL
        index_data[ j++ ] = n * nx + e + 1;
        index_data[ j++ ] = (n+1) * nx + e;
        index_data[ j++ ] = n * nx + e + 1;
        index_data[ j++ ] = (n+1) * nx + e+1;
        index_data[ j++ ] = (n+1) * nx + e;
      }
    }
  #else
    for ( int n = 0; n < ny; ++n ) {
      for ( int e = 0; e < nx-1; ++e ) {
        index_data[ j++ ] = n * nx + e;
        index_data[ j++ ] = n * nx + e+1;
      }
    }
    for ( int e = 0; e < nx; ++e ) {
      for ( int n = 0; n < ny-1; ++n ) {
        index_data[ j++ ] = n * nx + e;
        index_data[ j++ ] = (n+1) * nx + e;
      }
    }
  #endif
  if ( j > nIndex ) {
    LOGW("ERROR overrun index array: %d / %d ", j, nIndex );
  } else if ( j < nIndex ) {
    LOGW("ERROR underrun index array: %d / %d ", j, nIndex );
  }
  SetVertex( vertex_data );
  SetIndex( index_data );
  SetVertexStride( stride * sizeof(float) );
  SetNPos( 3 ); // vertex position

  SetGeometry( this );
  SetShader( shader );

  // Vector4 & color = this->GetAmbientColor();
  // color.m_x = 128; // RGBA
  // color.m_y = 128;
  // color.m_z = 128;
  // color.m_w = 0.2f;
  // // this->SetAmbientColor( color );
}

